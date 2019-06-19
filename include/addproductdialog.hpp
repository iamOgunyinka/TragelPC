#ifndef ADDPRODUCTDIALOG_HPP
#define ADDPRODUCTDIALOG_HPP

#include <QDialog>
#include <QStandardItemModel>
#include <QJsonArray>
#include "resources.hpp"

namespace Ui {
class AddProductDialog;
}

class AddProductDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddProductDialog(QWidget *parent = 0);
    ~AddProductDialog();
private:
    void OnContextMenuItemRequested( QPoint const & point );
    void OnShowImageButtonClicked();
    void OnUploadDataToServerButtonClicked();
    void OnAddItemButtonClicked();
    void OnRemoveItemButtonClicked();
    void OnEditItemButtonClicked();
    void UpdateModel( utilities::ProductData const &value, qint64 const size );
private:
    Ui::AddProductDialog *ui;
    QStandardItemModel *data_model;
    QString temp_file {};
    QVector<utilities::ProductData> product_item_list {};
};

#endif // ADDPRODUCTDIALOG_HPP
