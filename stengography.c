#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <png.h>
#include <math.h>
#include <stdbool.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb/stb_image_write.h"

unsigned char ***dynamicArray(int height, int width, int channels);
void loadPixelData(unsigned char ***imageArray, unsigned char *image, int height, int width, int channels);
void printArray(unsigned char ***imageArray, int height, int width, int channels);
void free3DDynamicArray(unsigned char ***imageArray, int height, int width, int channels);
void reverseArray(int *array, int length);
void swap(int *x, int *y);
int *convertCharToBinary(unsigned char character, int length);
int *convertStringToBinary(char *string);
unsigned char *updated1DPixelData(unsigned char ***imageArray, int height, int width, int channels);
void createPng(char *fileName, unsigned char *pixels, int height, int width);
char *getOutputFileName(char *fileName);
void encode(char *fileName, char *sentence, char *outputFileName);
void decode(char *outputPictureFileName, char *outputTextFileName);
char *readTextFromFile(char *inputTextFileName);

int main(int argc, char *argv[]) {
    /*
    USE CASE
        -e: encoding
            Step 1: Run make
            Step 2: ./stegano -e {input picture file path} {text file to read}
        -d: decoding
            Step 1: Run make
            Step 2: ./stegano -d {output picture file path} {text file to write to}
    */

    if (argc != 4) {
        printf("Not enough arguements\n");
	return 1;
    }

    char *option = argv[1];
    char *pictureFileName = argv[2];
    char *outputFileName = getOutputFileName(pictureFileName);
    char *textFileName = argv[3];

    if (strcmp("-e", option) == 0 && argc == 4) {
        char *sentenceToEncode = readTextFromFile(textFileName);
        encode(pictureFileName, sentenceToEncode, outputFileName);
        free(sentenceToEncode);
    } else if (strcmp("-d", option) == 0 && argc == 4) {
        decode(argv[2], argv[3]);
        free(outputFileName);
    } else printf("Invalid Option\n");

    return 0;
}

unsigned char ***dynamicArray(int height, int width, int channels) {
    /*
    Summary:
        Returns a pointer to an allocated 3D array, where the innermost array has a tuple of 3 (RGB) values that is 
        supposed to represent all the pixels in the array.

    Args:
        height (int): Represnts the height of the image
        width (int): Represnts the width of the image
        channels (int): Represnts 3 RGB values, and channels is usually 3

    Returns:
        Returns a pointer to the allocated 3D array on the heap. Memory must be freed.
    */

    unsigned char ***imageArray = (unsigned char ***)malloc(height * sizeof(unsigned char **));

    if (!imageArray) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }

    for (int x = 0; x < height; x++) {
        imageArray[x] = (unsigned char **)malloc(width * sizeof(unsigned char *));
        
        if (!imageArray[x]) {
            fprintf(stderr, "Memory allocation failed\n");
            exit(1);
        }

        for (int y = 0; y < width; y++) {
            imageArray[x][y] = (unsigned char *)malloc(channels * sizeof(unsigned char));
            
            if (!imageArray[x][y]) {
                fprintf(stderr, "Memory allocation failed\n");
                exit(1);
            }
        }
    }

    return imageArray;
}

void loadPixelData(unsigned char ***imageArray, unsigned char *image, int height, int width, int channels) {
    /*
    Summary:
        Returns a pointer to an allocated 3D array, where the innermost array has a tuple of 3 (RGB) values. Meaning at
        array[0][0][0] is the first red pixel value, at array[0][0][1] the array[0][0][1] is the first green pixel
        value, the array[0][0][2] is teh first blue pixel.

    Args:
        imageArray (unsigned char 3D image array): Represnts the image as a 3D array of RGB values
        image (unsigned char linear array): Represents the image as a linear
        height (int): Represents the height of the image
        width (int): Represents the width of the image
        channels (int): Represents the 3 RGB values (typically 3)

    Return:
        A void return that modifies the array in place that converts the 1D image array into a 3D image array
    */
    
    for (int x = 0; x < height; x++) {
        for (int y = 0; y < width; y++) {
            for (int channel = 0; channel < channels; channel++) {
                imageArray[x][y][channel] = image[(x * width + y) * channels + channel];
            }
        }
    }
}

void printArray(unsigned char ***imageArray, int height, int width, int channels) {
    /*
    Summary:
        Prints the RGB values of each pixel in a 3D image array. For each pixel, the Red (R), Green (G), and Blue (B)
        values are printed in the format "R: value | G: value | B: value".
    
    Args:
        imageArray (unsigned char 3D image array): Represents the image as a 3D array of RGB values. 
        height (int): Represents the height of the image (number of rows of pixels).
        width (int): Represents the width of the image (number of columns of pixels).
        channels (int): Represents the number of channels per pixel, typically 3 for RGB.

    Return:
        This function does not return any value; it prints the RGB values for each pixel to the console.
    */

    for (int x = 0; x < height; x++) {
        for (int y = 0; y < width; y++) {
            printf("R: %d | G: %d | B: %d\n", imageArray[x][y][0], imageArray[x][y][1], imageArray[x][y][2]);
        }
    }
    
    printf("\n");
}

void free3DDynamicArray(unsigned char ***imageArray, int height, int width, int channels) {
    /*
    Summary:
        Frees the memory allocated for a 3D image array. This function deallocates the memory row by row, and then frees
        the entire array, preventing memory leaks.

    Args:
        imageArray (unsigned char 3D image array): The dynamically allocated 3D array of RGB values to be freed.
        height (int): The height of the image (number of rows of pixels).
        width (int): The width of the image (number of columns of pixels).
        channels (int): The number of channels per pixel, typically 3 for RGB.

    Return:
        This function does not return any value; it frees the memory of the image array.
    */

    for (int x = 0; x < height; x++) {
        for (int y = 0; y < width; y++) free(imageArray[x][y]);
        free(imageArray[x]);
    }

    free(imageArray);
}

void swap(int *x, int *y) {
    /*
    Summary:
        Swaps the values of two integers. This function exchanges the values of the two given integers using a temporary variable.

    Args:
        x (int*): Pointer to the first integer.
        y (int*): Pointer to the second integer.

    Return:
        This function does not return any value; it swaps the values of the two integers in place.
    */

    int temp = *x;
    *x = *y;
    *y = temp;
}

int *convertCharToBinary(unsigned char character, int length) {
    /*
    Summary:
        Converts a given character to its 7-bit binary representation (ASCII). The function dynamically allocates an array
        to store the binary digits and returns the array. The binary digits are ordered from most significant bit to 
        least significant bit.

    Args:
        character (char): The character to be converted to binary.

    Return:
        Returns a pointer to an integer array containing the 7-bit binary representation of the character.
        The caller is responsible for freeing the allocated memory after use.
    */

    int *binaryArray = (int *)malloc(length * sizeof(int));
    
    if (!binaryArray) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }

    int index = 0;
    int characterAsASCII = (int)character;

    while (characterAsASCII) {
        binaryArray[index] = characterAsASCII % 2;
        characterAsASCII /= 2;
        index++;
    }

    while (index < length) {
        binaryArray[index++] = 0;
    }

    reverseArray(binaryArray, length);

    return binaryArray;
}

void reverseArray(int *array, int length) {
    /*
    Summary:
        Reverses the elements of an integer array in place. The function swaps the elements from the front and back
        of the array until it reaches the middle, effectively reversing the order of the elements.

    Args:
        array (int*): Pointer to the array to be reversed.
        length (int): The number of elements in the array.

    Return:
        This function does not return any value; it reverses the array in place.
    */

    int middle = length / 2;

    for (int front = 0, back = length - 1; front < middle; front++, back--) {
        swap(&array[front], &array[back]);
    }
}

int *convertStringToBinary(char *string) {
    /*
    Summary:
        Converts a string into its binary representation. Each character in the string is converted to its 7-bit binary
        equivalent (ASCII), and the function combines these into a single dynamically allocated 1D array. The binary
        digits for each character are stored sequentially in the array. The function also pads the end of the array with
        7 additional 0 bits.

    Args:
        string (char*): The input string to be converted into binary.

    Return:
        Returns a pointer to a dynamically allocated 1D integer array containing the 7-bit binary representation
        of the string, followed by 7 additional 0 bits. The caller is responsible for freeing the allocated memory after use.

    Note:
        If memory allocation fails, the function prints an error message and exits the program.
    */

    int **totalBinaryArray = (int **)malloc(strlen(string) * sizeof(int *));
    
    if (!totalBinaryArray) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }

    for (int character = 0; character < strlen(string); character++) {
        totalBinaryArray[character] = convertCharToBinary((unsigned char)string[character], 7);
    }

    int *oneDimBinaryArray = (int *)malloc(((strlen(string) * 7) + 7) * sizeof(int));
    
    if (!oneDimBinaryArray) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }

    for (int character = 0; character < strlen(string); character++) {
        for (int bit = 0; bit < 7; bit++) {
            oneDimBinaryArray[character * 7 + bit] = totalBinaryArray[character][bit];
        }

        free(totalBinaryArray[character]);
    }

    free(totalBinaryArray);

    for (int index = (strlen(string) * 7); index < ((strlen(string) * 7) + 7); index++) {
        oneDimBinaryArray[index] = 0;
    }
    
    return oneDimBinaryArray;
}

unsigned char *updated1DPixelData(unsigned char ***imageArray, int height, int width, int channels) {
    /*
    Summary:
        Converts a 3D image array back into a 1D array of pixel data. This function also updates each pixel's RGB values by 
        subtracting the remainder when divided by 2, effectively setting the least significant bit of each channel to 0. 
        The resulting 1D array contains the modified pixel data in row-major order.

    Args:
        imageArray (unsigned char 3D image array): A 3D array representing the image, where each pixel has multiple channels (e.g., RGB).
        height (int): The height of the image (number of rows of pixels).
        width (int): The width of the image (number of columns of pixels).
        channels (int): The number of channels per pixel, typically 3 for RGB.

    Return:
        Returns a pointer to a dynamically allocated 1D array containing the modified pixel data, with each pixel's RGB
        values flattened into a linear sequence. The caller is responsible for freeing the allocated memory after use.

    Note:
        If memory allocation fails, the function prints an error message and exits the program.
    */

    unsigned char *flattened3DArray = (unsigned char *)malloc((height * width * channels) * sizeof(unsigned char));
    
    if (!flattened3DArray) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }
    
    for (int x = 0; x < height; x++) {
        for (int y = 0; y < width; y++) {
            for (int channel = 0; channel < channels; channel++) {
                imageArray[x][y][channel] = imageArray[x][y][channel] - (imageArray[x][y][channel] % 2);

                int index = (x * width * channels) + (y * channels) + channel;
                flattened3DArray[index] = imageArray[x][y][channel];
            }
        }
    }

    return flattened3DArray;
}

void createPng(char *filename, unsigned char *pixels, int height, int width) {
    /*
    Summary:
        Creates a PNG image file from a 1D array of pixel data. The function writes pixel data to the file specified
        by the given filename, with the image being generated from the pixel values in RGB format.

    Args:
        filename (char*): The name of the file where the PNG image will be saved.
        pixels (unsigned char*): A 1D array of pixel data in RGB format. Each pixel consists of three consecutive values (R, G, B).
        height (int): The height of the image (number of rows of pixels).
        width (int): The width of the image (number of columns of pixels).

    Return:
        This function does not return any value; it creates and writes the PNG file to the specified location.

    Note:
        If any step (file opening, memory allocation, PNG struct creation) fails, the function prints an error message
        and safely exits. The caller must ensure valid pixel data is passed and handle memory freeing after use.
    */

    FILE *fp = fopen(filename, "wb");
    
    if (!fp) {
        perror("Error opening file");
        return;
    }

    png_structp png = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

    if (!png) {
        fclose(fp);
        return;
    }

    png_infop info = png_create_info_struct(png);

    if (!info) {
        png_destroy_write_struct(&png, NULL);
        fclose(fp);
        return;
    }

    if (setjmp(png_jmpbuf(png))) {
        png_destroy_write_struct(&png, &info);
        fclose(fp);
        return;
    }

    png_init_io(png, fp);
    png_set_IHDR(png, info, width, height, 8, PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);
    png_write_info(png, info);
    
    png_bytep row = (png_bytep)malloc(3 * width * sizeof(png_byte));
    
    if (!row) {
        png_destroy_write_struct(&png, &info);
        fclose(fp);
        return;
    }

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            row[x * 3 + 0] = pixels[(y * width + x) * 3 + 0];
            row[x * 3 + 1] = pixels[(y * width + x) * 3 + 1];
            row[x * 3 + 2] = pixels[(y * width + x) * 3 + 2];
        }

        png_write_row(png, row);
    }

    free(row);
    png_write_end(png, NULL);
    png_destroy_write_struct(&png, &info);
    fclose(fp);
}

char *getOutputFileName(char *fileName) {
    /*
    Summary:
        Generates a new output file name by appending "-output.png" to the original file name (excluding the file extension).
        This is useful for saving processed images with a modified name based on the original file.

    Args:
        fileName (char*): The original file name, including the extension.

    Return:
        Returns a dynamically allocated string containing the new file name with the "-output.png" suffix.
        The caller is responsible for freeing the allocated memory after use.

    Note:
        If memory allocation fails, the function behavior is undefined. The caller must ensure proper memory handling.
    */

    char *outputFileName = (char *)malloc((strlen(fileName) + 11) * sizeof(unsigned char));

    for (int index = 0; index < strlen(fileName); index++) {
        if (fileName[index] == '.') break;
        else outputFileName[index] = fileName[index];
    }

    strcat((char *)outputFileName, "-output.png");

    return outputFileName;
}

void encode(char *fileName, char *sentence, char *outputFileName) {
    /*
    Summary:
        Encodes a text message into an image file by modifying the least significant bits of each pixel in the input image.
        The encoded message is saved as a new image file. Each character in the text is converted to binary, and the binary 
        data is embedded sequentially into the pixel data.

    Args:
        fileName (char*): The name of the input image file to encode the message into.
        sentence (char*): The text message to be encoded within the image.
        outputFileName (char*): The name of the output image file to save the encoded image.

    Return:
        This function does not return any value; it performs encoding in place and saves the result to the output file.

    Note:
        Memory for the dynamically allocated 3D image array, binary array, and updated pixel data is freed after use. 
        The function relies on the stb_image library to load and free image data. Error handling includes memory 
        management checks, and any failed memory allocation or image loading operation results in an error message to stderr.
    */

    int width, height, channels;
    unsigned char *img = stbi_load(fileName, &width, &height, &channels, 0);

    if (!img) {
        fprintf(stderr, "Error loading image\n");
        exit(0);
    }
    
    unsigned char ***imageArray = dynamicArray(height, width, channels);
    loadPixelData(imageArray, img, height, width, channels);

    int *binaryArray = convertStringToBinary(sentence);
    unsigned char *updatedPixels = updated1DPixelData(imageArray, height, width, channels);

    binaryArray = (int *)realloc(binaryArray, ((strlen(sentence) * 7) + 7 + 7) * sizeof(int)); 

    for (int index = (strlen(sentence) * 7) + 7; index < ((strlen(sentence) * 7) + 7 + 7); index++) binaryArray[index] = 0;

    // This line needs work, because this just changes the pixel to either a 0 or 1, and that annoying,
    // the potential fix is to convert the pixel to its binary value 
    for (int index = 0; index < ((strlen(sentence) * 7) + 7 + 7); index++) updatedPixels[index] = updatedPixels[index] + (unsigned char)binaryArray[index];

    createPng(outputFileName, updatedPixels, height, width);

    free3DDynamicArray(imageArray, height, width, channels);
    free(binaryArray);
    free(updatedPixels);
    stbi_image_free(img);
}


void decode(char *outputPictureFileName, char *outputTextFileName) {
    /*
    Summary:
        Decodes a message hidden within the least significant bits of each pixel in an encoded image and writes the decoded 
        text to an output text file. This function iteratively extracts bits from pixel values, reconstructs characters, and 
        terminates upon detecting a null character (0).

    Args:
        outputPictureFileName (char*): The name of the image file containing the encoded message.
        outputTextFileName (char*): The name of the output file where the decoded text will be saved.

    Return:
        This function does not return any value; it decodes the message and writes it to the specified text file.

    Note:
        The function allocates memory for the decoded message string and relies on the stb_image library to load and free 
        image data. If the image cannot be loaded or memory allocation fails, an error is printed to stderr. The caller 
        must handle any necessary memory management after calling this function.
    */

    int binaryPlaces[7] = {0};
    int width, height, channels;

    unsigned char *encodedImageArray = stbi_load(outputPictureFileName, &width, &height, &channels, 0);
    char *outputSentence = (char *)malloc((height * width * channels) * sizeof(char));

    FILE *outputDecodedFile = fopen(outputTextFileName, "w+");

    if (outputDecodedFile == NULL) {
        fprintf(stderr, "Error opening file: %s\n", outputTextFileName);
        free(outputSentence);
        return;
    }

    for (int index = 0; index < 7; index++) binaryPlaces[index] = (int)pow(2, 6-index);

    bool stillRunning = true;
    int imageIndex = 0;
    int charIndex = 0;

    while (stillRunning) {
        int bits[7] = {0};

        for (int index = 0; index < 7; index++) {
            bits[index] = encodedImageArray[imageIndex] % 2; // Gets the LSB of every pixel
                
            imageIndex++;
        }

        int charCode = 0;

        for (int i = 0; i < 7; i++) charCode += binaryPlaces[i] * bits[i];

        if (charCode == 0) stillRunning = false; // Null terminator check
        else outputSentence[charIndex++] = (char)(charCode);
    }

    outputSentence[charIndex] = '\0';

    fprintf(outputDecodedFile, "%s", outputSentence);

    fclose(outputDecodedFile);
    stbi_image_free(encodedImageArray);
    free(outputSentence);
}

char *readTextFromFile(char *inputTextFileName) {
    /*
    Summary:
        Reads the contents of a text file into a dynamically allocated string. The function opens the specified file,
        determines its size, and allocates a buffer to hold the entire content of the file, which it then reads into 
        the buffer.

    Args:
        inputTextFileName (char*): The name of the input text file to be read.

    Return:
        Returns a pointer to a dynamically allocated string containing the contents of the file. The caller is 
        responsible for freeing the allocated memory after use. If the file cannot be opened or memory allocation fails, 
        the function returns NULL.

    Note:
        If any error occurs (file not found, memory allocation failure), an appropriate error message is printed to 
        stderr, and the function safely exits by returning NULL.
    */


    FILE *inputFile = fopen(inputTextFileName, "r+");

    if (inputFile == NULL) {
        perror("Error opeaning file\n");
        return NULL;
    }

    fseek(inputFile, 0, SEEK_END);

    unsigned long long fileSize = (unsigned long long)ftell(inputFile);

    fseek(inputFile, 0, SEEK_SET);

    char *buffer = (char *)malloc((fileSize + 1) * sizeof(char));

    if (buffer == NULL) {
        perror("Memmory allocation failed\n");
        fclose(inputFile);
        return NULL;
    }

    fread(buffer, 1, fileSize, inputFile);
    fclose(inputFile);

    return buffer;
}
