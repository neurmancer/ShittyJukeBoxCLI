/*
    Same oled  project but for my own player lol 
*/


#define _DEFAULT_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <hidapi/hidapi.h>
#include <unistd.h>
#include <gio/gio.h>
#include <limits.h>

#define VENDOR_ID     0x1038
#define PRODUCT_ID    0x161c
#define SCREEN_WIDTH  128
#define SCREEN_HEIGHT 40
#define FB_SIZE       (SCREEN_WIDTH * SCREEN_HEIGHT / 8)

//Font bitmap
const unsigned char font[95][5] = {
    {0x00,0x00,0x00,0x00,0x00},{0x00,0x00,0x4f,0x00,0x00},{0x00,0x07,0x00,0x07,0x00},{0x14,0x7f,0x14,0x7f,0x14},
    {0x24,0x2a,0x7f,0x2a,0x12},{0x23,0x13,0x08,0x64,0x62},{0x36,0x49,0x55,0x22,0x50},{0x00,0x05,0x03,0x00,0x00},
    {0x00,0x1c,0x22,0x41,0x00},{0x00,0x41,0x22,0x1c,0x00},{0x14,0x08,0x3e,0x08,0x14},{0x08,0x08,0x3e,0x08,0x08},
    {0x00,0x50,0x30,0x00,0x00},{0x08,0x08,0x08,0x08,0x08},{0x00,0x60,0x60,0x00,0x00},{0x20,0x10,0x08,0x04,0x02},
    {0x3e,0x51,0x49,0x45,0x3e},{0x00,0x42,0x7f,0x40,0x00},{0x42,0x61,0x51,0x49,0x46},{0x21,0x41,0x45,0x4b,0x31},
    {0x18,0x14,0x12,0x7f,0x10},{0x27,0x45,0x45,0x45,0x39},{0x3c,0x4a,0x49,0x49,0x30},{0x01,0x71,0x09,0x05,0x03},
    {0x36,0x49,0x49,0x49,0x36},{0x06,0x49,0x49,0x29,0x1e},{0x00,0x36,0x36,0x00,0x00},{0x00,0x56,0x36,0x00,0x00},
    {0x08,0x14,0x22,0x41,0x00},{0x14,0x14,0x14,0x14,0x14},{0x00,0x41,0x22,0x14,0x08},{0x02,0x01,0x51,0x09,0x06},
    {0x32,0x49,0x79,0x41,0x3e},{0x7e,0x11,0x11,0x11,0x7e},{0x7f,0x49,0x49,0x49,0x36},{0x3e,0x41,0x41,0x41,0x22},
    {0x7f,0x41,0x41,0x22,0x1c},{0x7f,0x49,0x49,0x49,0x41},{0x7f,0x09,0x09,0x09,0x01},{0x3e,0x41,0x49,0x49,0x7a},
    {0x7f,0x08,0x08,0x08,0x7f},{0x00,0x41,0x7f,0x41,0x00},{0x20,0x40,0x41,0x3f,0x00},{0x7f,0x08,0x14,0x22,0x41},
    {0x7f,0x40,0x40,0x40,0x40},{0x7f,0x02,0x0c,0x02,0x7f},{0x7f,0x04,0x08,0x10,0x7f},{0x3e,0x41,0x41,0x41,0x3e},
    {0x7f,0x09,0x09,0x09,0x06},{0x3e,0x41,0x51,0x21,0x5e},{0x7f,0x09,0x19,0x29,0x46},{0x46,0x49,0x49,0x49,0x31},
    {0x01,0x01,0x7f,0x01,0x01},{0x3f,0x40,0x40,0x40,0x3f},{0x1f,0x20,0x40,0x20,0x1f},{0x3f,0x40,0x38,0x40,0x3f},
    {0x63,0x14,0x08,0x14,0x63},{0x07,0x08,0x70,0x08,0x07},{0x61,0x51,0x49,0x45,0x43},{0x00,0x7f,0x41,0x41,0x00},
    {0x02,0x04,0x08,0x10,0x20},{0x00,0x41,0x41,0x7f,0x00},{0x04,0x02,0x01,0x02,0x04},{0x40,0x40,0x40,0x40,0x40},
    {0x00,0x01,0x02,0x04,0x00},{0x20,0x54,0x54,0x54,0x78},{0x7f,0x48,0x44,0x44,0x38},{0x38,0x44,0x44,0x44,0x20},
    {0x38,0x44,0x44,0x48,0x7f},{0x38,0x54,0x54,0x54,0x18},{0x08,0x7e,0x09,0x01,0x02},{0x0c,0x52,0x52,0x52,0x3e},
    {0x7f,0x08,0x04,0x04,0x78},{0x00,0x44,0x7d,0x40,0x00},{0x20,0x40,0x44,0x3d,0x00},{0x7f,0x10,0x28,0x44,0x00},
    {0x00,0x41,0x7f,0x40,0x00},{0x7c,0x04,0x18,0x04,0x78},{0x7c,0x08,0x04,0x04,0x78},{0x38,0x44,0x44,0x44,0x38},
    {0x7c,0x14,0x14,0x14,0x08},{0x08,0x14,0x14,0x18,0x7c},{0x7c,0x08,0x04,0x04,0x08},{0x48,0x54,0x54,0x54,0x20},
    {0x04,0x3f,0x44,0x40,0x20},{0x3c,0x40,0x40,0x20,0x7c},{0x1c,0x20,0x40,0x20,0x1c},{0x3c,0x40,0x30,0x40,0x3c},
    {0x44,0x28,0x10,0x28,0x44},{0x0c,0x50,0x50,0x50,0x3c},{0x44,0x64,0x54,0x4c,0x44}
};

void clearFrameBuffer(unsigned char *framebuffer);
void drawString(unsigned char *framebuffer, int x, int y, const char *str);
void drawChar(unsigned char *framebuffer, int x, int y, char c);
void setPixel(unsigned char *framebuffer, int x, int y, int state);

int get_sjb_info(GDBusConnection *bus, char *first, size_t first_size, char *second, size_t second_size);
int sendFBuffer(hid_device *handle, unsigned char *framebuffer);
int stringWidth(const char *str);


int main(void) {
    if (hid_init() < 0) {
        fprintf(stderr, "hid_init failed\n");
        return(1);
    }

    struct hid_device_info *devs = hid_enumerate(VENDOR_ID, PRODUCT_ID);
    hid_device *handle = NULL;

    for (struct hid_device_info *cur = devs; cur; cur = cur->next) {
        if (cur->interface_number == 1) {
            handle = hid_open_path(cur->path);
            if (handle) { break; }
        }
    }
    hid_free_enumeration(devs);

    if (!handle) {
        fprintf(stderr, "Can't open Apex OLED interface 1\n");
        hid_exit();
        return(1);
    }

    printf("OLED opened successfully\n");

    GError *error = NULL;
    GDBusConnection *bus = g_bus_get_sync(G_BUS_TYPE_SESSION, NULL, &error);
    if (!bus) {
        fprintf(stderr, "Session bus: %s\n", error->message);
        g_error_free(error);
        hid_close(handle);
        hid_exit();
        return(1);
    }

    unsigned char framebuffer[FB_SIZE];
    char first[512] = {0};
    char second[512]  = {0};
    char last_first[512] = {0};
    char last_second[512]  = {0};

    int scroll_a = 0;
    int scroll_t = 0;

    while (1) {
        get_sjb_info(bus, first, sizeof(first), second, sizeof(second));

        if (strcmp(first, last_first)) {
            strcpy(last_first, first);
            scroll_a = 0;
        }
        if (strcmp(second, last_second)) {
            strcpy(last_second, second);
            scroll_t = 0;
        }

        clearFrameBuffer(framebuffer);

        int aw = stringWidth(first);
        if (aw <= SCREEN_WIDTH - 4) {
            drawString(framebuffer, (SCREEN_WIDTH - aw) / 2, 5, first);
        } else {
            drawString(framebuffer, 2 - scroll_a, 5, first);
            drawString(framebuffer, 2 - scroll_a + aw + 24, 5, first);
            scroll_a++;
            if (scroll_a > aw + 24) { scroll_a = 0; }
        }

        int tw = stringWidth(second);
        if (tw <= SCREEN_WIDTH - 4) {
            drawString(framebuffer, (SCREEN_WIDTH - tw) / 2, 22, second);
        } else {
            drawString(framebuffer, 2 - scroll_t, 22, second);
            drawString(framebuffer, 2 - scroll_t + tw + 24, 22, second);
            scroll_t++;
            if (scroll_t > tw + 24) { scroll_t = 0; }
        }

        sendFBuffer(handle, framebuffer);
        usleep(50000); // 50ms
    }

    g_object_unref(bus);
    hid_close(handle);
    hid_exit();
    return(0);
}



void setPixel(unsigned char *framebuffer, int x, int y, int state) {
    if (x < 0 || x >= SCREEN_WIDTH || y < 0 || y >= SCREEN_HEIGHT) return;
    int byte_index = y * (SCREEN_WIDTH / 8) + (x / 8);
    int bit_pos    = 7 - (x % 8);
    if (state){ framebuffer[byte_index] |=  (1 << bit_pos); }
    else{ framebuffer[byte_index] &= ~(1 << bit_pos); }
}

void drawChar(unsigned char *framebuffer, int x, int y, char c) {
    if (c < 32 || c > 126){ c = ' ';}
    int font_idx = c - 32;
    for (int col = 0; col < 5; col++) {
        unsigned char line = font[font_idx][col];
        for (int row = 0; row < 7; row++) {
            setPixel(framebuffer, x + col, y + row, (line >> row) & 1);
        }
    }
}

static char *displayText(const char *text)
{
    char *valid = g_utf8_make_valid(text, -1);
    char *out = valid;
    for (const char *p = valid; *p;) {
        gunichar character = g_utf8_get_char(p);
        p = g_utf8_next_char(p);
        if (character == 0x0131) { character = 'i'; }
        out += g_unichar_to_utf8(character, out);
    }
    *out = '\0';
    char *ascii = g_str_to_ascii(valid, "en");
    g_free(valid);
    /* Tiny-ass font gets readable letters, not one blank per UTF-8 byte. */
    for (char *p = ascii; *p; ++p) {
        if ((unsigned char)*p < 32 || (unsigned char)*p == 127) { *p = ' '; }
    }
    return(ascii);
}

void drawString(unsigned char *framebuffer, int x, int y, const char *str) {
    char *text = displayText(str);
    for (const char *p = text; *p && x < SCREEN_WIDTH; ++p) {
        drawChar(framebuffer, x, y, *p);
        x += 6;
    }
    g_free(text);
}

int stringWidth(const char *str) {
    char *text = displayText(str);
    size_t length = strlen(text);
    g_free(text);
    return(length > INT_MAX / 6 ? INT_MAX : (int)length * 6);
}

void clearFrameBuffer(unsigned char *framebuffer) {
    memset(framebuffer, 0, FB_SIZE);
}

int sendFBuffer(hid_device *handle, unsigned char *framebuffer) {
    unsigned char report[642];
    report[0] = 0x61;
    memcpy(report + 1, framebuffer, FB_SIZE);
    report[641] = 0x00;
    return(hid_send_feature_report(handle, report, 642));
}

static void metadata_rows(GVariant *metadata, char *first, size_t first_size,
                          char *second, size_t second_size)
{
    const char *lead = "", *backing = "", *title = "";
    g_variant_lookup(metadata, "sjb:lyricWord", "&s", &lead);
    g_variant_lookup(metadata, "sjb:backingWord", "&s", &backing);
    char *lead_text = displayText(lead), *backing_text = displayText(backing);
    g_strstrip(lead_text);
    g_strstrip(backing_text);
    if (*lead_text || *backing_text) {
        snprintf(first, first_size, "%s", *lead_text ? lead_text : backing_text);
        snprintf(second, second_size, "%s", *lead_text ? backing_text : "");
    }
    else {
        char **artists = NULL;
        g_variant_lookup(metadata, "xesam:artist", "^as", &artists);
        g_variant_lookup(metadata, "xesam:title", "&s", &title);
        char *joined = artists ? g_strjoinv(", ", artists) : g_strdup("");
        char *artist_text = displayText(joined), *title_text = displayText(title);
        snprintf(first, first_size, "%s", *artist_text || *title_text ? artist_text : "OwO");
        snprintf(second, second_size, "%s", title_text);
        g_free(artist_text);
        g_free(title_text);
        g_free(joined);
        g_strfreev(artists);
    }
    g_free(lead_text);
    g_free(backing_text);
}

int get_sjb_info(GDBusConnection *bus, char *first, size_t first_size, char *second, size_t second_size)
{
    GVariant *reply = g_dbus_connection_call_sync(bus, "org.mpris.MediaPlayer2.ShittyJukeBox",
        "/org/mpris/MediaPlayer2", "org.freedesktop.DBus.Properties", "Get",
        g_variant_new("(ss)", "org.mpris.MediaPlayer2.Player", "Metadata"),
        G_VARIANT_TYPE("(v)"), G_DBUS_CALL_FLAGS_NO_AUTO_START, 500, NULL, NULL);
    if (!reply) {
        snprintf(first, first_size, "No SJB :/");
        if (second_size) { second[0] = '\0'; }
        return(-1);
    }
    GVariant *metadata = NULL;
    g_variant_get(reply, "(v)", &metadata);
    if (!g_variant_is_of_type(metadata, G_VARIANT_TYPE_VARDICT)) {
        g_variant_unref(metadata);
        g_variant_unref(reply);
        snprintf(first, first_size, "No SJB :/");
        if (second_size) { second[0] = '\0'; }
        return(-1);
    }
    metadata_rows(metadata, first, first_size, second, second_size);
    g_variant_unref(metadata);
    g_variant_unref(reply);
    return(0);
}
