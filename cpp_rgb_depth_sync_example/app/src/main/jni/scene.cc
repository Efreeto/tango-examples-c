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
  if (capture_) {
      CaptureImage();
      capture_ = false;
  }
}

void Scene::InitializeGL() { camera_texture_drawable_.InitializeGL(); }

void Scene::SetDepthAlphaValue(float alpha) {
  camera_texture_drawable_.SetBlendAlpha(alpha);
}

void Scene::CaptureImage() {
    LOGI("CaptureImage");

    GLuint width = viewport_width_;
    GLuint height = viewport_height_;

    GLubyte* pixels_RGB = new GLubyte[3 * width * height];
    glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, pixels_RGB);

    size_t i, j, cur;
    FILE *file = fopen("/sdcard/hello.ppm", "w");
    fprintf(file, "P3\n%d %d\n%d\n", width, height, 255);
    for (i = 0; i < height; i++) {
        for (j = 0; j < width; j++) {
            cur = 3 * ((height - i - 1) * width + j);
            fprintf(file, "%3d %3d %3d ", pixels_RGB[cur], pixels_RGB[cur + 1], pixels_RGB[cur + 2]);
        }
        fprintf(file, "\n");
    }
    fclose(file);
}

}  // namespace rgb_depth_sync
