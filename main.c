#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image.h"
#include "stb_image_write.h"
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

enum { original = 0, share1, share2 };

typedef struct imageHandler{
    char* currentplace,* newplace ;
    unsigned char * pixelPointer;
    int width, height, channels;
}imageHandler;

#define getPointerChannel(image, x, y, channel) \
    (image.pixelPointer + y*image.width*image.channels + x*image.channels + channel)

#define getpixelchannel(image, x, y, channel) \
    (*(getPointerChannel(image, x, y, channel)))

#define getPixel(image, x, y) \
    ((((int)getpixelchannel(image, x, y, 0)) << 24) \
    |(((int)getpixelchannel(image, x, y, 1)) << 16) \
    |(((int)getpixelchannel(image, x, y, 2)) << 8))

#define getPixelRGBA(image, x, y) \
    ((((int)getpixelchannel(image, x, y, 0)) << 24) \
    |(((int)getpixelchannel(image, x, y, 1)) << 16) \
    |(((int)getpixelchannel(image, x, y, 2)) << 8) \
    |(((int)getpixelchannel(image, x, y, 3))))

//TODO: Check if this name make any sense
#define lerp(images, i, j, k) \
    (lround(99.0 / 255.0 * getpixelchannel(images, i, j, k)))

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
    srand (time(NULL));


//#TODO: Fix aguments not red because of other without value "-m -i ..."
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
#ifdef DEBUG
                    printf("image[%d].currentplace = %s\n", i+1, images[i+1].currentplace);
#endif
                }
                printf("\n");
                images[original].newplace = images[original].currentplace;
                break;
            case 'o':
#ifdef DEBUG
                printf("Output option: \t%s\n", optarg);
                printf("\t\t\t\t%s\n",argv[optind]);
#endif
                for (int i = -1; i < 1; i++) {
                    images[i+2].newplace = (char *) argv[optind+i];
#ifdef DEBUG
                    printf("image[%d].newplace = %s\n", i+2, images[i+2].newplace);
#endif
                }
                printf("\n");
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

    for (int i = 0; i < 3; i++) {
        images[i].pixelPointer = stbi_load(images[i].currentplace, &images[i].width
                , &images[i].height, &images[i].channels, STBI_default);
#ifdef DEBUG
        printf("Image [%d] - height: %d  width:%d channels:%d\nFrom:%s\nTo:%s\n\n", i, images[i].height
                , images[i].width, images[i].channels, images[i].currentplace, images[i].newplace);
#endif
    }
    /*
    int height = images[1].height, width = images[1].width, channels = images[1].channels;
    printf("\nFrom:%s\n\n", images[1].currentplace);

    printf("(%08x)\n\n", getPixel(images[1], 599, 599));
    printf("image[o] nb channels:%d\n%s\n", images[0].channels, images[0].currentplace);
    printf("(%08x)\n\n", getPixelRGBA(images[0], 0 , 2));
    */
    /*
    printf("Pixel 1x1 Before process:\nfrom %s\n\n", images[share1].newplace);
    for (int i = 0; i < 4; ++i) {
        printf("%d ",*getPointerChannel(images[share1], 247, 128,i));
    }
    printf("\n\n");
     */


    // TODO: Change all these delimiters
    // images[original].height
    // images[original].width
    for (int i = 0; i < images[original].height; i++) {
        for (int j = 0; j < images[original].width; j++) {
            for (int k = 0; k < 3; k++) {

                // TODO: RENAME THIS VARIABLE
                unsigned char renameMe = (unsigned char)lerp(images[original], j, i, k);
                unsigned char originalTen = (unsigned char)(renameMe / 10);
                unsigned char originalUnity = (unsigned char)(renameMe % 10);
                const unsigned char randTen = randomUnity;
                const unsigned char randUnity = randomUnity;

                //printf("Original channel: %d => %d\n", getpixelchannel(images[original], i, j, k), renameMe);
                //printf("Share 1 Before: %d ", getpixelchannel(images[share1], i, j, k));

                unsigned char* sharePixel = getPointerChannel(images[share1], j, i, k);
                unsigned char sharePixelHundred = *sharePixel;
                unsigned char sharePixelTen = (unsigned char) (10 * ((randTen + originalTen) % 10));
                
                if (sharePixelHundred >= 200 && (sharePixelTen + randUnity >= 55)){
                    sharePixelHundred = 100;
                } else if(sharePixelHundred >= 200){
                    sharePixelHundred = 200;
                } else if(sharePixelHundred >= 100){
                    sharePixelHundred = 100;
                } else {
                    sharePixelHundred = 0;
                }

                *sharePixel = (sharePixelHundred + sharePixelTen + randUnity);

                //printf("After: %d\n", getpixelchannel(images[share1], i, j, k));
                //printf("Share 2 Before: %d ", getpixelchannel(images[share2], i, j, k));

                sharePixel = getPointerChannel(images[share2], j, i, k);
                sharePixelHundred = *sharePixel;
                sharePixelTen = (unsigned char) (10 * ((randUnity + originalUnity) % 10));

                if(sharePixelHundred >= 200 && (sharePixelTen + randTen >= 55)){
                    sharePixelHundred = 100;
                } else if(sharePixelHundred >= 200){
                    sharePixelHundred = 200;
                } else if(sharePixelHundred >= 100){
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

    /*
    printf("Pixel 1x1 Before process:\nfrom %s\n\n", images[share1].newplace);
    for (int i = 0; i < 4; ++i) {
        printf("%d ",*getPointerChannel(images[share1], 247, 128,i));
    }
    printf("\n\n");
     */

    printf("image 0: %d channels\n", images[original].channels);
    printf("image 1: %d channels\n", images[share1].channels);
    stbi_write_bmp(images[share1].newplace, images[share1].width, images[share1].height,
                   images[share1].channels, images[share1].pixelPointer);
                   //images[share1].width * images[share1].channels);
    printf("image 2: %d channels\n", images[share2].channels);
    stbi_write_bmp(images[share2].newplace, images[share2].width, images[share2].height,
                   images[share2].channels, images[share2].pixelPointer);
                   //images[share2].width * images[share2].channels);


    /*
    printf("Share 1:\n");
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 5; ++j) {
            printf("%08x ", getPixel(images[share1],i,j));
        }
        printf("\n");
    }

    printf("\nShare 2:\n");
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 5; ++j) {
            printf("%08x ", getPixel(images[share2],i,j));
        }
        printf("\n");
    }
    */

	return 0;
}
