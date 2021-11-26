#include <algorithm>
#include <iostream>
#include <chrono>
#include <future>
#include <ctime>
#include <iomanip>
#include <sstream>

#include "TrafficObject.h"

// https://stackoverflow.com/questions/24686846/get-current-time-in-milliseconds-or-hhmmssmmm-format

// It is good to show on logs time and thread id. It helps on debugging or chasing a bug. 
// I learned this in my previous job (in 2008) on a multithread  TCP/IP dispatcher in C language. 

std::string time_in_HH_MM_SS_MMM()
{
    using namespace std::chrono;
    // get current time
    auto now = system_clock::now();

   // get number of milliseconds for the current second
    // (remainder after division into seconds)
    auto ms = duration_cast<milliseconds>(now.time_since_epoch()) % 1000;

    // convert to std::time_t in order to convert to std::tm (broken time)

    std::time_t timer = system_clock::to_time_t(now);

    // convert to broken time
    struct std::tm * ptm = std::localtime(&timer);

    std::ostringstream oss;

    oss << std::put_time(ptm, "%H:%M:%S"); // HH:MM:SS
    oss << '.' << std::setfill('0') << std::setw(3) << ms.count() << ' ' << std::this_thread::get_id() << ' ';

    return oss.str();
}


// init static variable
int TrafficObject::_idCnt = 0;

std::mutex TrafficObject::_mtx;

void TrafficObject::setPosition(double x, double y)
{
    _posX = x;
    _posY = y;
}

void TrafficObject::getPosition(double &x, double &y)
{
    x = _posX;
    y = _posY;
}

TrafficObject::TrafficObject()
{
    _type = ObjectType::noObject;
    _id = _idCnt++;
}

// Why is not call this destructor when CTRL+C?
// I think is because the signal don't call destructors, just exit at once. 

TrafficObject::~TrafficObject()
{
    auto type = this->_type;
    auto id = this->_id;

    std::cout << time_in_HH_MM_SS_MMM() << " BEGIN TrafficObject::~TrafficObject -- Type " << static_cast<std::underlying_type<ObjectType>::type>(type) << " _id " << id << std::endl;

    // set up thread barrier before this object is destroyed
    std::for_each(threads.begin(), threads.end(), [type, id](std::thread &t) {
        std::cout << time_in_HH_MM_SS_MMM() << " Type " << static_cast<std::underlying_type<ObjectType>::type>(type) << " _id " << id << std::endl;
        t.join();
    });
    std::cout << time_in_HH_MM_SS_MMM() << " END   TrafficObject::~TrafficObject -- Type " << static_cast<std::underlying_type<ObjectType>::type>(type) << " _id " << id << std::endl;
}
