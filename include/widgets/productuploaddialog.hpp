#ifndef PRODUCTUPLOADDIALOG_HPP
#define PRODUCTUPLOADDIALOG_HPP

#include <QDialog>
#include <QWidget>
#include <QVector>

namespace utilities {
struct Product;
}

namespace Ui {
class ProductUploadDialog;
}

class ProductUploadDialog : public QDialog
{
    Q_OBJECT
signals:
    void image_upload_completed( bool with_error );
    void uploads_completed( bool with_error );
public:
    explicit ProductUploadDialog( QVector<utilities::Product> &products,
                                  QWidget *parent = nullptr );
    ~ProductUploadDialog();
    void StartUpload();
private:
    enum class UploadStatus: unsigned char
    {
        PendingUpload = 0x8,
        Uploaded = 0x10,
        ErrorEncountered = 0x20
    };

    void OnUploadCompleted( bool const has_error );
    void UploadProductImagesByIndex( int const index );
    void SendUploadCompleteSignalWhenDone();
    void UploadProducts( QJsonArray const & product_list );
private:
    Ui::ProductUploadDialog *ui;
    QVector<utilities::Product> &products_;
    QVector<UploadStatus> product_upload_status_;
};

#endif // PRODUCTUPLOADDIALOG_HPP
