#ifndef ALLPRODUCTSDIALOG_HPP
#define ALLPRODUCTSDIALOG_HPP

#include <QDialog>
#include "resources.hpp"

namespace Ui {
class AllProductsDialog;
}

class AllProductsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AllProductsDialog(QWidget *parent = 0);
    ~AllProductsDialog();
    void DownloadProducts();
private:
    void OnCustomContextMenuRequested( QPoint const & point );
    void OnDownloadResultObtained( QJsonObject const & result );
    void OnEditItemButtonClicked();
    void OnRemoveItemButtonClicked();
private:
    Ui::AllProductsDialog *ui;
    utilities::ProductPageQuery product_query_data;
};

#endif // ALLPRODUCTSDIALOG_HPP
