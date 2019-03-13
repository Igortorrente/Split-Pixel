# Split-Pixel

Split Pixel is a program to hide a image (secret) inside of two others images (covers) and restore the original image without quality loss. 
Currently only png, bmp, tga and jpg (only for decrypt) are supported.

## Requirements

* GCC or Clang with c11 support
* OpenMP
* FreeImage.h

## Build

```
$ git clone --recursive https://github.com/Igortorrente/Split-Pixel.git
$ cd Split-Pixel
$ mkidr build
$ cmake ..
$ make
```

## Example

# Original images

Sample 1                   | sample 2                  |  Secret sample
:-------------------------:|:-------------------------:|:-------------------------
![](https://user-images.githubusercontent.com/18224386/54292277-a6577c00-458c-11e9-83c1-076c84225296.JPG)| ![](https://user-images.githubusercontent.com/18224386/54292284-a6f01280-458c-11e9-8657-2d8b584efef3.JPG) | ![](https://user-images.githubusercontent.com/18224386/54292275-a5bee580-458c-11e9-9ee6-9b6b98fac831.jpg)

# After 

Cover 1                    | Cover 2                   | Secret Image
:-------------------------:|:-------------------------:|:-------------------------:
![out1](https://user-images.githubusercontent.com/18224386/54292279-a6577c00-458c-11e9-99f6-9807ef74f129.png)| ![out2](https://user-images.githubusercontent.com/18224386/54292280-a6577c00-458c-11e9-9256-efc6bf5f3603.png) | ![out3](https://user-images.githubusercontent.com/18224386/54292282-a6f01280-458c-11e9-81e3-ec5001768cf8.png)

## License

The source code of this project is licensed under the Apache-2.0 
license.
