
#ifndef JPEGUTILITIES_H
#define JPEGUTILITIES_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <setjmp.h>
#include <vector>

extern "C"
{
#include "jpeglib.h"
}


using namespace std;
int get_exif_orient (const char *filename);
int GetJpegImage ( const char* strFilename,unsigned char *&ImageColor,int& width,int& height,int& colornum,int &orient );
#endif
