#include <iostream>
#include <fstream>
#include <thread>
#include <concrt.h>
#include <chrono>
#include "stack.h"

const int PRODUCER_COUNT = 3;
const int CONSUMER_COUNT = 2;

std::atomic<int> elementsNeededCount;
std::atomic<int> elementsConsumedCount;

volatile int maxValue;

concurrency::critical_section cs;

void produce(std::ifstream& source, thrsafe_stack& stack) {
    while (source && elementsNeededCount-- > 0) {
        cs.lock();
        int value;
        source >> value;
        cs.unlock();
        stack.push(value);
    }
}

void consume(thrsafe_stack& stack) {
    while (elementsConsumedCount-- > 0) {
        int value;
        while (stack.pop(value)) {
            cs.lock();
            std::cout << value << " " << std::this_thread::get_id() << std::endl;
            if (value > maxValue) {
                std::cout << "Was " << maxValue << ". Become: " << value << " " << std::this_thread::get_id() << std::endl;
                maxValue = value;
            }
            cs.unlock();
        }
    }
}

int main() {
    std::cout << "Threads available: " << std::thread::hardware_concurrency() << std::endl;
    std::cout << "Producing threads count: " << PRODUCER_COUNT << std::endl;
    std::cout << "Consuming threds count: " << CONSUMER_COUNT << std::endl;

    thrsafe_stack st(cs);

    std::ifstream reader("input.txt");
    if (reader) {
        int value;
        reader >> value;
        elementsNeededCount.store(value);
        elementsConsumedCount = value;

        std::thread producingThreads[PRODUCER_COUNT];
        std::thread consumingThreads[CONSUMER_COUNT];

        for (int i = 0; i < PRODUCER_COUNT; i++) {
            producingThreads[i] = std::thread(&produce, std::ref(reader), std::ref(st));
        }
        for (int i = 0; i < CONSUMER_COUNT; i++) {
            consumingThreads[i] = std::thread(&consume, std::ref(st));
        }

        for (int i = 0; i < PRODUCER_COUNT; i++) {
            producingThreads[i].join();
        }
        for (int i = 0; i < CONSUMER_COUNT; i++) {
            consumingThreads[i].join();
        }

        reader.close();

        std::cout << "Max value is " << maxValue << std::endl;
    } else {
        std::cout << "File not found or there's an error";
    }
}