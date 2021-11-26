#include <iostream>
#include <random>
#include "TrafficLight.h"

/* Implementation of class "MessageQueue" . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . */

template <typename T>
bool MessageQueue<T>::is_empty(){
    std::lock_guard<std::mutex> uLock(_mutex);
    return _queue.empty();
}

template <typename T>
T MessageQueue<T>::receive()
{
    // perform queue modification under the lock
    std::cout << time_in_HH_MM_SS_MMM() << "MessageQueue::receive " << std::endl;
     std::unique_lock<std::mutex> uLock(_mutex);
    _cond.wait(uLock, [this] { return !_queue.empty(); }); // pass unique lock to condition variable

    // remove last vector element from queue
    T msg = std::move(_queue.front());
    _queue.pop_front();

    return msg; // will not be copied due to return value optimization (RVO) in C++
}

template <typename T>
void MessageQueue<T>::send(T &&msg)
{
    // FP.4a : The method send should use the mechanisms std::lock_guard<std::mutex> 
    // as well as _condition.notify_one() to add a new message to the queue and afterwards send a notification.

    // perform vector modification under the lock
    std::lock_guard<std::mutex> uLock(_mutex);

    std::cout << time_in_HH_MM_SS_MMM() << "MessageQueue::send " << std::endl;

    // add vector to queue
    _queue.push_back(std::move(msg));
    _cond.notify_one(); 
}


/* Implementation of class "TrafficLight" . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . */

TrafficLight::TrafficLight()
{
    _currentPhase = TrafficLightPhase::red;
}

// FP.5 
void TrafficLight::waitForGreen()
{
    TrafficLightPhase phase;

    while(true)
    {
        phase = _PhasesQueue.receive();
        if(phase == TrafficLightPhase::green)
        // posible data race in here, but it is safe in this case. 
            if(_PhasesQueue.is_empty())         // Why I do so?   **
                return;
    }
}

// ** If an intersection, and therefore trafficlight, doesn't receive cars for a long time there will be a lot of elements on the queue. 
// The last one must be green. 
// It could be a data race just between if "green" and if "empty", but no problem. If it turns red then car will wait (another loop), 
// otherwise was green in that moment as a real life.

TrafficLightPhase TrafficLight::getCurrentPhase()
{
    std::lock_guard<std::mutex> uLock(_mutex);
    return _currentPhase;
}

void TrafficLight::setCurrentPhase(TrafficLightPhase tlp)
{
    std::lock_guard<std::mutex> uLock(_mutex);
    _currentPhase = tlp;
}

void TrafficLight::simulate()
{
    threads.emplace_back(std::thread(&TrafficLight::cycleThroughPhases, this));     // FP.2b
}


// virtual function which is executed in a thread
void TrafficLight::cycleThroughPhases()
{
    int max = 4000, min = 6000;    // Between 4 and 6 seconds in miliseconds
    long rand_cycleDuration;       // duration of a single simulation cycle in miliseconds
    TrafficLightPhase to_red {TrafficLightPhase::red};
    TrafficLightPhase to_green {TrafficLightPhase::green};

    std::chrono::time_point<std::chrono::system_clock> lastUpdate;

    // init stop watch
    lastUpdate = std::chrono::system_clock::now();
    rand_cycleDuration = rand() % (max - min + 1) + min;

    while (true)
    {
        // sleep at every iteration to reduce CPU usage
        std::this_thread::sleep_for(std::chrono::milliseconds(1));

        // compute time difference to stop watch
        long timeSinceLastUpdate = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - lastUpdate).count();
        if (timeSinceLastUpdate >= rand_cycleDuration)
        {
            if(this->getCurrentPhase() == TrafficLightPhase::green) {
                
                // GREEN --> TO RED;
                
                this->_PhasesQueue.send(std::move(to_red));
                this->setCurrentPhase(TrafficLightPhase::red);
            }
            else {

                // RED  --> TO GREEN;
                
                this->_PhasesQueue.send(std::move(to_green));
                this->setCurrentPhase(TrafficLightPhase::green);
            }
            
            lastUpdate = std::chrono::system_clock::now();
            rand_cycleDuration = rand() % (max - min + 1) + min;
        }
    }
}

