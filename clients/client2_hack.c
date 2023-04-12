#include <stdio.h>
#include <sys/mman.h>

#include "client2_hack.h"

/**
 * @file hack.c
 */

int substitute_function(void *old, void *new)
{
    void *addr = (void*)((long)old & (~4095L));
    if (mprotect(addr, 4096, PROT_WRITE | PROT_READ | PROT_EXEC) == -1) {
	perror("mprotect:");
	return -1;
    }
    char instr[12] = {
	0x48, 0xb8, // movabs $-------, %rax
	0, 0, 0, 0, 0, 0, 0, 0,
	0xff, 0xe0  // jmpq *%rax  // LULZ
    };
    *(void**)(instr+2) = new;
    for (int i = 0; i < 12; i++) {
	((char*)old)[i] = instr[i];
    }
    return 0;
}
