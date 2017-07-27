/*
 * Copyright 2014 Google Inc. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "tango-gl/conversions.h"

#include "rgb-depth-sync/scene.h"

namespace rgb_depth_sync {

Scene::Scene() {
  camera_texture_drawable_.SetBlendAlpha(0.0f);
  rgbd_file_ready_ = false;
}

Scene::~Scene() {}

void Scene::SetupViewPort(int screen_width, int screen_height) {
  if (screen_height == 0 || screen_width == 0) {
    LOGE("The Scene received an invalid height of 0 in SetupViewPort.");
    return;
  }

  viewport_width_ = screen_width;
  viewport_height_ = screen_height;
  glViewport(0, 0, screen_width, screen_height);
}

void Scene::Clear() {
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
}

// We'll render the scene from a pose.
void Scene::Render(GLuint color_texture, GLuint depth_texture,
                   TangoSupportRotation camera_to_display_rotation) {
  glViewport(0, 0, viewport_width_, viewport_height_);
  if (color_texture == 0 || depth_texture == 0) {
    return;
  }

  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
  camera_texture_drawable_.SetColorTextureId(color_texture);
  camera_texture_drawable_.SetDepthTextureId(depth_texture);
  camera_texture_drawable_.RenderImage(camera_to_display_rotation);
  if (capture_mode_) {
    CaptureImage();
  }
}

void Scene::InitializeGL() { camera_texture_drawable_.InitializeGL(); }

void Scene::SetDepthAlphaValue(float alpha) {
  camera_texture_drawable_.SetBlendAlpha(alpha);
}

void Scene::ResetCapture() {
    TangoService_resetMotionTracking();
    rgbd_file_ = RGBDFile(viewport_width_, viewport_height_);
    rgbd_file_ready_ = true;
}

void Scene::CaptureImage() {
    /* I tried to capture both color and depth images on one go.
//     * kind of like https://github.com/apitrace/apitrace/commit/21ffc8ca702feb86e4663a41da9bbfef8f16ff09
//     * But I couldn't do it */
//    if (capture_mode_ == 1) {
//        LOGI("CaptureImage");
//        if (!rgbd_file_ready_) {
//            ResetCapture();
//        }
//
//        GLint texture = 0;
//        glGetIntegerv(GL_NONE, &texture);
//        LOGI("GL_NONE: [%d]", texture);
//        glGetIntegerv(GL_TEXTURE_BINDING_2D, &texture);
//        LOGI("GL_TEXTURE_BINDING_2D: [%d]", texture);
//        glGetIntegerv(GL_TEXTURE_BINDING_CUBE_MAP, &texture);
//        LOGI("GL_TEXTURE_BINDING_CUBE_MAP: [%d]", texture);
//        glGetIntegerv(GL_TEXTURE_BINDING_EXTERNAL_OES, &texture);
//        LOGI("GL_TEXTURE_BINDING_EXTERNAL_OES: [%d]", texture);
//
//        GLint prev_fbo = 0;
//        GLuint fbo = 0;
//        GLenum status;
//        glGetIntegerv(GL_FRAMEBUFFER_BINDING, &prev_fbo);
//        glGenFramebuffers(1, &fbo);
//        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
//
//        glActiveTexture(GL_TEXTURE0);
//        glBindTexture(GL_TEXTURE_EXTERNAL_OES, camera_texture_drawable_.GetColorTextureId());
//        LOGI("Color Texture Id: [%d]", camera_texture_drawable_.GetColorTextureId());
//        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_EXTERNAL_OES, camera_texture_drawable_.GetColorTextureId(), 0);
//        status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
//        if (status != GL_FRAMEBUFFER_COMPLETE) {
//            capture_mode_ = 0;
//            LOGE("status");
//            return;
//        }
//        GLubyte* pixels_RGB = rgbd_file_.GetBGRPointer();
//        glReadPixels(0, 0, viewport_width_, viewport_height_, GL_RGB, GL_UNSIGNED_BYTE, pixels_RGB);
//
//        glActiveTexture(GL_TEXTURE1);
//        glBindTexture(GL_TEXTURE_2D, camera_texture_drawable_.GetDepthTextureId());
//        LOGI("Depth Texture Id: [%d]", camera_texture_drawable_.GetDepthTextureId());
//        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, camera_texture_drawable_.GetDepthTextureId(), 0);
//        status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
//        if (status != GL_FRAMEBUFFER_COMPLETE) {
//            capture_mode_ = 0;
//            LOGE("status");
//            return;
//        }
//        GLubyte *pixels_Alpha = rgbd_file_.GetAlphaPointer();
//        glReadPixels(0, 0, viewport_width_, viewport_height_, GL_RGB, GL_UNSIGNED_BYTE, pixels_Alpha);
//
//        TangoCoordinateFramePair frames_of_reference;
//        frames_of_reference.base = TANGO_COORDINATE_FRAME_START_OF_SERVICE;
//        frames_of_reference.target = TANGO_COORDINATE_FRAME_DEVICE;
//        TangoPoseData pose_data;
//        TangoService_getPoseAtTime(0.0, frames_of_reference, &pose_data);
//
//        rgbd_file_.OutputBuffersToFiles(pose_data);
//
//        glBindFramebuffer(GL_FRAMEBUFFER, prev_fbo);
//        glDeleteFramebuffers(1, &fbo);
//
//        capture_mode_ = 0;
//    }
    if (capture_mode_ == 1) {
        LOGI("CaptureImage");
        prev_alpha_value_ = camera_texture_drawable_.GetBlendAlpha();

        if (!rgbd_file_ready_) {
            ResetCapture();
        }

        SetDepthAlphaValue(0.0);
        capture_mode_ = 2;
    }
    else if (capture_mode_ == 2) {
        GLubyte* pixels_RGB = rgbd_file_.GetBGRPointer();
        glReadPixels(0, 0, viewport_width_, viewport_height_, GL_RGB, GL_UNSIGNED_BYTE, pixels_RGB);

        SetDepthAlphaValue(1.0);
        capture_mode_ = 3;
    }
    else if (capture_mode_ == 3) {
        GLubyte* pixels_Alpha = rgbd_file_.GetAlphaPointer();
        glReadPixels(0, 0, viewport_width_, viewport_height_, GL_RGB, GL_UNSIGNED_BYTE, pixels_Alpha);

        TangoCoordinateFramePair frames_of_reference;
        frames_of_reference.base = TANGO_COORDINATE_FRAME_START_OF_SERVICE;
        frames_of_reference.target = TANGO_COORDINATE_FRAME_DEVICE;
        TangoPoseData pose_data;
        TangoService_getPoseAtTime(0.0, frames_of_reference, &pose_data);

        rgbd_file_.OutputBuffersToFiles(pose_data);

        SetDepthAlphaValue(prev_alpha_value_);
        capture_mode_ = 0;
    }
    else {
        capture_mode_ = 1;
    }
}

}  // namespace rgb_depth_sync
