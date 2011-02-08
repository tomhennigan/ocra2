
#include "libocra2.h"
#include <stdio.h>

int main(int argc, char * argv[])
{
	if(argc != 2) {
		printf("%s file\n", argv[0]);
	}
	
	FILE * fp = fopen(argv[1], "r");
	if(fp == NULL) {
		return 1;
	}
	
	data d = getc(fp);
	while(!feof(fp)) {
		printf("[%02X] ", d);
		d = getc(fp);
	}
	fclose(fp);
	
	printf("\n");
}
