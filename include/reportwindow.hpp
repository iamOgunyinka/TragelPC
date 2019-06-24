#ifndef REPORTWINDOW_HPP
#define REPORTWINDOW_HPP

#include <QMainWindow>

namespace Ui {
class ReportWindow;
}

class ReportWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit ReportWindow( QWidget *parent = nullptr );
    ~ReportWindow();

private:
    Ui::ReportWindow *ui;
};

#endif // REPORTWINDOW_HPP
