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

	virtual void setPlots(std::vector<std::shared_ptr<RTPlotCore>> widgets, size_t num_rows, size_t num_cols) = 0;
};

}
