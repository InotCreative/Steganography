# Steganography: A C Program for Encoding and Decoding Messages in Images

# Introduction
This C program implements a steganography algorithm that hides text messages within digital images. It utilizes the least significant bit (LSB) technique to embed the binary representation of the message into the RGB channels of each pixel. The encoded message is then stored in a new image file.

# Features
Encodes and decodes text messages within images using the LSB technique. Supports various image formats (JPEG, PNG, BMP, etc.) thanks to the stb_image library. Efficiently handles memory allocation and deallocation. Provides clear error messages for invalid input or unexpected conditions.

# Dependencies
stb_image library: For loading and saving images.
libpng library: For creating PNG images.

# How It Works
Image Loading: The program loads the input image using the stb_image library.
Message Encoding: The text message is converted into binary format. Each bit is embedded into the least significant bit of a pixel's RGB channels.
Image Modification: The modified pixel data is used to create a new image file.
Message Decoding: The encoded image is loaded, and the hidden message is extracted by reading the least significant bits of each pixel.

### Compile the code:
```bash
gcc stegano.c -o stegano -lpng -lstb_image
```

### Encode A Message
```bash
./stegano -e {input_image_file} {text_file}
```

### Decode a message:
```bash
./stegano -d {input_image_file} {text_file}
```
