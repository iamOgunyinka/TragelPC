#ifndef MAKEORDERDIALOG_HPP
#define MAKEORDERDIALOG_HPP

#include <QDialog>

namespace Ui {
class MakeOrderDialog;
}

namespace utilities
{
struct Product;
struct MakeOrderItem;
}

class MakeOrderDialog : public QDialog
{
    Q_OBJECT

public:
    explicit MakeOrderDialog(QWidget *parent = nullptr);
    ~MakeOrderDialog();
    void DownloadProducts();
private:
    void OnDownloadResultObtained( QJsonObject const & result );
    void OnRemoveItemActionClicked();
    void OnItemCustomContextMenuRequested( QPoint const &p );
    void OnAddItemButtonClicked();
    void OnMakeOrderButtonClicked();
private:
    Ui::MakeOrderDialog *ui;
    QVector<utilities::Product> products_;
    QVector<utilities::MakeOrderItem> order;
};

#endif // MAKEORDERDIALOG_HPP
