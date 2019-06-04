#ifndef PRODUCTUPLOADDIALOG_HPP
#define PRODUCTUPLOADDIALOG_HPP

#include <QDialog>
#include <QWidget>
#include <QList>

#include "resources.hpp"

namespace Ui {
class ProductUploadDialog;
}

class ProductUploadDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ProductUploadDialog( QList<utilities::ProductData> &products,
                                  QWidget *parent = nullptr );
    ~ProductUploadDialog();
    void StartUpload();
private:
    void UploadProductImagesByIndex( int const index );
    void SendUploadCompleteSignalWhenDone();
private:
    Ui::ProductUploadDialog *ui;
    QList<utilities::ProductData> &products_;
};

#endif // PRODUCTUPLOADDIALOG_HPP
