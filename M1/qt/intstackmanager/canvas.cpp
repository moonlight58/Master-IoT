#include "canvas.h"
#include <qpainter.h>
#include <QMouseEvent>
#include <cstdlib>

Canvas::Canvas(QWidget *parent) : QWidget{parent} {}

void Canvas::mousePressEvent(QMouseEvent*)
{
    if (leftStack.size() < 10) {
        leftStack.push(1 + std::rand() % 99); // fill phase
    } else if (!leftStack.isEmpty()) {
        rightStack.push(leftStack.pop());     // drain phase
    }
    update(); // schedules repaint, never call paintEvent() directly
}

void Canvas::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.fillRect(rect(), Qt::white);
    leftStack.drawStack(leftStack.getTop(), painter, height(), 50);
    rightStack.drawStack(rightStack.getTop(), painter, height(), 400);
}