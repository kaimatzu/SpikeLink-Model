#include "pch.h"
#include "event.h"
//using namespace std;



// Event system
extern std::priority_queue<Event, std::vector<Event>, CompareEvent> eventQueue;
extern std::mutex queueMutex;
extern std::condition_variable cv;
extern bool finished;


int main() {
    std::vector<Neuron*> inputNeurons;
    std::vector<Neuron*> outputNeurons;
    

    // Initialize input and output neurons
    Neuron* input1 = new Neuron();
    Neuron* input2 = new Neuron();
    inputNeurons.push_back(input1);
    inputNeurons.push_back(input2);

    Neuron* output1 = new Neuron();
    outputNeurons.push_back(output1);

    // Connect the input neurons to the output neuron
    for (auto neuron : inputNeurons) {
        neuron->connectToNeuron(output1);
    }

    // Start event generation and processing in separate threads
    std::thread producer([&inputNeurons, &outputNeurons]() { generateEvents(inputNeurons, outputNeurons); });
    std::thread consumer([&]() { processEvents(); });

    // TODO: Change implementation of loop break later. Maybe to pause training and save weights.
    std::cout << "Press Enter to stop...\n";
    std::cin.get();

    {
        std::lock_guard<std::mutex> guard(queueMutex);
        finished = true;
    }
    cv.notify_one(); // Notify in case the consumer is waiting

    producer.join();
    consumer.join();

    // TODO: Change to delete via iterating through vector
    delete input1;
    delete input2;

    return 0;
}