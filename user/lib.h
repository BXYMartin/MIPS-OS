#ifndef LIB_H
#define LIB_H
#include "fd.h"
#include "pmap.h"
#include <mmu.h>
#include <trap.h>
#include <env.h>
#include <args.h>
#include <kerelf.h>
#include <unistd.h>
/////////////////////////////////////////////////////head
extern void umain();
extern void libmain();
extern void exit();
extern void exit_all();

extern struct Env *env;
extern struct Env *envs;

#define USED(x) (void)(x)
//////////////////////////////////////////////////////printf
#include <stdarg.h>
//#define               LP_MAX_BUF      80

void user_lp_Print(void (*output)(void *, const char *, int),
                                   void *arg,
                                   const char *fmt,
                                   va_list ap);

void writef(char *fmt, ...);

void _user_panic(const char *, int, const char *, ...)
__attribute__((noreturn));

#define user_panic(...) _user_panic(__FILE__, __LINE__, __VA_ARGS__)


/////////////////////////////////////////////////////fork spawn
int spawn(char *prog, char **argv);
int spawnl(char *prot, char *args, ...);
int fork(void);

void user_bcopy(const void *src, void *dst, size_t len);
void user_bzero(void *v, u_int n);
//////////////////////////////////////////////////syscall_lib
extern int msyscall(int, int, int, int, int, int);

void syscall_putchar(char ch);
u_int syscall_getenvid(void);
void syscall_yield(void);
int syscall_env_destroy(u_int envid);
int syscall_set_pgfault_handler(u_int envid, void (*func)(void),
                                                                u_int xstacktop);
int syscall_mem_alloc(u_int envid, u_int va, u_int perm);
int syscall_mem_map(u_int srcid, u_int srcva, u_int dstid, u_int dstva,
                                        u_int perm);
int syscall_mem_unmap(u_int envid, u_int va);
int syscall_write_dev(u_int va, u_int dev, u_int offset);
int syscall_read_dev(u_int va, u_int dev, u_int offset);
inline static int syscall_env_alloc()
{
    return msyscall(SYS_env_alloc, 0, 0, 0, 0, 0);
}

int syscall_set_env_status(u_int envid, u_int status);
int syscall_set_trapframe(u_int envid, struct Trapframe *tf);
void syscall_panic(char *msg);
int syscall_ipc_can_send(u_int envid, u_int value, u_int srcva, u_int perm);
void syscall_ipc_recv(u_int dstva);
int syscall_cgetc();
void syscall_curpath(char *path, u_int op);
int syscall_perm(int set, int type, char *perm);
// string.c
int strlen(const char *s);
char *strcpy(char *dst, const char *src);
const char *strchr(const char *s, char c);
void *memcpy(void *destaddr, void const *srcaddr, u_int len);
int strcmp(const char *p, const char *q);
char* strcat(char* strDest , const char* strSrc);

// ipc.c
void    ipc_send(u_int whom, u_int val, u_int srcva, u_int perm);
u_int   ipc_recv(u_int *whom, u_int dstva, u_int *perm);

// wait.c
void wait(u_int envid);

// console.c
int opencons(void);
int iscons(int fdnum);

// pipe.c
int pipe(int pfd[2]);
int pipeisclosed(int fdnum);

// pageref.c
int     pageref(void *);

// pgfault.c
void set_pgfault_handler(void (*fn)(u_int va));

// fprintf.c
int fwritef(int fd, const char *fmt, ...);

// fsipc.c
int     fsipc_open(const char *, u_int, struct Fd *);
int     fsipc_map(u_int, u_int, u_int);
int     fsipc_set_size(u_int, u_int);
int     fsipc_close(u_int);
int     fsipc_dirty(u_int, u_int);
int     fsipc_remove(const char *);
int     fsipc_sync(void);
int     fsipc_incref(u_int);
int fsipc_ls(u_int, u_int);
int fsipc_create(const char *, u_int);

// fd.c
int     close(int fd);
int     read(int fd, void *buf, u_int nbytes);
int     write(int fd, const void *buf, u_int nbytes);
int     seek(int fd, u_int offset);
void    close_all(void);
int     readn(int fd, void *buf, u_int nbytes);
int     dup(int oldfd, int newfd);
int fstat(int fdnum, struct Stat *stat);
int     stat(const char *path, struct Stat *);

// file.c
int     open(const char *path, int mode);
int     read_map(int fd, u_int offset, void **blk);
int     delete(const char *path);
int     ftruncate(int fd, u_int size);
int     sync(void);
int list(int fdnum, int recursive);
int create(const char *path, int type);

#define user_assert(x)  \
        do {    if (!(x)) user_panic("assertion failed: %s", #x); } while (0)

/* File open modes */
#define O_RDONLY        0x0000          /* open for reading only */
#define O_WRONLY        0x0001          /* open for writing only */
#define O_RDWR          0x0002          /* open for reading and writing */
#define O_ACCMODE       0x0003          /* mask for above modes */

#define O_CREAT         0x0100          /* create if nonexistent */
#define O_TRUNC         0x0200          /* truncate to zero length */
#define O_EXCL          0x0400          /* error if already exists */
#define O_MKDIR         0x0800          /* create directory, not regular file */

/* Define color modes */
#define RED(str) "\033[0;32;31m" # str "\033[m"
#define LIGHT_RED(str) "\033[1;31m" # str "\033[m"
#define GREEN(str) "\033[0;32;32m" # str "\033[m"
#define LIGHT_GREEN(str) "\033[1;32m" # str "\033[m"
#define BLUE(str) "\033[0;32;34m" # str "\033[m"
#define LIGHT_BLUE(str) "\033[1;34m" # str "\033[m"
#define DARK_GRAY(str) "\033[1;30m" # str "\033[m"
#define CYAN(str) "\033[0;36m" # str "\033[m"
#define LIGHT_CYAN(str) "\033[1;36m" # str "\033[m"
#define PURPLE(str) "\033[0;35m" # str "\033[m"
#define LIGHT_PURPLE(str) "\033[1;35m" # str "\033[m"
#define BROWN(str) "\033[0;33m" # str "\033[m"
#define YELLOW(str) "\033[1;33m" # str "\033[m"
#define LIGHT_GRAY(str) "\033[0;37m" # str "\033[m"
#define WHITE(str) "\033[1;37m" # str "\033[m"

/* Define loading bar and elements */
#define BAR GREEN(█)
#define lBAR GREEN(▎)
#define leftup LIGHT_GRAY(■)
#define leftdown LIGHT_GRAY(■)
#define rightup LIGHT_GRAY(■)
#define rightdown LIGHT_GRAY(■)
#define vert LIGHT_GRAY(■)
#define hori LIGHT_GRAY(■)

/* Define permission bits for users */
#define READ    0
#define WRITE   1
#define EXE     2

#endif