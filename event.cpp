#include "pch.h"
#include "event.h"


// Event structure
struct Event {
    long long timestamp; // Timestamp in microseconds
    std::function<void()> action; // Action to be performed when the event is processed

    Event(long long ts, std::function<void()> act) : timestamp(ts), action(std::move(act)) {}
};
// Compare for priority queue, to sort events by timestamp in ascending order
struct CompareEvent {
    bool operator()(const Event& a, const Event& b) {
        return a.timestamp > b.timestamp;
    }
};

// Event Queue declaration
std::priority_queue<Event, std::vector<Event>, CompareEvent> eventQueue;
std::mutex queueMutex;
std::condition_variable cv;
bool finished = false;

void generateEvents(std::vector<Neuron*>& inputNeurons, std::vector<Neuron*>& outputNeurons) {
    using namespace std::chrono;
    while (!finished) {
        auto now = high_resolution_clock::now();
        auto timestamp = duration_cast<microseconds>(now.time_since_epoch()).count();

        {
            std::lock_guard<std::mutex> guard(queueMutex);
            // Process events for all neurons
            eventQueue.push(Event(timestamp, [inputNeurons, outputNeurons]() {
                for (Neuron* neuron : inputNeurons) {
                    neuron->currentPotential += 5; // Arbitrary stimulation value
                    neuron->activateSpike(); // Check for activation
                }
                for (Neuron* neuron : outputNeurons) {
                    neuron->activateSpike(); // Check for activation
                }
            }));
        }
        cv.notify_one(); // Notify the processing thread

        std::this_thread::sleep_for(seconds(3));
    }
}

void processEvents() {
    std::unique_lock<std::mutex> lock(queueMutex, std::defer_lock);
    long long lastTimestamp = -1; // Initialize with an invalid value to check for the first event

    while (true) {
        lock.lock();
        cv.wait(lock, [] { return !eventQueue.empty() || finished; }); // Wait for an event or finish signal

        if (finished && eventQueue.empty()) break; // Exit if finished and no events left to process

        auto event = eventQueue.top();
        eventQueue.pop();
        lock.unlock();
        
        // Perform event action lambda
        event.action();

        if (lastTimestamp != -1) { // Check if this is not the first event
            long long delta = event.timestamp - lastTimestamp; 
            std::cout << "Processing event at timestamp: " << event.timestamp << " unix timestamp\n";
            std::cout << "Delta: " << delta << " microseconds\n";
        }
        else {
            std::cout << "Processing first event at timestamp: " << event.timestamp << " unix timestamp\n";
        }

        lastTimestamp = event.timestamp; // Update the lastTimestamp for the next iteration
    }
}
