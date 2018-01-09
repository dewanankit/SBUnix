#ifndef ___SYS_DEFS_H
#define ___SYS_DEFS_H

#define NULL ((void*)0)
#define PAGESIZE 0x1000
#define MAX_FILE 85

typedef unsigned long  uint64_t;
typedef          long   int64_t;
typedef unsigned int   uint32_t;
typedef          int    int32_t;
typedef unsigned short uint16_t;
typedef          short  int16_t;
typedef unsigned char   uint8_t;
typedef          char    int8_t;

typedef uint64_t size_t;
typedef int64_t ssize_t;

typedef uint64_t off_t;

typedef uint32_t pid_t;

#define BUFFER_SIZE	200
#define FILE_PERM	0x0777

#define STDIN		0
#define STDOUT		1
#define STDERR		2

#define FILE_ERROR			5555
#define FILE_ALREADY_OPEN	5556

#define	SYS_READ 		0
#define	SYS_WRITE 		1
#define	SYS_OPEN		2
#define	SYS_CLOSE		3
#define SYS_MALLOC		9
#define SYS_PIPE		22
#define SYS_YIELD		24
#define SYS_DUP2		33
#define SYS_KILL		62
#define SYS_SLEEP		35

#define SYS_GETPID		39

#define	SYS_FORK 		57
#define SYS_EXECV		59
#define	SYS_EXIT 		60

#define SYS_CAT			75

#define SYS_GETDENTS	78
#define SYS_CWD			79
#define SYS_CHDIR		80
#define SYS_READLINE	95
#define SYS_CLRSCR		99

#define SYS_GETPPID		110
#define SYS_PS			270
#define SYS_WAITID		247

#define FILE_NOT_READ 0
#define FILE_PARTIALLY_READ 1
#define FILE_READ_COMPLETELY 2


#endif
