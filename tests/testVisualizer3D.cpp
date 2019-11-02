/* ----------------------------------------------------------------------------
 * Copyright 2017, Massachusetts Institute of Technology,
 * Cambridge, MA 02139
 * All Rights Reserved
 * Authors: Luca Carlone, et al. (see THANKS for the full author list)
 * See LICENSE for the license information
 * -------------------------------------------------------------------------- */

/**
 * @file   testVisualizer3D.cpp
 * @brief  test Visualizer3D
 * @author Antoni Rosinol
 */

#include <algorithm>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <random>

#include <gflags/gflags.h>
#include <glog/logging.h>
#include <gtest/gtest.h>

#include "kimera-vio/utils/ThreadsafeQueue.h"
#include "kimera-vio/visualizer/Visualizer3D.h"

DECLARE_string(test_data_path);

namespace VIO {

TEST(testVisualizer3D, DISABLED_visualizeMesh2D) {
  // Construct a frame from image name.
  FrameId id = 0;
  Timestamp tmp = 123;
  const std::string imgName =
      std::string(FLAGS_test_data_path) + "/chessboard_small.png";

  Frame f(
      id, tmp, CameraParams(), UtilsOpenCV::ReadAndConvertToGrayScale(imgName));
  f.extractCorners();
  for (int i = 0; i < f.keypoints_.size();
       i++) {  // populate landmark structure with fake data
    f.landmarks_.push_back(i);
  }

  // Compute mesh.
  const std::vector<cv::Vec6f>& mesh_2d = f.createMesh2D();

  // Visualize mesh.
  Visualizer3D::DisplayCallback null_callback;
  Visualizer3D::InputQueue input_queue("VisualizerInputQueue");
  Visualizer3D::OutputQueue output_queue("VisualizerOutputQueue");
  Visualizer3D visualizer(&input_queue,
                          &output_queue,
                          VisualizationType::NONE,
                          BackendType::Stereo,
                          null_callback,
                          false);
  EXPECT_NO_THROW(visualizer.visualizeMesh2D(mesh_2d, f.img_));
}

}  // namespace VIO
