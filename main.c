#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STB_IMAGE_RESIZE_IMPLEMENTATION

#include "stb/stb_image.h"
#include "stb/stb_image_write.h"
#include "stb/stb_image_resize.h"
#include "randombytes/randombytes.h"
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include <getopt.h>

enum{ share1 = 0, share2, original };
enum{ undefined = 1, decrypt, encrypt };
enum{ width = 0, height };

// Image struct
typedef struct imageHandler{
    char* currentplace, * newplace;
    unsigned char* pixelPointer;
    int width, height, channels;
}imageHandler;

// Get a address of especified channel of image
#define getPointerChannel(image, x, y, channel) \
    ((image).pixelPointer + (x)*(image).width*(image).channels + (y)*(image).channels + (channel))

// Get a value of especified channel of image
#define getpixelchannel(image, x, y, channel) \
    (*(getPointerChannel(image, x, y, channel)))

//TODO: Check if this name make any sense
// Map number in range 0-255 to 0-99
#define reduce(images, i, j, k) \
    (lround(99.0 / 255.0 * getpixelchannel(images, i, j, k)))

// Get ten and unity of a number
#define ten(number) \
    ((number) % 100)

// TODO: Change function rand to another one
// Gereate a random number between 0-9
#define randomUnity ((unsigned char) (rand() / (RAND_MAX / 10)))

#define WRITE_BMP(image) \
    (!stbi_write_bmp(image.newplace, image.width, image.height, \
                     image.channels, image.pixelPointer))

#define WRITE_TGA(image) \
    (!stbi_write_tga(image.newplace, image.width, image.height, \
                     image.channels, image.pixelPointer))

// TODO: Verify what is the best value
#define WRITE_JPEG(image) \
    (!stbi_write_jpg(image.newplace, image.width, image.height, \
                     image.channels, image.pixelPointer, 80))

#define WRITE_PNG(image) \
    (!stbi_write_jpg(image.newplace, image.width, image.height, \
                     image.channels, image.pixelPointer, \
                     image.width * image.channels))

#define STBI_RESIZE(image, newResolution, resizedImage) \
        stbir_resize_uint8(image.pixelPointer, image.width, image.height, 0 , \
        resizedImage, newResolution[width], newResolution[height], 0, image.channels)

#define IMAGE_MALLOC(image, newResolution) \
        malloc((size_t) (newResolution[width] * newResolution[height] * image.channels* sizeof(char)))

// Adjust of the tone of color of pixel
#define TONE_AJUST(sharePixelHundred, sharePixelTen, rand)\
        if ((sharePixelHundred) >= 200 && ((sharePixelTen) + (rand) >= 55)){ \
            (sharePixelHundred) = 100; \
        } else if ((sharePixelHundred) >= 200){ \
            (sharePixelHundred) = 200; \
        } else if ((sharePixelHundred) >= 50){ \
            (sharePixelHundred) = 100; \
        } else { \
             (sharePixelHundred) = 0; \
        } \

// Verify if the resolution of images are not the same
#define RESOLUTION_NOT_EQUAL(image1, image2) \
        images[image1].width != images[image2].width || \
        images[image1].height != images[image2].height

// Supported Formats
#define PNG  '\0gnp'
#define JPG  '\0gpj'
#define JPEG 'gepj'
#define BMP  '\0pmb'
#define TGA  '\0agt'

// Inicialization of long options of opt
#define LONG_OPTIONS \
    { \
        {"mode", required_argument, NULL, 'm'}, \
        {"help", no_argument, NULL, 'h'}, \
        {"input", required_argument, NULL, 'i'}, \
        {"output", required_argument, NULL, 'o'}, \
        {"resize", required_argument, NULL, 'r'}, \
        {"verbose", no_argument, NULL, 'v'}, \
        {0, 0, 0, 0} \
    }

#define HELP_MESSAGE \
"Split Pixel 0.1\n" \
"Program based on Leandro Ribeiro's algorithm which allows to hide one image " \
"into two other images and then retrieve the content.\n\n"\
"Usage:\tSplit-Pixel -i [input images] -o [output images] -m <Mode> [Options]\n" \
"\tFlags don't need be in this order.\n" \
"\tSupported formats: png, jpeg, bmp and tga.\n\n" \
"\tExample 1: ./Split-Pixel --mode encrypt --input example1.png example2.bmp Hidden.tga --output "\
"output1.jpg output2.jpg -r 1920 1080\n" \
"\tExample 2: ./Split-Pixel -m encrypt --input example1.bmp example2.bmp Hidden.tga -o "\
"output1.jpeg output2.bmp\n\n" \
"Mode:\n" \
"\tencrypt: This mode receive two cover images and one " \
"secret image which will be hidden.\n" \
"\tdecrypt: This mode receive two cover images which contains " \
"a secret image to be decrypted.\n\n" \
"\t-m,\t--mode {encrypt, decrypt}\n\n" \
"Inputs images:\n" \
"\t[encrypt Mode]:\t<Cover1> <Cover2> <Secret>\n" \
"\t[Decrypt Mode]:\t<Cover1> <Cover2>\n\n" \
"\t-i,\t--input\t [input images]\n\n" \
"Output images:\n" \
"\tHere you put the names of output image(s) with desired extension \n" \
"\t[encrypt Mode]:\t<Output1> <Output2>\n" \
"\t[Decrypt Mode]:\t<Output>\n\n" \
"\t-o,\t--output [output images]\n\n" \
"Options:\n" \
"\t-r,\t--resize <Width> <Height>\tResize image(s) of output (Only works with encrypt).\n" \
"\t-h,\t--help\t Show This Message.\n\n" \
"Project repository: https://github.com/Igortorrente/Split-Pixel\n" \
"Paper: <Leandro's Papers Here!!>\n"

// Function to save images in different formats
int writeImages(imageHandler* images, int position){
    int imageFormat = 0;
    const char* formatPointer = strrchr(images[position].newplace, '.');

    if (formatPointer != NULL){
        memcpy(&imageFormat, formatPointer + 1, 4 * sizeof(char));
    }

    switch (imageFormat){
        case PNG:
            return WRITE_PNG(images[position]);
        case JPEG:
        case JPG:
            return WRITE_JPEG(images[position]);
        case BMP:
            return WRITE_BMP(images[position]);
        case TGA:
            return WRITE_TGA(images[position]);
        default:
            if (imageFormat == 0){
                return -2;
            } else {
                return -1;
            }
    }
}

int saveImages(imageHandler* images, int position){
    switch (writeImages(images, position)){
        case -2:
            fprintf(stderr, "Error: Missing format of image %d\n", position + 1);
            return 1;
        case -1:
            fprintf(stderr, "Error: Format unsupported of image %d\n", position + 1);
            return 1;
        case 0:
            return 0;
        default:
            fprintf(stderr, "Error: Coudn't save image %d '%s' on disk\n",
                    position + 1, images[position].newplace);
            return 1;
    }
}

// Function to easy resize images
int resizeImages(imageHandler* images, int* newResolution, int begin, int end){
    for (int i = begin; i <= end; ++i){
        unsigned char* resizedImage = IMAGE_MALLOC(images[i], newResolution);
        if (resizedImage == NULL){
            return 1;
        }
        if(!STBI_RESIZE(images[i], newResolution, resizedImage)){
            return 1;
        }
        free(images[i].pixelPointer);
        images[i].pixelPointer = resizedImage;
        images[i].width = newResolution[width];
        images[i].height = newResolution[height];
    }
    return 0;
}

int main(const int argc, const char* argv[], const char* env_var_ptr[]){

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
    int newResolution[2] = {0, 0}; // inputCount = 0, outputCount = 0;
    char mode = undefined;
    srand(time(NULL));

    struct option longOptions[] = LONG_OPTIONS;

    int option = 0;
    while ((option = getopt_long(argc, (char* const*) argv, "m:r:i:o:hv", longOptions, NULL)) != -1){
        switch (option){
            case 'i':
                // Get input addresses
                for (int i = -1; i < 2; i++){
                    images[i + 1].currentplace = (char*) argv[optind + i];
                }
                break;
            case 'o':
                // Get output addresses
                for (int i = -1; i < 1; i++){
                    images[i + 1].newplace = (char*) argv[optind + i];
                }
                break;
            case 'h':
                printf(HELP_MESSAGE);
                return 0;
            case 'v':
                printf("This suppose to be a verbose mode, not implemented yet :(\n");
                break;
            case 'r':
                for (int i = -1; i < 1; i++){
                    newResolution[i + 1] = atoi(argv[optind + i]);
                    if (!newResolution[i + 1]){
                        fprintf(stderr, "Error:'%s' is not valid for resolution of image\n", argv[optind + i]);
                        return 1;
                    }
                }
                break;
            case 'm':
                if (!strcmp("encrypt", optarg)){
                    mode = encrypt;
                } else if (!strcmp("decrypt", optarg)){
                    mode = decrypt;
                } else {
                    fprintf(stderr, "Error:'%s' is not a valid mode\nTry encrypt, decrypt", optarg);
                    return 1;
                }
                break;
            case '?':
                fflush(stderr);
                printf("Try --help\n");
                return 1;
        }
    }

    switch (mode){
        case decrypt:

            // Load input images
            for (int i = 0; i < decrypt; i++){
                images[i].pixelPointer = stbi_load(images[i].currentplace, &images[i].width,
                                                   &images[i].height, &images[i].channels, STBI_default);

                if (images[i].pixelPointer == NULL){
                    fprintf(stderr, "Error: Can't open image from '%s'\n", images[i].currentplace);
                    return 1;
                }
            }

            // Correct some values and initialize output images
            images[original] = images[share1];
            images[share1].newplace = NULL;
            images[original].currentplace = NULL;

            // Allocate matrix of recovered image
            images[original].pixelPointer = malloc(images[original].height *
                                                   images[original].width * (size_t) images[original].channels);
            if (images[original].pixelPointer == NULL){
                fprintf(stderr, "Error: Couldn't allocate image matrix\n");
                return 1;
            }

            // TODO: Change all these delimiters
            // Decrypt for
            for (int i = 0; i < images[share1].height; ++i){
                for (int j = 0; j < images[share1].width; ++j){
                    for (int k = 0; k < 3; ++k){
                        // Get pointer to current channel
                        unsigned char* share1Pixel = getPointerChannel(images[share1], i, j, k);
                        unsigned char* share2Pixel = getPointerChannel(images[share2], i, j, k);
                        // TODO: Verify if this name of variable have any sense
                        // Calculates the reduced ten and unity of original(Hidden) image
                        char reducedTen = (char) (((ten(*share1Pixel) / 10)) - ((ten(*share2Pixel) % 10)));
                        reducedTen = reducedTen < 0 ? (char) 10 + reducedTen : reducedTen;

                        char reducedUnity = (char) (((ten(*share2Pixel) / 10)) - ((ten(*share1Pixel) % 10)));
                        reducedUnity =  reducedUnity < 0 ? (char) 10 + reducedUnity : reducedUnity;

                        // Writes in image a retrieved channel
                        *getPointerChannel(images[original], i, j, k) =
                                (unsigned char) round(((double) 255 * (10 * reducedTen + reducedUnity)) / 99);
                    }
                }
            }
            // Records on disk a retrieved image
            if(saveImages(images, original) == 1)
                return 1;

            break;
        case encrypt:

            // Load all images from disk (share1, share2 and original(Hidden))
            for (int i = 0; i < encrypt; i++){
                images[i].pixelPointer = stbi_load(images[i].currentplace, &images[i].width,
                                                   &images[i].height, &images[i].channels, STBI_default);
                if (images[i].pixelPointer == NULL){
                    fprintf(stderr, "Error: Couldn't open image from '%s'\n", images[i].currentplace);
                    return 1;
                }
            }

            // Verify if resize was required
            if (newResolution[0] != 0){
                if(resizeImages(images, newResolution, share1, original)){
                    return 1;
                }
            } else {
                // Verify if resolution of share1 is different of original(Hidden) image
                if (RESOLUTION_NOT_EQUAL(share1, original)){
                    int resolution[] = {images[original].width, images[original].height};
                    // Resize share1 to resolution of image original(Hidden)
                    if (resizeImages(images, resolution, share1, share1)){
                        fprintf(stderr, "Error: Couldn't resize cover image 1\n");
                        return 1;
                    }
                }
                //Same here
                if (RESOLUTION_NOT_EQUAL(share2, original)){
                    int resolution[] = {images[original].width, images[original].height};
                    if (resizeImages(images, resolution, share2, share2)){
                        fprintf(stderr, "Error: Couldn't resize cover image 2\n");
                        return 1;
                    }
                }
            }

            // TODO: Change all these delimiters
            // encrypt for
            for (int i = 0; i < images[share1].height; i++){
                for (int j = 0; j < images[share1].width; j++){
                    for (int k = 0; k < 3; k++){

                        // TODO: RENAME THIS VARIABLE
                        unsigned char renameMe = (unsigned char) reduce(images[original], i, j, k);
                        // Get ten and unity of original(Hidden) image
                        unsigned char originalTen = (unsigned char) (renameMe / 10);
                        unsigned char originalUnity = (unsigned char) (renameMe % 10);
                        // Generates a ten and unity random number
                        const unsigned char randTen = randomUnity;
                        const unsigned char randUnity = randomUnity;

                        //printf("Original(Hidden) channel: %d => %d\n", getpixelchannel(images[original(Hidden)], i, j, k), renameMe);
                        //printf("Share 1 Before: %d ", getpixelchannel(images[share1], i, j, k));

                        unsigned char* sharePixel = getPointerChannel(images[share1], i, j, k);
                        unsigned char sharePixelHundred = *sharePixel;
                        unsigned char sharePixelTen = (unsigned char) (10 * ((randTen + originalTen) % 10));

                        TONE_AJUST(sharePixelHundred, sharePixelTen, randUnity)

                        *sharePixel = (sharePixelHundred + sharePixelTen + randUnity);

                        //printf("After: %d\n", getpixelchannel(images[share1], i, j, k));
                        //printf("Share 2 Before: %d ", getpixelchannel(images[share2], i, j, k));

                        sharePixel = getPointerChannel(images[share2], i, j, k);
                        sharePixelHundred = *sharePixel;
                        sharePixelTen = (unsigned char) (10 * ((randUnity + originalUnity) % 10));

                        TONE_AJUST(sharePixelHundred, sharePixelTen, randTen)

                        *sharePixel = (sharePixelHundred + sharePixelTen + randTen);

                        //printf("After: %d\n", getpixelchannel(images[share2], i, j, k));
                        //printf("randUnity: %d randTen: %d\n",randUnity, randTen);
                        //printf("DEBUG: %d\n", sharePixelTen + randTen);
                    }
                }
            }

            // Records on disk the shares
            for (int i = share1; i <= share2; ++i){
                //printf("share%d : resolution:%dx%dx%d\n%s\n", i, images[i].width,
                //       images[i].height,images[i].channels, images[i].newplace);
                if(saveImages(images,i) == 1)
                    return 1;
            }
            break;
        case undefined:
            fprintf(stderr, "Error: Mode not defined\nTry --help");
            return 1;
    }

    return 0;
}
