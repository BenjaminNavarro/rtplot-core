/*      File: inputparserthread.h
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

#include <thread>

namespace rtp {

class RTPlot;

class InputParserThread {
public:
    InputParserThread(RTPlot* mw);
    ~InputParserThread();

    void run();

    void stop();

    void join();

private:
    int peek_stdin(unsigned int secs);
    bool stop_;
    RTPlot* mw_;
    std::thread th_;

    void process();
};

} // namespace rtp
