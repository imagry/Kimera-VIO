/* ----------------------------------------------------------------------------
 * Copyright 2017, Massachusetts Institute of Technology,
 * Cambridge, MA 02139
 * All Rights Reserved
 * Authors: Luca Carlone, et al. (see THANKS for the full author list)
 * See LICENSE for the license information
 * -------------------------------------------------------------------------- */

/**
 * @file   InitializationBackEnd.h
 * @brief  Derived class from VioBackEnd for bundle adjustment and alignment
 * for the online initialization
 * @author Antoni Rosinol
 * @author Sandro Berchier
 * @author Luca Carlone
 */

#pragma once

#include <queue>

#include "kimera-vio/backend/VioBackEnd.h"
#include "kimera-vio/initial/InitializationBackEnd-definitions.h"
#include "kimera-vio/utils/ThreadsafeQueue.h"
#include "kimera-vio/utils/Timer.h"

namespace VIO {

class InitializationBackEnd : public VioBackEnd {
 public:
  KIMERA_POINTER_TYPEDEFS(InitializationBackEnd);
  KIMERA_DELETE_COPY_CONSTRUCTORS(InitializationBackEnd);
  EIGEN_MAKE_ALIGNED_OPERATOR_NEW
  typedef ThreadsafeQueue<InitializationInputPayload::UniquePtr>::InternalQueue
      InitializationQueue;
  /* ------------------------------------------------------------------------ */
  // Create and initialize InitializationBackEnd, without initiaing pose.
  InitializationBackEnd(const gtsam::Pose3 &B_Pose_leftCam,
                        const StereoCalibPtr& stereo_calibration,
                        const VioBackEndParams &backend_params,
                        const bool log_output = false);

  /* ------------------------------------------------------------------------ */
  virtual ~InitializationBackEnd() = default;

 public:
  /* ------------------------------------------------------------------------ */
  // Perform Bundle-Adjustment and initial gravity alignment
  bool bundleAdjustmentAndGravityAlignment(InitializationQueue& output_frontend,
                                           gtsam::Vector3* gyro_bias,
                                           gtsam::Vector3* g_iter_b0,
                                           gtsam::NavState* init_navstate);

 public:
  /* ------------------------------------------------------------------------ */
  std::vector<gtsam::Pose3> addInitialVisualStatesAndOptimize(
      const std::vector<VioBackEndInputPayload::UniquePtr>& input);

 private:
  /* ------------------------------------------------------------------------ */
  // Adding of states for bundle adjustment used in initialization.
  // [in] timestamp_kf_nsec, keyframe timestamp.
  // [in] status_smart_stereo_measurements_kf, vision data.
  // [in] stereo_ransac_body_pose, inertial data.
  virtual void addInitialVisualState(
      const Timestamp& timestamp_kf_nsec,
      const StatusStereoMeasurements& status_smart_stereo_measurements_kf,
      std::vector<Plane>* planes,
      boost::optional<gtsam::Pose3> stereo_ransac_body_pose,
      const int verbosity);

  /* ------------------------------------------------------------------------ */
  // TODO(Toni): do not return vectors...
  std::vector<gtsam::Pose3> optimizeInitialVisualStates(
      const Timestamp &timestamp_kf_nsec,
      const FrameId &cur_id,
      const size_t &max_extra_iterations,
      const std::vector<size_t> &extra_factor_slots_to_delete =
          std::vector<size_t>(),
      const int verbosity = 0);
};

}  // namespace VIO
