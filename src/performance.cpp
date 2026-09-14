#include "performance.hpp"
#include "constants.hpp"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <sstream>
#include <iomanip>

namespace fs = std::filesystem;

namespace {

constexpr const char* REPORT_DIRECTORY = "report";

double milliseconds(std::chrono::steady_clock::duration duration) {
    return std::chrono::duration<double, std::milli>(duration).count();
}

double average_fps(const std::vector<double>& frame_times) {
    if (frame_times.empty()) {
        return 0.0;
    }
    const double sum = std::accumulate(frame_times.begin(), frame_times.end(), 0.0);
    return 1000.0 / (sum / static_cast<double>(frame_times.size()));
}

double percentile(const std::vector<double>& sorted, double value) {
    const std::size_t index = static_cast<std::size_t>(
        std::round(static_cast<double>(sorted.size() - 1) * value)
    );
    return sorted[index];
}

void write_stats(std::ofstream& writer, const std::string& name, const std::vector<double>& samples) {
    if (samples.empty()) {
        writer << name << "_samples=0\n";
        return;
    }

    std::vector<double> sorted = samples;
    std::sort(sorted.begin(), sorted.end());

    const double average = std::accumulate(sorted.begin(), sorted.end(), 0.0)
                            / static_cast<double>(sorted.size());

    writer << name << "_samples=" << sorted.size() << "\n";
    writer << name << "_average=" << std::fixed << std::setprecision(4) << average << "\n";
    writer << name << "_min=" << std::fixed << std::setprecision(4) << sorted.front() << "\n";
    writer << name << "_p95=" << std::fixed << std::setprecision(4) << percentile(sorted, 0.95) << "\n";
    writer << name << "_max=" << std::fixed << std::setprecision(4) << sorted.back() << "\n";
}

std::uint32_t next_report_number() {
    std::uint32_t highest = 0;

    for (const auto& entry : fs::directory_iterator(REPORT_DIRECTORY)) {
        const std::string file_name = entry.path().filename().string();

        constexpr std::string_view prefix = "report_";
        constexpr std::string_view suffix = ".txt";

        if (file_name.size() <= prefix.size() + suffix.size()) {
            continue;
        }
        if (file_name.substr(0, prefix.size()) != prefix) {
            continue;
        }
        if (file_name.substr(file_name.size() - suffix.size()) != suffix) {
            continue;
        }

        const std::string number_str = file_name.substr(
            prefix.size(),
            file_name.size() - prefix.size() - suffix.size()
        );

        try {
            const std::uint32_t number = static_cast<std::uint32_t>(std::stoul(number_str));
            highest = std::max(highest, number);
        } catch (...) {
            continue; // pas un nombre valide, on ignore (équivalent du `continue` sur `parse().ok()` en Rust)
        }
    }

    return highest + 1;
}

} // namespace

Performance::Performance()
    : started_at(std::chrono::steady_clock::now())
{
}

void Performance::record_frame(std::chrono::steady_clock::duration frame_time,
                                std::chrono::steady_clock::duration compute_time) {
    frame_times.push_back(milliseconds(frame_time));
    compute_times.push_back(milliseconds(compute_time));
}

void Performance::write_report(int frame_count, std::size_t occupied_voxels) const {
    fs::create_directories(REPORT_DIRECTORY);

    const std::uint32_t report_number = next_report_number();

    std::ostringstream path_stream;
    path_stream << REPORT_DIRECTORY << "/report_"
                << std::setw(4) << std::setfill('0') << report_number << ".txt";
    const std::string report_path = path_stream.str();

    std::ofstream writer(report_path);
    if (!writer) {
        std::cerr << "Unable to write session report: could not open " << report_path << "\n";
        return;
    }

    const double elapsed = std::chrono::duration<double>(
        std::chrono::steady_clock::now() - started_at
    ).count();

    writer << "Voxel Raytracer Session Report\n";
    writer << "==============================\n";
    writer << "report_number=" << std::setw(4) << std::setfill('0') << report_number << "\n";
    writer << "voxel_texture_size=" << VOXEL_TEXTURE_SIZE << "^3\n";
    writer << "voxel_count=" << (VOXEL_TEXTURE_SIZE * VOXEL_TEXTURE_SIZE * VOXEL_TEXTURE_SIZE) << "\n";
    writer << "occupied_voxels=" << occupied_voxels << "\n";
    writer << "display_width=" << DISPLAY_WIDTH << "\n";
    writer << "display_height=" << DISPLAY_HEIGHT << "\n";
    writer << "frames=" << frame_count << "\n";
    writer << "session_seconds=" << std::fixed << std::setprecision(3) << elapsed << "\n";
    writer << "average_fps=" << std::fixed << std::setprecision(2) << average_fps(frame_times) << "\n";

    write_stats(writer, "frame_ms", frame_times);
    write_stats(writer, "compute_ms", compute_times);

    writer.flush();

    fs::copy_file(
        report_path,
        "report.txt",
       fs::copy_options::overwrite_existing
    );

    std::cout << "Session report created with a new number\n";
}