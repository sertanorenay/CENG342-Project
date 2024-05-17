#include <omp.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "stb_image.h"
#include "stb_image_write.h"
#define CHANNEL_NUM 1

void edgeDetection(uint8_t* rgb_image, int **robert1, int **robert2, int width, int height, uint8_t *result_image);
void initializeRoberts(int **robert1, int **robert2);

int main(int argc, char* argv[]) 
{
    if (argc <= 2) {
        printf("Usage: %s <input.jpg> <output.jpg>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int width, height, bpp;
    int** robert1 = malloc(2 * sizeof(int*));
    int** robert2 = malloc(2 * sizeof(int*));
    initializeRoberts(robert1, robert2);

    // Reading the image in grey colors
    uint8_t* input_image = stbi_load(argv[1], &width, &height, &bpp, CHANNEL_NUM);
    uint8_t *result_image = malloc(height * width + height);
    struct timeval start, end;
    
    printf("Width: %d  Height: %d \n", width, height);
    printf("Input: %s , Output: %s \n", argv[1], argv[2]);
    
    // Start the timer
    gettimeofday(&start, NULL);
    edgeDetection(input_image, robert1, robert2, width, height, result_image);
    gettimeofday(&end, NULL);

    double time_taken = (end.tv_sec + (end.tv_usec / 1e6)) - (start.tv_sec + (start.tv_usec / 1e6));
    printf("Elapsed time: %lf \n", time_taken);
    
    // Storing the image 
    stbi_write_jpg(argv[2], width, height, CHANNEL_NUM, result_image, 100);
    stbi_image_free(input_image);

    for (int i = 0; i < 2; i++) {
        free(robert1[i]);
        free(robert2[i]);
    }
    free(robert1);
    free(robert2);
    free(result_image);

    return 0;
}

int* calculateRobert(uint8_t *imageData, int** matrix, int width, int height, int *localStart, int *localEnd) {

    int *resultRobert = (int*)malloc((height * width) * sizeof(int));

    #pragma omp parallel for schedule(static, 100)
    for (int i = 0; i < height - 1; i++) {
        for (int j = 0; j < width - 1; j++) {
            int dynamicThreshold = (imageData[i * width + j] * matrix[0][0]) + 
                                   (imageData[((i+1) * width) + j+1] * matrix[1][1]) + 
                                   (imageData[(i * width) + j+1] * matrix[0][1]) + 
                                   (imageData[((i+1) * width) + j] * matrix[1][0]);
            resultRobert[i * width + j] = dynamicThreshold;
        }
    }

    *localStart = 0;
    *localEnd = height;

    return resultRobert;
}

void edgeDetection(uint8_t* rgb_image, int **sobel1, int **sobel2, int width, int height, uint8_t* result_image) {    
    int middleThreshold;
    int defaultThreshold = 125;
    float scale_factor = 2.75;
    int localStart, localEnd;
    int* firstResult = calculateRobert(rgb_image, sobel1, width, height, &localStart, &localEnd);
    int* secondResult = calculateRobert(rgb_image, sobel2, width, height, &localStart, &localEnd);

    #pragma omp parallel for collapse(2) schedule(static, 100)
    for (int i = localStart; i < localEnd; i++) {
        for (int j = 0; j < width; j++) {
            int middleThreshold = abs(firstResult[(i * width) + j] * scale_factor) + abs(secondResult[(i * width) + j] * scale_factor);

            if (middleThreshold > defaultThreshold) {
                middleThreshold = defaultThreshold;
            }

            result_image[(i * width) + j] = middleThreshold;
        }
    }

    free(firstResult);
    free(secondResult);
}

void initializeRoberts(int **robert1, int **robert2) {
    for (int i = 0; i < 2; i++) {
        robert1[i] = malloc(2 * sizeof(int));
        robert2[i] = malloc(2 * sizeof(int));
    }

    robert1[0][0] = 1;
    robert1[0][1] = 0;
    robert1[1][0] = 0;
    robert1[1][1] = -1;

    robert2[0][0] = 0;
    robert2[0][1] = 1;
    robert2[1][0] = -1;
    robert2[1][1] = 0;
}