#ifndef TRAFFICLIGHT_H
#define TRAFFICLIGHT_H

#include <mutex>
#include <deque>
#include <condition_variable>
#include "TrafficObject.h"

// forward declarations to avoid include cycle
class Vehicle;


enum class TrafficLightPhase { red, green };

template <class Type>
class MessageQueue
{
public:
    void send(Type&&);
    Type receive();

private:
    std::deque<Type> _queue;
    std::condition_variable _cv;
    std::mutex _mtx;
};

class TrafficLight : public TrafficObject
{
public:

    // constructor / desctructor
    TrafficLight();

    // getters / setters
    TrafficLightPhase getCurrentPhase();

    // typical behaviour methods
    void waitForGreen();
    void simulate();

private:
    // typical behaviour methods
    void cycleThroughPhases();

    TrafficLightPhase _currentPhase;
    std::condition_variable _condition;
    std::mutex _mutex;
    MessageQueue<TrafficLightPhase> _msgQueue;
};

#endif