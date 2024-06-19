#include <iostream>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cerrno>
#include <cstdarg>
#include <cstdint>
#include <sqlite3.h>
#include <fuse.h>

#include "filter.h" // Assuming this is your custom header

#define NOTES_EPOCH 978307200

sqlite3 *ppDb;
FILE *lf;

void debug(const char *fmt, ...) {
    if (lf == nullptr)
        return;

    va_list args;
    va_start(args, fmt);
    vfprintf(lf, fmt, args);
    va_end(args);

    fflush(lf);
}

static int notesfs_unlink(const char *path) {
    debug(":: UNLINK[path=%s]\n", path);

    if (path[1] == '0') {
        return -ENOTSUP;
    }

    int r;
    sqlite3_stmt *ppStmt = nullptr;
    const char *pzTail = nullptr;

    sqlite3_prepare(ppDb, "DELETE FROM note_bodies WHERE note_id IN (SELECT ROWID FROM note WHERE note.title = ?);", -1, &ppStmt, &pzTail);
    sqlite3_bind_text(ppStmt, 1, &path[1], -1, SQLITE_TRANSIENT);

    r = sqlite3_step(ppStmt);
    if (r != SQLITE_DONE) {
        sqlite3_finalize(ppStmt);
        debug("---- Couldn't DELETE FROM note_bodies WHERE note_id IN (SELECT ROWID FROM note WHERE note.title = '%s'). Returning -EINVAL\n", &path[1]);
        return -EINVAL;
    }

    debug("---- Successfully DELETED FROM note_bodies\n");
    sqlite3_finalize(ppStmt);

    sqlite3_prepare(ppDb, "DELETE FROM note WHERE note.title = ?;", -1, &ppStmt, &pzTail);
    sqlite3_bind_text(ppStmt, 1, &path[1], -1, SQLITE_TRANSIENT);

    r = sqlite3_step(ppStmt);
    if (r != SQLITE_DONE) {
        sqlite3_finalize(ppStmt);
        debug("---- Couldn't DELETE FROM note WHERE note.title = '%s'. Returning -EINVAL\n", &path[1]);
        return -EINVAL;
    }

    debug("---- Successfully DELETED FROM note\n");
    sqlite3_finalize(ppStmt);

    return 0;
}

static int notesfs_getxattr(const char *path, const char *buf, char *fi, size_t size) {
    debug(":! GETXATTR[path=%s]\n", path);

    debug("---- Operation not supported by me.\n");
    return -ENOTSUP;
}

static int notesfs_flush(const char *path, struct fuse_file_info *fi) {
    debug(":0 FLUSH[path=%s]\n", path);
    debug("---- file_info=[flags:%d,writepage=%d,direct_io=%d,keep_cache=%d,flush=%d,nonseekable=%d,fh=%" PRIu64 ",lock_owner=%" PRIu64 "]\n",
          fi->flags, fi->writepage, fi->direct_io, fi->keep_cache, fi->flush, fi->nonseekable, fi->fh, fi->lock_owner);
    return 0;
}

static int notesfs_truncate(const char *path, off_t o) {
    debug(":: TRUNCATE[path=%s]\n", path);

    if (path[1] != '0') {
        int r;
        sqlite3_stmt *ppStmt = nullptr;
        const char *pzTail = nullptr;

        sqlite3_prepare(ppDb, "UPDATE note_bodies SET data = ? WHERE note_id IN (SELECT ROWID FROM note WHERE note.title = ?);", -1, &ppStmt, &pzTail);
        sqlite3_bind_text(ppStmt, 1, &path[1], -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(ppStmt, 2, &path[1], -1, SQLITE_TRANSIENT);

        r = sqlite3_step(ppStmt);
        if (r != SQLITE_DONE) {
            sqlite3_finalize(ppStmt);
            return -EINVAL;
        }

        return 0;
    } else {
        return -ENOTSUP;
    }
}

static int notesfs_ftruncate(const char * path, off_t o, struct fuse_file_info *fi) {
    debug(":0 FTRUNCATE[path=%s]\n", path);
    debug("---- file_info=[flags:%d,writepage=%d,direct_io=%d,keep_cache=%d,flush=%d,nonseekable=%d,fh=%" PRIu64 ",lock_owner=%" PRIu64 "]\n",
          fi->flags, fi->writepage, fi->direct_io, fi->keep_cache, fi->flush, fi->nonseekable, fi->fh, fi->lock_owner);
    return 0;
}

static int notesfs_utime(const char *path, struct utimbuf *u) {
    debug(":0 UTIME[path=%s]\n", path);
    return 0;
}

static int notesfs_utimens(const char * path, const struct timespec tv[2]) {
    debug(":0 UTIMENS[path=%s]\n", path);
    return 0;
}

static int notesfs_chown(const char *path, uid_t u, gid_t g) {
    debug(":0 CHOWN[path=%s]\n", path);
    return 0;
}

static int notesfs_chmod(const char *path, mode_t m) {
    debug(":0 CHMOD[path=%s]\n", path);
    return 0;
}

static int notesfs_getattr(const char *path, struct stat *stbuf) {
    debug(":: GETATTR[path=%s]\n", path);

    int res = 0;

    memset(stbuf, 0, sizeof(struct stat));
    if (strcmp(path, "/") == 0) {
        stbuf->st_mode = S_IFDIR | 0755;
        stbuf->st_nlink = 2;
    } else if (path[1] != '0') {
        stbuf->st_mode = S_IFREG | 0666;
        stbuf->st_nlink = 1;

        sqlite3_stmt *ppStmt = nullptr;
        const char *pzTail;
        const char *sql = "SELECT substr(replace(replace(replace(replace(data,'<div>','\n'),'</div>',''),'<br>',''),'&lt;','<'),?) FROM note_bodies WHERE note_id IN (SELECT ROWID FROM note WHERE note.title = ?);";
        sqlite3_prepare(ppDb, sql, -1, &ppStmt, &pzTail);
        sqlite3_bind_int(ppStmt, 1, strlen(path) + 1);
        sqlite3_bind_text(ppStmt, 2, &path[1], -1, SQLITE_TRANSIENT);

        int r = sqlite3_step(ppStmt);
        if (r == SQLITE_ROW) {
            int v = sqlite3_column_bytes(ppStmt, 0);
            stbuf->st_size = v;
        }

        sqlite3_finalize(ppStmt);

        sqlite3_prepare(ppDb, "SELECT creation_date, modification_date FROM note WHERE title == ?;", -1, &ppStmt, &pzTail);
        sqlite3_bind_text(ppStmt, 1, &path[1], -1, SQLITE_TRANSIENT);

        r = sqlite3_step(ppStmt);
        if (r != SQLITE_ROW) {
            sqlite3_finalize(ppStmt);
            return -ENOENT;
        }

        stbuf->st_ctime = sqlite3_column_int(ppStmt, 0) + NOTES_EPOCH;
        stbuf->st_mtime = sqlite3_column_int(ppStmt, 1) + NOTES_EPOCH;

        sqlite3_finalize(ppStmt);
    } else if (path[1] == '0') {
        stbuf->st_mode = S_IFREG | 0444;
        stbuf->st_nlink = 1;

        char s[5];
        s[0] = path[2];
        s[1] = path[3];
        s[2] = path[4];
        s[3] = path[5];

        sqlite3_stmt *ppStmt = nullptr;
        const char *pzTail;
        sqlite3_prepare(ppDb, "SELECT replace(replace(replace(replace(data,'<div>','\n'),'</div>',''),'<br>',''),'&lt;','<') FROM note_bodies WHERE note_id == ?;", -1, &ppStmt, &pzTail);
        sqlite3_bind_int(ppStmt, 1, atoi(s));

        int r = sqlite3_step(ppStmt);
        if (r == SQLITE_ROW) {
            int v = sqlite3_column_bytes(ppStmt, 0);
            stbuf->st_size = v;
        }

        sqlite3_finalize(ppStmt);

        sqlite3_prepare(ppDb, "SELECT creation_date, modification_date FROM note WHERE ROWID == ?;", -1, &ppStmt, &pzTail);
        sqlite3_bind_int(ppStmt, 1, atoi(s));

        r = sqlite3_step(ppStmt);
        if (r != SQLITE_ROW) {
            sqlite3_finalize(ppStmt);
            return -ENOENT;
        }

        stbuf->st_ctime = sqlite3_column_int(ppStmt, 0) + NOTES_EPOCH;
        stbuf->st_mtime = sqlite3_column_int(ppStmt, 1) + NOTES_EPOCH;

        sqlite3_finalize(ppStmt);
    } else {
        res = -ENOENT;
    }

    return res;
}

static int notesfs_readlink(const char * path, char *buf, size_t s) {
    debug(":! READLINK[path=%s]\n", path);
    return -ENOTSUP;
}

static int notesfs_opendir(const char * path, struct fuse_file_info *fi) {
    debug(":! OPENDIR[path=%s]\n", path);
    return 0;
}

static int notesfs_releasedir(const char * path, struct fuse_file_info *fi) {
    debug(":! RELEASEDIR[path=%s]\n", path);
    return 0;
}

static int notesfs_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi) {
    debug(":! READ[path=%s]\n", path);
    return -ENOTSUP;
}

static int notesfs_readdir(const char * path, void *buf, fuse_fill_dir_t filler, off_t o, struct fuse_file_info *fi) {
    debug(":! READDIR[path=%s]\n", path);
    return -ENOTSUP;
}

static int notesfs_release(const char * path, struct fuse_file_info *fi) {
    debug(":! RELEASE[path=%s]\n", path);
    return 0;
}

static int notesfs_create(const char *path, mode_t mode, struct fuse_file_info *fi) {
    debug(":: CREATE[path=%s]\n", path);

    int r;
    sqlite3_stmt *ppStmt = nullptr;
    const char *pzTail = nullptr;

    sqlite3_prepare(ppDb, "INSERT INTO note (creation_date, modification_date, title) VALUES (?, ?, ?);", -1, &ppStmt, &pzTail);
    sqlite3_bind_int(ppStmt, 1, time(NULL) - NOTES_EPOCH);
    sqlite3_bind_int(ppStmt, 2, time(NULL) - NOTES_EPOCH);
    sqlite3_bind_text(ppStmt, 3, &path[1], -1, SQLITE_TRANSIENT);

    r = sqlite3_step(ppStmt);
    if (r != SQLITE_DONE) {
        sqlite3_finalize(ppStmt);
        return -EINVAL;
    }

    sqlite3_finalize(ppStmt);

    return 0;
}

static int notesfs_open(const char *path, struct fuse_file_info *fi) {
    debug(":: OPEN[path=%s]\n", path);

    return 0;
}

static int notesfs_read_dir(struct fuse_req *req, fuse_ino_t ino, size_t size, off_t off, struct fuse_file_info *fi) {
    debug(":: READ_DIR[ino=%lu, size=%zu, off=%lld]\n", ino, size, off);
    return -ENOTSUP;
}

int main(int argc, char *argv[]) {
    lf = fopen("/tmp/notesfs.log", "w");
    if (lf == nullptr)
        return 1;

    sqlite3_open(":memory:", &ppDb);

    sqlite3_exec(ppDb, "CREATE TABLE note (creation_date INT, modification_date INT, title TEXT);", NULL, NULL, NULL);
    sqlite3_exec(ppDb, "CREATE TABLE note_bodies (data TEXT, note_id INT);", NULL, NULL, NULL);

    fuse_main(argc, argv, &notesfs_operations, NULL);

    sqlite3_close(ppDb);
    fclose(lf);

    return 0;
}
