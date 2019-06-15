//
// Created by lixin04 on 19-4-2.
//

#include "process.hpp"

void process::init(){
    img_data     = nullptr;
    img_data_new = nullptr;
    image_width  = 0;
    image_height = 0;
    BMP_bfOffBits = 0;
    BMP_biHeight = 0;
    taskH = 0;
    taskW = 0;
}

bool process::operation(const char* const inputFileNmae, const char* const outputFileName, const char& operation) {
    bool successed = false;
    BITMAPFILEHEADER * fileHeader = new BITMAPFILEHEADER;
    BITMAPINFOHEADER * infoHeader = new BITMAPINFOHEADER;
    readFromBMP(inputFileNmae,fileHeader,infoHeader);
    switch(operation){
        case 't':
            infoHeader->biBitCount = 24;
            img_data_new = new IMAGEPIXEL* [image_height];
            for(int i=0; i<image_height; ++i){
                img_data_new[i] = new IMAGEPIXEL[image_width];
            }
            for(int i = 0; i < image_height; ++i){
                for (int j=0;j<image_width;++j) {
                    img_data_new[i][j].rgbRed = img_data[i][j].rgbRed;
                    img_data_new[i][j].rgbBlue = img_data[i][j].rgbBlue;
                    img_data_new[i][j].rgbGreen = img_data[i][j].rgbGreen;
                }
            }
            successed = writeToBMP(outputFileName, fileHeader, infoHeader);
            break;
        case 's':
            scaling_image();
            infoHeader->biWidth = taskW;
            infoHeader->biHeight = taskH;
            successed = writeToBMP(outputFileName, fileHeader, infoHeader);
            break;
        case 'c':
//            successed = operation_convert();
            break;
        default:
            std::cout << "wrong opeartion" << std::endl;
            break;
    }
    return successed;
}

bool process::readFromBMP(const char* const fileName, BITMAPFILEHEADER * fileHeader, BITMAPINFOHEADER * infoHeader){
    init();
    printf("init OK\n");
    FILE* fp = fopen(fileName, "rb");
    if(fp== nullptr) return false;

    bool successed=false;
    do{
        if(1 != fread(fileHeader, sizeof(BITMAPFILEHEADER), 1, fp)) break;
        if(fileHeader->bfType!=0x4D42) break;

        if(1 != fread(infoHeader, sizeof(BITMAPINFOHEADER), 1, fp)) break;
//        if(infoHeader->biBitCount!=24 || infoHeader->biCompression!=0) break;

//        COLOR read_color;
//        if(infoHeader->biBitCount == 24)    read_color = RGB;
//        else if(infoHeader->biBitCount == 8)    read_color = GRAY;
//        else break;
        int width = infoHeader->biWidth;
        int height = infoHeader->biHeight < 0 ? (-infoHeader->biHeight) : infoHeader->biHeight;
//        if((width&7)!=0 || (height&7)!=0) break;

        int bmpSize = 0, read_width = 0;
        if(infoHeader->biBitCount == 24)    read_width = width * 3;
        else if(infoHeader->biBitCount == 8)    read_width = width;
        else break;
        bmpSize = read_width * height;

        auto buffer = new unsigned char[bmpSize];
        fseek(fp, fileHeader->bfOffBits, SEEK_SET);
        if(infoHeader->biHeight>0){
            for(int i=0; i<height; i++){
                if(read_width != fread(buffer+(height-1-i)*read_width, 1, read_width, fp)){
                    delete[] buffer;
                    return false;
                }
            }
        }
        else{
            if(read_width*height != fread(buffer, 1, read_width*height, fp)){
                delete[] buffer;
                return false;
            }
        }
        image_width = width;
        image_height = height;
        BMP_bfOffBits = fileHeader->bfOffBits;
        BMP_biHeight = infoHeader->biHeight;

        img_data = new IMAGEPIXEL* [image_height];
        for(int i=0; i<image_height; ++i){
            img_data[i] = new IMAGEPIXEL[image_width];
        }

        for(int i=0,j=0;i<image_width*image_height*3;i = i+3){
            j=i/3;
            img_data[j/image_width][j%image_width].rgbBlue = buffer[i];
            img_data[j/image_width][j%image_width].rgbGreen = buffer[i+1];
            img_data[j/image_width][j%image_width].rgbRed = buffer[i+2];
//            RGB_Buff[i%3][j/image_width][j%image_width]=buffer[i];
        }
        successed=true;

    }while(false);

    fclose(fp);

    printf("read image is %d * %d\n",image_height,image_width);

    return successed;
}

bool process::writeToBMP(const char* const fileName, BITMAPFILEHEADER * fileHeader, BITMAPINFOHEADER * infoHeader){
    bool successed = false;

    FILE* fp = fopen(fileName, "wb+");
    if(fp== nullptr) return false;

    do{
        COLOR save_color;
        if(infoHeader->biBitCount == 24)    save_color = RGB;
        else if(infoHeader->biBitCount == 8)    save_color = GRAY;
        else break;

        if(save_color == GRAY){
            auto rgbq= new IMAGEPIXEL[256];
            for(int i=0;i<256;i++) {
                rgbq[i].rgbBlue=rgbq[i].rgbGreen=rgbq[i].rgbRed= static_cast<unsigned char>(i);
                rgbq[i].rgbReserved=0;
            }
            infoHeader->biSizeImage= static_cast<unsigned int> (((infoHeader->biWidth * 3 + 3) / 4) * 4  * infoHeader->biHeight);
            fileHeader->bfOffBits = sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)+256*sizeof(IMAGEPIXEL);
            fileHeader->bfSize = fileHeader->bfOffBits + infoHeader->biSizeImage;
            fwrite(fileHeader,sizeof(BITMAPFILEHEADER),1,fp);
            fwrite(infoHeader,sizeof(BITMAPINFOHEADER),1,fp);
            fwrite(rgbq,sizeof(IMAGEPIXEL),256,fp);
        }
        else{
            infoHeader->biSizeImage= static_cast<unsigned int>(infoHeader->biWidth * infoHeader->biHeight * 3);
            fileHeader->bfOffBits = sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER);
            fileHeader->bfSize = fileHeader->bfOffBits + infoHeader->biSizeImage;
            fwrite(fileHeader,sizeof(BITMAPFILEHEADER),1,fp);
            fwrite(infoHeader,sizeof(BITMAPINFOHEADER),1,fp);
        }

        fseek(fp, fileHeader->bfOffBits, SEEK_SET);
        if(infoHeader->biHeight>0){
            if(save_color == RGB){
                for(int i=0; i<infoHeader->biHeight; i++){
                    for(int j=0;j<infoHeader->biWidth;j++){
                        fwrite(&img_data_new[infoHeader->biHeight-i-1][j].rgbBlue,1,1,fp);
                        fwrite(&img_data_new[infoHeader->biHeight-i-1][j].rgbGreen,1,1,fp);
                        fwrite(&img_data_new[infoHeader->biHeight-i-1][j].rgbRed,1,1,fp);
                    }
                }
            }
            else if(save_color == GRAY){
                for(int i=0; i<infoHeader->biHeight; i++){
                    for(int j=0;j<infoHeader->biWidth;j++){
                        fwrite(&img_data_new[infoHeader->biHeight-i-1][j].rgbReserved,1,1,fp);
                    }
                }
            }
        }
//        else{
//            fwrite(new_RGB_Buff, 3, infoHeader->biWidth*infoHeader->biHeight, fp);
//        }
        successed=true;
    }while(false);

    fclose(fp);
    printf("writed image is %d * %d\n",infoHeader->biHeight,infoHeader->biWidth);
    return successed;
}


inline double san_f(double x){
    if(x<0){
        x = -x;
    }else if(x>=0&&x<=1){
        return (1-2*pow(x,2)+pow(x,3));
    }else if(x>=2){
        return 0.0;
    }
    return 0.0;
}

//TODO there is some BUG, case small picture can not changed correct
bool process::scaling_image() {

    bool successed = false;
    do{
        std::cout << "please input the taskH:";
        std::cin >> taskH;
        std::cout << "please input the taskW:";
        std::cin >> taskW;

        img_data_new = new IMAGEPIXEL* [taskH];
        for(int i=0; i<taskH; ++i){
            img_data_new[i] = new IMAGEPIXEL[taskW];
        }

        for(int i=0; i< taskH; ++i) {
            double y = ((double) i + 0.5) * (double) image_height / (double) taskH - 0.5;
            for (int j = 0; j < taskW; ++j) {
                double x = ((double) j + 0.5) * (double) image_width / (double) taskW - 0.5;
                int iy = (int) y;
                int ix = (int) x;
                int u = (int) ((y - (int) y) * 1);
                int v = (int) ((x - (int) x) * 1);
                if (ix >= (image_width - 3)) {
                    ix = (image_width - 3);
                } else if (ix <= 1) {
                    ix = 1;
                }
                if (iy >= image_height - 3) {
                    iy = image_height - 3;
                } else if (iy <= 1) {
                    iy = 1;
                }
                double a1, a2, a3, a4;
                double re1, re2, re3, re4;
                double c1, c2, c3, c4;
                double save_data;
                a1 = san_f(1 + v);
                a2 = san_f(v);
                a3 = san_f(1 - v);
                a4 = san_f(2 - v);
                c1 = san_f(1 + u);
                c2 = san_f(u);
                c3 = san_f(1 - u);
                c4 = san_f(2 - u);
                //rgbRed
                re1 = a1 * (double) img_data[iy - 1][ix - 1].rgbRed
                      + a2 * (double) img_data[iy][ix - 1].rgbRed
                      + a3 * (double) img_data[iy + 1][ix - 1].rgbRed
                      + a4 * (double) img_data[iy + 2][ix - 1].rgbRed;

                re2 = a1 * (double) img_data[iy - 1][ix].rgbRed
                      + a2 * (double) img_data[iy][ix].rgbRed
                      + a3 * (double) img_data[iy + 1][ix].rgbRed
                      + a4 * (double) img_data[iy + 2][ix].rgbRed;

                re3 = a1 * (double) img_data[iy - 1][ix + 1].rgbRed
                      + a2 * (double) img_data[iy][ix + 1].rgbRed
                      + a3 * (double) img_data[iy + 1][ix + 1].rgbRed
                      + a4 * (double) img_data[iy + 2][ix + 1].rgbRed;

                re4 = a1 * (double) img_data[iy - 1][ix + 2].rgbRed
                      + a2 * (double) img_data[iy][ix + 2].rgbRed
                      + a3 * (double) img_data[iy + 1][ix + 2].rgbRed
                      + a4 * (double) img_data[iy + 2][ix + 2].rgbRed;
                save_data = re1 * c1 + re2 * c2 + re3 * c3 + re4 * c4;
                img_data_new[i][j].rgbRed = static_cast<unsigned char>(save_data);

                //regGreen
                re1 = a1 * (double) img_data[iy - 1][ix - 1].rgbGreen
                      + a2 * (double) img_data[iy][ix - 1].rgbGreen
                      + a3 * (double) img_data[iy + 1][ix - 1].rgbGreen
                      + a4 * (double) img_data[iy + 2][ix - 1].rgbGreen;

                re2 = a1 * (double) img_data[iy - 1][ix].rgbGreen
                      + a2 * (double) img_data[iy][ix].rgbGreen
                      + a3 * (double) img_data[iy + 1][ix].rgbGreen
                      + a4 * (double) img_data[iy + 2][ix].rgbGreen;

                re3 = a1 * (double) img_data[iy - 1][ix + 1].rgbGreen
                      + a2 * (double) img_data[iy][ix + 1].rgbGreen
                      + a3 * (double) img_data[iy + 1][ix + 11].rgbGreen
                      + a4 * (double) img_data[iy + 2][ix + 1].rgbGreen;

                re4 = a1 * (double) img_data[iy - 1][ix + 2].rgbGreen
                      + a2 * (double) img_data[iy][ix + 2].rgbGreen
                      + a3 * (double) img_data[iy + 1][ix + 2].rgbGreen
                      + a4 * (double) img_data[iy + 2][ix + 2].rgbGreen;
                save_data = re1 * c1 + re2 * c2 + re3 * c3 + re4 * c4;
                img_data_new[i][j].rgbGreen = static_cast<unsigned char>(save_data);

                //regBlue
                re1 = a1 * (double) img_data[iy - 1][ix - 1].rgbBlue
                      + a2 * (double) img_data[iy][ix - 1].rgbBlue
                      + a3 * (double) img_data[iy + 1][ix - 1].rgbBlue
                      + a4 * (double) img_data[iy + 2][ix - 1].rgbBlue;

                re2 = a1 * (double) img_data[iy - 1][ix].rgbBlue
                      + a2 * (double) img_data[iy][ix].rgbBlue
                      + a3 * (double) img_data[iy + 1][ix].rgbBlue
                      + a4 * (double) img_data[iy + 2][ix].rgbBlue;

                re3 = a1 * (double) img_data[iy - 1][ix + 1].rgbBlue
                      + a2 * (double) img_data[iy][ix + 1].rgbBlue
                      + a3 * (double) img_data[iy + 1][ix + 11].rgbBlue
                      + a4 * (double) img_data[iy + 2][ix + 1].rgbBlue;

                re4 = a1 * (double) img_data[iy - 1][ix + 2].rgbBlue
                      + a2 * (double) img_data[iy][ix + 2].rgbBlue
                      + a3 * (double) img_data[iy + 1][ix + 2].rgbBlue
                      + a4 * (double) img_data[iy + 2][ix + 2].rgbBlue;
                save_data = re1 * c1 + re2 * c2 + re3 * c3 + re4 * c4;
                img_data_new[i][j].rgbBlue = static_cast<unsigned char>(save_data);
            }
        }
    }while(false);
    return successed;
}