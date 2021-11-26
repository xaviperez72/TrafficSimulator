#ifndef TRAFFICLIGHT_H
#define TRAFFICLIGHT_H

#include <mutex>
#include <deque>
#include <condition_variable>
#include "TrafficObject.h"

// forward declarations to avoid include cycle
class Vehicle;

// FP.3

template <class T>  
class MessageQueue
{
public:
    T receive();
    void send(T&& msg);
    bool is_empty();
private:
    std::mutex _mutex;
    std::condition_variable _cond;
    std::deque<T> _queue;
};




// FP.1 

enum class TrafficLightPhase
{
    red,
    green,
};

class TrafficLight : public TrafficObject
{
public:
    // constructor / desctructor
    TrafficLight();
    // getters / setters
    TrafficLightPhase getCurrentPhase();        // FP.1
    void setCurrentPhase(TrafficLightPhase);    // FP.1

    // typical behaviour methods
    void waitForGreen();                        // FP.1
    void simulate();                            // FP.1
    

private:
    // typical behaviour methods
    virtual void cycleThroughPhases();          // FP.1

    MessageQueue<TrafficLightPhase> _PhasesQueue;

    std::condition_variable _condition;
    std::mutex _mutex;
    TrafficLightPhase _currentPhase;               // FP.1
};

#endif