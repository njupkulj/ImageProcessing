//
// Created by lixin04 on 19-4-2.
//

#ifndef IMAGEPROCESSING_PROCESS_HPP
#define IMAGEPROCESSING_PROCESS_HPP

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <iostream>

class process{
public:
    bool operation(const char*inputFileNmae, const char*outputFileName, const char& operation);

private:
    // PNG header info
#pragma pack(2)
    typedef struct {
        unsigned short	bfType;			//the type of the file 2B
        unsigned int	bfSize;			//tell us how bing this file is,by Byte	 4B
        unsigned short	bfReserved1;	//reserve. must be 0 2B
        unsigned short	bfReserved2;	//reserve. must be 0 2B
        unsigned int	bfOffBits; 		//this value tell us the distance from begin to the data 4B
    } BITMAPFILEHEADER;

    typedef struct {
        unsigned int	biSize;			//
        int				biWidth;		//width of the picture
        int				biHeight;		//height of the picture. And if(biHeght>0)picture is reverse store;if<0,is
        unsigned short	biPlanes;		//
        unsigned short	biBitCount;		//1\4\8\24\32
        unsigned int	biCompression;	//
        unsigned int	biSizeImage;	//the size of image
        int				biXPelsPerMeter;//pixels-per-meter
        int				biYPelsPerMeter;//pixels-per-meter
        unsigned int	biClrUsed;
        unsigned int	biClrImportant;
    } BITMAPINFOHEADER;

    typedef struct{
        unsigned char rgbRed;
        unsigned char rgbGreen;
        unsigned char rgbBlue;
        unsigned char rgbReserved;
    }IMAGEPIXEL;
#pragma pack()

    int                 image_width;
    int                 image_height;
    IMAGEPIXEL**        img_data;
    IMAGEPIXEL**        img_data_new;
//    unsigned char***    RGB_Buff;
//    unsigned char***    new_RGB_Buff;
    unsigned int        BMP_bfOffBits;
    int                 BMP_biHeight;
    int                 taskH;
    int                 taskW;

    enum COLOR
    {
        GRAY=1, RGB = 3
    };

    void init();
    // 从BMP文件读
    bool readFromBMP(const char* fileName, BITMAPFILEHEADER * fileHeader, BITMAPINFOHEADER * infoHeader);//,IMAGEPIXEL **imagebuff);
    bool writeToBMP(const char* fileName, BITMAPFILEHEADER * fileHeader, BITMAPINFOHEADER * infoHeader);
    bool scaling_image();

};


#endif //IMAGEPROCESSING_PROCESS_HPP
