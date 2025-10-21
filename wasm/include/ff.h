#ifndef FF_H
#define FF_H

#include <stdint.h>
#include <stddef.h>

#define FA_READ             0x01
#define FA_WRITE            0x02
#define FA_OPEN_EXISTING    0x00
#define FA_CREATE_NEW       0x04
#define FA_OPEN_ALWAYS      0x08

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    void *handle;
    uint32_t fptr;
    uint32_t fsize;
    unsigned char path[256];
} FIL;

typedef enum {
    FR_OK = 0,
    FR_DISK_ERR,
    FR_INT_ERR,
    FR_NOT_READY,
    FR_EXIST,
    FR_DENIED,
    FR_INVALID_OBJECT,
    FR_NO_FILE,
    FR_NO_PATH
} FRESULT;

int ff_init(void);

typedef struct {
    void *entries;
    size_t count;
    size_t index;
    unsigned char path[256];
} FATFS_DIR;

typedef struct {
    unsigned char fname[256];
    unsigned long fsize;
} FILINFO;

FRESULT f_open(FIL* fp, const unsigned char* path, uint8_t mode);
FRESULT f_close(FIL* fp);
FRESULT f_read(FIL* fp, void* buff, unsigned int btr, unsigned int* br);
FRESULT f_write(FIL* fp, const void* buff, unsigned int btw, unsigned int* bw);
FRESULT f_lseek(FIL* fp, unsigned long ofs);
FRESULT f_chdir(const unsigned char* path);
FRESULT f_mkdir(const unsigned char* path);
FRESULT f_unlink(const unsigned char* path);
FRESULT f_opendir(FATFS_DIR* dp, const unsigned char* path);
FRESULT f_readdir(FATFS_DIR* dp, FILINFO* fno);
FRESULT f_closedir(FATFS_DIR* dp);

#ifdef __cplusplus
}
#endif

#endif
