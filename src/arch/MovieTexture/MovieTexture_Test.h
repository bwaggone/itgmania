#ifndef RAGE_MOVIE_TEXTURE_TEST_H
#define RAGE_MOVIE_TEXTURE_TEST_H

#include <string>
#include <vector>

namespace MovieTextureTest {

struct FrameMetric {
  int index = 0;
  float pts = 0.0f;
  float duration = 0.0f;
  float copy_latency_us = 0.0f;
};

struct TestResult {
  bool success = false;
  std::string filename;
  int width = 0;
  int height = 0;
  int total_frames = 0;
  float total_decode_time_s = 0.0f;
  float throughput_fps = 0.0f;
  float min_pts = 0.0f;
  float max_pts = 0.0f;
  float avg_copy_latency_us = 0.0f;
  float max_copy_latency_us = 0.0f;
  float p99_copy_latency_us = 0.0f;
  int out_of_order_count = 0;
  bool loop_wrap_verified = false;
  bool rewind_verified = false;
  bool paced_mode = false;
  int displayed_frames = 0;
  int dropped_frames = 0;
  float max_sync_drift_ms = 0.0f;
  std::string error_message;
  std::vector<FrameMetric> sample_frames;
};

struct StressResult {
  bool success = false;
  int iterations = 0;
  float total_time_s = 0.0f;
  float avg_cycle_ms = 0.0f;
  std::string error_message;
};

TestResult RunVideoTest(
    const std::string& path, bool test_loop = false, bool paced = false,
    float sim_fps = 60.0f, float max_duration = 0.0f);
StressResult RunStressTest(const std::string& path, int iterations = 50);

void PrintResultText(const TestResult& result);
void PrintResultJson(const TestResult& result);
void PrintStressResultText(const StressResult& result);
void PrintStressResultJson(const StressResult& result);
void SaveResultJsonToFile(
    const TestResult& result, const std::string& filepath);
void SaveStressResultJsonToFile(
    const StressResult& result, const std::string& filepath);

}  // namespace MovieTextureTest

#endif
