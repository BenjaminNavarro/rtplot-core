/*      File: rtplot_core.h
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

#include <utility>
#include <map>
#include <deque>
#include <vector>
#include <mutex>
#include <set>
#include <limits>

namespace rtp {

/**
 * Common interface for all RTPlot implementations.
 */
class RTPlotCore {
public:
    using Pairf = std::pair<float, float>;
    using PointXY = Pairf;

    RTPlotCore();
    virtual ~RTPlotCore();

    /**
     * Refresh the plot. Must trigger a repaint event.
     */
    virtual void refresh() = 0;

    /**
     * Add a new point to a curve.
     * @param curve the index of the curve. User defined, can be any number.
     * @param x     the x coordinate of the point.
     * @param y     the y coordinate of the point.
     */
    void addPoint(int curve, float x, float y);

    /**
     * Remove the first point of a curve.
     * @param curve the index of the curve. Must match with the index of a
     * previously created curve (see addPoint()).
     */
    void removeFirstPoint(int curve);

    /**
     * Display the curves' labels.
     */
    void displayLabels();

    /**
     * Hide the curves' labels.
     */
    void hideLabels();

    /**
     * Toggle the display of the curves' labels.
     */
    void toggleLabels();

    /**
     * Set the number of subdivisions for the x and y axes
     * @param sub number of subdivisions
     */
    void setSubdivisions(int sub);

    /**
     * Set the display range for the x axis
     * @param min  minimum value
     * @param max  maximum value
     */
    void setXRange(float min, float max);

    /**
     * Set the display range for the y axis
     * @param min  minimum value
     * @param max  maximum value
     */
    void setYRange(float min, float max);

    /**
     * Set the x axis label for a given plot.
     * @param the string to use as a label.
     */
    void setXLabel(const std::string& label);

    /**
     * Set the y axis label for a given plot.
     * @param the string to use as a label.
     */
    void setYLabel(const std::string& label);

    /**
     * Set the name for a given plot. Will be printed on top of it.
     * @param name the name for the plot
     */
    void setPlotName(const std::string& name);

    /**
     * Set the label for a given curve. Will be displayed inside the curves'
     * label area on the right of the plot.
     * @param curve the index of the curve. User defined, can be any number.
     * @param name  the label for the curve.
     */
    void setCurveLabel(int curve, const std::string& label);

    /**
     * Enable the automatic computation of the display range for the x axis.
     */
    void setAutoXRange();

    /**
     * Enable the automatic computation of the display range for the y axis.
     */
    void setAutoYRange();

    /**
     * Set the maximum number of points to be displayed for a given curve.
     * @param curve the index of the curve. User defined, can be any number.
     * @param count number of points to display
     */
    void setMaxPoints(int curve, size_t count);

    /**
     * Set the maximum number of points to be displayed. The same value is
     * applied to all curves.
     * @param count number of points to display
     */
    void setMaxPoints(size_t count);

    /**
     * Set the size of the plotting widget
     * @param size the size
     */
    virtual void setSize(const Pairf& size) = 0;

    /**
     * Set the postition of the plotting widget
     * @param size the postition
     */
    virtual void setPosition(const PointXY& position) = 0;

    /**
     * Change the color palette used to draw the curves
     * @param palette the vector containing the colors to use
     */
    void setColorPalette(const std::vector<Colors>& palette);

    /**
     * Get the color palette used to draw the curves
     * @return the vector containing the colors to use
     */
    const std::vector<Colors>& getColorPalette();

    /**
     * Get the average time spent to redraw the widget
     * @return the duration, in seconds
     */
    virtual double getAverageRedrawDuration() const final;
    virtual double getAverageDrawLineDuration() const final;
    virtual double getAverageEndLineDuration() const final;

protected:
    enum class LineStyle { Solid, Dotted };
    enum class MouseEvent {
        EnterWidget,
        LeaveWidget,
        MoveInsideWidget,
        Unknown
    };

    /**
     * Get the current width of the widget
     * @return the width in pixels
     */
    virtual size_t getWidth() = 0;

    /**
     * Get the current height of the widget
     * @return the height in pixels
     */
    virtual size_t getHeight() = 0;

    /**
     * Get the current x position of the widget
     * @return the x position in pixels
     */
    virtual int getXPosition() = 0;

    /**
     * Get the current y position of the widget
     * @return the y position in pixels
     */
    virtual int getYPosition() = 0;

    /**
     * Set the clipping area. No drawing outside this region must be allowed
     * @param start the coordinates of the top-left corner of the clipping area.
     * @param size  the size in pixels of the clipping area.
     */
    virtual void pushClip(const PointXY& start, const Pairf& size) = 0;

    /**
     * Reset the clipping area to its previous value.
     */
    virtual void popClip() = 0;

    /**
     * Start of a line drawing section
     */
    virtual void startLine() = 0;

    /**
     * Draw a line
     * @param start The coordinates of the starting point
     * @param end   The coordinates of the ending point
     */
    virtual void drawLine(const PointXY& start, const PointXY& end) = 0;

    /**
     * End of a line drawing section
     */
    virtual void endLine() = 0;

    /**
     * Set the style of the lines to draw
     * @param style the style.
     */
    virtual void setLineStyle(LineStyle style) = 0;

    /**
     * Draw a text
     * @param text     the text to draw
     * @param position the position of the text
     * @param angle    the text angle in degrees
     */
    virtual void drawText(const std::string& text, const PointXY& position,
                          int angle = 0) = 0;

    /**
     * Measure the size of a text
     * @param  text the text to measure
     * @return      the size in pixels of the text
     */
    virtual Pairf measureText(const std::string& text) = 0;

    /**
     * Set the color to use for drawing
     * @param color the color
     */
    virtual void setColor(Colors color) = 0;

    /**
     * Save to color currently in use
     */
    virtual void saveColor() = 0;

    /**
     * Restore a previously saved color. See saveColor()
     */
    virtual void restoreColor() = 0;

    /**
     * Must be called when a mouse event occurs with the widget.
     * @param event           the reveived event
     * @param cursor_position the cursor position at the moment of the event
     */
    virtual void handleWidgetEvent(MouseEvent event,
                                   PointXY cursor_position) final;

    /**
     * Must be called the label toggle button is pressed.
     */
    virtual void labelsToggleButtonCallback() final;

    /**
     * Give the text to be displayed by the label toggle button
     * @return the button's text
     */
    virtual const std::string& getToggleButtonText() final;

    /**
     * Give the size of the label toggel button
     * @return the size in pixels
     */
    virtual Pairf getToggleButtonSize() final;

    /**
     * Give the position of the label toggel button
     * @return the position in pixels
     */
    virtual Pairf getToggleButtonPosition() final;

    /**
     * Draw the widget. Must be called for each repaint.
     */
    virtual void drawPlot() final;

private:
    /**
     * Draw the plot's axes, with ticks and labels
     */
    virtual void drawAxes() final;

    /**
     * Draw the curves labels
     */
    virtual void drawLabels() final;

    /**
     * Initialize the scaleToPlot method. For performance reason only.
     */
    virtual void initScaleToPlot() final;

    /**
     * Convert the coordinates of a point into pixels coordinates
     * @param in_point  the point to process
     * @param out_point the result in pixels
     */
    virtual void scaleToPlot(const PointXY& in_point, PointXY& out_point) final;

    /**
     * Convert the pixels coordinates into point coordinates
     * @param  point the pixels coordinates
     * @return       the corresponding point
     */
    virtual PointXY scaleToGraph(const PointXY& point) final;

    /**
     * Draw the value associated to a x axis tick.
     * @param num   the value to draw
     * @param point the text position
     */
    virtual void drawXTickValue(float num, const PointXY& point) final;

    /**
     * Draw the value associated to a y axis tick.
     * @param num   the value to draw
     * @param point the text position
     */
    virtual void drawYTickValue(float num, const PointXY& point) final;

    struct CurveData {
        CurveData() : max_points(std::numeric_limits<size_t>::max()) {
        }

        std::deque<PointXY> points;
        std::pair<std::multiset<float>, std::multiset<float>> ordered_list;
        std::string label;
        size_t max_points;
        std::mutex lock_;
    };
    std::map<int, CurveData> curves_data_;
    Pairf xrange_;
    Pairf yrange_;
    Pairf xrange_auto_;
    Pairf yrange_auto_;
    std::pair<int, int> xbounds_;
    std::pair<int, int> ybounds_;
    PointXY plot_offset_;
    Pairf plot_size_;
    int label_area_width_;
    int subdivisions_;
    std::string xlabel_;
    std::string ylabel_;
    std::string plot_name_;
    std::pair<size_t, size_t> last_cursor_position_;
    bool auto_xrange_;
    bool auto_yrange_;
    bool display_labels_;
    bool toggle_labels_;
    bool display_cursor_coordinates_;

    Pairf current_xrange_, current_yrange_;
    float current_xscale_, current_yscale_;

    std::string display_labels_btn_text_;
    std::vector<Colors> palette_;

    class Timer {
    public:
        Timer() : total_duration_ns_(0), total_calls_(0) {
        }
        void start() {
            t_start = std::chrono::high_resolution_clock::now();
        }

        void end() {
            std::chrono::high_resolution_clock::time_point t_end =
                std::chrono::high_resolution_clock::now();
            total_duration_ns_ +=
                std::chrono::duration_cast<std::chrono::nanoseconds>(t_end -
                                                                     t_start)
                    .count();
            ++total_calls_;
        }

        double getAverageTime() const {
            return 1e-9 * double(total_duration_ns_) / double(total_calls_);
        }

    private:
        size_t total_duration_ns_;
        size_t total_calls_;
        std::chrono::high_resolution_clock::time_point t_start;
    };

    Timer draw_timer;
    Timer draw_line_timer;
    Timer end_line_timer;
};

} // namespace rtp
