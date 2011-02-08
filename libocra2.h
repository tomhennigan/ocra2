
#ifndef __LIBOCRA2_H_INCLUDE
#define __LIBOCRA2_H_INCLUDE

#if (DEBUG == 1)
#	define OCRA2_DEBUG 1
#else
#	define OCRA2_DEBUG 0
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

typedef uint8_t addr;
typedef uint8_t data;
typedef uint8_t inst;

typedef struct _ocra2 {
	addr pc;			// Program counter
	data a;				// Accumulator
	addr x;				// Index register
	addr i;				// Instruction register
	inst mem[0x100];	// Memory map
	
	// Used to store the last command.
	char * mot;
	char * ocr;
} ocra2;

enum {
	print_ocr		= 1<<0,
	print_motorola	= 1<<1,
};

ocra2 * ocra2_init();
void ocra2_free();

void ocra2_clearmem(ocra2 *);

addr ocra2_incr_pc(ocra2 * m);
addr ocra2_dec_pc(ocra2 * m);

inst ocra2_fetch(ocra2 *);
void ocra2_execute(ocra2 *);

data ocra2_read(ocra2 *, addr);
data ocra2_write(ocra2 *, addr, data);

addr ocra2_load_program(ocra2 *, char *);
void ocra2_print_mmap(ocra2 * m);

#endif
