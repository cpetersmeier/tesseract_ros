/**
 * @file tesseract_planning_server.h
 * @brief A planning server with a default set of motion planners
 *
 * @author Levi Armstrong
 * @date August 18, 2020
 * @version TODO
 * @bug No known bugs
 *
 * @copyright Copyright (c) 2020, Southwest Research Institute
 *
 * @par License
 * Software License Agreement (Apache License)
 * @par
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * http://www.apache.org/licenses/LICENSE-2.0
 * @par
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef TESSERACT_ROS_TESSERACT_PLANNING_SERVER_H
#define TESSERACT_ROS_TESSERACT_PLANNING_SERVER_H

#include <tesseract_common/macros.h>
TESSERACT_COMMON_IGNORE_WARNINGS_PUSH
#include <deque>
#include <shared_mutex>
#include <ros/ros.h>
#include <actionlib/server/simple_action_server.h>
#include <tf2_ros/buffer.h>
#include <tf2_ros/transform_listener.h>
#include <tesseract_msgs/GetMotionPlanAction.h>
TESSERACT_COMMON_IGNORE_WARNINGS_POP

#include <tesseract_environment/environment_monitor.h>
#include <tesseract_process_managers/core/process_planning_server.h>

namespace tesseract_planning_server
{
class TesseractPlanningServer
{
public:
  static const std::string DEFAULT_PLANNER_PROFILES_PARAM;  // "/tesseract_planner_profiles"

  static const std::string DEFAULT_GET_MOTION_PLAN_ACTION;  // "/tesseract_get_motion_plan"

  TesseractPlanningServer(const std::string& robot_description,
                          std::string name,
                          size_t n = std::thread::hardware_concurrency());

  TesseractPlanningServer(tesseract_environment::Environment::UPtr env,
                          std::string name,
                          size_t n = std::thread::hardware_concurrency());

  ~TesseractPlanningServer() = default;
  TesseractPlanningServer(const TesseractPlanningServer&) = delete;
  TesseractPlanningServer& operator=(const TesseractPlanningServer&) = delete;
  TesseractPlanningServer(TesseractPlanningServer&&) = delete;
  TesseractPlanningServer& operator=(TesseractPlanningServer&&) = delete;

  tesseract_environment::EnvironmentMonitor& getEnvironmentMonitor();
  const tesseract_environment::EnvironmentMonitor& getEnvironmentMonitor() const;

  tesseract_planning::ProcessPlanningServer& getProcessPlanningServer();
  const tesseract_planning::ProcessPlanningServer& getProcessPlanningServer() const;

  tesseract_planning::EnvironmentCache& getEnvironmentCache();
  const tesseract_planning::EnvironmentCache& getEnvironmentCache() const;

  void onMotionPlanningCallback(const tesseract_msgs::GetMotionPlanGoalConstPtr& goal);

protected:
  ros::NodeHandle nh_;

  /** @brief The environment monitor to keep the planning server updated with the latest */
  tesseract_environment::EnvironmentMonitor::Ptr monitor_;

  /** @brief The environment cache being used by the process planning server */
  tesseract_planning::EnvironmentCache::Ptr environment_cache_;

  /** @brief The process planning server */
  tesseract_planning::ProcessPlanningServer::UPtr planning_server_;

  /** @brief The motion planning action server */
  actionlib::SimpleActionServer<tesseract_msgs::GetMotionPlanAction> motion_plan_server_;

  /** @brief TF buffer to track TCP transforms */
  std::shared_ptr<tf2_ros::Buffer> tf_buffer_;

  /** @brief TF listener to lookup TCP transforms */
  tf2_ros::TransformListener tf_listener_;

  void ctor();

  void loadDefaultPlannerProfiles();

  Eigen::Isometry3d tfFindTCPOffset(const tesseract_planning::ManipulatorInfo& manip_info);
};
}  // namespace tesseract_planning_server
#endif  // TESSERACT_ROS_TESSERACT_PLANNING_SERVER_H
