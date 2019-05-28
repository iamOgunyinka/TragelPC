#ifndef EDITPRODUCTDIALOG_HPP
#define EDITPRODUCTDIALOG_HPP

#include <QDialog>
#include <QJsonObject>

namespace Ui {
class EditProductDialog;
}

class EditProductDialog : public QDialog
{
    Q_OBJECT

public:
    explicit EditProductDialog(QWidget *parent = 0);
    ~EditProductDialog();
    QJsonObject GetValue() const;
    void SetThumbnail( QString const & );
    void SetName( QString const & );
    void SetPrice( double const );
private:
    void OnChangeButtonClicked();
private:
    Ui::EditProductDialog *ui;
    QString temp_file {};
};

#endif // EDITPRODUCTDIALOG_HPP
