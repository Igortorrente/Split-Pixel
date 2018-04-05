#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image.h"
#include "stb_image_write.h"
#include <unistd.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>

enum { share1 = 0, share2, original };
enum { undefined = 1,  decrypt, crypt };

typedef struct imageHandler{
    char* currentplace,* newplace ;
    unsigned char * pixelPointer;
    int width, height, channels;
}imageHandler;

#define getPointerChannel(image, x, y, channel) \
    (image.pixelPointer + x*image.width*image.channels + y*image.channels + channel)

#define getpixelchannel(image, x, y, channel) \
    (*(getPointerChannel(image, x, y, channel)))

//TODO: Check if this name make any sense
#define reduce(images, i, j, k) \
    (lround(99.0 / 255.0 * getpixelchannel(images, i, j, k)))

#define ten(number) \
    ((number) % 100)

// TODO: Change function rand to another one
#define randomUnity ((unsigned char) (rand() / (RAND_MAX / 10)))

int main(int argc, const char *argv[], char *env_var_ptr[]){

#ifdef DEBUG
	for (int i = 1; i < argc; ++i){
		printf("%d: %s\n", i, argv[i]);
	}
    for (int i = 0; *env_var_ptr != NULL; ++i) {
        printf ("\n%d: %s",i, *(env_var_ptr++));
    }
    printf("\n\n");
#endif
    imageHandler images[3];
    char mode = undefined;
    srand (time(NULL));


// TODO: Fix aguments not red because of other without value "-m -i ..."
    int option = 0;
	while ((option = getopt(argc, (char *const *)argv, "m:r:i:o:hv")) != -1) {
        switch(option) {
            case 'i':
#ifdef DEBUG
                printf("Input option: \t%s\n", optarg);
                printf("\t\t\t\t%s\n",argv[optind]);
                printf("\t\t\t\t%s\n",argv[optind+1]);
#endif
                for (int i = -1; i < 2; i++) {
                    images[i+1].currentplace = (char *) argv[optind + i];
                }
                break;
            case 'o':
#ifdef DEBUG
                printf("Output option: \t%s\n", optarg);
                printf("\t\t\t\t%s\n",argv[optind]);
#endif
                for (int i = -1; i < 1; i++) {
                    images[i+1].newplace = (char *) argv[optind + i];
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
                if (!strcmp("crypt", optarg))
                    mode = crypt;
                else if (!strcmp("decrypt", optarg))
                    mode = decrypt;
                else {
                    fprintf(stderr, "'%s' is not a valid mode\nTry crypt, decrypt", optarg);
                    return 1;
                }
                break;
            case '?':
                return 1;
        }
    }


    switch (mode){
        case decrypt:

            for (int i = 0; i < decrypt; i++) {
                images[i].pixelPointer = stbi_load(images[i].currentplace, &images[i].width,
                         &images[i].height, &images[i].channels, STBI_default);
            }

            images[original] = images[share1];
            images[share1].newplace = NULL;
            images[original].currentplace = NULL;

            images[original].pixelPointer = malloc(images[original].height *
                    images[original].width * (size_t) images[original].channels);

            // TODO: Change all these delimiters
            for (int i = 0; i < images[share1].height; ++i) {
                for (int j = 0; j < images[share1].width; ++j) {
                    for (int k = 0; k < 3; ++k) {

                        unsigned char* share1Pixel = getPointerChannel(images[share1], i, j, k);
                        unsigned char* share2Pixel = getPointerChannel(images[share2], i, j, k);
                        // TODO: Verify if this name have any sense
                        // TODO: Use define
                        char reducedTen = (char)(((ten(*share1Pixel)/10)) - ((ten(*share2Pixel)%10)));
                        reducedTen < 0 ? reducedTen = (char)10 + reducedTen : reducedTen;

                        char reducedUnity = (char)(((ten(*share2Pixel)/10)) - ((ten(*share1Pixel)%10)));
                        reducedUnity < 0 ? reducedUnity = (char)10 + reducedUnity : reducedUnity;

                        *getPointerChannel(images[original], i, j, k) =
                                (unsigned char) round(((double)255 * (10 * reducedTen + reducedUnity)) / 99);
                    }
                }
            }

            stbi_write_bmp(images[original].newplace, images[original].width, images[original].height,
                           images[original].channels, images[original].pixelPointer);

            break;

        case crypt:

            for (int i = 0; i < crypt; i++) {
                images[i].pixelPointer = stbi_load(images[i].currentplace, &images[i].width,
                          &images[i].height, &images[i].channels, STBI_default);
#ifdef DEBUG
                printf("Image [%d] - height: %d  width:%d channels:%d\nFrom:%s\nTo:%s\n\n", i, images[i].height
                , images[i].width, images[i].channels, in[i], out[i]);
#endif
            }

            // TODO: Change all these delimiters
            for (int i = 0; i < images[original].height; i++) {
                for (int j = 0; j < images[original].width; j++) {
                    for (int k = 0; k < 3; k++) {

                        // TODO: RENAME THIS VARIABLE
                        unsigned char renameMe = (unsigned char)reduce(images[original], i, j, k);
                        unsigned char originalTen = (unsigned char)(renameMe / 10);
                        unsigned char originalUnity = (unsigned char)(renameMe % 10);
                        const unsigned char randTen = randomUnity;
                        const unsigned char randUnity = randomUnity;

                        //printf("Original channel: %d => %d\n", getpixelchannel(images[original], i, j, k), renameMe);
                        //printf("Share 1 Before: %d ", getpixelchannel(images[share1], i, j, k));

                        unsigned char* sharePixel = getPointerChannel(images[share1], i, j, k);
                        unsigned char sharePixelHundred = *sharePixel;
                        unsigned char sharePixelTen = (unsigned char) (10 * ((randTen + originalTen) % 10));

                        if (sharePixelHundred >= 200 && (sharePixelTen + randUnity >= 55)){
                            sharePixelHundred = 100;
                        } else if(sharePixelHundred >= 200){
                            sharePixelHundred = 200;
                        } else if(sharePixelHundred >= 50){
                            sharePixelHundred = 100;
                        } else {
                            sharePixelHundred = 0;
                        }

                        *sharePixel = (sharePixelHundred + sharePixelTen + randUnity);

                        //printf("After: %d\n", getpixelchannel(images[share1], i, j, k));
                        //printf("Share 2 Before: %d ", getpixelchannel(images[share2], i, j, k));

                        sharePixel = getPointerChannel(images[share2], i, j, k);
                        sharePixelHundred = *sharePixel;
                        sharePixelTen = (unsigned char) (10 * ((randUnity + originalUnity) % 10));

                        if(sharePixelHundred >= 200 && (sharePixelTen + randTen >= 55)){
                            sharePixelHundred = 100;
                        } else if(sharePixelHundred >= 200){
                            sharePixelHundred = 200;
                        } else if(sharePixelHundred >= 50){
                            sharePixelHundred = 100;
                        } else {
                            sharePixelHundred = 0;
                        }

                        *sharePixel = (sharePixelHundred + sharePixelTen + randTen);

                        //printf("After: %d\n", getpixelchannel(images[share2], i, j, k));
                        //printf("randUnity: %d randTen: %d\n",randUnity, randTen);
                        //printf("DEBUG: %d\n", sharePixelTen + randTen);
                    }
                }
            }

            printf("image 0: %d channels\n", images[original].channels);
            printf("image 1: %d channels\n", images[share1].channels);
            stbi_write_bmp(images[share1].newplace, images[share1].width, images[share1].height,
                           images[share1].channels, images[share1].pixelPointer);
                           //images[share1].width * images[share1].channels);
            printf("image 2: %d channels\n", images[share2].channels);
            stbi_write_bmp(images[share2].newplace, images[share2].width, images[share2].height,
                           images[share2].channels, images[share2].pixelPointer);
                           //images[share2].width * images[share2].channels);

            break;
        case undefined:
            fprintf(stderr, "You need select de mode (crypt or decrypt)");
            return  1;
	}

	return 0;
}
