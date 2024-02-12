#pragma once
#include "pch.h"
#include "neuron.h"
#include <functional>

struct Event;
struct CompareEvent;

void generateEvents(std::vector<Neuron*>& inputNeurons, std::vector<Neuron*>& outputNeurons);
void processEvents();