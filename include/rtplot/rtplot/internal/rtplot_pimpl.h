#pragma once

#include <rtplot/rtplot.h>
#include <rtplot/rtplot_core.h>

#include <rtplot/internal/rtplot_window.h>
#include <rtplot/internal/rtplot_layout.h>

#include <thread>
#include <vector>
#include <mutex>
#include <cassert>

namespace rtp {

struct RTPlot::rtplot_members {
    rtplot_members() : auto_refresh_period_(0), grid_rows_(1), grid_cols_(1) {
    }

    ~rtplot_members() = default;

    std::unique_ptr<RTPlotWindow> window_;
    std::unique_ptr<RTPlotLayout> layout_;
    std::vector<std::shared_ptr<RTPlotCore>> plots_;

    std::thread auto_refresh_thread_;
    std::mutex auto_refresh_period_lock_;
    size_t auto_refresh_period_;
    size_t grid_rows_;
    size_t grid_cols_;
};

} // namespace rtp
