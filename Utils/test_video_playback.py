#!/usr/bin/env python3
"""
Video Subsystem Correctness & Performance Benchmark Runner for ITGmania.

Scans local song/theme folders for video files (.mp4, .avi, .mkv, .webm, etc.),
runs them through ITGmania's standalone movie decoding test harness, verifies PTS monotonicity
(0 out-of-order frames), checks looping wrap-around, and benchmarks throughput & latency.
"""

import argparse
import glob
import json
import os
import subprocess
import sys
import tempfile
import time
from pathlib import Path

VIDEO_EXTENSIONS = {".mp4", ".avi", ".mkv", ".webm", ".flv", ".mpg", ".mpeg", ".mov"}

def find_test_binary(root_dir: Path) -> Path:
    candidates = [
        root_dir / "Program" / "test_movie_decoding-release-symbols.exe",
        root_dir / "Program" / "test_movie_decoding-debug.exe",
        root_dir / "Program" / "test_movie_decoding.exe",
        root_dir / "Program" / "test_movie_decoding-debug",
        root_dir / "Program" / "test_movie_decoding",
        root_dir / "test_movie_decoding-debug",
        root_dir / "test_movie_decoding",
        root_dir / "Program" / "ITGmania-debug.exe",
        root_dir / "Program" / "ITGmania.exe",
        root_dir / "Program" / "itgmania-debug",
        root_dir / "Program" / "itgmania",
        root_dir / "itgmania-debug",
        root_dir / "itgmania",
        root_dir / "ITGmania.app" / "Contents" / "MacOS" / "ITGmania",
    ]
    for c in candidates:
        if c.is_file():
            return c
    raise FileNotFoundError(f"Could not locate test_movie_decoding or ITGmania binary in {root_dir}")

def scan_for_videos(paths: list[Path]) -> list[Path]:
    found = []
    for p in paths:
        if p.is_file() and p.suffix.lower() in VIDEO_EXTENSIONS and not p.name.startswith("._"):
            found.append(p)
        elif p.is_dir():
            for root, _, files in os.walk(p):
                for f in files:
                    if f.startswith("._"):
                        continue
                    ext = Path(f).suffix.lower()
                    if ext in VIDEO_EXTENSIONS:
                        found.append(Path(root) / f)
    # Remove duplicates while preserving order
    seen = set()
    deduped = []
    for f in found:
        resolved = f.resolve()
        if resolved not in seen:
            seen.add(resolved)
            deduped.append(f)
    return deduped

def run_single_test(
    binary: Path,
    root_dir: Path,
    video_file: Path,
    test_loop: bool = False,
    stress_iter: int = 0,
    paced: bool = False,
    sim_fps: float = 60.0,
    duration: float = 0.0,
) -> dict:
    try:
        rel_path = video_file.relative_to(root_dir)
        path_str = str(rel_path).replace("\\", "/")
    except ValueError:
        path_str = str(video_file.resolve()).replace("\\", "/")

    with tempfile.NamedTemporaryFile(suffix=".json", delete=False) as tf:
        out_json_path = Path(tf.name)

    is_standalone = "test_movie_decoding" in binary.name.lower()

    if is_standalone:
        cmd = [
            str(binary),
            path_str,
            f"--output={out_json_path.resolve()}",
            "--json"
        ]
        if stress_iter > 0:
            cmd.append(f"--stress={stress_iter}")
        elif test_loop:
            cmd.append("--loop")
        if paced:
            cmd.append("--paced")
            cmd.append(f"--sim-fps={sim_fps}")
            if duration > 0:
                cmd.append(f"--duration={duration}")
    else:
        cmd = [
            str(binary),
            f"--test-video={path_str}",
            f"--test-video-output={out_json_path.resolve()}"
        ]
        if stress_iter > 0:
            cmd.append(f"--test-video-stress={stress_iter}")
        elif test_loop:
            cmd.append("--test-video-loop")

    try:
        proc = subprocess.run(
            cmd,
            cwd=str(root_dir),
            capture_output=True,
            text=True,
            timeout=120
        )
        if out_json_path.exists() and out_json_path.stat().st_size > 0:
            raw_json = out_json_path.read_text(encoding="utf-8")
            data = json.loads(raw_json)
            data["returncode"] = proc.returncode
            return data
        else:
            stdout = proc.stdout
            json_start = stdout.find("{")
            json_end = stdout.rfind("}")
            if json_start != -1 and json_end != -1:
                raw_json = stdout[json_start:json_end + 1]
                data = json.loads(raw_json)
                data["returncode"] = proc.returncode
                return data
            else:
                return {
                    "filename": path_str,
                    "success": False,
                    "error_message": f"Harness produced no JSON output (code {proc.returncode}): {proc.stderr[:200]}",
                    "returncode": proc.returncode
                }
    except subprocess.TimeoutExpired:
        return {
            "filename": path_str,
            "success": False,
            "error_message": "Test timed out after 120s"
        }
    except Exception as e:
        return {
            "filename": path_str,
            "success": False,
            "error_message": str(e)
        }
    finally:
        if out_json_path.exists():
            try:
                out_json_path.unlink()
            except OSError:
                pass

def format_table(results: list[dict]) -> str:
    header = f"{'File':<40} | {'Res':<10} | {'Frames':<7} | {'PTS (s)':<14} | {'Order':<6} | {'FPS':<8} | {'Lat(us)':<8} | {'Status':<6}"
    divider = "-" * len(header)
    rows = [header, divider]

    for r in results:
        fn = Path(r.get("filename", "")).name
        if len(fn) > 38:
            fn = fn[:35] + "..."
        res = f"{r.get('width', 0)}x{r.get('height', 0)}" if r.get('width') else "N/A"
        frames = str(r.get("displayed_frames") if r.get("paced_mode") else r.get("total_frames", 0))
        pts = f"{r.get('min_pts', 0.0):.2f}-{r.get('max_pts', 0.0):.2f}" if r.get('total_frames') else "N/A"
        ooo = r.get("out_of_order_count", 0)
        order = "OK" if ooo == 0 else f"!{ooo}"
        fps = f"{r.get('throughput_fps', 0.0):.1f}" if r.get('throughput_fps') else "N/A"
        lat = f"{r.get('avg_copy_latency_us', 0.0):.1f}" if r.get('avg_copy_latency_us') else "N/A"
        status = "PASS" if r.get("success") else "FAIL"

        rows.append(f"{fn:<40} | {res:<10} | {frames:<7} | {pts:<14} | {order:<6} | {fps:<8} | {lat:<8} | {status:<6}")

    return "\n".join(rows)

def main():
    parser = argparse.ArgumentParser(description="ITGmania Video Subsystem Test & Benchmark Suite")
    parser.add_argument("--scan", action="append", default=[], help="Directories to scan for videos (default: Songs/, Themes/)")
    parser.add_argument("--file", type=str, help="Specific video file to test")
    parser.add_argument("--loop", action="store_true", help="Verify looping and rewind correctness")
    parser.add_argument("--paced", action="store_true", help="Simulate real-time in-game gameplay frame clock pacing")
    parser.add_argument("--sim-fps", type=float, default=60.0, help="Target game simulation FPS for paced mode (default: 60.0)")
    parser.add_argument("--duration", type=float, default=0.0, help="Max test duration in seconds (0 for full video)")
    parser.add_argument("--sample", type=int, default=0, help="Sample N videos across discovered files")
    parser.add_argument("--seed", type=int, default=42, help="Random seed for sampling (default: 42)")
    parser.add_argument("--stress", type=int, default=0, help="Run rapid open/close stress test with N iterations")
    parser.add_argument("--json", action="store_true", help="Output raw JSON results")
    parser.add_argument("--report", type=str, help="Save markdown report to file")
    args = parser.parse_args()

    root_dir = Path(__file__).resolve().parent.parent
    binary = find_test_binary(root_dir)

    scan_targets = []
    if args.file:
        scan_targets.append(Path(args.file))
    elif args.scan:
        scan_targets.extend([Path(s) for s in args.scan])
    else:
        for default_dir in ["Songs", "Themes"]:
            d = root_dir / default_dir
            if d.exists():
                scan_targets.append(d)

    videos = scan_for_videos(scan_targets)
    if not videos:
        print("No video files found to test.")
        sys.exit(0)

    if args.sample > 0 and args.sample < len(videos):
        import random
        random.seed(args.seed)
        videos = sorted(random.sample(videos, args.sample))

    print(f"Discovered {len(videos)} video file(s) across target folders.")
    print(f"Using test harness binary: {binary.relative_to(root_dir)}\n")

    results = []
    for idx, video in enumerate(videos, 1):
        rel = video.relative_to(root_dir) if video.is_relative_to(root_dir) else video
        print(f"[{idx}/{len(videos)}] Testing {rel} ...", end=" ", flush=True)
        res = run_single_test(
            binary,
            root_dir,
            video,
            test_loop=args.loop,
            stress_iter=args.stress,
            paced=args.paced,
            sim_fps=args.sim_fps,
            duration=args.duration,
        )
        results.append(res)
        if res.get("success"):
            f_count = res.get("displayed_frames") if res.get("paced_mode") else res.get("total_frames", 0)
            print(f"[PASS] ({f_count} frames, {res.get('throughput_fps', 0):.1f} FPS, {res.get('avg_copy_latency_us', 0):.1f} us copy)")
        else:
            print(f"[FAIL] ({res.get('error_message', 'Unknown error')})")

    print("\n" + format_table(results) + "\n")

    passed = sum(1 for r in results if r.get("success"))
    failed = len(results) - passed

    print(f"Results: {passed} passed, {failed} failed (Total: {len(results)})")

    if args.report:
        report_path = Path(args.report)
        md_content = f"# Video Subsystem Benchmark Report\n\n"
        md_content += f"- **Date**: {time.strftime('%Y-%m-%d %H:%M:%S')}\n"
        md_content += f"- **Total Videos**: {len(results)}\n"
        md_content += f"- **Passed**: {passed}\n"
        md_content += f"- **Failed**: {failed}\n\n"
        md_content += "```\n" + format_table(results) + "\n```\n"
        report_path.write_text(md_content, encoding="utf-8")
        print(f"Saved benchmark report to {report_path}")

    if args.json:
        print(json.dumps(results, indent=2))

    sys.exit(0 if failed == 0 else 1)

if __name__ == "__main__":
    main()
