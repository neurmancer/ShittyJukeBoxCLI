#include "mpris.h"
#include <gio/gio.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define ROOT "org.mpris.MediaPlayer2"
#define PLAYER ROOT ".Player"
#define PATH "/org/mpris/MediaPlayer2"
#define PROP(name, type, access) "<property name='" name "' type='" type "' access='" access "'/>"
static const char introspection[] =
    "<node><interface name='" ROOT "'>"
    "<method name='Raise'/><method name='Quit'/>"
    
    PROP("CanQuit", "b", "read") PROP("CanRaise", "b", "read")
    PROP("HasTrackList", "b", "read") PROP("Identity", "s", "read")
    PROP("SupportedUriSchemes", "as", "read") PROP("SupportedMimeTypes", "as", "read")
    
    "</interface><interface name='" PLAYER "'>"
    "<method name='Next'/><method name='Previous'/><method name='Pause'/>"
    "<method name='PlayPause'/><method name='Stop'/><method name='Play'/>"
    "<method name='Seek'><arg name='Offset' type='x' direction='in'/></method>"
    "<method name='SetPosition'><arg name='TrackId' type='o' direction='in'/>"
    "<arg name='Position' type='x' direction='in'/></method>"
    "<method name='OpenUri'><arg name='Uri' type='s' direction='in'/></method>"
    "<signal name='Seeked'><arg name='Position' type='x'/></signal>"
    
    PROP("PlaybackStatus", "s", "read") PROP("LoopStatus", "s", "readwrite")
    PROP("Rate", "d", "readwrite") PROP("Shuffle", "b", "readwrite")
    PROP("Metadata", "a{sv}", "read") PROP("Volume", "d", "readwrite")
    PROP("Position", "x", "read") PROP("MinimumRate", "d", "read") PROP("MaximumRate", "d", "read")
    PROP("CanGoNext", "b", "read") PROP("CanGoPrevious", "b", "read")
    PROP("CanPlay", "b", "read") PROP("CanPause", "b", "read")
    PROP("CanSeek", "b", "read") PROP("CanControl", "b", "read") "</interface></node>";

struct Mpris {
    GMainContext *main;
    
    GDBusConnection *bus;
    GDBusNodeInfo *info;
    guint registrations[2];
    
    char name[128];
    
    MprisRead read;
    MprisControl control;
    
    void *context;
    
    GVariant *last;
    
    uint64_t generation, seek_serial;
    bool changed;
};

static const char *playback_status(const MprisState *state)
{
    AudioState audio = state->audio.state;
    
    if (audio == AUDIO_PLAYING || audio == AUDIO_PAUSED || audio == AUDIO_LOADING) {
        return(state->audio.pause_requested ? "Paused" : "Playing");
    }
    return("Stopped");
}

static void track_path(const MprisState *state, char *path, size_t size)
{
    if (state->song) { snprintf(path, size, PATH "/track/t%llu", (unsigned long long)state->track); }
    
    else { snprintf(path, size, PATH "/TrackList/NoTrack"); }
}

static GVariant *string_value(const char *value)
{
    char *valid = g_utf8_make_valid(value ? value : "", -1);
    
    GVariant *result = g_variant_new_string(valid);
    
    g_free(valid);
    return(result);
}

/* Local paths must be percent-escaped file URIs for desktop clients */
static char *uri_value(const char *value)
{
    if (!value || !*value) { return(g_strdup("")); }
    
    char *scheme = g_uri_parse_scheme(value);
    
    if (scheme) { g_free(scheme); return(g_utf8_make_valid(value, -1)); }
    
    char *absolute = g_canonicalize_filename(value, NULL);
    char *uri = g_filename_to_uri(absolute, NULL, NULL);
    
    g_free(absolute);
    
    return(uri ? uri : g_strdup(""));
}

static GVariant *current_lyrics(const MprisState *state, const Lyrics *lyrics, int64_t duration)
{
    GString *text = g_string_new(NULL);
    if (lyrics && (state->audio.state == AUDIO_PLAYING || state->audio.state == AUDIO_PAUSED) &&
        state->audio.generation == state->track) {
        size_t active = lyrics_active(lyrics, state->audio.position_ms);
        for (size_t i = active; i < lyrics->count &&
             lyrics->cues[i].time_ms == lyrics->cues[active].time_ms; ++i) {
            if (!lyrics_cue_active(lyrics, i, state->audio.position_ms, duration)) { continue; }
            if (text->len) { g_string_append_c(text, ' '); }
            g_string_append(text, lyrics->cues[i].text);
        }
    }
    GVariant *result = string_value(text->str);
    g_string_free(text, TRUE);
    return(result);
}

static GVariant *metadata(const MprisState *state)
{
    GVariantBuilder map;
    
    g_variant_builder_init(&map, G_VARIANT_TYPE_VARDICT);
    
    if (state->song) {
        const DbSong *song = state->song;
        char path[128];
    
        track_path(state, path, sizeof path);
    
        g_variant_builder_add(&map, "{sv}", "mpris:trackid", g_variant_new_object_path(path));
    
        int64_t duration = state->audio.duration_ms >= 0 ? state->audio.duration_ms : song->duration_ms;
        g_variant_builder_add(&map, "{sv}", "sjb:lyric", current_lyrics(state, state->lyrics, duration));
        g_variant_builder_add(&map, "{sv}", "sjb:backingLyric", current_lyrics(state, state->backing_lyrics, duration));
    
        if (duration >= 0 && duration <= INT64_MAX / 1000) {
            g_variant_builder_add(&map, "{sv}", "mpris:length", g_variant_new_int64(duration * 1000));
        }
    
        g_variant_builder_add(&map, "{sv}", "xesam:title", string_value(song->title));
        g_variant_builder_add(&map, "{sv}", "xesam:album", string_value(song->album));
    
        char *artist = g_utf8_make_valid(song->artist ? song->artist : "", -1);
        const char *artists[] = {artist, NULL};
    
    
        g_variant_builder_add(&map, "{sv}", "xesam:artist", g_variant_new_strv(artists, -1));
        g_free(artist);
    
        char *uri = uri_value(song->media_uri);
    
    
        g_variant_builder_add(&map, "{sv}", "xesam:url", string_value(uri));
        g_free(uri);
    
        uri = uri_value(state->art_uri);
    
        if (*uri) { g_variant_builder_add(&map, "{sv}", "mpris:artUrl", string_value(uri)); }
    
        g_free(uri);
    }
    
    return(g_variant_builder_end(&map));
}

static GVariant *player_property(const MprisState *state, const char *name)
{
    if (!strcmp(name, "PlaybackStatus")) { return(g_variant_new_string(playback_status(state))); }
    if (!strcmp(name, "LoopStatus")) { return(g_variant_new_string(state->loop == 1 ? "Track" : state->loop == 2 ? "Playlist" : "None")); }
    if (!strcmp(name, "Shuffle")) { return(g_variant_new_boolean(state->shuffle)); }
    if (!strcmp(name, "Volume")) { return(g_variant_new_double(state->audio.volume_percent / 100.0)); }
    if (!strcmp(name, "Position")) { return(g_variant_new_int64(state->audio.position_ms * 1000)); }
    if (!strcmp(name, "Metadata")) { return(metadata(state)); }
    if (!strcmp(name, "CanGoNext")) { return(g_variant_new_boolean(state->can_next)); }
    if (!strcmp(name, "CanGoPrevious")) { return(g_variant_new_boolean(state->can_previous)); }
    if (!strcmp(name, "CanPlay")) { return(g_variant_new_boolean(state->can_play)); }
    if (!strcmp(name, "CanPause")) { return(g_variant_new_boolean(state->song != NULL)); }
    
    
    if (!strcmp(name, "CanSeek")) {
        return(g_variant_new_boolean(state->audio.seekable &&
               (state->audio.state == AUDIO_PLAYING || state->audio.state == AUDIO_PAUSED)));
    }
    
    if (!strcmp(name, "CanControl")) { return(g_variant_new_boolean(TRUE)); }
    if (!strcmp(name, "Rate") || !strcmp(name, "MinimumRate") || !strcmp(name, "MaximumRate")) { return(g_variant_new_double(1.0)); }
    
    return(NULL);
}

static GVariant *get_property(GDBusConnection *bus, const char *sender, const char *path,
                             const char *interface, const char *name, GError **error, void *opaque)
{
    (void)bus; (void)sender; (void)path; (void)error;
    
    Mpris *mpris = opaque;
    
    if (!strcmp(interface, PLAYER)) {
        MprisState state = mpris->read(mpris->context);
        return(player_property(&state, name));
    }
    
    if (!strcmp(name, "Identity")) { return(g_variant_new_string("ShittyJukeBox")); }
    if (!strcmp(name, "CanQuit")) { return(g_variant_new_boolean(TRUE)); }
    if (!strcmp(name, "SupportedUriSchemes")) {
    
        const char *schemes[] = {"file", "http", "https", NULL};
    
        return(g_variant_new_strv(schemes, -1));
    }
    
    if (!strcmp(name, "SupportedMimeTypes")) {
    
        const char *types[] = {"audio/mpeg", "audio/flac", "audio/ogg", "application/ogg", "audio/opus",
                               "audio/x-wav", "audio/wav", "audio/mp4", "audio/aac", "audio/x-aiff", "audio/x-ms-wma", NULL};
        return(g_variant_new_strv(types, -1));
    }
    
    return(g_variant_new_boolean(FALSE));
}

static bool command(Mpris *mpris, MprisCommand action, int64_t value, const char *text)
{
    bool result = mpris->control(mpris->context, action, value, text);
    
    mpris->changed = true;
    
    return(result);
}

static gboolean set_property(GDBusConnection *bus, const char *sender, const char *path, const char *interface, const char *name, GVariant *value, GError **error, void *opaque)
{
    (void)bus; (void)sender; (void)path; (void)interface;
    
    Mpris *mpris = opaque;
    
    if (!strcmp(name, "Volume")) {
        double volume = g_variant_get_double(value);
        if (isfinite(volume)) { return(command(mpris, MPRIS_VOLUME, volume <= 0 ? 0 : volume >= 1 ? 100 : (int64_t)lround(volume * 100), NULL)); }
    }
    
    else if (!strcmp(name, "Shuffle")) { return(command(mpris, MPRIS_SHUFFLE, g_variant_get_boolean(value), NULL)); }
    
    else if (!strcmp(name, "LoopStatus")) {
        const char *loop = g_variant_get_string(value, NULL);
    
        if (!strcmp(loop, "None") || !strcmp(loop, "Track") || !strcmp(loop, "Playlist")) {
            return(command(mpris, MPRIS_LOOP, !strcmp(loop, "Track") ? 1 : !strcmp(loop, "Playlist") ? 2 : 0, NULL));
        }
    }
    
    else if (!strcmp(name, "Rate")) {
        double rate = g_variant_get_double(value);
    
        if (rate == 1) { return(TRUE); }
        if (rate == 0) { return(command(mpris, MPRIS_PAUSE, 0, NULL)); }
    }
    
    g_set_error(error, G_DBUS_ERROR, G_DBUS_ERROR_INVALID_ARGS, "Unsupported value for %s", name);
    
    return(FALSE);
}

static void method_call(GDBusConnection *bus, const char *sender, const char *path, const char *interface,
                        const char *method, GVariant *parameters, GDBusMethodInvocation *invocation, void *opaque)
{
    (void)bus; (void)sender; (void)path; (void)interface;
    
    Mpris *mpris = opaque;
    MprisState state = mpris->read(mpris->context);
    
    bool ok = true;
    
    if (!strcmp(method, "Quit")) { ok = command(mpris, MPRIS_QUIT, 0, NULL); }
    
    else if (!strcmp(method, "Raise")) { /* A terminal cannot raise its parent window. */ }
    else if (!strcmp(method, "Next")) { ok = command(mpris, MPRIS_NEXT, 0, NULL); }
    else if (!strcmp(method, "Previous")) { ok = command(mpris, MPRIS_PREVIOUS, 0, NULL); }
    else if (!strcmp(method, "Play")) { ok = command(mpris, MPRIS_PLAY, 0, NULL); }
    else if (!strcmp(method, "Pause")) { ok = command(mpris, MPRIS_PAUSE, 0, NULL); }
    else if (!strcmp(method, "PlayPause")) { ok = command(mpris, MPRIS_TOGGLE, 0, NULL); }
    else if (!strcmp(method, "Stop")) { ok = command(mpris, MPRIS_STOP, 0, NULL); }
    else if (!strcmp(method, "Seek") || !strcmp(method, "SetPosition")) {
    
        gint64 position;
    
        bool relative = !strcmp(method, "Seek"), valid = true;
    
        if (relative) {
    
            g_variant_get(parameters, "(x)", &position);
    
            int64_t current = state.audio.position_ms * 1000;
    
            position = position > INT64_MAX - current ? INT64_MAX : current + position;
    
            if (position < 0) { position = 0; }
        }
    
        else {
            const char *track;
            char expected[128];
    
            g_variant_get(parameters, "(&ox)", &track, &position);
            track_path(&state, expected, sizeof expected);
    
            valid = state.song && !strcmp(track, expected) && position >= 0;
        }
    
        if (valid && state.audio.seekable && (state.audio.state == AUDIO_PLAYING || state.audio.state == AUDIO_PAUSED)) {
    
            if (state.audio.duration_ms >= 0 && state.audio.duration_ms <= INT64_MAX / 1000 &&
    
                position > state.audio.duration_ms * 1000) {
    
                if (relative) { ok = command(mpris, MPRIS_NEXT, 0, NULL); }
            }
            
            else { ok = command(mpris, MPRIS_SEEK, position / 1000, NULL); }
        }
    }
    
    else if (!strcmp(method, "OpenUri")) {
    
        const char *uri;
    
        g_variant_get(parameters, "(&s)", &uri);
    
        char *scheme = g_uri_parse_scheme(uri);
    
        if (!scheme || (g_ascii_strcasecmp(scheme, "file") && g_ascii_strcasecmp(scheme, "http") && g_ascii_strcasecmp(scheme, "https"))) {
            g_dbus_method_invocation_return_error(invocation, G_DBUS_ERROR, G_DBUS_ERROR_NOT_SUPPORTED, "Supported URI schemes: file, http, https");
            g_free(scheme);
            return;
        }
    
        char *local = !g_ascii_strcasecmp(scheme, "file") ? g_filename_from_uri(uri, NULL, NULL) : g_strdup(uri);
    
        g_free(scheme);
    
        ok = local && command(mpris, MPRIS_OPEN, 0, local);
    
        g_free(local);
    }
    
    if (ok) { g_dbus_method_invocation_return_value(invocation, NULL); }
    
    else { g_dbus_method_invocation_return_error(invocation, G_DBUS_ERROR, G_DBUS_ERROR_FAILED, "Cannot perform playback command"); }
}

static const GDBusInterfaceVTable vtable = { .method_call = method_call, .get_property = get_property, .set_property = set_property };

Mpris *mpris_create(MprisRead read, MprisControl control, void *context, char *error, size_t size)
{
    Mpris *mpris = g_new0(Mpris, 1);
    GError *failure = NULL;
    
    mpris->read = read;
    mpris->control = control;
    mpris->context = context;
    mpris->main = g_main_context_new();
    
    g_main_context_push_thread_default(mpris->main);
    
    char *address = g_dbus_address_get_for_bus_sync(G_BUS_TYPE_SESSION, NULL, &failure);
    
    if (address) {
        mpris->bus = g_dbus_connection_new_for_address_sync(address,
            G_DBUS_CONNECTION_FLAGS_AUTHENTICATION_CLIENT | G_DBUS_CONNECTION_FLAGS_MESSAGE_BUS_CONNECTION, NULL, NULL, &failure);
        g_free(address);
    }
    
    if (!mpris->bus) { goto failed; }
    
    g_dbus_connection_set_exit_on_close(mpris->bus, FALSE);
    
    mpris->info = g_dbus_node_info_new_for_xml(introspection, &failure);
    
    if (!mpris->info) { goto failed; }
    
    for (size_t i = 0; i < 2; ++i) {
        mpris->registrations[i] = g_dbus_connection_register_object(mpris->bus, PATH, mpris->info->interfaces[i], &vtable, mpris, NULL, &failure);
        if (!mpris->registrations[i]) { goto failed; }
    }
    
    snprintf(mpris->name, sizeof mpris->name, ROOT ".ShittyJukeBox");
    
    for (int attempt = 0; attempt < 2; ++attempt) {
        GVariant *reply = g_dbus_connection_call_sync(mpris->bus, "org.freedesktop.DBus", "/org/freedesktop/DBus",
            "org.freedesktop.DBus", "RequestName", g_variant_new("(su)", mpris->name, 4u), G_VARIANT_TYPE("(u)"),
            G_DBUS_CALL_FLAGS_NONE, 1500, NULL, &failure);
        if (!reply) { goto failed; }
    
        guint result;
    
        g_variant_get(reply, "(u)", &result);
        g_variant_unref(reply);
    
        if (result == 1) {
            g_main_context_pop_thread_default(mpris->main);
            return(mpris);
        }
    
        snprintf(mpris->name, sizeof mpris->name, ROOT ".ShittyJukeBox.instance%ld", (long)getpid());
    }

failed:
    snprintf(error, size, "MPRIS unavailable: %s", failure ? failure->message : "bus name already owned");
    g_clear_error(&failure);
    g_main_context_pop_thread_default(mpris->main);
    mpris_destroy(mpris);

    return(NULL);
}

bool mpris_poll(Mpris *mpris)
{
    if (!mpris || g_dbus_connection_is_closed(mpris->bus)) { return(false); }

    for (int i = 0; i < 32 && g_main_context_iteration(mpris->main, FALSE); ++i) { }

    MprisState state = mpris->read(mpris->context);
    GVariantBuilder all, changed;

    g_variant_builder_init(&all, G_VARIANT_TYPE_VARDICT);
    g_variant_builder_init(&changed, G_VARIANT_TYPE_VARDICT);

    bool any = false;

    for (GDBusPropertyInfo **property = mpris->info->interfaces[1]->properties; *property; ++property) {
        const char *name = (*property)->name;

        if (!strcmp(name, "Position")) { continue; }

        GVariant *value = g_variant_ref_sink(player_property(&state, name));
        GVariant *old = mpris->last ? g_variant_lookup_value(mpris->last, name, NULL) : NULL;

        if (!old || !g_variant_equal(old, value)) {
            g_variant_builder_add(&changed, "{sv}", name, value);
            any = true;
        }

        g_variant_builder_add(&all, "{sv}", name, value);

        if (old) { g_variant_unref(old); }

        g_variant_unref(value);
    }

    if (any) {
        g_dbus_connection_emit_signal(mpris->bus, NULL, PATH, "org.freedesktop.DBus.Properties", "PropertiesChanged",
            g_variant_new("(sa{sv}as)", PLAYER, &changed, NULL), NULL);
    }

    else { g_variant_builder_clear(&changed); }

    if (mpris->last) { g_variant_unref(mpris->last); }

    mpris->last = g_variant_ref_sink(g_variant_builder_end(&all));

    if (state.audio.seek_serial && (mpris->generation != state.audio.generation || mpris->seek_serial != state.audio.seek_serial)) {
        g_dbus_connection_emit_signal(mpris->bus, NULL, PATH, PLAYER, "Seeked",
            g_variant_new("(x)", (gint64)(state.audio.seek_position_ms * 1000)), NULL);
    }

    mpris->generation = state.audio.generation;
    mpris->seek_serial = state.audio.seek_serial;

    bool result = mpris->changed;

    mpris->changed = false;

    return(result);
}

void mpris_destroy(Mpris *mpris)
{
    if (!mpris) { return; }
    if (mpris->bus) {
        for (size_t i = 0; i < 2; ++i) {
            if (mpris->registrations[i]) { g_dbus_connection_unregister_object(mpris->bus, mpris->registrations[i]); }
        }

        if (!g_dbus_connection_is_closed(mpris->bus)) {
            g_dbus_connection_flush_sync(mpris->bus, NULL, NULL);
            g_dbus_connection_close_sync(mpris->bus, NULL, NULL);
        }
        g_object_unref(mpris->bus);
    }
    if (mpris->last) { g_variant_unref(mpris->last); }
    if (mpris->info) { g_dbus_node_info_unref(mpris->info); }

    g_main_context_unref(mpris->main);
    g_free(mpris);
}
