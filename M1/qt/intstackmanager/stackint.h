#pragma once
#include <iostream>

class QPainter; // forward declaration

class Element {
public:
    int data;
    Element* next;
    Element(int val) : data(val), next(nullptr) {}
};

class stackInt {
private:
    Element* top;
    int countSize(Element* current) const;

public:
    stackInt() : top(nullptr) {}
    ~stackInt();

    void push(int n);
    int pop();
    int size() const;
    bool deleteElement(int v);
    bool insertElement(int v, int prev);
    bool isEmpty() const;
    Element* getTop() const { return top; }

    void drawStack(Element* s, QPainter& painter, int canvasHeight, int startX) const;
};