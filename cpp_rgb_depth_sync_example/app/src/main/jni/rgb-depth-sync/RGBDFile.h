//
// Created by v-woki on 6/30/2017.
//

#ifndef CPP_RGB_DEPTH_SYNC_EXAMPLE_RGBD_FILE_H
#define CPP_RGB_DEPTH_SYNC_EXAMPLE_RGBD_FILE_H

#include <stdio.h>
#include <string>
#include <vector>
#include <ctime>
#include <sstream>
#include <GLES2/gl2.h>

class RGBDFile {
public:
    RGBDFile() {}
    RGBDFile(const char* file_path, GLsizei w, GLsizei h);
    ~RGBDFile();
    GLubyte* GetBGRPointer();
    GLubyte* GetAlphaPointer();
    void OutputBuffersToFiles();

private:
    void ReshapeOpenGL2Bitmap();
    void OutputBufferToFile(std::string file_path, std::vector<GLubyte>* pixels);

    std::string file_path_;
    GLsizei width_;
    GLsizei height_;

    std::vector<GLubyte> pixels_BGR;
    std::vector<GLubyte> pixels_Alpha;
};


#endif //CPP_RGB_DEPTH_SYNC_EXAMPLE_RGBD_FILE_H
