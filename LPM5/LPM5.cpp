#include <iostream>
#include <fstream>
#include <thread>
#include <concrt.h>
#include <chrono>
#include "stack.h"

const int PRODUCER_COUNT = 2;
const int CONSUMER_COUNT = 3;

volatile long elementsNeededCount;
volatile long elementsConsumedCount;

//concurrency::critical_section cs_main;

void produce(std::ifstream& source, thrsafe_stack& stack) {
    while (_InterlockedExchangeAdd(&elementsNeededCount, -1) > 0) {
        int value;
        source >> value;
        // std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        // std::cout << "Read: " << value << " " << std::this_thread::get_id() << std::endl;
        stack.push(value);
    }
}

struct consumer_info {
    std::thread::id _id;
    int thread_number;
    int maxValue;
    consumer_info() : thread_number(int()), maxValue(int()) { }
};

void consume(thrsafe_stack& stack, consumer_info& info) {
    info._id = std::this_thread::get_id();
    int maxValue = 0;
    while (_InterlockedExchangeAdd(&elementsConsumedCount, -1) > 0) {
        int value;
        while (stack.pop(value)) {
            // std::cout << "Consumed: " << value << " " << std::this_thread::get_id() << std::endl;
            // std::this_thread::sleep_for(std::chrono::milliseconds(500));
            if (value > maxValue) {
                // std::cout << "Was " << maxValue << ". Become: " << value << " " << std::this_thread::get_id() << std::endl;
                maxValue = value;
            }
        }
    }
    info.maxValue = maxValue;
}

int main() {
    std::cout << "Threads available: " << std::thread::hardware_concurrency() << std::endl;
    std::cout << "Producing threads count: " << PRODUCER_COUNT << std::endl;
    std::cout << "Consuming threds count: " << CONSUMER_COUNT << std::endl;

    thrsafe_stack st;

    std::ifstream reader("input.txt");
    if (reader) {
        int value;
        reader >> value;
        elementsNeededCount = value;
        elementsConsumedCount = value;

        std::thread producingThreads[PRODUCER_COUNT];
        std::thread consumingThreads[CONSUMER_COUNT];
        consumer_info infos[CONSUMER_COUNT];

        for (int i = 0; i < PRODUCER_COUNT; i++) {
            producingThreads[i] = std::thread(&produce, std::ref(reader), std::ref(st));
        }
        for (int i = 0; i < CONSUMER_COUNT; i++) {
            consumingThreads[i] = std::thread(&consume, std::ref(st), std::ref(infos[i]));
            infos[i].thread_number = i;
        }

        for (int i = 0; i < PRODUCER_COUNT; i++) {
            producingThreads[i].join();
        }
        for (int i = 0; i < CONSUMER_COUNT; i++) {
            consumingThreads[i].join();
        }

        reader.close();
        for (int i = 0; i < CONSUMER_COUNT; i++) {
            std::cout << infos[i].thread_number << " [" << infos[i]._id << "]. Max Value: " << infos[i].maxValue << std::endl;
        }
        
    } else {
        std::cout << "File not found or there's an error";
    }
}