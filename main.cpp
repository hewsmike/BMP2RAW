/***************************************************************************
 *   Copyright (C) 2020 by Oliver Bock                                     *
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
#include <string>

int main(int arg c, char* argv[]) {
    // Exit conditions.
    const int SUCCESS(0);
    const int FAILURE(1);
    const int PIXEL_DATA_OFFSET_POS(10);
    const int IMAGE_WIDTH_POS(18);
    const int IMAGE_HEIGHT_POS(22);
    const int IMAGE_ROW_ALIGNMENT(4);
    const int BITS_PER_PIXEL_POS(28);
    const int RGB_SIZE(24);
    const int RGBA_SIZE(32);

    std::cout << "BMP2RAW pixel data extractor." << std::endl;
 
    // Must have correct number of command line arguments.
    if(argc != 2 ) {
        std::cout << "Must have only two command line parameters!" << std::endl;
        std::cout << "Usage : bmp2raw inputfilename outputfilename" << std::endl;
        exit(FAILURE);          
        }
        
    // argv[0] is this executable file's name.
    // The first parameter is the input file name.
    std::string input_file_name(argv[1]);
    std::cout << "Input file name is : " << input_file_name.c_str() << std::endl;
    
    // The second parameter is the output file name.
    std::string output_file_name(argv[2]);
    std::cout << "Output file name is : " << output_file_name.c_str() << std::endl;
    
    // Attempt to open the input file for reading
    // as a binary file using input stream semantics.
    ifstream inFile;
    inFile.open(input_file_name, ios::binary);
    
    // Check for and handle failure to open.
    if(!inFile.is_open()) {
        std::cout << "Failed to open input file !" << std::endl;
        exit(FAILURE);
        }
        
    // Check for expected filetype ie. 'BM'.
    char[2] fileType;
    inFile >> fileType;
    if((fileType[0] != 'B') || (fileType[1] != 'M')) {
        std::cout << "Input file not of BMP type !" << std::endl;
        inFile.close();
        exit(FAILURE);
        }
        
    // Get the file size.
    uint32_t fileSize;
    inFile >> fileSize;
    std::cout << "Input file size is " << fileSize << " bytes."
    
    // Skip reading in the four 'reserved' bytes, as we don't care what they are.
    // But get the offset to the pixel data.
    inFile.seekg(PIXEL_DATA_OFFSET_POS, ios::beg);
    uint32_t pixelDataOffset;
    inFile >> pixelDataOffset;
    
    // Next get the image width in pixels.
    inFile.seekg(IMAGE_WIDTH_POS, ios::beg);
    int32_t imageWidth;
    inFile >> imageWidth;
    
    // Calculate the number of padding bytes per row.
    int paddingBytes = imageWidth % IMAGE_ROW_ALIGNMENT;
    
    // Then the image height in pixels.
    int32_t imageHeight;
    inFile >> imageHeight;    
    
    // Next we need the number of bits per pixel.
    inFile.seekg(BITS_PER_PIXEL_POS, ios::beg);
    uint16_t bitsPerPixel;
    inFile >> bitsPerPixel;
    
    // Check that we have either 24 bit or 32 bit color depth.
    if((bitsPerPixel != RGB_SIZE) && (bitsPerPixel != RGBA_SIZE)) {
        std::cout << "Input file not of RGB nor RGBA format !" << std::endl;
        inFile.close();
        exit(FAILURE);
        }
    
    // Move to the offset in the input file, ready to read the pixel data.
    inFile.seekg(pixelDataOffset, ios::beg); 
    
    // Attempt to open the output file for writing
    // as a binary file using output stream semantics.
    // We will deliberately truncate the file to zero if
    // already exists !!
    ofstream outFile;
    outFile.open(output_file_name, ios::binary | ios::trunc);
    
    // Check for and handle failure to open.
    if(!outFile.is_open()) {
        std::cout << "Failed to open output file !" << std::endl;
        inFile.close();
        exit(FAILURE);
        }
    
    // So we now have the input file pointing at the first byte of
    // pixel data and the empty output file ready to write.
    // Go along the rows.
    for(int row = 0; row < imageHeight; ++row) {
        // Go along the pixels. 
        for(int pixel = 0; pixel < imageWidth; ++pixel) {
            // Copy the input stream to the output stream, byte by byte.
            char temp;
            // Get the red component.
            inFile >> temp;
            outFile << temp;
            // Get the green component.
            inFile >> temp;
            outFile << temp;
            // Get the blue component.
            inFile >> temp;
            outFile << temp;
            // But discard any alpha values.
            if(bitsPerPixel == RGBA_SIZE) {
                char discard;
                inFile >> discard;
                }
            }
        // Toss away any padding bytes at the end of the row.
        for(int padding = 0; padding < paddingBytes; ++padding) {
            char discard;
            inFile >> discard;
            }
        }
        
    // Close the input and output files.
    inFile.close();
    outFile.close();
    
    return SUCCESS;
    }
