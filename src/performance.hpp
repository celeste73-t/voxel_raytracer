#pragma once
#include <chrono>
#include <vector>

class Performance
{
public:
    Performance();

    void record_frame(std::chrono::steady_clock::duration frame_time,
                       std::chrono::steady_clock::duration compute_time);

    void write_report(int frame_count, int occupied_voxels) const;

private:
    std::chrono::steady_clock::time_point started_at;
    std::vector<double> frame_times;
    std::vector<double> compute_times;
};