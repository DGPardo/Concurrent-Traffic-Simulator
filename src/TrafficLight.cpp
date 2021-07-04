#include <iostream>
#include <random>
#include "TrafficLight.h"

/* Implementation of class "MessageQueue" */

namespace
{

std::random_device r;
std::default_random_engine rndEngine(r());
std::uniform_real_distribution<float> distribution_4To6(4.0, 6.0);

float getRandomNumberBetween4And6()
{
    return distribution_4To6(rndEngine);
}

}

template <typename T>
T MessageQueue<T>::receive()
{
    std::unique_lock<std::mutex> uLck(_mtx);
    _cv.wait(uLck, [this] (){ return !_queue.empty(); });

    T msg = std::move(_queue.front());
    _queue.pop_front();
    return msg;
}


template <typename T>
void MessageQueue<T>::send(T &&msg)
{
    std::lock_guard<std::mutex> lock(_mtx);
    _queue.clear();
    _queue.push_back(msg);
    _cv.notify_one();
}


/* Implementation of class "TrafficLight" */


TrafficLight::TrafficLight()
{
    _currentPhase = TrafficLightPhase::red;
}


void TrafficLight::waitForGreen()
{
    if (_currentPhase == TrafficLightPhase::green)
        return;

    while (_msgQueue.receive() != TrafficLightPhase::green){}
}


TrafficLightPhase TrafficLight::getCurrentPhase()
{
    return _currentPhase;
}


void TrafficLight::simulate()
{
    threads.emplace_back(std::thread(&TrafficLight::cycleThroughPhases, this));
}


// virtual function which is executed in a thread
void TrafficLight::cycleThroughPhases()
{
    std::lock_guard<std::mutex> lck(_mutex);

    float cycleDuration(1000*getRandomNumberBetween4And6());  // Miliseconds
    auto lastSwitch = std::chrono::system_clock::now();

    while (true)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        auto elapsed_time = std::chrono::duration_cast<std::chrono::milliseconds>
        (
            std::chrono::system_clock::now() - lastSwitch
        );

        if (elapsed_time.count() > cycleDuration)
        {
            if (_currentPhase == TrafficLightPhase::red)
                _currentPhase = TrafficLightPhase::green;
            else
                _currentPhase = TrafficLightPhase::red;

            _msgQueue.send(TrafficLightPhase(_currentPhase));
            lastSwitch = std::chrono::system_clock::now();
        }
    }
}
