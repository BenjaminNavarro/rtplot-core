/*      File: rtplot_core.cpp
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
#include <rtplot/rtplot_core.h>

#include <iostream>
#include <cassert>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <thread>
#include <mutex>
#include <functional>

using namespace rtp;

constexpr int _plot_margin_left = 90;
constexpr int _plot_margin_top = 30;
constexpr int _plot_margin_right = 40;
constexpr int _plot_margin_bottom = 60;

RTPlotCore::RTPlotCore() {
    palette_ = {Colors::Red,      Colors::Green,       Colors::Yellow,
                Colors::Blue,     Colors::Magenta,     Colors::Cyan,
                Colors::DarkRed,  Colors::DarkGreen,   Colors::DarkYellow,
                Colors::DarkBlue, Colors::DarkMagenta, Colors::DarkCyan};

    xrange_ = std::make_pair(0.0, 10.0);
    yrange_ = std::make_pair(0.0, 10.0);

    xbounds_ = std::make_pair(0, 0);
    ybounds_ = std::make_pair(0, 0);

    label_area_width_ = 0;
    display_labels_ = false;
    toggle_labels_ = false;

    subdivisions_ = 6;

    auto_xrange_ = false;
    auto_yrange_ = false;

    display_cursor_coordinates_ = false;

    display_labels_btn_text_ = "+";
}

RTPlotCore::~RTPlotCore() = default;

void RTPlotCore::addPoint(int curve, float x, float y) {
    auto& data = curves_data_[curve];

    if (data.points.size() == data.max_points) {
        removeFirstPoint(curve);
    }

    std::lock_guard<std::mutex> lock(data.lock_);

    data.points.push_back(std::make_pair(x, y));
    data.ordered_list.first.insert(x);
    data.ordered_list.second.insert(y);

    if (auto_xrange_) {
        float min_val = std::numeric_limits<float>::infinity();
        float max_val = -std::numeric_limits<float>::infinity();
        for (auto& curve_data : curves_data_) {
            auto& list = curve_data.second.ordered_list.first; // xvalues
            if (list.empty()) {
                continue;
            }
            float c_min = *list.begin();
            float c_max = *(--list.end());
            min_val = std::min(min_val, c_min);
            max_val = std::max(max_val, c_max);
        }
        xrange_auto_.first = min_val;
        xrange_auto_.second = max_val;
    }
    if (auto_yrange_) {
        float min_val = std::numeric_limits<float>::infinity();
        float max_val = -std::numeric_limits<float>::infinity();
        for (auto& curve_data : curves_data_) {
            auto& list = curve_data.second.ordered_list.second; // yvalues
            if (list.empty()) {
                continue;
            }
            float c_min = *list.begin();
            float c_max = *(--list.end());
            min_val = std::min(min_val, c_min);
            max_val = std::max(max_val, c_max);
        }
        yrange_auto_.first = min_val;
        yrange_auto_.second = max_val;
    }
}

void RTPlotCore::removeFirstPoint(int curve) {
    CurveData* data;
    try {
        data = &(curves_data_.at(curve));               // check for existance
        if (data->points.begin() == data->points.end()) // check if non-empty
            return;
    } catch (const std::out_of_range& oor) {
        std::cerr << "Curve " << curve
                  << " doesn't exist, can't remove a point from it\n";
        return;
    }

    std::lock_guard<std::mutex> lock(data->lock_);

    auto& points = data->points;
    auto& ordered_list = data->ordered_list;

    PointXY removed_point = points.front();

    points.pop_front();

    auto& xlist = ordered_list.first;
    auto it = xlist.find(removed_point.first);
    if (it != xlist.end()) { // Shouldn't be necessary
        xlist.erase(it);
    }

    auto& ylist = ordered_list.second;
    it = ylist.find(removed_point.second);
    if (it != ylist.end()) { // Shouldn't be necessary
        ylist.erase(it);
    }
}

void RTPlotCore::displayLabels() {
    if (not display_labels_) {
        display_labels_ = true;
        display_labels_btn_text_ = "-";

        int max_text_width = 0;
        for (auto& curve : curves_data_) {
            auto& lbl = curve.second.label;
            Pairf size = measureText(lbl);
            max_text_width = std::max<int>(max_text_width, size.first);
        }
        label_area_width_ = max_text_width;
        if (label_area_width_)
            label_area_width_ += 40;

        refresh();
    }
}
void RTPlotCore::hideLabels() {
    if (display_labels_) {
        display_labels_ = false;
        display_labels_btn_text_ = "+";
        label_area_width_ = 0;

        refresh();
    }
}

void RTPlotCore::toggleLabels() {
    toggle_labels_ = true;
}

void RTPlotCore::setSubdivisions(int sub) {
    assert(sub > 0);
    subdivisions_ = sub;
}

void RTPlotCore::setXRange(float min, float max) {
    xrange_ = std::make_pair(min, max);
    auto_xrange_ = false;
}

void RTPlotCore::setYRange(float min, float max) {
    yrange_ = std::make_pair(min, max);
    auto_yrange_ = false;
}

void RTPlotCore::setXLabel(const std::string& label) {
    xlabel_ = label;
}

void RTPlotCore::setYLabel(const std::string& label) {
    ylabel_ = label;
}

void RTPlotCore::setPlotName(const std::string& name) {
    plot_name_ = name;
}

void RTPlotCore::setCurveLabel(int curve, const std::string& label) {
    curves_data_[curve].label = label;
}

void RTPlotCore::setAutoXRange() {
    auto_xrange_ = true;
}

void RTPlotCore::setAutoYRange() {
    auto_yrange_ = true;
}

void RTPlotCore::setMaxPoints(int curve, size_t count) {
    curves_data_[curve].max_points = count;
}

void RTPlotCore::setMaxPoints(size_t count) {
    for (auto& data : curves_data_) {
        data.second.max_points = count;
    }
}

double RTPlotCore::getAverageRedrawDuration() const {
    return draw_timer.getAverageTime();
}

double RTPlotCore::getAverageDrawLineDuration() const {
    return draw_line_timer.getAverageTime();
}

double RTPlotCore::getAverageEndLineDuration() const {
    return end_line_timer.getAverageTime();
}

void RTPlotCore::labelsToggleButtonCallback() {
    toggleLabels();
}

const std::string& RTPlotCore::getToggleButtonText() {
    return display_labels_btn_text_;
}

RTPlotCore::Pairf RTPlotCore::getToggleButtonSize() {
    return Pairf{20, 20};
}

RTPlotCore::Pairf RTPlotCore::getToggleButtonPosition() {
    return Pairf{getXPosition() + getWidth() - _plot_margin_right + 10,
                 getYPosition() + 10};
}

void RTPlotCore::drawPlot() {
    draw_timer.start();

    saveColor();

    if (toggle_labels_) {
        toggle_labels_ = false;
        if (display_labels_) {
            hideLabels();
        } else {
            displayLabels();
        }
    }

    plot_size_ = std::make_pair(
        getWidth() - _plot_margin_left - _plot_margin_right - label_area_width_,
        getHeight() - _plot_margin_top - _plot_margin_bottom);
    plot_offset_ = std::make_pair(getXPosition() + _plot_margin_left,
                                  getYPosition() + _plot_margin_top);

    if (display_labels_)
        drawLabels();

    drawAxes();

    // Avoid drawing outside of the plot area
    pushClip(plot_offset_, plot_size_);
    int idx = 0;
    initScaleToPlot();
    for (auto& data : curves_data_) {
        data.second.lock_.lock();
        auto& c = data.second.points;
        if (c.size() > 1) {
            PointXY prev, curr;
            auto it = c.begin();

            scaleToPlot(*it, prev);

            setColor(palette_[idx++ % palette_.size()]);
            startLine();
            for (++it; it != c.end(); ++it) {
                scaleToPlot(*it, curr);
                draw_line_timer.start();
                drawLine(prev, curr);
                draw_line_timer.end();
                prev = curr;
            }
            end_line_timer.start();
            endLine();
            end_line_timer.end();
        }
        data.second.lock_.unlock();
    }
    popClip();

    if (display_cursor_coordinates_) {
        PointXY p = scaleToGraph(last_cursor_position_);
        setColor(Colors::Black);
        drawText(
            std::to_string(p.first) + ", " + std::to_string(p.second),
            PointXY{getXPosition() + 10, getYPosition() + getHeight() - 10});
    }

    restoreColor();

    draw_timer.end();
}

void RTPlotCore::handleWidgetEvent(MouseEvent event, PointXY cursor_position) {
    switch (event) {
    case MouseEvent::EnterWidget:
        display_cursor_coordinates_ = true;
        break;
    case MouseEvent::LeaveWidget:
        display_cursor_coordinates_ = false;
        break;
    case MouseEvent::MoveInsideWidget:
        last_cursor_position_ = cursor_position;
        break;
    default:
        break;
    }
}

void RTPlotCore::drawAxes() {
    Pairf xrange, yrange;
    xrange = auto_xrange_ ? xrange_auto_ : xrange_;
    yrange = auto_yrange_ ? yrange_auto_ : yrange_;
    setColor(Colors::Black);
    auto txt_size = measureText(ylabel_);
    drawText(
        ylabel_,
        PointXY{getXPosition() + 10 + txt_size.second / 2,
                plot_offset_.second + (plot_size_.second + txt_size.first) / 2},
        90);

    txt_size = measureText(xlabel_);
    drawText(xlabel_, PointXY{plot_offset_.first +
                                  (plot_size_.first - txt_size.first) / 2,
                              plot_offset_.second + plot_size_.second + 40});

    txt_size = measureText(plot_name_);
    drawText(plot_name_, PointXY{plot_offset_.first +
                                     (plot_size_.first - txt_size.first) / 2,
                                 plot_offset_.second - txt_size.second / 2});

    // Y axis line
    startLine();
    draw_line_timer.start();
    drawLine(plot_offset_, PointXY{plot_offset_.first,
                                   plot_offset_.second + plot_size_.second});
    draw_line_timer.end();
    end_line_timer.start();
    endLine();
    end_line_timer.end();
    // X axis line
    startLine();
    draw_line_timer.start();
    drawLine(
        PointXY{plot_offset_.first, plot_offset_.second + plot_size_.second},
        PointXY{plot_offset_.first + plot_size_.first,
                plot_offset_.second + plot_size_.second});
    draw_line_timer.end();
    end_line_timer.start();
    endLine();
    end_line_timer.end();

    // Draw axes ticks
    int nticks = 4 * subdivisions_;
    float xtick = plot_size_.first / float(nticks);
    float ytick = plot_size_.second / float(nticks);
    float xtick_range = float(xrange.second - xrange.first) / float(nticks);
    float ytick_range = float(yrange.second - yrange.first) / float(nticks);
    for (int i = 1; i <= nticks; ++i) {
        float xstart, xend, ystart, yend;
        // X axis tick
        xstart = xend = plot_offset_.first + i * xtick;
        ystart = yend = plot_offset_.second + plot_size_.second;

        if (i % 4)
            yend -= 3; // small tick
        else {
            yend -= 6; // big tick
            // Verical dashed gray line
            saveColor();
            setColor(Colors::Gray);
            setLineStyle(LineStyle::Dotted);
            startLine();
            draw_line_timer.start();
            drawLine(PointXY{xstart, ystart - 6},
                     PointXY{xend, plot_offset_.second});
            draw_line_timer.end();
            end_line_timer.start();
            endLine();
            end_line_timer.end();
            restoreColor();
            setLineStyle(LineStyle::Solid);

            drawXTickValue(i * xtick_range + xrange.first,
                           std::make_pair(xstart, ystart));
        }
        startLine();
        draw_line_timer.start();
        drawLine(PointXY{xstart, ystart}, PointXY{xend, yend});
        draw_line_timer.end();
        end_line_timer.start();
        endLine();
        end_line_timer.end();

        // Y axis tick
        xstart = xend = plot_offset_.first;
        ystart = yend = plot_offset_.second + plot_size_.second - i * ytick;

        if (i % 4)
            xend += 3; // small tick
        else {
            xend += 6; // big tick
            // Horizontal dashed gray line
            saveColor();
            setColor(Colors::Gray);
            setLineStyle(LineStyle::Dotted);
            startLine();
            draw_line_timer.start();
            drawLine(PointXY{xstart + 6, ystart},
                     PointXY{plot_offset_.first + plot_size_.first, yend});
            draw_line_timer.end();
            end_line_timer.start();
            endLine();
            end_line_timer.end();
            restoreColor();
            setLineStyle(LineStyle::Solid);

            drawYTickValue(i * ytick_range + yrange.first,
                           std::make_pair(xstart, ystart));
        }
        startLine();
        draw_line_timer.start();
        drawLine(PointXY{xstart, ystart}, PointXY{xend, yend});
        draw_line_timer.end();
        end_line_timer.start();
        endLine();
        end_line_timer.end();
    }
}

void RTPlotCore::drawLabels() {
    int texth = 16, yoffset = 0, idx = 0;
    int xstart = plot_offset_.first + plot_size_.first + 10;
    int ystart;

    pushClip(PointXY{xstart, getYPosition()},
             Pairf{xstart + label_area_width_, getHeight()});

    saveColor();

    for (auto& data : curves_data_) {
        auto& lbl = data.second.label;

        setColor(Colors::Black);

        ystart = plot_offset_.second + yoffset;
        drawText(lbl, PointXY{xstart + 30, ystart + texth / 2});

        setColor(palette_[idx++ % palette_.size()]);
        startLine();
        draw_line_timer.start();
        drawLine(PointXY{xstart, ystart + texth / 4},
                 PointXY{xstart + 20, ystart + texth / 4});
        draw_line_timer.end();
        end_line_timer.start();
        endLine();
        end_line_timer.end();

        yoffset += texth;
    }
    restoreColor();

    popClip();
}

void RTPlotCore::initScaleToPlot() {
    current_xrange_ = auto_xrange_ ? xrange_auto_ : xrange_;
    current_yrange_ = auto_yrange_ ? yrange_auto_ : yrange_;

    current_xscale_ =
        plot_size_.first / (current_xrange_.second - current_xrange_.first);
    current_yscale_ =
        plot_size_.second / (current_yrange_.first - current_yrange_.second);
}

void RTPlotCore::scaleToPlot(const PointXY& in_point, PointXY& out_point) {
    out_point.first =
        plot_offset_.first +
        current_xscale_ * (in_point.first - current_xrange_.first);
    out_point.second =
        plot_offset_.second +
        current_yscale_ * (in_point.second - current_yrange_.second);
}

RTPlotCore::PointXY RTPlotCore::scaleToGraph(const PointXY& point) {
    PointXY ret;
    Pairf xrange, yrange;
    xrange = auto_xrange_ ? xrange_auto_ : xrange_;
    yrange = auto_yrange_ ? yrange_auto_ : yrange_;
    ret.first = xrange.first + (xrange.second - xrange.first) *
                                   (point.first - getXPosition()) /
                                   plot_size_.first;
    ret.second = yrange.second - (yrange.second - yrange.first) *
                                     (point.second - plot_offset_.second) /
                                     plot_size_.second;

    return ret;
}

void RTPlotCore::drawXTickValue(float num, const PointXY& point) {

    char str[15];
    snprintf(str, 15, "%.2f", num);
    auto value = std::string(str);
    auto txt_size = measureText(value);
    drawText(value, PointXY{point.first - txt_size.first / 2,
                            point.second + txt_size.second});
}

void RTPlotCore::drawYTickValue(float num, const PointXY& point) {

    char str[15];
    snprintf(str, 15, "%.2f", num);
    auto value = std::string(str);
    auto txt_size = measureText(value);
    drawText(value, PointXY{point.first - txt_size.first - 5,
                            point.second + txt_size.second / 2 - 2});
}
