# BMP2RAW
A small linux utility program to convert BMP pixel maps to raw data files.
The BMP format is common and for me there is a need to extract per pixel values 
from a given BMP file and convert 24 bit (RGB) or 32 bit (RGBA) color depth
information into a simpler format. Usage :

bmp2raw inputfile outputfile

The output file will always be overwritten if existing already.

The utility will fail if:
    - file operations don't succeed.
    - too many/few command line parameters.
    - corrupted input file format.
    - bits perpixel neither 24 nor 32

The output file will contain a stream of bytes of the format :
    - a short header of TEN bytes
    - RGB repeating ( ie. red, green, blue ) or
    - RGBA repeating ( ie. red, green, blue, alpha )

The header has the format :
    - 4 bytes ( signed integer representing the width of image in pixels )
    - 4 bytes ( signed integer representing the height of image in pixels )
    - 2 bytes ( unsigned integer with value either 24 or 32 )
