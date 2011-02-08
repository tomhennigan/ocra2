
#include "libocra2.h"

ocra2 * ocra2_init()
{
	ocra2 * m = malloc(sizeof(ocra2));
	m->pc	= 0x00;
	m->a	= 0x00;
	m->x	= 0x00;
	m->i	= 0x00;
	
	m->mot = NULL;
	m->ocr = NULL;
	
	ocra2_clearmem(m);
	
	#if (OCRA2_DEBUG == 1)
	fprintf(stderr, "debug: ocra2_init() -> 0x%02X;\n", m);
	#endif
	
	return m;
}

void ocra2_free(ocra2 * m)
{
	#if (OCRA2_DEBUG == 1)
	fprintf(stderr, "debug: ocra2_free(0x%X);\n", m);
	#endif
	
	if(m->mot != NULL) { free(m->mot); }
	if(m->ocr != NULL) { free(m->ocr); }
	
	free(m);
}

void ocra2_clearmem(ocra2 * m)
{
	#if (OCRA2_DEBUG == 1)
	fprintf(stderr, "debug: ocra2_clearmem(0x%X);\n", m);
	#endif
	
	int i;
	for(i=0; i<0x100; i++) {
		m->mem[i] = (data)0x00;
	}
}

addr ocra2_dec_pc(ocra2 * m)
{
	#if (OCRA2_DEBUG == 1)
	fprintf(stderr, "debug: ocra2_dec_pc(0x%X) -> 0x%02X;\n", m, (m->pc-1));
	#endif
	
	return (--m->pc);
}

addr ocra2_incr_pc(ocra2 * m)
{
	#if (OCRA2_DEBUG == 1)
	fprintf(stderr, "debug: ocra2_incr_pc(0x%X) -> 0x%02X;\n", m, (m->pc+1));
	#endif
	
	return (++m->pc);
}

data ocra2_read(ocra2 * m, addr a)
{
	#if (OCRA2_DEBUG == 1)
	fprintf(stderr, "debug: ocra2_read(0x%X, 0x%02X) -> 0x%02X;\n", m, a, m->mem[a]);
	#endif
	return m->mem[a];
}

data ocra2_write(ocra2 * m, addr a, data d)
{
	#if (OCRA2_DEBUG == 1)
	fprintf(stderr, "debug: ocra2_write(0x%X, 0x%02X, 0x%02X) -> 0x%02X;\n", m, a, d, d);
	#endif
	
	return (m->mem[a] = d);
}

addr ocra2_load_program(ocra2 * m, char * file_name)
{
	#if (OCRA2_DEBUG == 1)
	fprintf(stderr, "debug: ocra2_load_program(0x%X, \"%s\");\n", m, file_name);
	#endif
	
	addr a = 0x00;
	
	FILE * fp = fopen(file_name, "r");
	if(fp == NULL) { return a; }
	
	data d = getc(fp);
	while(!feof(fp)) {
		ocra2_write(m, a++, d);
		
		if(a == 0x00) {
			a--;
			fprintf(stderr, "warning: ocra2_load_program: ran out of mem, before end of program file..\n");
			break;
		}
		
		d = getc(fp);
	}
	
	fclose(fp);
	
	return a;
}

inst ocra2_fetch(ocra2 * m)
{
	#if (OCRA2_DEBUG == 1)
	fprintf(stderr, "debug: ocra2_fetch(0x%X) -> 0x%02X;\n", m, m->mem[m->pc]);
	#endif
	
	inst r = (m->i = ocra2_read(m, m->pc));
	
	ocra2_incr_pc(m);
	
	return r;
}

/*
    void ocra2_execute(ocra2 * m);
    Takes the current program byte from the register stored in m, and executes the action associated with the opcode.
*/
void ocra2_execute(ocra2 * m)
{
	#if (OCRA2_DEBUG == 1)
	fprintf(stderr, "debug: ocra2_execute(0x%02X);\n", m);
	#endif
	
	// Used to store the payload.
	data p;
	
	// malloc the mot cmd string
	if(m->mot != NULL) { free(m->mot); }
	m->mot = malloc(sizeof(char) * 32);
	
	// malloc the ocr cmd string
	if(m->ocr != NULL) { free(m->ocr); }
	m->ocr = malloc(sizeof(char) * 32);
	
	switch(m->i) {
		case 0x3E: // A <- n
			p = ocra2_read(m, m->pc);
			sprintf(m->ocr, "A <- %02X", p);
			sprintf(m->mot, "LDA #$%02X", p);
			m->a = p;
			ocra2_incr_pc(m);
			break;
		case 0xC6: // A <- A + n
			p = ocra2_read(m, m->pc);
			sprintf(m->ocr, "A <- A + %02X", p);
			sprintf(m->mot, "ADA #$%02X", p);
			m->a = m->a + p;
			ocra2_incr_pc(m);
			break;
		case 0xD6: // A <- A - n
			p = ocra2_read(m, m->pc);
			sprintf(m->ocr, "A <- A - %02X", p);
			sprintf(m->mot, "DEC #$%02X", p);
			m->a = m->a - p;
			ocra2_incr_pc(m);
			break;
		case 0xE6: // A <- A & n
			p = ocra2_read(m, m->pc);
			sprintf(m->ocr, "A <- A AND %02X", p);
			sprintf(m->mot, "AND #$%02X", p);
			m->a = m->a & p;
			ocra2_incr_pc(m);
			break;
		case 0xEE: // A <- A eor n
			p = ocra2_read(m, m->pc);
			sprintf(m->ocr, "A <- A EOR %02X", p);
			sprintf(m->mot, "EOR #$%02X", p);
			m->a = m->a ^ p;
			ocra2_incr_pc(m);
			break;
		case 0x3A: // A <- M(n)
			p = ocra2_read(m, m->pc);
			sprintf(m->ocr, "A <- M(%02X)", p);
			sprintf(m->mot, "LDAA %02X", p);
			m->a = ocra2_read(m, p);
			ocra2_incr_pc(m);
			break;
		case 0x32: // M(n) <- A
			p = ocra2_read(m, m->pc);
			sprintf(m->ocr, "M(%02X) <- A", p);
			sprintf(m->mot, "STAA %02X", p);
			ocra2_write(m, p, m->a);
			ocra2_incr_pc(m);
			break;
		case 0x7E: // A <- M(X)
			sprintf(m->ocr, "A <- M(%02X)", m->x);
			sprintf(m->mot, "LDA %02X", m->x);
			m->a = ocra2_read(m, m->x);
			//ocra2_incr_pc(m);
			break;
		case 0x77: // M(X) <- A
			sprintf(m->ocr, "M(%02X) <- A", m->x);
			sprintf(m->mot, "STA %02X", m->x);
			ocra2_write(m, m->x, m->a);
			//ocra2_incr_pc(m);
			break;
		case 0x86: // A <- A + M(X)
			p = ocra2_read(m, m->x);
			sprintf(m->ocr, "A <- A + M(%02X)", p);
			sprintf(m->mot, "ADD %02X", p);
			m->a = m->a + p;
			//ocra2_incr_pc(m);
			break;
		case 0x96: // A <- A - M(X)
			p = ocra2_read(m, m->pc);
			sprintf(m->ocr, "A <- A - M(%02X)", p);
			sprintf(m->mot, "SUB %02X", p);
			m->a = m->a - p;
			//ocra2_incr_pc(m);
			break;
		case 0xA6: // A <- A & M(X)
			p = ocra2_read(m, m->pc);
			sprintf(m->ocr, "A <- A AND M(%02X)", p);
			sprintf(m->mot, "AND %02X", p);
			m->a = m->a & p;
			//ocra2_incr_pc(m);
			break;
		case 0xAE: // A <- A eor M(X)
			p = ocra2_read(m, m->pc);
			sprintf(m->ocr, "A <- A EOR M(%02X)", p);
			sprintf(m->mot, "EOR %02X", p);
			m->a = m->a ^ p;
			//ocra2_incr_pc(m);
			break;
		case 0x6F: // X <- A
			sprintf(m->ocr, "X <- A");
			sprintf(m->mot, "TAX");
			m->x = m->a;
			break;
		case 0x7D: // A <- X
			sprintf(m->ocr, "A <- X");
			sprintf(m->mot, "TXA");
			m->a = m->x;
			break;
		case 0xC9: // X <- X + 1
			sprintf(m->ocr, "X <- X + 1");
			sprintf(m->mot, "INX");
			m->x = m->x + 1;
			break;
		case 0xC3: // X <- X - 1
			p = ocra2_read(m, m->pc);
			sprintf(m->ocr, "PC <- %02X", p);
			sprintf(m->mot, "JMP %02X", p);
			m->pc = p;
			break;
		case 0xCA: // PC <- n IF A == 0
			p = ocra2_read(m, m->pc);
			sprintf(m->ocr, "PC <- %02X if A(%02X) == 0", p, m->a);
			sprintf(m->mot, "BEQ %02X", p);
			if(m->a == 0) { m->pc = p; } else { ocra2_incr_pc(m); }
			break;
		case 0xC2: // PC <- n IF A != 0
			p = ocra2_read(m, m->pc);
			sprintf(m->ocr, "PC <- %02X if A(%02X) != 0", p, m->a);
			sprintf(m->mot, "BNE %02X", p);
			if(m->a != 0) { m->pc = p; } else { ocra2_incr_pc(m); }
			break;
		case 0x00: // NOP
			sprintf(m->ocr, "No operation");
			sprintf(m->mot, "NOP");
			break;
		case 0x76: // Halt
			sprintf(m->ocr, "Halt");
			sprintf(m->mot, "SLEEP");
			ocra2_write(m, (m->pc-1), 0x00); // Write no op to ourself, so we quit.
			ocra2_dec_pc(m);
			break;
		default:
			sprintf(m->ocr, "Unknown OP Code: %02X", m->i);
			sprintf(m->mot, "Unknown OP Code: %02X", m->i);
			break;
	}
}

/*
    void ocra2_print_mmap(ocra2 * m);
    Prints the contents of the ocra2's mmap to the console.
*/
void ocra2_print_mmap(ocra2 * m)
{
    addr c = 0x00;
    
    int i;
    while(1) {
        printf("%02X:\t", c);
        for(i=0; i<15 && c!=0xff; i++) {
            printf(" %02X", ocra2_read(m, c));
            c++;
        }
        printf("\n");
        if(c == 0xff) {
            return;
        }
    }
}
