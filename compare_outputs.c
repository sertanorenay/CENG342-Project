#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "stb_image.h"
#include "stb_image_write.h"

int areImagesEqual(const char* file1, const char* file2);

int main(int argc, char** argv) {
    if (argc < 3) {
        printf("Usage: %s <image1> <image2>\n", argv[0]);
        return -1;
    }

    if (areImagesEqual(argv[1], argv[2])) {
        printf("The images are exactly the same.\n");
    } else {
        printf("The images are different.\n");
    }

    return 0;
}

int areImagesEqual(const char* file1, const char* file2) {
    int width1, height1, bpp1;
    int width2, height2, bpp2;

    uint8_t* img1 = stbi_load(file1, &width1, &height1, &bpp1, 0);
    uint8_t* img2 = stbi_load(file2, &width2, &height2, &bpp2, 0);

    if (img1 == NULL || img2 == NULL) {
        printf("Error loading one of the images.\n");
        if (img1) stbi_image_free(img1);
        if (img2) stbi_image_free(img2);
        return 0;
    }

    if (width1 != width2 || height1 != height2 || bpp1 != bpp2) {
        stbi_image_free(img1);
        stbi_image_free(img2);
        return 0;
    }

    int imgSize = width1 * height1 * bpp1;
    for (int i = 0; i < imgSize; i++) {
        if (img1[i] != img2[i]) {
            stbi_image_free(img1);
            stbi_image_free(img2);
            return 0;
        }
    }

    stbi_image_free(img1);
    stbi_image_free(img2);
    return 1;
}
