#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_mySlider_valueChanged(int value)
{
    ui->myCounter->setValue(value*2);
}


void MainWindow::on_myCounter_valueChanged(int arg1)
{
    ui->mySlider->setValue(arg1/2);
}

