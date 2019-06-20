#ifndef ORDERWINDOW_HPP
#define ORDERWINDOW_HPP

#include <QMainWindow>
#include <QDate>
#include <QUrl>
#include <QJsonObject>
#include "resources.hpp"


namespace Ui {
class OrderWindow;
}

class OrderWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit OrderWindow(QWidget *parent = 0);
    ~OrderWindow();
private:
    void    OnFindButtonClicked();
    void    SendNetworkRequest( QUrl const & address );
    void    DisplayOrderData( QJsonObject const & data );
    QString DateToString( QDate const & date );
    void    ParseOrderResult( QJsonArray const & order_object_list,
                              QVector<utilities::OrderData> &orders );
    void    OnCustomContextMenuRequested( QPoint const & point );
    void    OnOrderDetailsRequested();
    void    OnRemoveItemActionClicked();
    void    UpdatePageData();
    void    OnNextPageButtonClicked();
    void    OnPreviousPageButtonClicked();
    void    OnFirstPageButtonClicked();
    void    OnLastPageButtonClicked();
    QUrl    GetUrlNewMetaFromUrl( QString const &new_url );
private:
    utilities::PageQuery orders_page_query{};
    Ui::OrderWindow *ui;
    QNetworkRequest request_;
};

#endif // ORDERWINDOW_HPP