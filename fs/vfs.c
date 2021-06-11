#include "vfs.h"
#include "../debug/printf.h"
#include "../drivers/screen.h"
#include "../kernel/multiboot.h"
#include <errno.h>
#include <stdlib.h>
#include <string.h>

void *initrd_begin;
char *fpcontent;
int gotrun = 0;
int nrun = 69;
int runs = 0;

int __SEEK_SET = 0;
int __SEEK_CUR = 0;
int __SEEK_END = 0;

static void fls(char *path, void *base, saf_node_hdr_t *node) {
    // Check magic number
    if (node->magic != 0x766863726C706D73) {
        printf("Invalid magic number: 0x%llX\n", node->magic);
        kprint("Archive is damaged or of invalid format\n");
        return;
    }

    char *fullpath = (char *)malloc(1024);
    if (nrun == 0) {
        sprintf(fullpath, "%s/%s", path, node->name);
    } else if (nrun == 1) {
        sprintf(fullpath, "%s%s", path, node->name);
    } else {
        sprintf(fullpath, "%s", path);
    }

    printf("Found file %s\n", fullpath);

    if (node->flags == (1 << 0)) {
        saf_node_folder_t *fldr = (saf_node_folder_t *)node;
        kprint("\t(directory)\n");
        for (size_t i = 0; i < fldr->num_children; i++) {
            if (runs != 2) {
                nrun = 1;
                runs++;
            } else if (runs == 2 && nrun != 0) {
                nrun = 0;
            }

            fls(fullpath, base,
                (saf_node_hdr_t *)((uint8_t *)base + fldr->children[i]));
        }
    } else {
        saf_node_file_t *file = (saf_node_file_t *)node;
        printf("\tcontents: \"%.*s\"\n", (int)file->size,
               (char *)base + file->addr);
    }
}

void ls() {
    fls("/", initrd_begin, (saf_node_hdr_t *)initrd_begin);
    nrun = 69;
    runs = 0;
}

static void findfilenode(const char *tofind, char *path, void *base,
                         saf_node_hdr_t *node) {
    // Check magic number
    if (node->magic != 0x766863726C706D73) {
        printf("Invalid magic number: 0x%llX\n", node->magic);
        kprint("Archive is damaged or of invalid format\n");
        return;
    }

    char *fullpath = (char *)malloc(1024);
    if (nrun == 0) {
        sprintf(fullpath, "%s/%s", path, node->name);
    } else if (nrun == 1) {
        sprintf(fullpath, "%s%s", path, node->name);
    } else {
        sprintf(fullpath, "%s", path);
    }

    if (strcmp(tofind, fullpath) == 0) {
        gotrun = 1;
    }

    if (node->flags == (1 << 0)) {
        saf_node_folder_t *fldr = (saf_node_folder_t *)node;
        if (strcmp(tofind, fullpath) == 0) {
            // Magic string for directories
            strncpy(fpcontent, "__DIR__", 7);
        }

        for (size_t i = 0; i < fldr->num_children; i++) {
            if (runs != 2) {
                nrun = 1;
                runs++;
            } else if (runs == 2 && nrun != 0) {
                nrun = 0;
            }

            findfilenode(
              tofind, fullpath, base,
              (saf_node_hdr_t *)((uint8_t *)base + fldr->children[i]));
        }
    } else {
        saf_node_file_t *file = (saf_node_file_t *)node;
        if (strcmp(tofind, fullpath) == 0) {
            strncpy(fpcontent, (char *)base + file->addr, (uint32_t)file->size);
            fpcontent[file->size] = '\0';
            __SEEK_END = file->size;
        }
    }
}

char *findfile(const char *tofind) {
    findfilenode(tofind, "/", initrd_begin, (saf_node_hdr_t *)initrd_begin);
    nrun = 69;
    runs = 0;

    if (gotrun == 0) {
        errno = ENOENT;
        gotrun = 0;
        return NULL;
    } else {
        gotrun = 0;
        errno = 0;
        return fpcontent;
    }
}

void perror(const char *str) {
    if (str != NULL) {
        printf("%s: ", str);
    }

    printf("%s\n", strerror(errno));
}

FILE *fopen(const char *filename, const char *mode) {
    FILE *fp = (FILE *)malloc(sizeof(FILE));
    fp->magic = IO_MAGIC;
    switch (*mode++) {
        case 'r':
            fp->flags = 1;
            break;

        case 'w':
        case 'a':
            errno = ENOSYS;
            return NULL;

        default:
            errno = EINVAL;
            return NULL;
    }

    fp->ptr = SEEK_SET; // Begin of the file
    fp->error = 0;
    fp->eof = 0;
    strcpy(fp->name, filename);
    strcpy(fp->content, findfile(filename));
    strcpy(fp->nontent, findfile(filename));
    if (errno == ENOENT) {
        fclose(fp);
        return NULL;
    } else if (!memcmp(fp->content, "__DIR__", 7)) {
        errno = EISDIR;
        fclose(fp);
        return NULL;
    }

    return fp;
}

FILE *freopen(const char *filename, const char *mode, FILE *stream) {
    fclose(stream);
    return fopen(filename, mode);
}

int fseek(FILE *stream, long int offset, int whence) {
    if (stream->magic != IO_MAGIC) {
        stream->error = 1;
        errno = EBADF;
        return -1;
    }

    stream->ptr = whence + offset;
    __SEEK_CUR = stream->ptr;
    __SEEK_END = strlen(stream->nontent);
    stream->content = stream->nontent;
    if (stream->ptr >= SEEK_END) {
        stream->eof = EOF;
    } else {
        stream->eof = 0;
    }
    return 0;
}

long int ftell(FILE *stream) {
    if (stream->magic != IO_MAGIC) {
        stream->error = 1;
        errno = EBADF;
        return -1L;
    }

    return stream->ptr;
}

int feof(FILE *stream) {
    if (stream->magic != IO_MAGIC) {
        stream->error = 1;
        errno = EBADF;
        return -1;
    }

    return stream->eof;
}

size_t fread(void *ptr, size_t size, size_t nmemb, FILE *stream) {
    if (stream->magic != IO_MAGIC) {
        stream->error = 1;
        errno = EBADF;
        return -1;
    } else if (!ptr) {
        stream->error = 1;
        errno = EINVAL;
        return -1;
    }

    uint32_t i;
    uint32_t x;

    for (i = 0; i < nmemb; i++) {
        for (x = 0; x < size; x++) {
            // Such a large operation,
            // avoid "Wrong type argument to increment" warning
            *((*(char **)&ptr)++) = stream->content[stream->ptr];
            if (!feof(stream)) {
                fseek(stream, 1, stream->ptr);
            }
        }
    }

    return i;
}

int fgetpos(FILE *stream, fpos_t *pos) {
    if (stream->magic != IO_MAGIC) {
        stream->error = 1;
        errno = EBADF;
        return -1;
    }

    *pos = stream->ptr;
    return 0;
}

int fsetpos(FILE *stream, const fpos_t *pos) {
    if (stream->magic != IO_MAGIC) {
        stream->error = 1;
        errno = EBADF;
        return -1;
    }

    fseek(stream, *pos, SEEK_SET);
    return 0;
}

int fgetc(FILE *stream) {
    if (stream->magic != IO_MAGIC) {
        stream->error = 1;
        errno = EBADF;
        return -1;
    }

    char str[1];

    if (feof(stream)) {
        return EOF;
    }

    fread(str, 1, 1, stream);

    return (int)*str;
}

char *fgets(char *str, int n, FILE *stream) {
    int readb = 0;
    int ret;

    if (n <= 1) {
        errno = EINVAL;
        return (NULL);
    }

    while (readb < (n - 1)) {
        ret = fgetc(stream);
        if (ret == EOF) {
            if (readb == 0) {
                return (NULL);
            }

            break;
        } else if (ret < 0) {
            errno = EIO;
            return (NULL);
        } else if (ret == '\n') {
            str[readb++] = (char)ret;
            break;
        } else {
            str[readb++] = (char)ret;
        }
    }

    str[readb] = '\0';
    return str;
}

void rewind(FILE *stream) {
    if (stream->magic != IO_MAGIC) {
        stream->error = 1;
        return;
    }

    stream->error = 0;
    fseek(stream, 0, SEEK_SET);
}

int ferror(FILE *stream) {
    if (stream->magic != IO_MAGIC) {
        stream->error = 1;
        return -1;
    }

    return stream->error;
}

void clearerr(FILE *stream) {
    if (stream->magic != IO_MAGIC) {
        stream->error = 1;
        return;
    }

    stream->error = 0;
    stream->eof = 0;
}

int ungetc(int character, FILE *stream) {
    if (character == EOF) {
        stream->error = 1;
        return EOF;
    }

    stream->ptr = SEEK_SET + (SEEK_CUR - 1);
    __SEEK_CUR = stream->ptr;
    stream->eof = 0;

    stream->content[stream->ptr] = character;
    if ((unsigned)__SEEK_END < strlen(stream->content)) {
        // WHAT!? The file got somewhat bigger!
        __SEEK_END = strlen(stream->content);
    }

    return character;
}

int fclose(FILE *stream) {
    if (stream->magic != IO_MAGIC) {
        stream->error = 1;
        return -1;
    }

    free(stream);
    return 0;
}

void *vfs_init() {
    if (mbi->mods_count > 0) {
        extern uint32_t placement_address;
        multiboot_module_t *mod;

        mod = (multiboot_module_t *)mbi->mods_addr;
        initrd_begin = (void *)mod->mod_start;
        uint32_t initrd_end = *(uint32_t *)(mbi->mods_addr + 4);
        placement_address = initrd_end;

        return initrd_begin;
    } else {
        return NULL;
    }
}
