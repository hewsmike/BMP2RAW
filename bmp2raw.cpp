/***************************************************************************
 *   Copyright (C) 2020 by Mike Hewson                                     *
 *   hewsmike[AT]iinet.net.au                                              *
 *                                                                         *
 *   BMP2RAW is free software: you can redistribute it and/or modify       *
 *   it under the terms of the GNU General Public License as published     *
 *   by the Free Software Foundation, version 2 of the License.            *
 *                                                                         *
 *   BMP2RAW is distributed in the hope that it will be useful,            *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the          *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   A copy of the GNU General Public License is available at              *
 *   http://www.gnu.org/licenses/                                          *
 *                                                                         *
 ***************************************************************************/

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <istream>
#include <ostream>
#include <string>

union twoBytes {
        char as_char[2];
        uint16_t as_uint;
        };

union fourBytes {
        char as_char[4];
        uint32_t as_uint;
        };

int main(int argc, char** argv) {
    // Exit conditions.
    const int SUCCESS(0);
    const int FAILURE(1);

    const int FILE_SIZE_POS(2);
    const int PIXEL_DATA_OFFSET_POS(10);
    const int IMAGE_WIDTH_POS(18);
    const int IMAGE_HEIGHT_POS(22);
    const int IMAGE_ROW_ALIGNMENT(4);
    const int BITS_PER_PIXEL_POS(28);
    const int GRAYSCALE_BPP(8);
    const int RGB_BPP(24);
    const int RGBA_BPP(32);

    std::cout << "BMP2RAW pixel data extractor." << std::endl;

    // Must have correct number of command line arguments.
    if(argc != 3 ) {
        std::cout << "Must have two command line parameters!" << std::endl;
        std::cout << "Usage : bmp2raw inputfilename outputfilename" << std::endl;
        exit(FAILURE);
        }

    // argv[0] is this executable file's name.
    // The first parameter is the input file name.
    std::cout << "Input file name is : " << argv[1] << std::endl;

    // The second parameter is the output file name.
    std::cout << "Output file name is : " << argv[2] << std::endl;

    // Attempt to open the input file for reading
    // as a binary file using input stream semantics.
    std::ifstream inFile(argv[1], std::ios::in | std::ios::binary);

    // Check for and handle failure to open.
    if(inFile.bad()) {
        std::cout << "Failed to open input file !" << std::endl;
        exit(FAILURE);
        }

    // Check for expected filetype ie. 'BM'.
    char fileType[2];
    inFile.read(&fileType[0], 1);
    inFile.read(&fileType[1], 1);
    std::cout << "File type signature is '" << fileType[0]
              << fileType[1] << "'" << std::endl;

    // Get the file size.
    inFile.seekg(FILE_SIZE_POS, std::ios::beg);
    fourBytes thisFileSize;
    inFile.read(thisFileSize.as_char, sizeof(fourBytes));
    std::cout << "Input file size is " << thisFileSize.as_uint << " bytes." << std::endl;

    // Skip reading in the four 'reserved' bytes, as we don't care what they are.
    // But get the offset to the pixel data.
    inFile.seekg(PIXEL_DATA_OFFSET_POS, std::ios::beg);
    fourBytes pixelDataOffset;
    inFile.read(pixelDataOffset.as_char, sizeof(fourBytes));
    std::cout << "Pixel data offset " << pixelDataOffset.as_uint << " bytes." << std::endl;

    // Next get the image width in pixels.
    inFile.seekg(IMAGE_WIDTH_POS, std::ios::beg);
    twoBytes imageWidth;
    inFile.read(imageWidth.as_char, sizeof(twoBytes));
    std::cout << "Image width is " << imageWidth.as_uint << " pixels." << std::endl;

    // Then the image height in pixels.
    inFile.seekg(IMAGE_HEIGHT_POS, std::ios::beg);
    twoBytes imageHeight;
    inFile.read(imageHeight.as_char, sizeof(twoBytes));
    std::cout << "Image height is " << imageHeight.as_uint << " pixels." << std::endl;

    // Next we need the number of bits per pixel.
    inFile.seekg(BITS_PER_PIXEL_POS, std::ios::beg);
    twoBytes bitsPerPixel;
    inFile.read(bitsPerPixel.as_char, sizeof(twoBytes));
    std::cout << "Bits per pixel is " << bitsPerPixel.as_uint << std::endl;

    // Check that we have either 24 bit or 32 bit color depth.
    if((bitsPerPixel.as_uint != RGB_BPP) &&
       (bitsPerPixel.as_uint != RGBA_BPP) &&
       (bitsPerPixel.as_uint != GRAYSCALE_BPP)) {
        std::cout << "Input file not of RGB nor RGBA format !" << std::endl;
        inFile.close();
        exit(FAILURE);
        }

    uint paddingBytes = 0;
    if(bitsPerPixel.as_uint == RGB_BPP) {
        // Calculate the number of padding bytes per row.
        paddingBytes = (imageWidth.as_uint * 3) % IMAGE_ROW_ALIGNMENT;
        }
    else if(bitsPerPixel.as_uint == GRAYSCALE_BPP) {
        // Calculate the number of padding bytes per row.
        paddingBytes = imageWidth.as_uint % IMAGE_ROW_ALIGNMENT;
        }

    std::cout << "Padding per pixel row is " << paddingBytes << " bytes." << std::endl;

    // Move to the offset in the input file, ready to read the pixel data.
    inFile.seekg(pixelDataOffset.as_uint, std::ios::beg);

    // Attempt to open the output file for writing
    // as a binary file using output stream semantics.
    // We will deliberately truncate the file to zero if
    // already exists !!
    std::ofstream outFile(argv[2],  std::ios::out | std::ios::binary | std::ios::trunc);

    // Check for and handle failure to open.
    if(outFile.bad()) {
        std::cout << "Failed to open output file !" << std::endl;
        inFile.close();
        exit(FAILURE);
        }

    // So we now have the input file pointing at the first byte of
    // pixel data and the empty output file ready to write.
    // Go along the rows.
    for(uint row = 0; row < imageHeight.as_uint; ++row) {
        // Go along the pixels.
        for(uint pixel = 0; pixel < imageWidth.as_uint; ++pixel) {
            // Copy the input stream to the output stream, byte by byte.
            if(bitsPerPixel.as_uint == GRAYSCALE_BPP){
                // We're doing single component BMP.
                char temp_gray;
                inFile.read(&temp_gray, sizeof(temp_gray));
                outFile.write(&temp_gray, sizeof(temp_gray));
                }
            else {
                // We're doing three or four component BMP.
                // Check for any alpha values.
                char alpha;
                if(bitsPerPixel.as_uint == RGBA_BPP) {
                    inFile.read(&alpha, sizeof(alpha));
                    outFile.write(&alpha, sizeof(alpha));
                    }

                char temp_red;
                char temp_green;
                char temp_blue;

                // Get the color components.
                inFile.read(&temp_red, sizeof(temp_red));
                inFile.read(&temp_green, sizeof(temp_green));
                inFile.read(&temp_blue, sizeof(temp_blue));

                outFile.write(&temp_red, sizeof(temp_red));
                outFile.write(&temp_green, sizeof(temp_green));
                outFile.write(&temp_blue, sizeof(temp_blue));
                }
            }
        // Toss away any padding bytes at the end of the row.
        for(uint padding = 0; padding < paddingBytes; ++padding) {
            char discard;
            inFile.read(&discard, 1);
            }
        }

    // Close the input and output files.
    inFile.close();
    outFile.close();

    return SUCCESS;
    }
