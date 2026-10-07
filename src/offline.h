#ifndef SJB_OFFLINE_H
#define SJB_OFFLINE_H

#include "database.h"

typedef struct OfflineDownload OfflineDownload;
OfflineDownload *offline_download_start(Database *db, const char *root, int64_t song_id, char *message, size_t size);
int offline_download_poll(OfflineDownload *download, char *message, size_t size);
void offline_download_destroy(OfflineDownload *download);

char *offline_default_root(void);
int offline_open(Database *db, const char *root);
int offline_download(Database *db, const char *root, int64_t song_id);

void offline_downloads_refresh(void);
void offline_downloads_clear(void);
int offline_is_downloaded(const char *uri);

#endif
