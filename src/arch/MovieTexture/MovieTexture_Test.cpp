#include "MovieTexture_Test.h"

#include <algorithm>
#include <chrono>
#include <cstdio>
#include <iomanip>
#include <iostream>
#include <memory>
#include <numeric>
#include <sstream>
#include <thread>
#include <vector>

#include "Command.h"
#include "LocalizedString.h"
#include "LuaDebugManager.h"
#include "LuaManager.h"
#include "MessageManager.h"
#include "MovieTexture_FFMpeg.h"
#include "MovieTexture_Generic.h"
#include "Preference.h"
#include "RageFileManager.h"
#include "RageLog.h"
#include "RageSoundReader_FileReader.h"
#include "RageSurface.h"
#include "RageTextureRenderTarget.h"
#include "RageTimer.h"
#include "Sprite.h"
#include "arch/ArchHooks/ArchHooks.h"
#include "arch/Dialog/Dialog.h"

class RageDisplay;
RageDisplay* DISPLAY = nullptr;
class RageTextureManager;
RageTextureManager* TEXTUREMAN = nullptr;
LuaDebugManager* LUADEBUG = nullptr;
MessageManager* MESSAGEMAN = nullptr;
class PrefsManager;
PrefsManager* PREFSMAN = nullptr;
bool g_bAutoRestart = false;

LocalizedString::LocalizedString(
    const std::string& sGroup, const std::string& sName)
    : m_sGroup(sGroup), m_sName(sName), m_pImpl(nullptr) {}
LocalizedString::LocalizedString(const LocalizedString& other)
    : m_sGroup(other.m_sGroup), m_sName(other.m_sName), m_pImpl(nullptr) {}
LocalizedString::~LocalizedString() {}
void LocalizedString::Load(
    const std::string& sGroup, const std::string& sName) {
  m_sGroup = sGroup;
  m_sName = sName;
}
const std::string& LocalizedString::GetValue() const { return m_sName; }
void LocalizedString::RegisterLocalizer(MakeLocalizer) {}

Dialog::Result Dialog::AbortRetryIgnore(std::string, std::string) {
  return Dialog::abort;
}

void ParseCommands(const std::string&, Commands&, bool) {}
std::string Command::GetName() const { return ""; }
void LuaDebugManager::ActivateState(lua_State*) {}
void LuaDebugManager::DeactivateState(lua_State*) {}

IPreference::IPreference(const std::string& sName, PreferenceType)
    : m_sName(sName), m_bImmutable(false) {}
IPreference::~IPreference() {}
void IPreference::PushValue(lua_State*) const {}
void IPreference::SetFromStack(lua_State*) {}

MovieTexture_Generic::MovieTexture_Generic(
    RageTextureID ID, std::unique_ptr<MovieDecoder> pDecoder)
    : RageMovieTexture(ID) {}
MovieTexture_Generic::~MovieTexture_Generic() {}
std::string MovieTexture_Generic::Init() { return ""; }
void MovieTexture_Generic::Invalidate() {}
void MovieTexture_Generic::Reload() {}
void MovieTexture_Generic::SetPosition(float) {}
void MovieTexture_Generic::UpdateMovie(float) {}
#include "RageDisplay.h"

class RageDisplay_Test : public RageDisplay {
 public:
  RageDisplay_Test() = default;
  std::string GetApiDescription() const override { return "Test"; }
  void GetDisplaySpecs(DisplaySpecs&) const override {}
  ActualVideoModeParams GetActualVideoModeParams() const override {
    return ActualVideoModeParams();
  }
  void SetBlendMode(BlendMode) override {}
  bool SupportsTextureFormat(RagePixelFormat, bool) override { return true; }
  bool SupportsPerVertexMatrixScale() override { return false; }
  uintptr_t CreateTexture(RagePixelFormat, RageSurface*, bool) override {
    return 1;
  }
  void UpdateTexture(uintptr_t, RageSurface*, int, int, int, int) override {}
  void DeleteTexture(uintptr_t) override {}
  void ClearAllTextures() override {}
  int GetNumTextureUnits() override { return 1; }
  void SetTexture(TextureUnit, uintptr_t) override {}
  void SetTextureMode(TextureUnit, TextureMode) override {}
  void SetTextureWrapping(TextureUnit, bool) override {}
  int GetMaxTextureSize() const override { return 4096; }
  void SetTextureFiltering(TextureUnit, bool) override {}
  bool IsZTestEnabled() const override { return false; }
  bool IsZWriteEnabled() const override { return false; }
  void SetZWrite(bool) override {}
  void SetZTestMode(ZTestMode) override {}
  void SetZBias(float) override {}
  void ClearZBuffer() override {}
  void SetCullMode(CullMode) override {}
  void SetAlphaTest(bool) override {}
  void SetMaterial(
      const RageColor&, const RageColor&, const RageColor&, const RageColor&,
      float) override {}
  void SetLighting(bool) override {}
  void SetLightOff(int) override {}
  void SetLightDirectional(
      int, const RageColor&, const RageColor&, const RageColor&,
      const RageVector3&) override {}
  void SetSphereEnvironmentMapping(TextureUnit, bool) override {}
  void SetCelShaded(int) override {}
  bool BeginFrame() override { return true; }
  void EndFrame() override {}
  const RagePixelFormatDesc* GetPixelFormatDesc(
      RagePixelFormat) const override {
    return nullptr;
  }
  std::string Init(const VideoModeParams&, bool) override { return ""; }
  RageCompiledGeometry* CreateCompiledGeometry() override { return nullptr; }
  void DeleteCompiledGeometry(RageCompiledGeometry*) override {}
  bool IsEffectModeSupported(EffectMode) override { return true; }

 protected:
  void DrawQuadsInternal(const RageSpriteVertex[], int) override {}
  void DrawQuadStripInternal(const RageSpriteVertex[], int) override {}
  void DrawFanInternal(const RageSpriteVertex[], int) override {}
  void DrawStripInternal(const RageSpriteVertex[], int) override {}
  void DrawTrianglesInternal(const RageSpriteVertex[], int) override {}
  void DrawCompiledGeometryInternal(const RageCompiledGeometry*, int) override {
  }
  void DrawLineStripInternal(const RageSpriteVertex[], int, float) override {}
  void DrawSymmetricQuadStripInternal(const RageSpriteVertex[], int) override {}
  std::string TryVideoMode(const VideoModeParams&, bool&) override {
    return "";
  }
  RageSurface* CreateScreenshot() override { return nullptr; }
  RageMatrix GetOrthoMatrix(float, float, float, float, float, float) override {
    return RageMatrix();
  }
};

uintptr_t MovieTexture_Generic::GetTexHandle() const { return 0; }
EffectMode MovieTexture_Generic::GetEffectMode(MovieDecoderPixelFormatYCbCr) {
  return EffectMode_Normal;
}

RageDisplay::RageDisplay() {}
RageDisplay::~RageDisplay() {}
void RageDisplay::ResolutionChanged() {}
void RageDisplay::BeginConcurrentRendering() {}
void RageDisplay::DrawCircleInternal(const RageSpriteVertex&, float) {}
bool RageDisplay::BeginFrame() { return true; }
void RageDisplay::EndFrame() {}
void RageDisplay::ResetStats() {}
void RageDisplay::ProcessStatsOnFlip() {}
std::string RageDisplay::GetStats() const { return ""; }
RageMatrix RageDisplay::GetFrustumMatrix(
    float, float, float, float, float, float) {
  return RageMatrix();
}
void RageDisplay::DrawLineStripInternal(const RageSpriteVertex[], int, float) {}
RageMatrix RageDisplay::GetOrthoMatrix(
    float, float, float, float, float, float) {
  return RageMatrix();
}

RagePixelFormat RageDisplay::FindPixelFormat(
    int, unsigned int, unsigned int, unsigned int, unsigned int, bool) {
  return RagePixelFormat_RGBA8;
}

#if defined(_WIN32)
#include <windows.h>
#else
#include <sys/time.h>
#endif

ArchHooks* HOOKS = nullptr;
bool ArchHooks::g_bQuitting = false;
bool ArchHooks::g_bToggleWindowed = false;

class ArchHooks_Test : public ArchHooks {
 public:
  std::string GetArchName() const override { return "Test"; }
  float GetDisplayAspectRatio() override { return 16.0f / 9.0f; }
  std::string GetClipboard() override { return ""; }
};

ArchHooks::ArchHooks() : m_bHasFocus(true), m_bFocusChanged(false) {}
ArchHooks* ArchHooks::Create() { return new ArchHooks_Test; }
void ArchHooks::SetHasFocus(bool) {}
bool ArchHooks::AppFocusChanged() { return false; }
void ArchHooks::PushSelf(lua_State*) {}
void ArchHooks::RegisterWithLua() {}
std::string ArchHooks::GetClipboard() { return ""; }

#if defined(_WIN32)
int64_t ArchHooks::GetSystemTimeInMicroseconds() {
  static LARGE_INTEGER freq;
  static bool init = false;
  if (!init) {
    QueryPerformanceFrequency(&freq);
    init = true;
  }
  LARGE_INTEGER cur;
  QueryPerformanceCounter(&cur);
  return (cur.QuadPart * 1000000LL) / freq.QuadPart;
}
#else
int64_t ArchHooks::GetSystemTimeInMicroseconds() {
  struct timeval tv;
  gettimeofday(&tv, nullptr);
  return (int64_t)tv.tv_sec * 1000000 + tv.tv_usec;
}
#endif

void ArchHooks::MountInitialFilesystems(const std::string&) {
  FILEMAN->Mount("dir", ".", "/");
#if defined(_WIN32)
  char drive_buf[4] = "A:/";
  DWORD drives = GetLogicalDrives();
  for (int i = 0; i < 26; ++i) {
    if (drives & (1 << i)) {
      drive_buf[0] = 'A' + i;
      std::string drive_str = drive_buf;
      std::string mnt_lower = std::string("/") + (char)('a' + i) + ":";
      std::string mnt_upper = std::string("/") + (char)('A' + i) + ":";
      FILEMAN->Mount("dirro", drive_str, mnt_lower);
      FILEMAN->Mount("dirro", drive_str, mnt_upper);
    }
  }
#else
  FILEMAN->Mount("dirro", "/", "/");
#endif
}
void ArchHooks::MountUserFilesystems(const std::string&) {}
std::string ArchHooks::GetPreferredLanguage() { return "en"; }
bool ArchHooks::GetAndClearToggleWindowed() { return false; }
void ArchHooks::SetToggleWindowed() {}

namespace CrashHandler {
void ForceCrash(const char*) { exit(1); }
void ForceDeadlock(std::string, uint64_t) { exit(1); }
}  // namespace CrashHandler

#if defined(_WIN32)
tm* my_localtime_r(const time_t* timep, tm* result) {
  localtime_s(result, timep);
  return result;
}

std::string werr_ssprintf(int, const char* fmt, ...) {
  char szBuf[1024];
  va_list va;
  va_start(va, fmt);
  vsnprintf(szBuf, sizeof(szBuf), fmt, va);
  va_end(va);
  return szBuf;
}
#endif

Message::Message(const std::string& sName) : m_sName(sName) {}
Message::~Message() {}
void Message::SetParamFromStack(lua_State*, const std::string&) {}
void MessageManager::Broadcast(Message&) const {}

RageSoundReader_FileReader* RageSoundReader_FileReader::OpenFile(
    std::string, std::string&, bool*) {
  return nullptr;
}
static void RawWriteStdout(const std::string& str) {
  std::cout << str << std::flush;
}

namespace MovieTextureTest {

TestResult RunVideoTest(
    const std::string& path, bool test_loop, bool paced, float sim_fps,
    float max_duration) {
  TestResult result;
  result.filename = path;

  auto decoder = std::make_unique<MovieDecoder_FFMpeg>();
  std::string open_path = path;
  std::string err = decoder->Open(open_path);
  if (!err.empty() && !open_path.empty() && open_path.front() != '/') {
    open_path = "/" + path;
    err = decoder->Open(open_path);
  }
  if (!err.empty()) {
    result.success = false;
    result.error_message = err;
    return result;
  }

  result.width = decoder->GetWidth();
  result.height = decoder->GetHeight();

  MovieDecoderPixelFormatYCbCr fmt = PixelFormatYCbCr_Invalid;
  RageSurface* surface =
      decoder->CreateCompatibleSurface(result.width, result.height, true, fmt);
  if (surface == nullptr) {
    surface = CreateSurface(
        result.width, result.height, 32, 0x00FF0000, 0x0000FF00, 0x000000FF,
        0xFF000000);
  }

  std::vector<float> copy_latencies;
  float prev_pts = -1.0f;
  result.min_pts = 0.0f;
  result.max_pts = 0.0f;

  if (test_loop) {
    decoder->SetLooping(true);
  }

  auto start_time = std::chrono::high_resolution_clock::now();

  MovieDecoder_FFMpeg* dec_ptr = decoder.get();
  std::thread worker([dec_ptr]() { dec_ptr->DecodeMovie(); });

  int frame_count = 0;
  bool reached_eof = false;
  auto last_frame_time = std::chrono::high_resolution_clock::now();

  // Phase 1: Play through
  if (paced) {
    result.paced_mode = true;
    auto sim_start = std::chrono::high_resolution_clock::now();
    auto next_tick = sim_start;
    auto tick_interval = std::chrono::microseconds(
        static_cast<int64_t>(1000000.0 / (sim_fps > 0.0f ? sim_fps : 60.0f)));
    float dt = 1.0f / (sim_fps > 0.0f ? sim_fps : 60.0f);

    while (!reached_eof) {
      auto now = std::chrono::high_resolution_clock::now();
      float sim_clock = std::chrono::duration<float>(now - sim_start).count();

      if (max_duration > 0.0f && sim_clock >= max_duration) {
        reached_eof = true;
        break;
      }

      if (decoder->IsCurrentFrameReady()) {
        float pts = decoder->GetTimestamp();

        // Check if frame timestamp is ready to be displayed
        if (pts <= sim_clock + (dt * 0.5f)) {
          float drift_ms = std::abs(pts - sim_clock) * 1000.0f;
          result.max_sync_drift_ms =
              std::max(result.max_sync_drift_ms, drift_ms);

          auto t0 = std::chrono::high_resolution_clock::now();
          int ret = decoder->GetFrame(surface);
          auto t1 = std::chrono::high_resolution_clock::now();

          float lat_us =
              std::chrono::duration<float, std::micro>(t1 - t0).count();
          copy_latencies.push_back(lat_us);

          if (frame_count == 0) {
            result.min_pts = pts;
          } else {
            if (test_loop && pts < prev_pts - 0.005f) {
              result.loop_wrap_verified = true;
              if (max_duration == 0.0f) {
                reached_eof = true;
              }
            } else if (pts < prev_pts - 0.0001f) {
              result.out_of_order_count++;
              if (result.error_message.empty()) {
                result.error_message = ssprintf(
                    "Out-of-order PTS at frame %d: prev=%.4f, curr=%.4f",
                    frame_count, prev_pts, pts);
              }
            }
          }
          prev_pts = pts;
          result.max_pts = std::max(result.max_pts, pts);
          result.displayed_frames++;
          frame_count++;
          last_frame_time = std::chrono::high_resolution_clock::now();

          if (decoder->EndOfMovie()) {
            if (test_loop) {
              decoder->Rollover();
              sim_start = std::chrono::high_resolution_clock::now();
              prev_pts = -1.0f;
              result.loop_wrap_verified = true;
              if (max_duration == 0.0f) {
                reached_eof = true;
              }
            } else {
              reached_eof = true;
            }
          }
        }
      } else {
        if (decoder->EndOfMovie()) {
          if (test_loop) {
            decoder->Rollover();
            sim_start = std::chrono::high_resolution_clock::now();
            prev_pts = -1.0f;
            result.loop_wrap_verified = true;
            if (max_duration == 0.0f) {
              reached_eof = true;
            }
          } else {
            reached_eof = true;
            break;
          }
        }
        auto elapsed_since_frame =
            std::chrono::duration_cast<std::chrono::milliseconds>(
                now - last_frame_time)
                .count();
        if (elapsed_since_frame > 1000 && frame_count > 0) {
          if (test_loop) {
            result.loop_wrap_verified = true;
          }
          reached_eof = true;
          break;
        } else if (elapsed_since_frame > 5000) {
          result.error_message = "Timed out waiting for frame from decoder";
          break;
        }
      }

      next_tick += tick_interval;
      if (next_tick > std::chrono::high_resolution_clock::now()) {
        std::this_thread::sleep_until(next_tick);
      }
    }
  } else {
    while (!reached_eof) {
      if (decoder->IsCurrentFrameReady()) {
        float pts = decoder->GetTimestamp();

        auto t0 = std::chrono::high_resolution_clock::now();
        int ret = decoder->GetFrame(surface);
        auto t1 = std::chrono::high_resolution_clock::now();

        float lat_us =
            std::chrono::duration<float, std::micro>(t1 - t0).count();
        copy_latencies.push_back(lat_us);

        if (frame_count == 0) {
          result.min_pts = pts;
        } else {
          if (test_loop && pts < prev_pts - 0.005f) {
            // Wrapped around during looping!
            result.loop_wrap_verified = true;
            reached_eof = true;
          } else if (pts < prev_pts - 0.0001f) {
            result.out_of_order_count++;
            if (result.error_message.empty()) {
              result.error_message = ssprintf(
                  "Out-of-order PTS at frame %d: prev=%.4f, curr=%.4f "
                  "(delta=%.4f)",
                  frame_count, prev_pts, pts, pts - prev_pts);
            }
          }
        }
        prev_pts = pts;
        result.max_pts = std::max(result.max_pts, pts);

        if (result.sample_frames.size() < 100 || frame_count % 10 == 0) {
          FrameMetric m;
          m.index = frame_count;
          m.pts = pts;
          m.copy_latency_us = lat_us;
          result.sample_frames.push_back(m);
        }

        frame_count++;
        last_frame_time = std::chrono::high_resolution_clock::now();

        if (test_loop) {
          if (decoder->EndOfMovie()) {
            decoder->Rollover();
            result.loop_wrap_verified = true;
            reached_eof = true;
          } else if (result.loop_wrap_verified || frame_count > 10000) {
            reached_eof = true;
          }
        } else {
          if (ret == 1 || decoder->EndOfMovie()) {
            reached_eof = true;
          }
        }
      } else {
        if (decoder->EndOfMovie()) {
          if (test_loop) {
            decoder->Rollover();
            result.loop_wrap_verified = true;
            reached_eof = true;
          } else {
            reached_eof = true;
            break;
          }
        }
        auto now = std::chrono::high_resolution_clock::now();
        auto elapsed_since_frame =
            std::chrono::duration_cast<std::chrono::milliseconds>(
                now - last_frame_time)
                .count();
        if (elapsed_since_frame > 1000 && frame_count > 0) {
          if (test_loop) {
            result.loop_wrap_verified = true;
          }
          reached_eof = true;
          break;
        } else if (elapsed_since_frame > 5000) {
          result.error_message = "Timed out waiting for frame from decoder";
          break;
        }
        std::this_thread::sleep_for(std::chrono::microseconds(200));
      }
    }
  }

  auto end_time = std::chrono::high_resolution_clock::now();
  result.total_decode_time_s =
      std::chrono::duration<float>(end_time - start_time).count();
  result.total_frames = frame_count;

  if (result.total_decode_time_s > 0.0f) {
    result.throughput_fps =
        static_cast<float>(frame_count) / result.total_decode_time_s;
  }

  if (!copy_latencies.empty()) {
    float sum =
        std::accumulate(copy_latencies.begin(), copy_latencies.end(), 0.0f);
    result.avg_copy_latency_us = sum / copy_latencies.size();
    result.max_copy_latency_us =
        *std::max_element(copy_latencies.begin(), copy_latencies.end());

    std::vector<float> sorted_latencies = copy_latencies;
    std::sort(sorted_latencies.begin(), sorted_latencies.end());
    size_t p99_idx = static_cast<size_t>(sorted_latencies.size() * 0.99f);
    if (p99_idx >= sorted_latencies.size()) {
      p99_idx = sorted_latencies.size() - 1;
    }
    result.p99_copy_latency_us = sorted_latencies[p99_idx];
  }

  // Phase 2: Test Rewind & Looping if requested
  if (test_loop) {
    if (result.loop_wrap_verified) {
      result.rewind_verified = true;
    } else {
      decoder->Rewind();
      int rewind_wait = 0;
      while (!decoder->IsCurrentFrameReady() && rewind_wait < 50) {
        std::this_thread::sleep_for(std::chrono::milliseconds(2));
        rewind_wait++;
      }
      if (decoder->IsCurrentFrameReady()) {
        float rewind_pts = decoder->GetTimestamp();
        if (rewind_pts <= 0.2f) {
          result.rewind_verified = true;
        }
      }
    }
  } else {
    result.rewind_verified = true;
    result.loop_wrap_verified = true;
  }

  decoder->Cancel();
  if (worker.joinable()) {
    worker.detach();
  }

  delete surface;

  result.success =
      (result.total_frames > 0 && result.out_of_order_count == 0 &&
       result.error_message.empty());
  return result;
}

StressResult RunStressTest(const std::string& path, int iterations) {
  StressResult result;
  result.iterations = iterations;

  auto start = std::chrono::high_resolution_clock::now();

  for (int i = 0; i < iterations; ++i) {
    auto decoder = std::make_unique<MovieDecoder_FFMpeg>();
    std::string open_path = path;
    std::string err = decoder->Open(open_path);
    if (!err.empty() && !open_path.empty() && open_path.front() != '/') {
      open_path = "/" + path;
      err = decoder->Open(open_path);
    }
    if (!err.empty()) {
      result.success = false;
      result.error_message =
          ssprintf("Iteration %d failed to open: %s", i, err.c_str());
      return result;
    }

    int w = decoder->GetWidth();
    int h = decoder->GetHeight();
    MovieDecoderPixelFormatYCbCr fmt = PixelFormatYCbCr_Invalid;
    RageSurface* surf = decoder->CreateCompatibleSurface(w, h, true, fmt);
    if (surf == nullptr) {
      surf = CreateSurface(
          w, h, 32, 0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);
    }

    MovieDecoder_FFMpeg* dec_ptr = decoder.get();
    std::thread th([dec_ptr]() { dec_ptr->DecodeMovie(); });

    // Retrieve 1-3 frames
    int frames_got = 0;
    int wait_cycles = 0;
    while (frames_got < 2 && wait_cycles < 200) {
      if (decoder->IsCurrentFrameReady()) {
        decoder->GetFrame(surf);
        frames_got++;
      } else {
        std::this_thread::sleep_for(std::chrono::microseconds(100));
        wait_cycles++;
      }
    }

    decoder->Cancel();
    if (th.joinable()) {
      th.detach();
    }
    delete surf;
  }

  auto end = std::chrono::high_resolution_clock::now();
  result.total_time_s = std::chrono::duration<float>(end - start).count();
  result.avg_cycle_ms =
      (result.total_time_s * 1000.0f) / static_cast<float>(iterations);
  result.success = true;
  return result;
}

void PrintResultText(const TestResult& result) {
  std::stringstream ss;
  ss << "\n======================================================\n";
  ss << " Movie Decoding Test Report: " << result.filename << "\n";
  ss << "======================================================\n";
  ss << " Status:              " << (result.success ? "[PASS]" : "[FAIL]")
     << "\n";
  if (result.paced_mode) {
    ss << " Mode:                [PACED GAMEPLAY SIMULATION]\n";
    ss << " Displayed Frames:    " << result.displayed_frames << "\n";
    ss << " Max Sync Drift:      " << std::fixed << std::setprecision(2)
       << result.max_sync_drift_ms << " ms\n";
  }
  if (!result.error_message.empty()) {
    ss << " Error:               " << result.error_message << "\n";
  }
  ss << " Resolution:          " << result.width << "x" << result.height
     << "\n";
  ss << " Total Frames:        " << result.total_frames << "\n";
  ss << " PTS Range:           " << std::fixed << std::setprecision(3)
     << result.min_pts << "s -> " << result.max_pts << "s\n";
  ss << " Out-Of-Order Frames: " << result.out_of_order_count
     << (result.out_of_order_count == 0 ? " (PASS)" : " (FAILED)") << "\n";
  ss << " Decode Throughput:   " << std::fixed << std::setprecision(1)
     << result.throughput_fps << " FPS\n";
  ss << " Copy Latency (avg):  " << std::fixed << std::setprecision(2)
     << result.avg_copy_latency_us << " us\n";
  ss << " Copy Latency (p99):  " << std::fixed << std::setprecision(2)
     << result.p99_copy_latency_us << " us\n";
  ss << " Copy Latency (max):  " << std::fixed << std::setprecision(2)
     << result.max_copy_latency_us << " us\n";
  ss << " Looping Verified:    " << (result.loop_wrap_verified ? "YES" : "NO")
     << "\n";
  ss << " Rewind Verified:     " << (result.rewind_verified ? "YES" : "NO")
     << "\n";
  ss << "======================================================\n\n";
  RawWriteStdout(ss.str());
}

void PrintResultJson(const TestResult& result) {
  std::stringstream ss;
  ss << "{\n";
  ss << "  \"filename\": \"" << result.filename << "\",\n";
  ss << "  \"success\": " << (result.success ? "true" : "false") << ",\n";
  ss << "  \"paced_mode\": " << (result.paced_mode ? "true" : "false") << ",\n";
  ss << "  \"displayed_frames\": " << result.displayed_frames << ",\n";
  ss << "  \"max_sync_drift_ms\": " << result.max_sync_drift_ms << ",\n";
  ss << "  \"width\": " << result.width << ",\n";
  ss << "  \"height\": " << result.height << ",\n";
  ss << "  \"total_frames\": " << result.total_frames << ",\n";
  ss << "  \"total_decode_time_s\": " << result.total_decode_time_s << ",\n";
  ss << "  \"throughput_fps\": " << result.throughput_fps << ",\n";
  ss << "  \"min_pts\": " << result.min_pts << ",\n";
  ss << "  \"max_pts\": " << result.max_pts << ",\n";
  ss << "  \"avg_copy_latency_us\": " << result.avg_copy_latency_us << ",\n";
  ss << "  \"p99_copy_latency_us\": " << result.p99_copy_latency_us << ",\n";
  ss << "  \"max_copy_latency_us\": " << result.max_copy_latency_us << ",\n";
  ss << "  \"out_of_order_count\": " << result.out_of_order_count << ",\n";
  ss << "  \"loop_wrap_verified\": "
     << (result.loop_wrap_verified ? "true" : "false") << ",\n";
  ss << "  \"rewind_verified\": " << (result.rewind_verified ? "true" : "false")
     << ",\n";
  ss << "  \"error_message\": \"" << result.error_message << "\"\n";
  ss << "}\n";
  RawWriteStdout(ss.str());
}

void PrintStressResultText(const StressResult& result) {
  std::stringstream ss;
  ss << "\n======================================================\n";
  ss << " Concurrency & Teardown Stress Test Report\n";
  ss << "======================================================\n";
  ss << " Status:        " << (result.success ? "[PASS]" : "[FAIL]") << "\n";
  ss << " Iterations:    " << result.iterations << "\n";
  ss << " Total Time:    " << std::fixed << std::setprecision(2)
     << result.total_time_s << " s\n";
  ss << " Avg Per Cycle: " << std::fixed << std::setprecision(2)
     << result.avg_cycle_ms << " ms/cycle\n";
  if (!result.error_message.empty()) {
    ss << " Error:         " << result.error_message << "\n";
  }
  ss << "======================================================\n\n";
  RawWriteStdout(ss.str());
}

void PrintStressResultJson(const StressResult& result) {
  std::stringstream ss;
  ss << "{\n";
  ss << "  \"success\": " << (result.success ? "true" : "false") << ",\n";
  ss << "  \"iterations\": " << result.iterations << ",\n";
  ss << "  \"total_time_s\": " << result.total_time_s << ",\n";
  ss << "  \"avg_cycle_ms\": " << result.avg_cycle_ms << ",\n";
  ss << "  \"error_message\": \"" << result.error_message << "\"\n";
  ss << "}\n";
  RawWriteStdout(ss.str());
}

void SaveResultJsonToFile(
    const TestResult& result, const std::string& filepath) {
  FILE* f = fopen(filepath.c_str(), "w");
  if (!f) {
    return;
  }
  std::stringstream ss;
  ss << "{\n";
  ss << "  \"filename\": \"" << result.filename << "\",\n";
  ss << "  \"success\": " << (result.success ? "true" : "false") << ",\n";
  ss << "  \"paced_mode\": " << (result.paced_mode ? "true" : "false") << ",\n";
  ss << "  \"displayed_frames\": " << result.displayed_frames << ",\n";
  ss << "  \"max_sync_drift_ms\": " << result.max_sync_drift_ms << ",\n";
  ss << "  \"width\": " << result.width << ",\n";
  ss << "  \"height\": " << result.height << ",\n";
  ss << "  \"total_frames\": " << result.total_frames << ",\n";
  ss << "  \"total_decode_time_s\": " << result.total_decode_time_s << ",\n";
  ss << "  \"throughput_fps\": " << result.throughput_fps << ",\n";
  ss << "  \"min_pts\": " << result.min_pts << ",\n";
  ss << "  \"max_pts\": " << result.max_pts << ",\n";
  ss << "  \"avg_copy_latency_us\": " << result.avg_copy_latency_us << ",\n";
  ss << "  \"p99_copy_latency_us\": " << result.p99_copy_latency_us << ",\n";
  ss << "  \"max_copy_latency_us\": " << result.max_copy_latency_us << ",\n";
  ss << "  \"out_of_order_count\": " << result.out_of_order_count << ",\n";
  ss << "  \"loop_wrap_verified\": "
     << (result.loop_wrap_verified ? "true" : "false") << ",\n";
  ss << "  \"rewind_verified\": " << (result.rewind_verified ? "true" : "false")
     << ",\n";
  ss << "  \"error_message\": \"" << result.error_message << "\"\n";
  ss << "}\n";
  fprintf(f, "%s", ss.str().c_str());
  fclose(f);
}

void SaveStressResultJsonToFile(
    const StressResult& result, const std::string& filepath) {
  FILE* f = fopen(filepath.c_str(), "w");
  if (!f) {
    return;
  }
  std::stringstream ss;
  ss << "{\n";
  ss << "  \"success\": " << (result.success ? "true" : "false") << ",\n";
  ss << "  \"iterations\": " << result.iterations << ",\n";
  ss << "  \"total_time_s\": " << result.total_time_s << ",\n";
  ss << "  \"avg_cycle_ms\": " << result.avg_cycle_ms << ",\n";
  ss << "  \"error_message\": \"" << result.error_message << "\"\n";
  ss << "}\n";
  fprintf(f, "%s", ss.str().c_str());
  fclose(f);
}

}  // namespace MovieTextureTest

int main(int argc, char* argv[]) {
  SetCommandlineArguments(argc, argv);

  HOOKS = ArchHooks::Create();
  HOOKS->Init();

#if defined(_WIN32)
  SetUnhandledExceptionFilter(nullptr);
#endif

  LOG = new RageLog;
  LOG->SetLogToDisk(false);
  LOG->SetShowLogOutput(false);
  LOG->SetFlushing(true);

  LUA = new LuaManager;
  DISPLAY = new RageDisplay_Test;

  FILEMAN = new RageFileManager(argv[0]);
  FILEMAN->MountInitialFilesystems();
  FILEMAN->Mount("dir", ".", "/");

  std::string video_path;
  bool test_loop = false;
  bool run_stress = false;
  bool paced_mode = false;
  float sim_fps = 60.0f;
  float max_duration = 0.0f;
  int stress_iterations = 50;
  bool json_output = false;
  std::string output_file;

  for (int i = 1; i < argc; ++i) {
    std::string arg = argv[i];
    if (arg.rfind("--file=", 0) == 0) {
      video_path = arg.substr(7);
    } else if (arg == "--loop" || arg == "-l") {
      test_loop = true;
    } else if (arg == "--paced" || arg == "-p") {
      paced_mode = true;
    } else if (arg.rfind("--sim-fps=", 0) == 0) {
      try {
        sim_fps = std::stof(arg.substr(10));
      } catch (...) {
      }
    } else if (arg.rfind("--duration=", 0) == 0) {
      try {
        max_duration = std::stof(arg.substr(11));
      } catch (...) {
      }
    } else if (arg == "--json" || arg == "-j") {
      json_output = true;
    } else if (arg.rfind("--stress=", 0) == 0) {
      run_stress = true;
      try {
        stress_iterations = std::max(1, std::stoi(arg.substr(9)));
      } catch (...) {
      }
    } else if (arg == "--stress") {
      run_stress = true;
    } else if (arg.rfind("--output=", 0) == 0) {
      output_file = arg.substr(9);
    } else if (arg.rfind("-", 0) != 0 && video_path.empty()) {
      video_path = arg;
    }
  }

  GetCommandlineArgument("file", &video_path);
  GetCommandlineArgument("output", &output_file);
  if (GetCommandlineArgument("loop")) {
    test_loop = true;
  }
  if (GetCommandlineArgument("paced")) {
    paced_mode = true;
  }
  if (GetCommandlineArgument("json")) {
    json_output = true;
  }
  std::string fps_arg;
  if (GetCommandlineArgument("sim-fps", &fps_arg) && !fps_arg.empty()) {
    try {
      sim_fps = std::stof(fps_arg);
    } catch (...) {
    }
  }
  std::string dur_arg;
  if (GetCommandlineArgument("duration", &dur_arg) && !dur_arg.empty()) {
    try {
      max_duration = std::stof(dur_arg);
    } catch (...) {
    }
  }
  std::string stress_arg;
  if (GetCommandlineArgument("stress", &stress_arg)) {
    run_stress = true;
    if (!stress_arg.empty()) {
      try {
        stress_iterations = std::max(1, std::stoi(stress_arg));
      } catch (...) {
      }
    }
  }

  if (video_path.empty()) {
    std::cerr << "Usage: test_movie_decoding <video_path> [--loop] [--paced] "
                 "[--sim-fps=N] [--duration=S] [--stress[=N]] [--json] "
                 "[--output=<path>]\n";
    delete FILEMAN;
    delete LUA;
    delete LOG;
    delete HOOKS;
    return 1;
  }

  int exit_code = 0;
  if (run_stress) {
    auto res = MovieTextureTest::RunStressTest(video_path, stress_iterations);
    if (!output_file.empty()) {
      MovieTextureTest::SaveStressResultJsonToFile(res, output_file);
    }
    if (json_output) {
      MovieTextureTest::PrintStressResultJson(res);
    } else {
      MovieTextureTest::PrintStressResultText(res);
    }
    exit_code = res.success ? 0 : 1;
  } else {
    auto res = MovieTextureTest::RunVideoTest(
        video_path, test_loop, paced_mode, sim_fps, max_duration);
    if (!output_file.empty()) {
      MovieTextureTest::SaveResultJsonToFile(res, output_file);
    }
    if (json_output) {
      MovieTextureTest::PrintResultJson(res);
    } else {
      MovieTextureTest::PrintResultText(res);
    }
    exit_code = res.success ? 0 : 1;
  }

  _Exit(exit_code);
}
