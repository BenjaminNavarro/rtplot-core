#pragma once

#include <cstddef>

namespace rtp {

class RTPlotWindow {
public:
	RTPlotWindow() = default;
	virtual ~RTPlotWindow() = default;

	virtual void show() = 0;
	virtual void hide() = 0;
	virtual void setMinimumSize(size_t width, size_t height) = 0;
	virtual void redraw() = 0;
};

}
