/*      File: rtplot_pimpl.h
 *       This file is part of the program rtplot-core
 *       Program description : Core functionalities to be used by GUI libraries
 * for real time plotting Copyright (C) 2018 -  Benjamin Navarro (LIRMM). All
 * Right reserved.
 *
 *       This software is free software: you can redistribute it and/or modify
 *       it under the terms of the CeCILL license as published by
 *       the CEA CNRS INRIA, either version 2.1
 *       of the License, or (at your option) any later version.
 *       This software is distributed in the hope that it will be useful,
 *       but WITHOUT ANY WARRANTY without even the implied warranty of
 *       MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *       CeCILL License for more details.
 *
 *       You should have received a copy of the CeCILL License
 *       along with this software. If not, it can be found on the official
 * website of the CeCILL licenses family (http://www.cecill.info/index.en.html).
 */
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
