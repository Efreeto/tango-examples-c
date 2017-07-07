//
// Created by v-woki on 6/30/2017.
//

#include "rgb-depth-sync/RGBDFile.h"

RGBDFile::RGBDFile(const char* file_path, GLsizei w, GLsizei h) {
    file_path_ = file_path;
    width_ = w;
    height_ = h;

    pixels_BGR.resize(3 * w * h);
    pixels_Alpha.resize(3 * w * h);
}

RGBDFile::~RGBDFile() {
    pixels_BGR.clear();
    pixels_Alpha.clear();
}

GLubyte* RGBDFile::GetBGRPointer() {
    return &pixels_BGR[0];
}

GLubyte* RGBDFile::GetAlphaPointer() {
    return &pixels_Alpha[0];
}

void RGBDFile::OutputBuffersToFiles() {
    ReshapeOpenGL2Bitmap();
    time_t t = time(0);
    struct tm * now = localtime( & t );
    std::stringstream ss;
    ss << (now->tm_year + 1900) << '-'
         << (now->tm_mon + 1) << '-'
         << now->tm_mday << '-'
         << now->tm_hour << ':'
         << now->tm_min << ':'
         << now->tm_sec;
    std::string file_name = file_path_ + ss.str();
    OutputBufferToFile(file_name+"_rgb.bmp", &pixels_BGR);
    OutputBufferToFile(file_name+"_alpha.bmp", &pixels_Alpha);
}

/* https://stackoverflow.com/a/2654860/2680660 */
void RGBDFile::OutputBufferToFile(std::string file_path, std::vector<GLubyte>* pixels) {
    FILE *f;
    int filesize = 54 + 3*width_*height_;

    unsigned char bmpfileheader[14] = {'B','M', 0,0,0,0, 0,0, 0,0, 54,0,0,0};
    unsigned char bmpinfoheader[40] = {40,0,0,0, 0,0,0,0, 0,0,0,0, 1,0, 24,0};
    unsigned char bmppad[3] = {0,0,0};

    bmpfileheader[ 2] = (unsigned char)(filesize    );
    bmpfileheader[ 3] = (unsigned char)(filesize>> 8);
    bmpfileheader[ 4] = (unsigned char)(filesize>>16);
    bmpfileheader[ 5] = (unsigned char)(filesize>>24);

    bmpinfoheader[ 4] = (unsigned char)(width_    );
    bmpinfoheader[ 5] = (unsigned char)(width_>> 8);
    bmpinfoheader[ 6] = (unsigned char)(width_>>16);
    bmpinfoheader[ 7] = (unsigned char)(width_>>24);
    bmpinfoheader[ 8] = (unsigned char)(height_    );
    bmpinfoheader[ 9] = (unsigned char)(height_>> 8);
    bmpinfoheader[10] = (unsigned char)(height_>>16);
    bmpinfoheader[11] = (unsigned char)(height_>>24);

    f = fopen(file_path.c_str(),"wb");
    fwrite(bmpfileheader,1,14,f);
    fwrite(bmpinfoheader,1,40,f);
    for(size_t i=0; i<height_; i++)
    {
        fwrite(&(*pixels)[0]+(width_*(height_-i-1)*3),3,width_,f);
        fwrite(bmppad,1,(4-(width_*3)%4)%4,f);
    }
    fclose(f);
}

void RGBDFile::ReshapeOpenGL2Bitmap() {
    for (size_t x=0; x<width_; x++) {
        for (size_t y=0; y<height_; y++) {

            size_t index = ( y*width_ + x ) * 3;

            // change RGB to BGR
            GLubyte temp = pixels_BGR[index+0];
            pixels_BGR[index+0] = pixels_BGR[index+2];
            pixels_BGR[index+2] = temp;

            // flip y-axis
            if (y < height_/2) {
                GLubyte temp2[6] = {pixels_BGR[index+0],
                                    pixels_BGR[index+1],
                                    pixels_BGR[index+2],
                                    pixels_Alpha[index+0],
                                    pixels_Alpha[index+1],
                                    pixels_Alpha[index+2]
                };
                size_t new_index = ((height_ - y - 1)*width_ + x) * 3;

                pixels_BGR[index+0] = pixels_BGR[new_index+0];
                pixels_BGR[index+1] = pixels_BGR[new_index+1];
                pixels_BGR[index+2] = pixels_BGR[new_index+2];
                pixels_Alpha[index+0] = pixels_Alpha[new_index+0];
                pixels_Alpha[index+1] = pixels_Alpha[new_index+1];
                pixels_Alpha[index+2] = pixels_Alpha[new_index+2];
                pixels_BGR[new_index+0] = temp2[0];
                pixels_BGR[new_index+1] = temp2[1];
                pixels_BGR[new_index+2] = temp2[2];
                pixels_Alpha[new_index+0] = temp2[3];
                pixels_Alpha[new_index+1] = temp2[4];
                pixels_Alpha[new_index+2] = temp2[5];
            }
        }
    }
}