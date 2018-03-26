#include <stdio.h>
#include <stdlib.h>
//TODO: Make this library work without -std=gnu
//#include <unistd.h> // POSIX = Not compatible with windows :'(
#include <getopt.h> // POSIX = Not compatible with windows :'(
#include "stb_image.h"


int main(int argc, char const *argv[], char *env_var_ptr[]){
	
	for (int i = 1; i < argc; ++i){
		printf("%d: %s\n", i, argv[i]);
	}

    int option = 0;
	while ((option = getopt(argc, argv, "i:o:hv")) != -1) {
        switch(option) {
            case 'i':
                printf("Input option: \t%s\n", optarg);
                printf("\t\t%s\n",argv[optind]);
                break;
            case 'o':
                printf("Output option: \t%s\n", optarg);
                break;
            case 'h':
                printf("Message of help here\n");
                break;
            case 'v':
                printf("This suppose to be a verbose mode\n");
                break;
            case '?':
                exit(0);
        }
    }

	return 0;
}
