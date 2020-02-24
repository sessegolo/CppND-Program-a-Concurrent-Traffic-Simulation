#include <iostream>
#include <random>
#include <future>
#include "TrafficLight.h"

/* Implementation of class "MessageQueue" */


template <typename T>
T MessageQueue<T>::receive()
{
    std::unique_lock<std::mutex> uniqueLock(_mtx);
    _cond.wait(uniqueLock, [this] { return !_queue.empty(); });

    T message = std::move(_queue.back());
    _queue.pop_back();

    return message;
}

template <typename T>
void MessageQueue<T>::send(T &&msg)
{
    std::lock_guard<std::mutex> lockGuard(_mtx);
    _queue.emplace_back(std::move(msg));
    _cond.notify_one();
}

/* Implementation of class "TrafficLight" */


TrafficLight::TrafficLight()
{
    _currentPhase = TrafficLightPhase::red;
}

void TrafficLight::waitForGreen()
{
    while(true)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));

        TrafficLightPhase currentPhase = _queue.receive();
        if(currentPhase == TrafficLightPhase::green)
        {
            return;
        }
    }
}

TrafficLightPhase TrafficLight::getCurrentPhase()
{
    return _currentPhase;
}

void TrafficLight::simulate()
{
    std::thread cycleTrafficLightPhases = std::thread(&TrafficLight::cycleThroughPhases, this);
    threads.emplace_back(std::move(cycleTrafficLightPhases));
}

// virtual function which is executed in a thread
void TrafficLight::cycleThroughPhases()
{
    // Snippet from https://stackoverflow.com/questions/13445688/how-to-generate-a-random-number-in-c/13445752#13445752
    std::random_device randomDevice;
    std::mt19937 randomEngine(randomDevice());
    std::uniform_int_distribution<std::mt19937::result_type> dist(4,6);

    auto cycleDuration = dist(randomEngine); // in seconds

    std::chrono::time_point<std::chrono::system_clock> lastUpdate = std::chrono::system_clock::now();

    while(true)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        
        long elapsedSeconds = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now() - lastUpdate).count();

        if(elapsedSeconds >= cycleDuration)
        {
            _currentPhase = _currentPhase == TrafficLightPhase::red ? TrafficLightPhase::green : TrafficLightPhase::red;

            // notify the traffic light update
            TrafficLightPhase phase = _currentPhase;
            
            std::future<void> task = std::async(&MessageQueue<TrafficLightPhase>::send, &_queue, std::move(phase));
            task.wait();

            // update stopwatch
            lastUpdate = std::chrono::system_clock::now();
            cycleDuration = dist(randomEngine);
        }
    }
}