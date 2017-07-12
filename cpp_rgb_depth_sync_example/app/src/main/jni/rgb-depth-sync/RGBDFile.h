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
#include <iomanip>
#include <GLES2/gl2.h>
#include <tango_client_api.h>
#include <sys/stat.h>

class RGBDFile {
public:
    RGBDFile() {}
    RGBDFile(GLsizei w, GLsizei h);
    ~RGBDFile();
    GLubyte* GetBGRPointer();
    GLubyte* GetAlphaPointer();
    void OutputBuffersToFiles(TangoPoseData pose_data);

private:
    void ReshapeOpenGL2Bitmap();
    void OutputBufferToFile(std::string file_name, std::vector<GLubyte>* pixels);
    void OutputPoseToFile(std::string file_name, TangoPoseData pose_data);

    GLsizei width_;
    GLsizei height_;
    std::string file_path_;
    size_t capture_counter_;

    std::vector<GLubyte> pixels_BGR;
    std::vector<GLubyte> pixels_Alpha;
};


#endif //CPP_RGB_DEPTH_SYNC_EXAMPLE_RGBD_FILE_H
