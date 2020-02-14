/*      File: rtplot.h
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

#include "colors.h"

#include <string>
#include <memory>
#include <vector>

namespace rtp {

class RTPlotCore;

/**
 * GUI framework agnostic interface for real time data plotting.
 * RTPlot can handle multiple plots inside the same window, each containing
 * multiple curves, in real time with minimum CPU and memory usage.
 */
class RTPlot {
public:
    RTPlot();
    ~RTPlot();

    /**
     * Set the size of the grig containing the plots.
     * @param rows number of rows.
     * @param cols number of columns.
     */
    void setGridSize(size_t rows, size_t cols);

    /**
     * Add a new point to a curve.
     * @param plot  the index of the plot containing the curve. Must be in the
     * [0, \a rows*\a cols[ interval.
     * @param curve the index of the curve. User defined, can be any number.
     * @param x     the x coordinate of the point.
     * @param y     the y coordinate of the point.
     */
    void addPoint(size_t plot, int curve, float x, float y);

    /**
     * Remove the first point of a curve.
     * @param plot  the index of the plot containing the curve. Must be in the
     * [0, \a rows*\a cols[ interval.
     * @param curve the index of the curve. Must match with the index of a
     * previously created curve (see addPoint).
     */
    void removeFirstPoint(size_t plot, int curve);

    /**
     * Stop the plotting and close the window.
     */
    void quit();

    /**
     * Set the x axis label for a given plot.
     * @param plot the index of the plot containing the curve. Must be in the
     * [0, \a rows*\a cols[ interval.
     * @param the string to use as a label.
     */
    void setXLabel(size_t plot, const std::string& name);

    /**
     * Set the y axis label for a given plot.
     * @param plot the index of the plot containing the curve. Must be in the
     * [0, \a rows*\a cols[ interval.
     * @param the string to use as a label.
     */
    void setYLabel(size_t plot, const std::string& name);

    /**
     * Set the label for a given curve. Will be displayed inside the curves'
     * label area on the right of the plot.
     * @param plot  the index of the plot containing the curve. Must be in the
     * [0, \a rows*\a cols[ interval.
     * @param curve the index of the curve. User defined, can be any number.
     * @param name  the label for the curve.
     */
    void setCurveLabel(size_t plot, int curve, const std::string& name);

    /**
     * Set the name for a given plot. Will be printed on top of it.
     * @param plot the index of the plot containing the curve. Must be in the
     * [0, \a rows*\a cols[ interval.
     * @param name the name for the plot
     */
    void setPlotName(size_t plot, const std::string& name);

    /**
     * Refresh the plots with the current data.
     */
    virtual void refresh() = 0;

    /**
     * Enable the automatic refreshing of the plots.
     * @param period_ms the time in milliseconds between each call to refresh.
     */
    void enableAutoRefresh(uint period_ms);

    /**
     * Disable the automatic refreshing of the plots.
     */
    void disableAutoRefresh();

    /**
     * Set the display range for the x axis
     * @param plot the index of the plot containing the curve. Must be in the
     * [0, \a rows*\a cols[ interval.
     * @param min  minimum value
     * @param max  maximum value
     */
    void setXRange(size_t plot, float min, float max);

    /**
     * Set the display range for the y axis
     * @param plot the index of the plot containing the curve. Must be in the
     * [0, \a rows*\a cols[ interval.
     * @param min  minimum value
     * @param max  maximum value
     */
    void setYRange(size_t plot, float min, float max);

    /**
     * Enable the automatic computation of the display range for the x axis.
     * @param plot the index of the plot containing the curve. Must be in the
     * [0, \a rows*\a cols[ interval.
     */
    void autoXRange(size_t plot);

    /**
     * Enable the automatic computation of the display range for the y axis.
     * @param plot the index of the plot containing the curve. Must be in the
     * [0, \a rows*\a cols[ interval.
     */
    void autoYRange(size_t plot);

    /**
     * Set the maximum number of points to be displayed for a given plot. The
     * same value is applied to all curves.
     * @param plot the index of the plot containing the curve. Must be in the
     * [0, \a rows*\a cols[ interval.
     * @param count number of points to display
     */
    void setMaxPoints(size_t plot, size_t count);

    /**
     * Start plotting the data and respond to events triggered by the user. This
     * is a blocking call, the function will return once the window is closed.
     */
    virtual void run() = 0;

    /**
     * Refresh the window if necessary and handle events triggered by the user.
     * This is a non-blocking call. return true if the window is still alive,
     * false otherwise.
     */
    virtual bool check() = 0;

    /**
     * Change for all plots the color palette used to draw the curves
     * @param palette the vector containing the colors to use
     */
    void setColorPalette(const std::vector<Colors>& palette);

    /**
     * Change for a given plot the color palette used to draw the curves
     * @param palette the vector containing the colors to use
     */
    void setColorPalette(size_t plot, const std::vector<Colors>& palette);

    /**
     * Get the color palette used to draw the curves of a given plot
     * @return the vector containing the colors to use
     */
    const std::vector<Colors>& getColorPalette(size_t plot);

    /**
     * Get the average time spent to redraw the widget
     * @param plot the index of the plot containing the curve. Must be in the
     * [0, \a rows*\a cols[ interval.
     * @return the duration, in seconds
     */
    virtual double getAverageRedrawDuration(size_t plot) final;
    virtual double getAverageDrawLineDuration(size_t plot) final;
    virtual double getAverageEndLineDuration(size_t plot) final;

protected:
    /**
     * Must create and initialize the RTPlot window and layout.
     */
    virtual void create() = 0;

    /**
     * Create a new instance of the plotting widget
     * @return a shared pointer to the base class RTPlotCore
     */
    virtual std::shared_ptr<RTPlotCore> makePlot() = 0;

    /**
     * Must be call in the derived class constructor
     */
    virtual void init() final;

    /**
     * Get the default width for a plotting widget
     * @return the width in pixels
     */
    constexpr size_t getPlotWidth() {
        return 655;
    }

    /**
     * Get the default height for a plotting widget
     * @return the height in pixels
     */
    constexpr size_t getPlotHeight() {
        return 450;
    }

    struct rtplot_members;
    std::unique_ptr<rtplot_members> impl_;

private:
    /**
     * Check if the given plot exists. If not, create a new one.
     * @param plot the index of the plot containing the curve. Must be in the
     * [0, \a rows*\a cols[ interval.
     */
    void checkPlot(size_t plot);

    /**
     * Update the layout to display the already created plots
     */
    void updateLayout();
};

} // namespace rtp
