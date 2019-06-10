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
    void DownloadProducts( QUrl const & address );
    void OnCustomContextMenuRequested( QPoint const & point );
    void OnDownloadResultObtained( QJsonObject const & result );
    void OnEditItemButtonClicked();
    void OnRemoveItemButtonClicked();
    void UpdatePageData();
    void OnNextPageButtonClicked();
    void OnPreviousPageButtonClicked();
    void OnFirstPageButtonClicked();
    void OnLastPageButtonClicked();
private:
    Ui::AllProductsDialog *ui;
    utilities::PageQuery product_query_data;
};

#endif // ALLPRODUCTSDIALOG_HPP
