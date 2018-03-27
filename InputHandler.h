#ifndef SPLIT_PIXEL_SERIAL_INPUTHANDLER_H
#define SPLIT_PIXEL_SERIAL_INPUTHANDLER_H

//TODO: Make this library work without -std=gnu
#include <unistd.h> // POSIX = Not compatible with windows :'(
#include <fcntl.h> // Unix/unix-like library not compatible with windows :'(
#include <stdio.h>
#include "stb_image.h"


#if defined(__APPLE__) || defined(__FreeBSD__)
#include <copyfile.h>
#elif defined(__unix__)
#include <sys/sendfile.h>
#endif

typedef struct imageHandler{
    char* place;
    FILE* image;
}imageHandler;

int openImages(imageHandler *image, char *imagesInputPlace[]);


#endif SPLIT_PIXEL_SERIAL_INPUTHANDLER_H
