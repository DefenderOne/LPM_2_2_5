#include "stack.h"

node::node(int value, node* next) {
    this->value = value;
    this->next = next;
}

thrsafe_stack::thrsafe_stack(concurrency::critical_section& cs) : cs(cs) {
    head = new node();
    count = 0;
}

void thrsafe_stack::push(int value) {
    cs.lock();
    node* newNode = new node(value, head->next);
    head->next = newNode;
    count++;
    cs.unlock();
}

int thrsafe_stack::pop()
{
    cs.lock();
    int result = -1;
    if (head->next != nullptr) {
        node* removedNode = head->next;
        result = head->next->value;
        delete removedNode;
    }
    cs.unlock();
    return result;
}


bool thrsafe_stack::pop(int& out)
{
    cs.lock();
    bool result = false;
    out = -1;
    if (head->next != nullptr) {
        node* removedNode = head->next;
        result = true;
        out = head->next->value;
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



