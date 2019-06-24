#include "reportwindow.hpp"
#include "ui_reportwindow.h"

ReportWindow::ReportWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ReportWindow)
{
    ui->setupUi(this);
}

ReportWindow::~ReportWindow()
{
    delete ui;
}
