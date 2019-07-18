#include "widgets/stockdialog.hpp"
#include "ui_stockdialog.h"

StockDialog::StockDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::StockDialog)
{
    ui->setupUi(this);
}

StockDialog::~StockDialog()
{
    delete ui;
}
