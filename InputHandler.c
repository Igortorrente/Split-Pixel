#include "InputHandler.h"

int openImages(imageHandler* image, char* imagesInputPlace[]) {

    for (int i = 1; i < 3; ++i) {
        if(OSCopyFile(imagesInputPlace[i], image[i].place) == -1){
            return i + 1;
        }
    }

    int i = 0;
    for (; i < 3; i++) {
        image[i].image = fopen(image[i].place, "r+");
        if(image[i].image == NULL)
            break;
    }

    if (image[i].image == NULL) {
        int imageError = i + 1;
        for (i--; i >= 0; i--)
            fclose(image[i].image);
        return imageError;
    }

    return 0;
}

int OSCopyFile(const char* source, const char* destination) {

    int input, output;
    if ((input = open(source, O_RDONLY, 0660)) == -1) {
        return -1;
    }
    if ((output = creat(destination, 0660)) == -1){
        close(input);
        return -1;
    }

    //Here we use kernel-space copying for performance reasons
#if defined(__APPLE__) || defined(__FreeBSD__)
    //fcopyfile works on FreeBSD and OS X 10.5+
    int result = fcopyfile(input, output, 0, COPYFILE_ALL);
#else
    //sendfile will work with non-socket output (i.e. regular file) on Linux 2.6.33+
    off_t bytesCopied = 0;
    struct stat fileinfo = { 0 };
    fstat(input, &fileinfo);
    int result = sendfile(output, input, &bytesCopied, fileinfo.st_size);
#endif

    close(input);
    close(output);

    return result;
}