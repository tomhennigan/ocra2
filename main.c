
#include <stdio.h>
#include <libocra2.h>

int main(int argc, char * argv[])
{
	if(argc != 2) {
		fprintf(stderr, "Name:\tlibocra2\nTitle:\tlibocra2 microprocessor emulator library.\nAuthor:\ttomhennigan\nDesc:\tThis library emulates the functions found in the imaginary MPU as described by the OCR examination board for their A2 level electonics exam. This test program loads a test program from the first argument to the program, and run's it in the emulator.\n\nUsage:\t%s program_file\n", (char *)argv[0]);
		return 1;
	}
	
	ocra2 * m = ocra2_init();
	
	if(ocra2_load_program(m, argv[1]) == 0) {
		fprintf(stderr, "error: empty or non existant program file (%s)...\n", argv[1]);
		fprintf(stderr, "exiting...\n");
		return 1;
	}
	
	// This is the fetch execute cycle.
	while(ocra2_fetch(m) != (data)0x00) {
		ocra2_execute(m);
		printf("%s\n", m->ocr);
		/*printf("%s\n", m->mot);
		printf("\tpc:\t%02X\n", m->pc);
		printf("\t a:\t%02X\n", m->a);
		printf("\t x:\t%02X\n", m->x);
		printf("\t i:\t%02X\n", m->i);*/
	}
	
  ocra2_print_mmap(m);
	
	ocra2_free(m);
	
	return 0;
}

/*
on a 1.2GHz PPC:
	3591 commands were executed in 0.085 (real) seconds.
	~= 42.2 K commands per second
test prog (sequential delay loops):
[3E] [00] [C6] [01] [C2] [02] [3E] [00] [C6] [01] [C2] [08] [3E] [00] [C6] [01] [C2] [0E] [3E] [00] [C6] [01] [C2] [14] [3E] [00] [C6] [01] [C2] [1A] [3E] [00] [C6] [01] [C2] [20] [3E] [00] [C6] [01] [C2] [26]
*/
