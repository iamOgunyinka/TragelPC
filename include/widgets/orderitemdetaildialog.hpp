#ifndef ORDERITEMDETAILDIALOG_HPP
#define ORDERITEMDETAILDIALOG_HPP

#include <QDialog>
#include <QVector>

namespace utilities {
struct OrderResultData;
struct OrderingResultItem;
}

namespace Ui {
class OrderItemDetailDialog;
}

class OrderItemDetailDialog : public QDialog
{
    Q_OBJECT

public:
    explicit OrderItemDetailDialog( QVector<utilities::OrderingResultItem> const
                                    &items, QWidget *parent = nullptr );
    ~OrderItemDetailDialog();
    void SetConfirmationStatus( QString const & by, QDateTime const &on );
private:
    Ui::OrderItemDetailDialog *ui;
};

#endif // ORDERITEMDETAILDIALOG_HPP
