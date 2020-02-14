#include <rtplot/rtplot.h>
#include <rtplot/rtplot_core.h>
#include <rtplot/internal/rtplot_pimpl.h>
#include <rtplot/internal/inputparserthread.h>

#include <X11/Xlib.h>

#include <iostream>
#include <chrono>

using namespace std;
using namespace rtp;

namespace {
mutex auto_refresh_mtx;
}

RTPlot::RTPlot() {
    impl_ = std::make_unique<RTPlot::rtplot_members>();
}

void RTPlot::init() {
    create();
    impl_->window_->show();
}

RTPlot::~RTPlot() {
    quit();
}

void RTPlot::checkPlot(size_t plot) {
    assert(plot < impl_->grid_rows_ * impl_->grid_cols_);
    if (not static_cast<bool>(impl_->plots_[plot])) {
        impl_->plots_[plot] = makePlot();
        updateLayout();
    }
}

void RTPlot::updateLayout() {
    impl_->layout_->setPlots(impl_->plots_, impl_->grid_rows_,
                             impl_->grid_cols_);
    refresh();
}

void RTPlot::setGridSize(size_t rows, size_t cols) {
    assert((rows > 1) and (cols > 1));
    impl_->grid_rows_ = rows;
    impl_->grid_cols_ = cols;
    impl_->plots_.resize(rows * cols);
    impl_->window_->setMinimumSize(cols * getPlotWidth(),
                                   rows * getPlotHeight());
    updateLayout();
}

void RTPlot::quit() {
    // if(impl_->parser_)
    //  impl_->parser_->stop();
    disableAutoRefresh();
    // impl_->window_->hide();
}

void RTPlot::addPoint(size_t plot, int curve, float x, float y) {
    checkPlot(plot);
    impl_->plots_[plot]->addPoint(curve, x, y);
}

void RTPlot::removeFirstPoint(size_t plot, int curve) {
    checkPlot(plot);
    impl_->plots_[plot]->removeFirstPoint(curve);
}

void RTPlot::setXLabel(size_t plot, const std::string& name) {
    checkPlot(plot);
    impl_->plots_[plot]->setXLabel(name);
    refresh();
}

void RTPlot::setYLabel(size_t plot, const std::string& name) {
    checkPlot(plot);
    impl_->plots_[plot]->setYLabel(name);
    refresh();
}

void RTPlot::setCurveLabel(size_t plot, int curve, const std::string& name) {
    checkPlot(plot);
    impl_->plots_[plot]->setCurveLabel(curve, name);
    refresh();
}

void RTPlot::setPlotName(size_t plot, const std::string& name) {
    checkPlot(plot);
    impl_->plots_[plot]->setPlotName(name);
    refresh();
}

void RTPlot::enableAutoRefresh(uint period_ms) {
    bool create_thread = (impl_->auto_refresh_period_ == 0);
    impl_->auto_refresh_period_ = period_ms;

    if (create_thread) {
        impl_->auto_refresh_thread_ = thread([this]() {
            while (impl_->auto_refresh_period_) {
                auto_refresh_mtx.lock();
                this->refresh();
                auto_refresh_mtx.unlock();
                this_thread::sleep_for(
                    std::chrono::milliseconds(impl_->auto_refresh_period_));
            }
        });
    }
}

void RTPlot::disableAutoRefresh() {
    if (impl_->auto_refresh_period_) {
        impl_->auto_refresh_period_ = 0;
        impl_->auto_refresh_thread_.join();
    }
}

void RTPlot::setXRange(size_t plot, float min, float max) {
    checkPlot(plot);
    impl_->plots_[plot]->setXRange(min, max);
    refresh();
}

void RTPlot::setYRange(size_t plot, float min, float max) {
    checkPlot(plot);
    impl_->plots_[plot]->setYRange(min, max);
    refresh();
}

void RTPlot::autoXRange(size_t plot) {
    checkPlot(plot);
    impl_->plots_[plot]->setAutoXRange();
    refresh();
}

void RTPlot::autoYRange(size_t plot) {
    checkPlot(plot);
    impl_->plots_[plot]->setAutoYRange();
    refresh();
}

void RTPlot::setMaxPoints(size_t plot, size_t count) {
    checkPlot(plot);
    impl_->plots_[plot]->setMaxPoints(count);
    refresh();
}

double RTPlot::getAverageRedrawDuration(size_t plot) {
    checkPlot(plot);
    return impl_->plots_[plot]->getAverageRedrawDuration();
}

double RTPlot::getAverageDrawLineDuration(size_t plot) {
    checkPlot(plot);
    return impl_->plots_[plot]->getAverageDrawLineDuration();
}

double RTPlot::getAverageEndLineDuration(size_t plot) {
    checkPlot(plot);
    return impl_->plots_[plot]->getAverageEndLineDuration();
}
