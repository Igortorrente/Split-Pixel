#include "InputHandler.h"

#include <getopt.h> // POSIX = Not compatible with windows :'(




int main(int argc, const char *argv[], char *env_var_ptr[]){

#ifdef DEBUG
	for (int i = 1; i < argc; ++i){
		printf("%d: %s\n", i, argv[i]);
	}
    for (int i = 0; *env_var_ptr != NULL; ++i) {
        printf ("\n%d: %s",i, *(env_var_ptr++));
    }
    printf("\n");
#endif
    char* imagesInputPlace[3];
    imageHandler images[3];

    int option = 0;
	while ((option = getopt(argc, (char *const *)argv, "m:r:i:o:hv")) != -1) {
        switch(option) {
            case 'i':
#ifdef DEBUG
                printf("Input option: \t%s\n", optarg);
                printf("\t\t\t\t%s\n",argv[optind]);
                printf("\t\t\t\t%s\n",argv[optind+1]);
#endif
                for (int i = -1; i < 2; ++i) {
                    imagesInputPlace[i+1] = (char *) argv[optind + i];
                }
                images[0].place = imagesInputPlace[0];
                break;
            case 'o':
#ifdef DEBUG
                printf("Output option: \t%s\n", optarg);
                printf("\t\t\t\t%s\n",argv[optind]);
#endif
                for (int i = -1; i < 1; i++) {
                    images[i+2].place = (char *) argv[optind+i];
                }
                break;
            case 'h':
                printf("Message of help here\n");
                break;
            case 'v':
                printf("This suppose to be a verbose mode\n");
                break;
            case 'r':
                printf("This supposed to be a resize option\n");
            case 'm':
                printf("Mode select: crypt and decrypt\n");
                break;
            case '?':
                return 1;
        }
    }

    openImages(images, imagesInputPlace);

	return 0;
}
