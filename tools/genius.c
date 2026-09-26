#define _POSIX_C_SOURCE 200809L
#include "genius.h"
#include <curl/curl.h>
#include <libxml/HTMLparser.h>
#include <libxml/tree.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PAGE_LIMIT (4 * 1024 * 1024)
typedef struct { char *data; size_t length, capacity; } Buffer;

static int append(Buffer *buffer, const char *text, size_t length)
{
    if (length > PAGE_LIMIT - buffer->length) { return(-1); }
    size_t needed = buffer->length + length + 1;
    if (needed > buffer->capacity) {
        size_t capacity = buffer->capacity ? buffer->capacity : 4096;
        while (capacity < needed) { capacity *= 2; }
        char *grown = realloc(buffer->data, capacity);
        if (!grown) { return(-1); }
        buffer->data = grown;
        buffer->capacity = capacity;
    }
    memcpy(buffer->data + buffer->length, text, length);
    buffer->length += length;
    buffer->data[buffer->length] = '\0';
    return(0);
}

static size_t receive(char *data, size_t size, size_t count, void *context)
{
    if (size && count > PAGE_LIMIT / size) { return(0); }
    size_t length = size * count;
    return(append(context, data, length) < 0 ? 0 : length);
}

static int element(xmlNode *node, const char *name)
{
    return(node->type == XML_ELEMENT_NODE && xmlStrEqual(node->name, BAD_CAST name));
}

static int container(xmlNode *node)
{
    xmlChar *attribute = xmlGetProp(node, BAD_CAST "data-lyrics-container");
    int found = attribute && xmlStrEqual(attribute, BAD_CAST "true");
    xmlFree(attribute);
    return(found);
}

static int content(xmlNode *node, Buffer *out)
{
    for (; node; node = node->next) {
        if (node->type == XML_TEXT_NODE && node->content) {
            /* Collapse HTML formatting whitespace, keeping line breaks from <br>. */
            for (const unsigned char *p = node->content; *p; ++p) {
                if (isspace(*p)) {
                    if (out->length && out->data[out->length - 1] != ' ' && out->data[out->length - 1] != '\n') {
                        if (append(out, " ", 1) < 0) { return(-1); }
                    }
                }

                else if (*p >= 32 && *p != 127) {
                    if (append(out, (const char *)p, 1) < 0) { return(-1); }
                }
            }
        }

        else if (element(node, "br")) {
            while (out->length && out->data[out->length - 1] == ' ') { --out->length; }
            if (append(out, "\n", 1) < 0) { return(-1); }
        }

        else if (node->type == XML_ELEMENT_NODE) {
            xmlChar *exclude = xmlGetProp(node, BAD_CAST "data-exclude-from-selection");
            int skip = exclude != NULL || element(node, "script") || element(node, "style") ||
                       element(node, "button") || element(node, "svg");
            xmlFree(exclude);
            if (!skip && content(node->children, out) < 0) { return(-1); }
        }
    }
    return(0);
}

static int collect(xmlNode *node, Buffer *out)
{
    for (; node; node = node->next) {
        if (node->type != XML_ELEMENT_NODE) { continue; }
        if (container(node)) {
            if (out->length && append(out, "\n\n", 2) < 0) { return(-1); }
            if (content(node->children, out) < 0) { return(-1); }
            while (out->length && isspace((unsigned char)out->data[out->length - 1])) { --out->length; }
            if (out->data) { out->data[out->length] = '\0'; }
        }

        else if (collect(node->children, out) < 0) { return(-1); }
    }
    return(0);
}

int genius_extract(const char *html, size_t length, char **lyrics, char *error, size_t error_size)
{
    *lyrics = NULL;
    if (!html || !length || length > PAGE_LIMIT) {
        snprintf(error, error_size, "Empty or oversized Genius page");
        return(-1);
    }
    htmlDocPtr document = htmlReadMemory(html, (int)length, NULL, "UTF-8",
        HTML_PARSE_RECOVER | HTML_PARSE_NONET | HTML_PARSE_NOERROR | HTML_PARSE_NOWARNING);
    Buffer result = {0};
    int ok = document && collect(xmlDocGetRootElement(document), &result) == 0 && result.length;
    xmlFreeDoc(document);
    if (!ok) {
        free(result.data);
        snprintf(error, error_size, "No readable lyrics found (blocked page, changed markup, or allocation limit)");
        return(-1);
    }
    *lyrics = result.data;
    return(0);
}

static int allowed_url(const char *url)
{
    CURLU *parsed = curl_url();
    char *host = NULL, *scheme = NULL, *user = NULL, *port = NULL;
    int ok = parsed && curl_url_set(parsed, CURLUPART_URL, url, 0) == CURLUE_OK &&
        curl_url_get(parsed, CURLUPART_HOST, &host, 0) == CURLUE_OK &&
        curl_url_get(parsed, CURLUPART_SCHEME, &scheme, 0) == CURLUE_OK &&
        !strcmp(scheme, "https") && (!strcmp(host, "genius.com") || !strcmp(host, "www.genius.com"));
    if (ok && curl_url_get(parsed, CURLUPART_USER, &user, 0) == CURLUE_OK) { ok = 0; }
    if (ok && curl_url_get(parsed, CURLUPART_PORT, &port, 0) == CURLUE_OK && strcmp(port, "443")) { ok = 0; }
    curl_free(host); curl_free(scheme); curl_free(user); curl_free(port);
    curl_url_cleanup(parsed);
    return(ok);
}

static int transfer_progress(void *context, curl_off_t total, curl_off_t received,
                             curl_off_t upload_total, curl_off_t uploaded)
{
    (void)total; (void)received; (void)upload_total; (void)uploaded;
    const volatile sig_atomic_t *cancelled = context;
    return(cancelled && *cancelled);
}

int genius_fetch(const char *url, char **lyrics, char *error, size_t error_size)
{
    return(genius_fetch_cancelable(url, lyrics, error, error_size, NULL));
}

int genius_fetch_cancelable(const char *url, char **lyrics, char *error, size_t error_size,
                            const volatile sig_atomic_t *cancelled)
{
    *lyrics = NULL;
    if (curl_global_init(CURL_GLOBAL_DEFAULT) != CURLE_OK) {
        snprintf(error, error_size, "Cannot initialize libcurl");
        return(-1);
    }
    CURL *curl = curl_easy_init();
    char *current = url ? strdup(url) : NULL;
    Buffer page = {0};
    int result = -1;
    snprintf(error, error_size, "Cannot allocate Genius request");
    if (!curl || !current) { goto done; }
    curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);
    curl_easy_setopt(curl, CURLOPT_XFERINFOFUNCTION, transfer_progress);
    curl_easy_setopt(curl, CURLOPT_XFERINFODATA, cancelled);
    curl_easy_setopt(curl, CURLOPT_PROTOCOLS_STR, "https");
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "ShittyJukeBox/1.0 (lyrics importer)");
    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 10L);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 30L);
    curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1L);
    curl_easy_setopt(curl, CURLOPT_ACCEPT_ENCODING, "");
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, receive);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &page);
    for (int redirect = 0; redirect <= 4; ++redirect) {
        if (cancelled && *cancelled) { goto done; }
        if (!allowed_url(current)) {
            snprintf(error, error_size, "Use an https://genius.com/ song URL (redirects must stay on Genius)");
            goto done;
        }
        page.length = 0;
        curl_easy_setopt(curl, CURLOPT_URL, current);
        CURLcode status = curl_easy_perform(curl);
        if (status != CURLE_OK) {
            snprintf(error, error_size, "Genius fetch failed: %s", curl_easy_strerror(status));
            goto done;
        }
        long http = 0;
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http);
        if (http >= 300 && http < 400) {
            char *target = NULL;
            curl_easy_getinfo(curl, CURLINFO_REDIRECT_URL, &target);
            char *next = target ? strdup(target) : NULL;
            if (!next) { snprintf(error, error_size, "Invalid Genius redirect"); goto done; }
            free(current);
            current = next;
            continue;
        }
        if (http != 200) {
            snprintf(error, error_size, "Genius returned HTTP %ld; lyrics were not changed", http);
            goto done;
        }
        result = genius_extract(page.data, page.length, lyrics, error, error_size);
        goto done;
    }
    snprintf(error, error_size, "Too many Genius redirects");
done:
    if (cancelled && *cancelled) {
        free(*lyrics);
        *lyrics = NULL;
        snprintf(error, error_size, "Lyrics fetch cancelled");
        result = -1;
    }
    free(current); free(page.data);
    curl_easy_cleanup(curl);
    curl_global_cleanup();
    return(result);
}
