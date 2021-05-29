#include <iostream>
#include "stack.h"

node::node(int value, node* next) {
    this->value = value;
    this->next = next;
}

thrsafe_stack::thrsafe_stack() {
    head = new node();
    count = 0;
}

void thrsafe_stack::push(int value) {
    cs.lock();
    //std::this_thread::sleep_for(std::chrono::milliseconds(100));
    node* newNode = new node(value, head->next);
    head->next = newNode;
    std::cout << std::this_thread::get_id() << ". Pushed: " << value << std::endl;
    count++;
    cs.unlock();
}

int thrsafe_stack::pop()
{
    cs.lock();
    // std::this_thread::sleep_for(std::chrono::milliseconds(500));
    int result = -1;
    if (head->next != nullptr) {
        node* removedNode = head->next;
        result = head->next->value;
        std::cout << std::this_thread::get_id() << ". Popped: " << result << std::endl;
        delete removedNode;
    }
    cs.unlock();
    return result;
}


bool thrsafe_stack::pop(int& out)
{
    cs.lock();
    //std::this_thread::sleep_for(std::chrono::milliseconds(500));
    bool result = false;
    out = -1;
    if (head->next != nullptr) {
        node* removedNode = head->next;
        result = true;
        out = head->next->value;
        std::cout << std::this_thread::get_id() << ". Popped: " << out << std::endl;
        head->next = removedNode->next;
        delete removedNode;
    }
    cs.unlock();
    return result;
}

int thrsafe_stack::top() {
    cs.lock();
    int result = 0;
    if (head->next != nullptr) {
        result = head->next->value;
    }
    cs.unlock();
    return result;
}

bool thrsafe_stack::isEmpty()
{
    bool result = true;
    cs.lock();
    if (head->next != nullptr) {
        result = false;
    }
    cs.unlock();
    return result;
}



