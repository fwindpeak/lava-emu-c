#include "ff.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <emscripten/emscripten.h>

EM_JS(void, lava_fs_notify_write, (const char* path), {
  if (Module.onFileWritten) {
    Module.onFileWritten(UTF8ToString(path));
  }
});

int ff_init(void)
{
    return 1;
}

static void update_fil_size(FIL* fp)
{
    if (!fp || !fp->handle) return;
    long current = ftell((FILE*)fp->handle);
    fseek((FILE*)fp->handle, 0, SEEK_END);
    long size = ftell((FILE*)fp->handle);
    fseek((FILE*)fp->handle, current, SEEK_SET);
    if (size >= 0)
        fp->fsize = (uint32_t)size;
    fp->fptr = (uint32_t)current;
}

static const char* mode_to_string(uint8_t mode)
{
    if ((mode & FA_WRITE) && (mode & FA_READ))
    {
        if (mode & FA_CREATE_NEW) return "w+b";
        if (mode & FA_OPEN_ALWAYS) return "a+b";
        return "r+b";
    }
    if (mode & FA_WRITE)
    {
        if (mode & FA_CREATE_NEW) return "wb";
        if (mode & FA_OPEN_ALWAYS) return "ab";
        return "wb";
    }
    return "rb";
}

FRESULT f_open(FIL* fp, const char* path, uint8_t mode)
{
    if (!fp || !path) return FR_INVALID_OBJECT;

    const char* open_mode = mode_to_string(mode);
    FILE* handle = fopen(path, open_mode);

    if (!handle)
    {
        if (mode & (FA_OPEN_ALWAYS | FA_CREATE_NEW))
        {
            handle = fopen(path, "w+b");
            if (!handle) return FR_DISK_ERR;
        }
        else
        {
            return FR_NO_FILE;
        }
    }

    fp->handle = handle;
    strncpy(fp->path, path, sizeof(fp->path) - 1);
    fp->path[sizeof(fp->path) - 1] = '\0';
    update_fil_size(fp);
    return FR_OK;
}

FRESULT f_close(FIL* fp)
{
    if (!fp || !fp->handle) return FR_INVALID_OBJECT;
    fclose((FILE*)fp->handle);
    lava_fs_notify_write(fp->path);
    fp->handle = NULL;
    fp->fptr = fp->fsize = 0;
    return FR_OK;
}

FRESULT f_read(FIL* fp, void* buff, unsigned int btr, unsigned int* br)
{
    if (!fp || !fp->handle) return FR_INVALID_OBJECT;
    size_t read = fread(buff, 1, btr, (FILE*)fp->handle);
    if (br) *br = (unsigned int)read;
    update_fil_size(fp);
    return (read < btr && ferror((FILE*)fp->handle)) ? FR_DISK_ERR : FR_OK;
}

FRESULT f_write(FIL* fp, const void* buff, unsigned int btw, unsigned int* bw)
{
    if (!fp || !fp->handle) return FR_INVALID_OBJECT;
    size_t written = fwrite(buff, 1, btw, (FILE*)fp->handle);
    if (bw) *bw = (unsigned int)written;
    fflush((FILE*)fp->handle);
    update_fil_size(fp);
    if (written < btw)
        return FR_DISK_ERR;
    lava_fs_notify_write(fp->path);
    return FR_OK;
}

FRESULT f_lseek(FIL* fp, unsigned long ofs)
{
    if (!fp || !fp->handle) return FR_INVALID_OBJECT;
    if (fseek((FILE*)fp->handle, (long)ofs, SEEK_SET) != 0)
        return FR_DISK_ERR;
    update_fil_size(fp);
    return FR_OK;
}

FRESULT f_chdir(const char* path)
{
    if (!path) return FR_INVALID_OBJECT;
    return (chdir(path) == 0) ? FR_OK : FR_NO_PATH;
}

FRESULT f_mkdir(const char* path)
{
    if (!path) return FR_INVALID_OBJECT;
    return (mkdir(path, 0777) == 0) ? FR_OK : FR_DENIED;
}

FRESULT f_unlink(const char* path)
{
    if (!path) return FR_INVALID_OBJECT;
    return (remove(path) == 0) ? FR_OK : FR_NO_FILE;
}

static void free_dir_entries(FATFS_DIR* dp)
{
    return;
    if (!dp || !dp->entries) return;
    struct dirent** list = (struct dirent**)dp->entries;
    for (size_t i = dp->index; i < dp->count; ++i)
    {
        free(list[i]);
    }
    free(list);
    dp->entries = NULL;
    dp->count = dp->index = 0;
}

FRESULT f_opendir(FATFS_DIR* dp, const char* path)
{
    if (!dp) return FR_INVALID_OBJECT;
    free_dir_entries(dp);
    const char* target = (path && path[0]) ? path : ".";
    struct dirent** namelist = NULL;
    int n = scandir(target, &namelist, NULL, alphasort);
    if (n < 0)
        return FR_NO_PATH;
    dp->entries = namelist;
    dp->count = (size_t)n;
    dp->index = 0;
    strncpy(dp->path, target, sizeof(dp->path) - 1);
    dp->path[sizeof(dp->path) - 1] = '\0';
    return FR_OK;
}

FRESULT f_readdir(FATFS_DIR* dp, FILINFO* fno)
{
    if (!dp || !fno) return FR_INVALID_OBJECT;
    if (!dp->entries || dp->index >= dp->count)
    {
        fno->fname[0] = '\0';
        fno->fsize = 0;
        free_dir_entries(dp);
        return FR_OK;
    }
    struct dirent** list = (struct dirent**)dp->entries;
    struct dirent* entry = list[dp->index++];
    strncpy(fno->fname, entry->d_name, sizeof(fno->fname) - 1);
    fno->fname[sizeof(fno->fname) - 1] = '\0';

    struct stat st;
    char fullpath[512];
    snprintf(fullpath, sizeof(fullpath), "%s/%s", dp->path, entry->d_name);
    if (stat(fullpath, &st) == 0)
        fno->fsize = (unsigned long)st.st_size;
    else
        fno->fsize = 0;
    free(entry);
    if (dp->index == dp->count)
    {
        free(list);
        dp->entries = NULL;
        dp->count = dp->index = 0;
    }
    return FR_OK;
}

FRESULT f_closedir(FATFS_DIR* dp)
{
    free_dir_entries(dp);
    return FR_OK;
}
