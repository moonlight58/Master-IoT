#include "stackint.h"
#include <qpainter.h>

stackInt::~stackInt() { while (!isEmpty()) pop(); }

void stackInt::push(int n) {
    Element* newNode = new Element(n);
    newNode->next = top;
    top = newNode;
}

int stackInt::pop() {
    if (isEmpty()) {
        std::cout << "Stack Underflow: Cannot pop from empty stack." << std::endl;
        return -1;
    }
    Element* tmp = top;
    int poppedValue = tmp->data;
    top = top->next;
    delete tmp;
    return poppedValue;
}

int stackInt::size() const { return countSize(top); }

int stackInt::countSize(Element* current) const {
    if (current == nullptr) return 0;
    return 1 + countSize(current->next);
}

bool stackInt::deleteElement(int v) {
    if (top == nullptr) return false;
    if (top->data == v) {
        Element* tmp = top;
        top = top->next;
        delete tmp;
        return true;
    }
    Element* prev = top;
    Element* curr = top->next;
    while (curr != nullptr) {
        if (curr->data == v) {
            prev->next = curr->next;
            delete curr;
            return true;
        }
        prev = curr;
        curr = curr->next;
    }
    return false;
}

bool stackInt::insertElement(int v, int prev) {
    if (top == nullptr) return false;
    Element* curr = top;
    while (curr != nullptr) {
        if (curr->data == prev) {
            Element* newNode = new Element(v);
            newNode->next = curr->next;
            curr->next = newNode;
            return true;
        }
        curr = curr->next;
    }
    return false;
}

bool stackInt::isEmpty() const { return top == nullptr; }

void stackInt::drawStack(Element* s, QPainter& painter, int canvasHeight, int startX) const {
    if (s == nullptr) return;

    const int boxWidth = 80, boxHeight = 40, gap = 25;
    const int baseY = canvasHeight - 60;

    int count = 0;
    for (Element* t = s; t != nullptr; t = t->next) count++;

    Element* curr = s;
    int index = 0;
    while (curr != nullptr) {
        int y = baseY - (count - 1 - index) * (boxHeight + gap);
        QRect boxRect(startX, y, boxWidth, boxHeight);
        painter.setPen(Qt::black);
        painter.setBrush(Qt::lightGray);
        painter.drawRect(boxRect);
        painter.drawText(boxRect, Qt::AlignCenter, QString::number(curr->data));

        if (curr->next != nullptr) {
            int arrowX = startX + boxWidth / 2;
            int arrowStartY = y + boxHeight;
            int arrowEndY = arrowStartY + gap;
            painter.drawLine(arrowX, arrowStartY, arrowX, arrowEndY);
            painter.drawLine(arrowX, arrowEndY, arrowX - 4, arrowEndY - 6);
            painter.drawLine(arrowX, arrowEndY, arrowX + 4, arrowEndY - 6);
        }
        curr = curr->next;
        index++;
    }
}