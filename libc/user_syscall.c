#include "sys/defs.h"
#include "user_syscall.h"


inline uint64_t syscall_args_0(uint64_t syscall){
	uint64_t retVal;
	__asm__ __volatile__ ("int $0x80" : "=a" (retVal): "a" (syscall) : "memory");
	return retVal;
}
inline uint64_t syscall_args_1(uint64_t syscall, uint64_t arg_1){
	uint64_t retVal;
	__asm__ __volatile__ ("int $0x80" : "=a" (retVal) : "a" (syscall), "b" (arg_1) : "memory");
	return retVal;
}

inline uint64_t syscall_args_2(uint64_t syscall, uint64_t arg_1, uint64_t arg_2){
	uint64_t retVal;
	__asm__ __volatile__ ("int $0x80" : "=a" (retVal) : "a" (syscall), "b" (arg_1), "c" (arg_2) );
	return retVal;
}

inline uint64_t syscall_args_3(uint64_t syscall, uint64_t arg_1, uint64_t arg_2, uint64_t arg_3){
        uint64_t retVal;
        __asm__ __volatile__ ("int $0x80" : "=a" (retVal) : "a" (syscall), "b" (arg_1), "c" (arg_2), "d" (arg_3): "memory");
        return retVal;
}

inline uint64_t syscall_args_4(uint64_t syscall, uint64_t arg_1, uint64_t arg_2, uint64_t arg_3, uint64_t arg_4){
	uint64_t retVal;
	__asm__ __volatile__ ("int $0x80" : "=a" (retVal) : "a" (syscall), "b" (arg_1), "c" (arg_2), "d" (arg_3), "S" (arg_4) : "memory");
	return retVal;
}

inline uint64_t syscall_args_5(uint64_t syscall, uint64_t arg_1, uint64_t arg_2, uint64_t arg_3, uint64_t arg_4, uint64_t arg_5){
	uint64_t retVal;
	__asm__ __volatile__ ("int $0x80" : "=a" (retVal) : "a" (syscall), "b" (arg_1), "c" (arg_2), "d" (arg_3), "S" (arg_4), "D" (arg_5) : "memory");
	return retVal;
}

/*inline uint64_t _execve_ (uint64_t syscall, char* fd, char** str, char** count) {
     uint64_t ret;
    __asm__ __volatile__ ("syscall": "=a" (ret): "a" (syscall),
                                "D" (fd), "S" (str), "b" (count): "memory","cc");
     return ret;
  }*/
