#ifndef EDITPRODUCTDIALOG_HPP
#define EDITPRODUCTDIALOG_HPP

#include <QDialog>
#include <QJsonObject>
#include "utils/resources.hpp"

namespace Ui {
class EditProductDialog;
}

class EditProductDialog : public QDialog
{
    Q_OBJECT

public:
    explicit EditProductDialog(QWidget *parent = nullptr);
    ~EditProductDialog();
    utilities::Product GetValue() const;
    void SetThumbnail( QString const & );
    void SetName( QString const & );
    void SetPrice( double const );
    void dropEvent( QDropEvent *event ) override;
    void dragEnterEvent(QDragEnterEvent *event) override;
private:
    void OnChangeButtonClicked();
    void ValidateImageData( QString const & filename );
private:
    Ui::EditProductDialog *ui;
    QString temp_file {};
};

#endif // EDITPRODUCTDIALOG_HPP
