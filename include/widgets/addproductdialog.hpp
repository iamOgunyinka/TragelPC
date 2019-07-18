#ifndef ADDPRODUCTDIALOG_HPP
#define ADDPRODUCTDIALOG_HPP

#include <QDialog>
#include <QStandardItemModel>
#include <QJsonArray>

namespace utilities{
struct Product;
}

namespace Ui {
class AddProductDialog;
}

class AddProductDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddProductDialog( QWidget *parent = nullptr );
    ~AddProductDialog();
protected:
    void closeEvent( QCloseEvent * );
private:
    void OnContextMenuItemRequested( QPoint const & point );
    void OnShowImageButtonClicked();
    void OnUploadDataToServerButtonClicked();
    void OnAddItemButtonClicked();
    void OnRemoveItemButtonClicked();
    void OnEditItemButtonClicked();
    void UpdateModel( utilities::Product const &value, int const size );
private:
    Ui::AddProductDialog *ui;
    QStandardItemModel *data_model;
    QString temp_file {};
    QVector<utilities::Product> product_item_list;
};

#endif // ADDPRODUCTDIALOG_HPP
