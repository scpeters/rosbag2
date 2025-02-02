// Copyright 2018, Bosch Software Innovations GmbH.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <gmock/gmock.h>

#include <cstdlib>
#include <string>
#include <thread>

#include "rcpputils/filesystem_helper.hpp"

#include "rosbag2_test_common/process_execution_helpers.hpp"
#include "rosbag2_test_common/tested_storage_ids.hpp"

using namespace ::testing;  // NOLINT

class InfoEndToEndTestFixture : public Test, public WithParamInterface<std::string>
{
public:
  InfoEndToEndTestFixture()
  {
    // _SRC_RESOURCES_DIR_PATH defined in CMakeLists.txt
    bags_path_ = (rcpputils::fs::path(_SRC_RESOURCES_DIR_PATH) / GetParam()).string();
  }

  std::string bags_path_;
};

TEST_P(InfoEndToEndTestFixture, info_end_to_end_test) {
  internal::CaptureStdout();
  auto exit_code = execute_and_wait_until_completion("ros2 bag info cdr_test", bags_path_);
  std::string output = internal::GetCapturedStdout();
  auto expected_storage = GetParam();
  auto expected_file = rosbag2_test_common::bag_filename_for_storage_id("cdr_test_0", GetParam());
  std::string expected_ros_distro = "unknown";

  EXPECT_THAT(exit_code, Eq(EXIT_SUCCESS));
  // The bag size depends on the os and is not asserted, the time is asserted time zone independent
  EXPECT_THAT(
    output, ContainsRegex(
      "\nFiles:             " + expected_file +
      "\nBag size:          .*B"
      "\nStorage id:        " + expected_storage +
      "\nROS Distro:        " + expected_ros_distro +
      "\nDuration:          0\\.151s"
      "\nStart:             Apr  .+ 2020 .*:.*:36.763 \\(1586406456\\.763\\)"
      "\nEnd:               Apr  .+ 2020 .*:.*:36.914 \\(1586406456\\.914\\)"
      "\nMessages:          7"
      "\nTopic information: "));
  EXPECT_THAT(
    output, HasSubstr(
      "Topic: /test_topic | Type: test_msgs/msg/BasicTypes | Count: 3 | "
      "Serialization Format: cdr\n"));
  EXPECT_THAT(
    output, HasSubstr(
      "Topic: /array_topic | Type: test_msgs/msg/Arrays | Count: 4 | "
      "Serialization Format: cdr"));
}

TEST_P(InfoEndToEndTestFixture, info_with_verbose_option_and_topic_name_option) {
  internal::CaptureStdout();
  auto exit_code = execute_and_wait_until_completion(
    "ros2 bag info bag_with_topics_and_service_events --verbose --topic-name",
    bags_path_);
  std::string output = internal::GetCapturedStdout();
  auto expected_storage = GetParam();
  auto expected_file = rosbag2_test_common::bag_filename_for_storage_id(
    "bag_with_topics_and_service_events", GetParam());
  std::string expected_ros_distro = "unknown";

  EXPECT_THAT(exit_code, Eq(EXIT_SUCCESS));

  EXPECT_THAT(
    output, HasSubstr(
      "Warning! You have set both the '-t' and '-v' parameters. The '-t' parameter "
      "will be ignored.\n"));

  // The bag size depends on the os and is not asserted, the time is asserted time zone independent
  EXPECT_THAT(
    output, ContainsRegex(
      "\nFiles:             " + expected_file +
      "\nBag size:          .*B"
      "\nStorage id:        " + expected_storage +
      "\nROS Distro:        " + expected_ros_distro +
      "\nDuration:          0\\.70s"
      "\nStart:             Nov  7 2023 .*:30:36\\..* \\(1699345836\\..*\\)"
      "\nEnd:               Nov  7 2023 .*:30:36\\..* \\(1699345836\\..*\\)"
      "\nMessages:          2"
      "\nTopic information: "));

  EXPECT_THAT(output, HasSubstr("Service:           2\n"));
}

TEST_P(InfoEndToEndTestFixture, info_with_verbose_option_end_to_end_test) {
  internal::CaptureStdout();
  auto exit_code = execute_and_wait_until_completion(
    "ros2 bag info bag_with_topics_and_service_events --verbose",
    bags_path_);
  std::string output = internal::GetCapturedStdout();
  auto expected_storage = GetParam();
  auto expected_file = rosbag2_test_common::bag_filename_for_storage_id(
    "bag_with_topics_and_service_events", GetParam());
  std::string expected_ros_distro = "unknown";

  EXPECT_THAT(exit_code, Eq(EXIT_SUCCESS));
  // The bag size depends on the os and is not asserted, the time is asserted time zone independent
  EXPECT_THAT(
    output, ContainsRegex(
      "\nFiles:             " + expected_file +
      "\nBag size:          .*B"
      "\nStorage id:        " + expected_storage +
      "\nROS Distro:        " + expected_ros_distro +
      "\nDuration:          0\\.70s"
      "\nStart:             Nov  7 2023 .*:30:36\\..* \\(1699345836\\..*\\)"
      "\nEnd:               Nov  7 2023 .*:30:36\\..* \\(1699345836\\..*\\)"
      "\nMessages:          2"
      "\nTopic information: "));
  EXPECT_THAT(
    output, HasSubstr(
      "Topic: /events/write_split | Type: rosbag2_interfaces/msg/WriteSplitEvent | Count: 0 | "
      "Serialization Format: cdr\n"));
  EXPECT_THAT(
    output, HasSubstr(
      "Topic: /test_topic1 | Type: test_msgs/msg/Strings | Count: 1 | "
      "Serialization Format: cdr\n"));
  EXPECT_THAT(
    output, HasSubstr(
      "Topic: /test_topic2 | Type: test_msgs/msg/Strings | Count: 1 | "
      "Serialization Format: cdr\n"));

  EXPECT_THAT(output, HasSubstr("Service:           2\n"));

  EXPECT_THAT(
    output, HasSubstr(
      "Service: /test_service1 | Type: test_msgs/srv/BasicTypes | Request Count: 2 | "
      "Response Count: 2 | Serialization Format: cdr\n"));

  EXPECT_THAT(
    output, HasSubstr(
      "Service: /test_service2 | Type: test_msgs/srv/BasicTypes | Request Count: 2 | "
      "Response Count: 2 | Serialization Format: cdr\n"));
}

// TODO(Martin-Idel-SI): Revisit exit code non-zero here, gracefully should be exit code zero
TEST_P(InfoEndToEndTestFixture, info_fails_gracefully_if_bag_does_not_exist) {
  internal::CaptureStderr();
  auto exit_code =
    execute_and_wait_until_completion("ros2 bag info does_not_exist", bags_path_);
  auto error_output = internal::GetCapturedStderr();

  EXPECT_THAT(exit_code, Ne(EXIT_SUCCESS));
  EXPECT_THAT(error_output, HasSubstr("'does_not_exist' does not exist"));
}

TEST_P(InfoEndToEndTestFixture, info_fails_gracefully_if_metadata_yaml_file_does_not_exist) {
  internal::CaptureStderr();
  auto exit_code =
    execute_and_wait_until_completion("ros2 bag info " + bags_path_, bags_path_);
  auto error_output = internal::GetCapturedStderr();

  EXPECT_THAT(exit_code, Eq(EXIT_FAILURE));
  EXPECT_THAT(error_output, HasSubstr("Could not find metadata"));
}

INSTANTIATE_TEST_SUITE_P(
  TestInfoEndToEnd,
  InfoEndToEndTestFixture,
  ::testing::ValuesIn(rosbag2_test_common::kTestedStorageIDs)
);
