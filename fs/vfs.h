#ifndef VFS_H
#define VFS_H

#include "saf.h"
#include <stddef.h>

#define IO_MAGIC 0x69696969

typedef struct IO_FILE {
    char
      *nontent; // Should be file contents, but shouldn't be modified by ungetc
    char *content; // Should be file contents
    char *name;    // Should be file name
    int magic;     // Should be IO_MAGIC
    int flags;     // Should be file permissions
    int error;     // Should be set if we had a problem with the file
    int ptr;       // Should be current pointer
    int eof;       // Should be set if we already got to the end of file
} FILE;

typedef int fpos_t;

extern int __SEEK_SET;
extern int __SEEK_CUR;
extern int __SEEK_END;

#ifdef EOF
    #undef EOF
#endif

#define EOF -1

#ifdef SEEK_SET
    #undef SEEK_SET
#endif

#define SEEK_SET __SEEK_SET

#ifdef SEEK_CUR
    #undef SEEK_CUR
#endif

#define SEEK_CUR __SEEK_CUR

#ifdef SEEK_END
    #undef SEEK_END
#endif

#define SEEK_END __SEEK_END

#define FILENAME_MAX 256

#define getc fgetc

FILE *freopen(const char *filename, const char *mode, FILE *stream);
size_t fread(void *ptr, size_t size, size_t nmemb, FILE *stream);
int fseek(FILE *stream, long int offset, int whence);
FILE *fopen(const char *filename, const char *mode);
int fsetpos(FILE *stream, const fpos_t *pos);
char *fgets(char *str, int n, FILE *stream);
int ungetc(int character, FILE *stream);
int fgetpos(FILE *stream, fpos_t *pos);
void perror(const char *str);
long int ftell(FILE *stream);
void rewind(FILE *stream);
int fclose(FILE *stream);
int ferror(FILE *stream);
int fgetc(FILE *stream);
int feof(FILE *stream);
void *vfs_init();
void ls();

#endif
