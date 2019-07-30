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
    void OnUploadDataToServerButtonClicked();
    void OnAddItemButtonClicked();
    void OnRemoveItemButtonClicked();
private:
    Ui::AddProductDialog *ui;
};

#endif // ADDPRODUCTDIALOG_HPP
