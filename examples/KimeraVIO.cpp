/* ----------------------------------------------------------------------------
 * Copyright 2017, Massachusetts Institute of Technology,
 * Cambridge, MA 02139
 * All Rights Reserved
 * Authors: Luca Carlone, et al. (see THANKS for the full author list)
 * See LICENSE for the license information
 * -------------------------------------------------------------------------- */

/**
 * @file   SparkVio.cpp
 * @brief  Example of VIO pipeline.
 * @author Antoni Rosinol
 * @author Luca Carlone
 */

#include <future>
#include <memory>
#include <utility>

#include <gflags/gflags.h>
#include <glog/logging.h>

#include "kimera-vio/datasource/ETH_parser.h"
#include "kimera-vio/datasource/KittiDataSource.h"
#include "kimera-vio/frontend/StereoImuSyncPacket.h"
#include "kimera-vio/logging/Logger.h"
#include "kimera-vio/pipeline/Pipeline.h"
#include "kimera-vio/utils/Statistics.h"
#include "kimera-vio/utils/Timer.h"

DEFINE_bool(parallel_run, false, "Run parallelized pipeline.");
DEFINE_int32(dataset_type,
             0,
             "Type of parser to use:\n"
             "0: EuRoC\n"
             "1: Kitti");

int main(int argc, char* argv[]) {
  // Initialize Google's flags library.
  google::ParseCommandLineFlags(&argc, &argv, true);
  // Initialize Google's logging library.
  google::InitGoogleLogging(argv[0]);

  // Build dataset parser.
  std::unique_ptr<VIO::DataProviderInterface> dataset_parser;
  switch (FLAGS_dataset_type) {
    case 0: {
      dataset_parser = VIO::make_unique<VIO::ETHDatasetParser>();
    } break;
    case 1: {
      dataset_parser = VIO::make_unique<VIO::KittiDataProvider>();
    } break;
    default: {
      LOG(FATAL) << "Unrecognized dataset type: " << FLAGS_dataset_type << "."
                 << " 0: EuRoC, 1: Kitti.";
    }
  }
  CHECK(dataset_parser);
  dataset_parser->pipeline_params_.parallel_run_ = FLAGS_parallel_run;

  VIO::Pipeline vio_pipeline(dataset_parser->pipeline_params_);

  // Register callback to vio pipeline.
  dataset_parser->registerVioCallback(
      std::bind(&VIO::Pipeline::spin, &vio_pipeline, std::placeholders::_1));

  // Spin dataset.
  auto tic = VIO::utils::Timer::tic();
  bool is_pipeline_successful = false;
  if (FLAGS_parallel_run) {
    auto handle = std::async(std::launch::async,
                             &VIO::DataProviderInterface::spin,
                             std::move(dataset_parser));
    auto handle_pipeline = std::async(std::launch::async,
                                      &VIO::Pipeline::shutdownWhenFinished,
                                      &vio_pipeline);
    vio_pipeline.spinViz();
    is_pipeline_successful = handle.get();
    handle_pipeline.get();
  } else {
    is_pipeline_successful = dataset_parser->spin();
  }

  // Output stats.
  auto spin_duration = VIO::utils::Timer::toc(tic);
  LOG(WARNING) << "Spin took: " << spin_duration.count() << " ms.";
  LOG(INFO) << "Pipeline successful? "
            << (is_pipeline_successful ? "Yes!" : "No!");
  VIO::utils::Statistics::WriteAllSamplesToCsvFile("StatisticsVIO.csv");

  if (is_pipeline_successful) {
    // Log overall time of pipeline run.
    VIO::PipelineLogger logger;
    logger.logPipelineOverallTiming(spin_duration);
  }

  return is_pipeline_successful ? EXIT_SUCCESS : EXIT_FAILURE;
}
