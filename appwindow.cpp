#include "appwindow.h"
#include "./ui_appwindow.h"

AppWindow::AppWindow(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::AppWindow)
{
    ui->setupUi(this);
}

AppWindow::~AppWindow()
{
    delete ui;
}

