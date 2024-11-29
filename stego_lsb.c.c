#include <stdio.h>
#include <stdlib.h>

#define WIDTH 512 //image width
#define HEIGHT 512 //image height

// Function prototypes
int writePGMText(const char *filename, unsigned char *pixels, int width, int height);
int readPGMText(const char *filename, unsigned char *pixels, int width, int height);
void embedLSB(unsigned char *coverPixels, unsigned char *secretPixels, int width, int height);
int writePGMBinary(const char *filename, unsigned char *pixels, int width, int height);
void extractLSB(unsigned char *coverPixels, unsigned char *outputPixels, int width, int height);

int main() {
    char cover_image[] = "C:/Users/VANSHIKA/Documents/UOWD/CSCI 291/Lab4_Q2 (1)/baboon.pgm";
    char secret_image[] = "C:/Users/VANSHIKA/Documents/UOWD/CSCI 291/Lab4_Q2 (1)/farm.pgm";
    char stego_image[] = "C:/Users/VANSHIKA/Documents/UOWD/CSCI 291/Lab4_Q2 (1)/stego_image_bin.pgm"; 
    char extracted_secret[] = "C:/Users/VANSHIKA/Documents/UOWD/CSCI 291/Lab4_Q2 (1)/extracted_secret.pgm"; 
    
    //image pointers
    unsigned char *coverPixels; 
    unsigned char *secretPixels; 
    unsigned char *outputPixels; 

    //cover image width and height
    int coverWidth = WIDTH;
    int coverHeight = HEIGHT; 

    //secret image width and height
    int secretWidth = WIDTH;
    int secretHeight = HEIGHT; 

    //allocating memory for the cover image
    coverPixels = (unsigned char *)malloc(WIDTH * HEIGHT);
    if (coverPixels == NULL) {
        printf("Error: Memory allocation for coverPixels failed.\n");
        return 1;
    }

    //reading the cover image
    if (readPGMText(cover_image, coverPixels, coverWidth, coverHeight) != 0) {
        printf("Error: Unable to read cover image.\n");
        free(coverPixels);
        return 1;
    }
    printf("\nCover image read successfully.\n");

    secretPixels = (unsigned char *)malloc(WIDTH * HEIGHT); //allocating memory for the secret image
    if (secretPixels == NULL) {
        printf("Error: Memory allocation for secretPixels failed.\n");
        free(coverPixels);
        return 1;
    }

    //reading the secret image
    if (readPGMText(secret_image, secretPixels, secretWidth, secretHeight) != 0) {
        printf("Error: Unable to read secret image.\n");
        free(coverPixels);
        free(secretPixels);
        return 1;
    }
    printf("\nSecret image read successfully.\n");

    //checking if dimensions match of both the coverimage and secretimage
    if (coverWidth != secretWidth || coverHeight != secretHeight) {
        printf("Error: Image dimensions do not match.\n");
        free(coverPixels);
        free(secretPixels);
        return 1;
    }

    //calling the embedLSB() function so the secret image is put into the cover image   
    embedLSB(coverPixels, secretPixels, WIDTH, HEIGHT); 
    printf("\nSecret image embedded into cover image successfully.\n");

    //saving the stego image in binary format
    if (writePGMBinary(stego_image, coverPixels, WIDTH, HEIGHT) != 0) { 
        printf("Error: Unable to save stego image.\n");
        free(coverPixels);
        free(secretPixels);
        return 1;
    }
    printf("\nStego image saved successfully.\n");

    //allocate memory for the extracted secret image
    outputPixels = (unsigned char *)malloc(WIDTH * HEIGHT);
    if (outputPixels == NULL) {
        printf("Error: Memory allocation for outputPixels failed.\n");
        free(coverPixels);
        free(secretPixels);
        return 1;
    }

    //calling the extractLSB() function to extract the secret image from the stego image
    extractLSB(coverPixels, outputPixels, WIDTH, HEIGHT); 
    printf("\nSecret image extracted from stego image successfully.\n");

    // Save the secret image gotten in text format
    if (writePGMText(extracted_secret, outputPixels, WIDTH, HEIGHT) != 0){
        printf("Error: Unable to save extracted secret image.\n");
    } else {
        printf("\nExtracted secret image saved successfully.\n");
    }

    //free the allocated memory
    free(coverPixels); 
    free(secretPixels); 
    free(outputPixels); 

    // Indicate that all tasks were successfully completed
    printf("\nTask completed successfully!\n");

    return 0;
}

// Function to read an ASCII PGM file
int readPGMText(const char *filename, unsigned char *pixels, int width, int height) {
    FILE *file = fopen(filename, "r"); //opening file in read only 
    if (file == NULL) {
        return 1; //error opening file
    }

    char buffer[16];
    if (fgets(buffer, sizeof(buffer), file) == NULL || buffer[0] != 'P' || buffer[1] != '2') {
        fclose(file);
        return 1; // Invalid PGM format
    }

    // Skip comments
    do{
        if (fgets(buffer, sizeof(buffer), file) == NULL) {
            fclose(file); //close file
            return 1; //error reading file
        }
    } while(buffer[0] == '#');

    int w;
    int h;
    int maxVal;
    sscanf(buffer, "%d %d", &w, &h); //stroing image dimensions and max pixel value
    if(fscanf(file, "%d", &maxVal)!=1||w!=width||h!=height||maxVal!=255){
        fclose(file);//close file
        return 1; //mismatched dimensions or unsupported format
    }

    for(int i=0;i<width*height;i++){
        if(fscanf(file, "%hhu", &pixels[i])!=1){ //read the pixel data
            fclose(file); //closing file
            return 1; //error reading pixel data
        }
    }

    fclose(file); //close file
    return 0; //when carried out succesfully
}

// Function to write an ASCII PGM file
int writePGMText(const char *filename, unsigned char *pixels, int width, int height) {
    FILE *file=fopen(filename, "w"); //opening file in write mode
    if(file==NULL){
        return 1; //error opening file
    }

    fprintf(file, "P2\n# Created by stego_lsb.c\n%d %d\n255\n", width, height); //wrting the header

    for(int i=0;i<width*height;i++) {
        fprintf(file,"%d\n", pixels[i]); //writing the pixel data
    }

    fclose(file); //closing file
    return 0; //when carried out succesfully
}

// Function to write a binary PGM file
int writePGMBinary(const char *filename, unsigned char *pixels, int width, int height){
    FILE *file=fopen(filename, "wb"); //opening the file in binary write mode
    if(file==NULL){
        return 1; //error opening file
    }

    fprintf(file, "P5\n# Created by stego_lsb.c\n%d %d\n255\n", width, height); //writing the header
    fwrite(pixels, sizeof(unsigned char), width * height, file); //writing the binary pixel data

    fclose(file); //closing file
    return 0; //when carried out succesfully
}

//putting the secret image into the cover image 
void embedLSB(unsigned char *coverPixels, unsigned char *secretPixels, int width, int height){
    for(int i=0;i<width*height;i++){ 
        //putting the upper 4 bits of secret in to the cover image
        coverPixels[i]=(coverPixels[i]/16)*16+(secretPixels[i]/16); 
    }
}

//taking the secret image from the stego image 
void extractLSB(unsigned char *coverPixels, unsigned char *outputPixels, int width, int height){
    for(int i=0;i<width*height;i++){
        outputPixels[i]=(coverPixels[i]%16)*16; //after extraction reconstrcution of secret pixel
    }
}