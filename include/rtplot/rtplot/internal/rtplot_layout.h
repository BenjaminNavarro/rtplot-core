/*      File: rtplot_layout.h
*       This file is part of the program rtplot-core
*       Program description : Core functionalities to be used by GUI libraries for real time plotting
*       Copyright (C) 2018 -  Benjamin Navarro (LIRMM). All Right reserved.
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
*       along with this software. If not, it can be found on the official website
*       of the CeCILL licenses family (http://www.cecill.info/index.en.html).
*/
#pragma once

#include <vector>
#include <memory>
#include <cstddef>

namespace rtp {

class RTPlotCore;

class RTPlotLayout {
public:
    RTPlotLayout() = default;
    virtual ~RTPlotLayout() = default;

    virtual void setPlots(std::vector<std::shared_ptr<RTPlotCore>> widgets,
                          size_t num_rows, size_t num_cols) = 0;
};

} // namespace rtp
