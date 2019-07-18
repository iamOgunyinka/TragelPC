#ifndef ORDERWINDOW_HPP
#define ORDERWINDOW_HPP

#include <QMainWindow>
#include <QDate>
#include <QUrl>
#include <QJsonObject>
#include <QNetworkRequest>

namespace Ui {
class OrderWindow;
}
namespace utilities{
struct OrderResultData;
struct PageResultQuery;
}

class OrderWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit OrderWindow(QWidget *parent = nullptr);
    ~OrderWindow();
private:
    void    SendNetworkRequest( QUrl const & address );
    void    DisplayOrderData( QJsonObject const & data );
    QString DateToString( QDate const & date );
    void    ParseOrderResult( QJsonArray const & order_object_list,
                              QVector<utilities::OrderResultData> &orders );
    void    OnCustomContextMenuRequested( QPoint const & point );
    void    OnOrderDetailsRequested();
    void    OnRemoveItemActionClicked();
    void    OnConfirmOrderPaymentTriggered();
    void    UpdatePageData();
    void    OnNextPageButtonClicked();
    void    OnPreviousPageButtonClicked();
    void    OnFirstPageButtonClicked();
    void    OnLastPageButtonClicked();

    void    OnFindButtonClicked();
    void    OnFindAllOrdersTriggered();
    void    OnFindOrdersFromLastSevenDays();
    void    OnFindThisMonthsOrderTriggered();
    void    OnFindTodaysOrderTriggered();

    QUrl    GetUrlNewMetaFromUrl( QString const &new_url );
private:
    Ui::OrderWindow *ui;
    QVector<utilities::OrderResultData> orders_;
    QScopedPointer<utilities::PageResultQuery> orders_page_query;
    QNetworkRequest request_;
};

#endif // ORDERWINDOW_HPP
