/*
	gcc -fopenmp -o omp_main omp_main.c -lm && ./omp_main papagan.jpg output_seq.jpg
*/

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#define CHANNEL_NUM 1

void seq_edgeDetection(uint8_t* rgb_image,int **rober1, int **rober2, int width, int height, uint8_t *result_imag);

int main(int argc,char* argv[]) 
{	
	if (argc <= 2) {
		printf("Usage: %s <input.jpg> <output.jpg>\n", argv[0]);
		exit(EXIT_FAILURE);
	}

    int width, height, bpp;
	int** robert1 = malloc(2 * sizeof(int*));
	int** robert2 = malloc(2 * sizeof(int*));

	for (int i = 0; i<2; i++) {

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

	// Reading the image in grey colors
    uint8_t* input_image = stbi_load(argv[1], &width, &height, &bpp, CHANNEL_NUM);
	uint8_t *result_image = malloc(height * width + height);
	struct timeval start, end;
	
    printf("Width: %d  Height: %d \n",width,height);
	printf("Input: %s , Output: %s  \n",argv[1],argv[2]);
	
	// start the timer
	gettimeofday(&start, NULL);
	seq_edgeDetection(input_image, robert1, robert2, width, height, result_image);
	gettimeofday(&end, NULL);

	double time_taken = (end.tv_sec + (end.tv_usec / 1e6)) - (start.tv_sec + (start.tv_usec / 1e6));
	printf("Elapsed time: %lf \n",time_taken);
	
	// Storing the image 
    stbi_write_jpg(argv[2], width, height, CHANNEL_NUM, result_image, 100);
    stbi_image_free(input_image);

    return 0;
}

int* calculateRobert(uint8_t *imageData, int** matrix, int width, int height) {

	uint8_t *middleImage;
	int *resultRobert;
	int dynamicThreshold;

	middleImage = (uint8_t*)malloc(height * width * sizeof(uint8_t));

	for (int i = 0; i < height; i++) {
      for (int j = 0; j < width; j++) {          
          middleImage[i * width + j] = imageData[i * width + j];
      }
    }

	resultRobert = (int*)malloc(height * width * sizeof(int));

	for (int i = 1; i < height-1; i++) {
      for (int j = 1; j < width-1; j++) {
		dynamicThreshold = (middleImage[i * width + j] * matrix[0][0]) + 
						   (middleImage[((i+1) * width) + j+1] * matrix[1][1]) + 
						   (middleImage[(i * width) + j+1] * matrix[0][1]) + 
						   (middleImage[((i+1) * width) + j] * matrix[1][0]);
		resultRobert[i * width + j] = dynamicThreshold;
		}
	}

	return resultRobert;

}

void seq_edgeDetection(uint8_t* rgb_image, int **robert1, int **robert2, int width, int height, uint8_t* result_image)
{	
	int middleThreshold;
	int defaultThreshold = 125;
	float scale_factor = 2.75;
	int* firstResult = calculateRobert(rgb_image, robert1, width, height);
	int* secondResult = calculateRobert(rgb_image, robert2, width, height);

	for (int i = 0; i < height; i++) {

		for (int j = 0; j < width; j++) {

			middleThreshold = abs(firstResult[i * width + j] * scale_factor) + abs(secondResult[i * width + j] * scale_factor);

		if (middleThreshold > defaultThreshold) {

			middleThreshold = defaultThreshold;

		}

		result_image[i * width + j] = middleThreshold;

		}

	}
}
