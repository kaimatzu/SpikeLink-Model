#pragma once

class Neuron {
public:
    std::vector<Neuron*> incoming; // Vector of pointers to input neurons (dendrites)
    std::vector<Neuron*> outgoing; // Vector of pointers to output neurons (axon-branch)
    int currentPotential = 0; // Current potential. 
    int decayRate = 1; // Decay rate. Change later.
    int threshold = 10; // Threshold of action potential. Change later.

    Neuron() = default;

    // Set incoming connection
    void setIncomingConnection(Neuron* inputNeuron) {
        incoming.push_back(inputNeuron);
    }

    // Connect to a neuron
    void connectToNeuron(Neuron* outputNeuron) {
        outgoing.push_back(outputNeuron);
        outputNeuron->setIncomingConnection(this);
    }

    // Check if currentPotential >= threshold
    void activateSpike() {
        if (currentPotential >= threshold) {
            std::cout << "Neuron Spiked.\n";
            propagateActionPotential();
        }
    }

    // Propagate action potential to outgoing connections
    void propagateActionPotential() {
        for (Neuron* neuron : outgoing) {
            // Increment the currentPotential of the connected neurons.
            neuron->currentPotential += 3;
        }
        // Reset the current potential after firing. Change this implementation later to rapid decrements instead of instantaneous.
        currentPotential = 0;
    }

    // Single update event per delta
    void update() {
        currentPotential -= decayRate; // Apply decay
        if (currentPotential < 0) currentPotential = 0; // Ensure the potential doesn't go negative

        // Check for spike activation after decay
        activateSpike();
    }

    ~Neuron() = default;

    // More functionalities soon
};