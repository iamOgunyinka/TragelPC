#ifndef MAKEORDERDIALOG_HPP
#define MAKEORDERDIALOG_HPP

#include <QDialog>
#include "orderingitemmodel.hpp"

namespace Ui {
class MakeOrderDialog;
}

class MakeOrderDialog : public QDialog
{
    Q_OBJECT

public:
    explicit MakeOrderDialog(QWidget *parent = 0);
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
    QVector<utilities::ProductData> products_;
    QVector<utilities::ItemData> order;
};

#endif // MAKEORDERDIALOG_HPP
