#ifndef ORDERWINDOW_HPP
#define ORDERWINDOW_HPP

#include <QMainWindow>
#include <QDate>
#include <QUrl>
#include <QJsonObject>


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
private:
    QStandard
    Ui::OrderWindow *ui;
};

#endif // ORDERWINDOW_HPP
