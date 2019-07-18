#ifndef ALLPRODUCTSDIALOG_HPP
#define ALLPRODUCTSDIALOG_HPP

#include <QDialog>
#include <QCloseEvent>

namespace utilities{
struct Product;
struct ApplicationSettings;
struct PageResultQuery;
}


namespace Ui {
class AllProductsDialog;
}

class AllProductsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AllProductsDialog( QWidget *parent = nullptr );
    ~AllProductsDialog();
    void DownloadProducts();
protected:
    void closeEvent( QCloseEvent * )override;
private:
    QUrl GetUrlWithQuery( QString const & address );
    void DownloadProducts( QUrl const & address );
    void OnCustomContextMenuRequested( QPoint const & point );
    void OnDownloadResultObtained( QJsonObject const & result );
    void OnEditItemButtonClicked();
    void OnRemoveItemButtonClicked();
    void OnUpdateButtonClicked();
    void UpdatePageData();
    void OnNextPageButtonClicked();
    void OnPreviousPageButtonClicked();
    void OnFirstPageButtonClicked();
    void OnLastPageButtonClicked();
private:
    Ui::AllProductsDialog *ui;
    QScopedPointer<utilities::PageResultQuery> product_query_data_;
    QVector<utilities::Product> products_;
    utilities::ApplicationSettings& app_settings;
};

#endif // ALLPRODUCTSDIALOG_HPP
