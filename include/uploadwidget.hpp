#ifndef UPLOADWIDGET_HPP
#define UPLOADWIDGET_HPP

#include <QWidget>

namespace Ui {
class UploadWidget;
}

class UploadWidget : public QWidget
{
    Q_OBJECT

public:
    explicit UploadWidget(QWidget *parent = 0);
    ~UploadWidget();
    void SetMessage( QString const & message );
    void SetProgress( qint64 current_value, qint64 total );
private:
    Ui::UploadWidget *ui;
};

#endif // UPLOADWIDGET_HPP
