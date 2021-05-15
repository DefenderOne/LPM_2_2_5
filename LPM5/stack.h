#pragma once
#include <concrt.h>

struct node {
    int value;
    node* next;
    node(int value = 0, node* next = nullptr);
};

class thrsafe_stack {
private:
    node* head;
    int count;
    concurrency::critical_section& cs;

public:
    thrsafe_stack(concurrency::critical_section& cs);
    void push(int value);
    int pop();
    bool pop(int& out);
    int top();
    bool isEmpty();
};
