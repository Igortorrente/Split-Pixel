//#include "randombytes/randombytes.h"
#include <FreeImage.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include <getopt.h>

// Image position on vector of images
enum{ cover1 = 0, cover2, secret };
// Mode
enum{ undefined = 1, decrypt, encrypt };
enum{ width = 0, height };
enum{ none };

// Image struct
typedef struct imageData{
    char* currentplace, * newplace;
    FIBITMAP* pixelPointer;
    uint8_t channels, BPP;
    uint32_t width, height;
} imageData;

// TODO: Check if this name make any sense
// Maps a number on from range of 0-255 on 0-99
#define reduce(image) (lround((99.0 / 255.0) * (image)))

// Gets ten and unit of a number
#define ten(number) ((int8_t)((number) % 100))

// Retrieves a unit of a channeel
#define retrieveUnit(cover1, cover2) (((ten(cover1) / 10)) - ((cover2) % 10))

// TODO: Change function rand to another one
// Generates a random number between 0-9
#define randomUnit ((BYTE) (rand() / (RAND_MAX / 10)))

// Adjusts of the tone of color of pixel
#define TONE_ADJUST(coverPixelHundred, coverPixelTen, rand) \
    if ((coverPixelHundred) >= 200 && ((coverPixelTen) + (rand) > 55)){ \
        (coverPixelHundred) = 100; \
    } else if ((coverPixelHundred) >= 200){ \
        (coverPixelHundred) = 200; \
    } else if ((coverPixelHundred) >= 100){ \
        (coverPixelHundred) = 100; \
    } else { \
        (coverPixelHundred) = 0; \
    } \

// Verifies if the resolution of images are not the same
#define RESOLUTION_NOT_EQUAL(image1, image2) \
        images[image1].width != images[image2].width || \
        images[image1].height != images[image2].height

// Supported Formats
#define PNG  '\0gnp'
#define JPG  '\0gpj'
#define JPEG 'gepj'
#define BMP  '\0pmb'
#define TGA  '\0agt'
#define C_PNG  '\0GNP'
#define C_JPG  '\0GPJ'
#define C_JPEG 'GEPJ'
#define C_BMP  '\0PMB'
#define C_TGA  '\0AGT'

// Inicialization of long options of opt
#define LONG_OPTIONS \
    { \
        {"mode", required_argument, NULL, 'm'}, \
        {"help", no_argument, NULL, 'h'}, \
        {"input", required_argument, NULL, 'i'}, \
        {"output", required_argument, NULL, 'o'}, \
        {"resize", required_argument, NULL, 'r'}, \
        {0, 0, 0, 0} \
    }

// Message of --help option
#define HELP_MESSAGE \
    "Split Pixel 0.2 (pre-alpha)\n" \
    "Program based on Leandro Ribeiro's algorithm which allows to hide one image " \
    "into two other images and then retrieve the content.\n\n"\
    "Usage:\tSplit-Pixel -i [input images] -o [output images] -m <Mode> [Options]\n" \
    "\tFlags don't need be in this order.\n" \
    "\tSupported formats: png, jpeg(only for decrypt), bmp and tga.\n\n" \
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
    "\t[encrypt Mode]:\t<Cover 1> <Cover 2> <Secret>\n" \
    "\t[Decrypt Mode]:\t<Cover 1> <Cover 2>\n\n" \
    "\t-i,\t--input\t [input images]\n\n" \
    "Output images:\n" \
    "\tHere you put the names of output image(s) with desired extension \n" \
    "\t[encrypt Mode]:\t<Output 1> <Output 2>\n" \
    "\t[Decrypt Mode]:\t<Output>\n\n" \
    "\t-o,\t--output [output images]\n\n" \
    "Options:\n" \
    "\t-r,\t--resize <Width> <Height>\tResize image(s) of output (Only works with encrypt).\n" \
    "\t-h,\t--help\t Show This Message.\n\n" \
    "Project repository: https://github.com/Igortorrente/Split-Pixel\n" \
    "Paper: <Leandro's Papers Here!!>\n"

// Function that identifier of image format
// Receive a string with a path of image, including image name itself
// Returns a int that represent image format
uint32_t imageFormat(char* imageName){
    uint32_t imageFormat = none;
    
    // Get a pointer to latest '.'
    const char* formatPointer = strrchr(imageName, '.');

    // Check if some '.' was found
    if (formatPointer != none){
        memcpy(&imageFormat, formatPointer + 1, 4 * sizeof(char));
    }

    return imageFormat;
}

// This function identify and loads a image 
// Receives a imageData struct 
// Returns a pointer to image
FIBITMAP* loadImage(imageData* image){
    switch(imageFormat(image->currentplace)){
        case PNG: case C_PNG:
            return FreeImage_Load(FIF_PNG, image->currentplace, PNG_IGNOREGAMMA);
        case JPEG: case JPG: case C_JPEG: case C_JPG:
            return FreeImage_Load(FIF_JPEG, image->currentplace, JPEG_ACCURATE);
        case BMP: case C_BMP:
            return FreeImage_Load(FIF_BMP, image->currentplace, BMP_DEFAULT);
        case TGA: case C_TGA:
            return FreeImage_Load(FIF_TARGA, image->currentplace, TARGA_LOAD_RGB888);
        // Case '.' not found
        default:
            return NULL;
    }
}

// Function to save images based on format
// Receives a imageData struct 
// Returns the result of write
int8_t writeImage(imageData* image){
    // Based on identifier save image on correct format
    switch (imageFormat(image->newplace)){
        case PNG: case C_PNG:
            return !FreeImage_Save(FIF_PNG, image->pixelPointer, image->newplace, PNG_Z_NO_COMPRESSION);
        case JPEG: case JPG: case C_JPEG: case C_JPG:
            return !FreeImage_Save(FIF_JPEG, image->pixelPointer, image->newplace, JPEG_QUALITYSUPERB);
        case BMP: case C_BMP:
            return !FreeImage_Save(FIF_BMP, image->pixelPointer, image->newplace, BMP_DEFAULT);
        case TGA: case C_TGA:
            return !FreeImage_Save(FIF_TARGA, image->pixelPointer, image->newplace, TARGA_DEFAULT);
        // Case '.' not found
        case none: 
            return -2;
        // Case format not supported
        default:
            return -1;
    }
}

// Function to save a image, and if an error occurred, print a error message
// Receives a imageData struct
// Returns the result of write
int8_t saveImage(imageData* images){
    switch (writeImage(images)){
        case -2:
            fprintf(stderr, "Error: Missing format of image '%s'\n", images->newplace);
            return 1;
        case -1:
            fprintf(stderr, "Error: Format unsupported of image '%s'\n", images->newplace);
            return 1;
        case 0:
            return 0;
        default:
            fprintf(stderr, "Error: Couldn't save image '%s' on disk\n", images->newplace);
            return 1;
    }
}

// Resizes an image with a new height and width
// Receives a imageData struct, and two integers, a height and a width
// Returns a error 
int8_t resizeImages(imageData* image, uint32_t height, uint32_t width){
    FIBITMAP* resizedImage;
    resizedImage = FreeImage_Rescale(image->pixelPointer, width, height, FILTER_BSPLINE);
    if (resizedImage == NULL){
        return 1;
    }

    FreeImage_Unload(image->pixelPointer);
    image->height = FreeImage_GetHeight(resizedImage);
    image->width = FreeImage_GetWidth(resizedImage);
    image->pixelPointer = resizedImage;

    return 0;
}

int main(const int argc, const char* argv[], const char* env_var_ptr[]){

    // Gets all inputs of program
    #ifdef DEBUG
        for (int i = 1; i < argc; ++i){
            printf("%d: %s\n", i, argv[i]);
        }
        for (int i = 0; *env_var_ptr != NULL; ++i) {
            printf ("\n%d: %s",i, *(env_var_ptr++));
        }
        printf("\n\n");
    #endif
    // Image vector, with space for each image
    imageData images[3];
    uint32_t newResolution[2] = { none, none };
    // Initializes mode
    uint8_t mode = undefined;
    // Initializes srand
    srand((unsigned int) time(NULL));

    struct option longOptions[] = LONG_OPTIONS;

    // Parser of options
    int option = 0;
    while ((option = getopt_long(argc, (char *const *)argv, "m:r:i:o:hv", longOptions, NULL)) != -1){
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
            case 'r':
                // Get values of -r option
                for (int i = -1; i < 1; i++){
                    // Convert to int the value
                    newResolution[i + 1] = atoi(argv[optind + i]);
                    // If 0 or error 
                    if (!newResolution[i + 1]){
                        fprintf(stderr, "Error:'%s' is not valid value for resolution of image\n", argv[optind + i]);
                        return 1;
                    }
                }
                break;
            case 'm':
                // Discover mode
                if (!strcmp("encrypt", optarg)){
                    mode = encrypt;
                } else if (!strcmp("decrypt", optarg)){
                    mode = decrypt;
                } else{
                    fprintf(stderr, "Error:'%s' is not a valid mode\nTry encrypt or decrypt", optarg);
                    return 1;
                }
                break;
            case '?':
                // flush opt message
                fflush(stderr);
                printf("Try --help\n");
                return 1;
        }
    }

    if(mode == undefined){
        fprintf(stderr, "Error: Mode not defined\nTry --help");
        return 1;
    }

    // Initialize Free Image without plugin
    FreeImage_Initialise(FALSE);

    // Loads input images
    for (uint8_t i = cover1; i < mode; i++){
        // Loads the image
        images[i].pixelPointer = loadImage(&images[i]);
        // Checks if image has been loaded
        if (images[i].pixelPointer == NULL){
            fprintf(stderr, "Error: Can't open image from '%s'\n", images[i].currentplace);
            return 1;
        } 

        FIBITMAP* image = images[i].pixelPointer;
        images[i].BPP = FreeImage_GetBPP(image);
        // Checks if Bits Per Pixel(BPP) of the image are supported
        switch(images[i].BPP){
            case 1: case 4: case 16:
                fprintf(stderr, "Image with %d bit(s) per channels not supported\n", images[i].BPP);
                return 1;
            default:
                break;
        }

        // Updates metadata of image
        images[i].height = FreeImage_GetHeight(image);
        images[i].width = FreeImage_GetWidth(image);  
        images[i].channels = (FreeImage_GetLine(image) / images[i].width);
    }

    // Checks if secret image is grayscale 
    if(images[secret].BPP == 8){
        // Converts cover 1 and 2 to grayscale
        for (uint8_t image = cover1; image <= cover2; ++image){
            images[image].pixelPointer = FreeImage_ConvertToGreyscale(images[image].pixelPointer);
            images[image].BPP = 8;
            images[image].channels = 1;
        }
    } else{
        // Checks if secret image has alpha
        uint8_t limit = cover2;
        if(images[secret].BPP == 32){
            limit = secret;
        }
        // Removes alpha channel of images 
        for (uint8_t image = cover1; image <= limit; ++image){
            images[image].pixelPointer = FreeImage_ConvertTo24Bits(images[image].pixelPointer);
            images[image].BPP = 24;
            images[image].channels = 3;
        }
    }

    if(mode == decrypt){
        // Fixes some values and initialize output images
        images[secret] = images[cover1];
        images[cover1].newplace = NULL;
        images[secret].currentplace = NULL;

        if(RESOLUTION_NOT_EQUAL(cover1, cover2)){
            fprintf(stderr, "Error: Resolution of covers are not equal\n");
            return 1;
        }

        // Allocates a matrix for recovered image
        images[secret].pixelPointer = FreeImage_Allocate(images[secret].width, 
            images[secret].height, 24, FI_RGBA_RED_MASK, FI_RGBA_GREEN_MASK, FI_RGBA_BLUE_MASK);
        // Check if allocation has successful
        if (images[secret].pixelPointer == NULL){
            fprintf(stderr, "Error: Couldn't allocate image matrix\n");
            return 1;
        }

        // Decrypt for
        // get line size in bytes and height of image
        uint64_t line = images[secret].width * images[secret].channels;
        uint32_t imageHeight = images[cover1].height;
        for (uint32_t i = 0; i < imageHeight; ++i){
            // Get a pointer of image line i
            BYTE* cover1Pointer = (BYTE *)FreeImage_GetScanLine(images[cover1].pixelPointer, i);
            BYTE* cover2Pointer = (BYTE *)FreeImage_GetScanLine(images[cover2].pixelPointer, i);
            BYTE* secretPointer = (BYTE *)FreeImage_GetScanLine(images[secret].pixelPointer, i);
            for (uint64_t j = 0; j < line; ++j){
                // Calculates the reduced ten and unit of secret(Hidden) image
                int8_t retrievedTen = retrieveUnit(cover1Pointer[j], cover2Pointer[j]);
                retrievedTen = retrievedTen < 0 ? (int8_t) 10 + retrievedTen : retrievedTen;

                int8_t retrievedUnit = retrieveUnit(cover2Pointer[j], cover1Pointer[j]); 
                retrievedUnit = retrievedUnit < 0 ? (int8_t) 10 + retrievedUnit : retrievedUnit;

                // Writes in image a retrieved channel
                secretPointer[j] = (BYTE) round((255.0 / 99) * (10 * retrievedTen + retrievedUnit));
            }
        }
        // Records on disk a retrieved image
        if (saveImage(&images[secret]) == 1){
            return 1;
        }
    } else if(mode == encrypt){

        // Initializes the newplace
        images[secret].newplace = NULL;

        // Verifies if resize was required
        if (newResolution[0] != none){
            // Resize all images
            for (int cover = cover1; cover <= secret; ++cover){
                if (resizeImages(&images[cover], newResolution[height], newResolution[width])){
                    return 1;
                }                
            }
        } else{
            // Verify if resolution of cover is different of secret(Hidden) image
            for (int cover = cover1; cover <= cover2; ++cover){
                if (RESOLUTION_NOT_EQUAL(cover, secret)){
                    // Resize cover to resolution of image secret(Hidden)
                    if (resizeImages(&images[cover], images[secret].height, images[secret].width)){
                        fprintf(stderr, "Error: Couldn't resize cover image %d\n", cover);
                        return 1;
                    }
                }
            }
        }

        // encrypt for
        uint32_t line = images[secret].width * images[secret].channels;
        uint64_t imageHeight = images[cover1].height;
        for (uint32_t i = 0; i < imageHeight; i++){
            BYTE* cover1Pointer = (BYTE*)FreeImage_GetScanLine(images[cover1].pixelPointer, i);
            BYTE* cover2Pointer = (BYTE*)FreeImage_GetScanLine(images[cover2].pixelPointer, i);
            BYTE* secretPointer = (BYTE*)FreeImage_GetScanLine(images[secret].pixelPointer, i);
            for (uint64_t j = 0; j < line; j++){
                // Reduces the value of channel
                BYTE reducedChannel = (BYTE) reduce(secretPointer[j]);

                // Gets ten and unit of secret(Hidden) image
                BYTE originalUnit = (BYTE) (reducedChannel % 10);
                BYTE originalTen = (BYTE) (reducedChannel / 10);
                // Generates a ten and unit random number
                const BYTE randTen = randomUnit;
                const BYTE randUnit = randomUnit;

                BYTE coverPixelHundred = cover1Pointer[j];
                BYTE coverPixelTen = (BYTE) (10 * ((randTen + originalTen) % 10));

                // Adjusts tone of channel of cover 1
                TONE_ADJUST(coverPixelHundred, coverPixelTen, randUnit);

                // Saves the new value of channel on cover1
                cover1Pointer[j] = coverPixelHundred + coverPixelTen + randUnit;

                coverPixelHundred = cover2Pointer[j];
                coverPixelTen = (BYTE) (10 * ((randUnit + originalUnit) % 10));
                
                // Adjusts tone of channel of cover 2
                TONE_ADJUST(coverPixelHundred, coverPixelTen, randTen);
                
                // Saves the new value of channel on cover 2
                cover2Pointer[j] = (coverPixelHundred + coverPixelTen + randTen);
            }
        }
        // Records on disk the shares
        for (int i = cover1; i <= cover2; ++i){
            if (saveImage(&images[i]) == 1){
              return 1;
            }
        }
    }

    return 0;
}
