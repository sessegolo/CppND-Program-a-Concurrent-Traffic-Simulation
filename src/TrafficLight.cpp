#include <iostream>
#include <random>
#include <future>
#include "TrafficLight.h"

/* Implementation of class "MessageQueue" */

/* 
template <typename T>
T MessageQueue<T>::receive()
{
    // FP.5a : The method receive should use std::unique_lock<std::mutex> and _condition.wait() 
    // to wait for and receive new messages and pull them from the queue using move semantics. 
    // The received object should then be returned by the receive function. 
}

template <typename T>
void MessageQueue<T>::send(T &&msg)
{
    // FP.4a : The method send should use the mechanisms std::lock_guard<std::mutex> 
    // as well as _condition.notify_one() to add a new message to the queue and afterwards send a notification.
}
*/

/* Implementation of class "TrafficLight" */


TrafficLight::TrafficLight()
{
    _currentPhase = TrafficLightPhase::red;
}

void TrafficLight::waitForGreen()
{
    // FP.5b : add the implementation of the method waitForGreen, in which an infinite while-loop 
    // runs and repeatedly calls the receive function on the message queue. 
    // Once it receives TrafficLightPhase::green, the method returns.
}

TrafficLightPhase TrafficLight::getCurrentPhase()
{
    return _currentPhase;
}

void TrafficLight::simulate()
{
    // FP.2b : Finally, the private method „cycleThroughPhases“ should be started in a thread when the public method „simulate“ is called. To do this, use the thread queue in the base class. 
    std::thread cycleTrafficLightPhases = std::thread(&TrafficLight::cycleThroughPhases, this);
    threads.emplace_back(std::move(cycleTrafficLightPhases));
}

// virtual function which is executed in a thread
void TrafficLight::cycleThroughPhases()
{
    // FP.2a : Implement the function with an infinite loop that measures the time between two loop cycles 
    // and toggles the current phase of the traffic light between red and green and sends an update method 
    // to the message queue using move semantics. The cycle duration should be a random value between 4 and 6 seconds. 
    // Also, the while-loop should use std::this_thread::sleep_for to wait 1ms between two cycles. 

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

            // update stopwatch
            lastUpdate = std::chrono::system_clock::now();
            cycleDuration = dist(randomEngine);
        }
    }
}