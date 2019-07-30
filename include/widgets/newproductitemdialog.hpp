#ifndef NEWPRODUCTITEMDIALOG_HPP
#define NEWPRODUCTITEMDIALOG_HPP

#include <QDialog>

namespace Ui {
class NewProductItemDialog;
}

namespace utilities{
struct Product;
}
class NewProductItemDialog : public QDialog
{
    Q_OBJECT

public:
    explicit NewProductItemDialog(QWidget *parent = nullptr);
    ~NewProductItemDialog();
    utilities::Product GetProduct() const;
private:
    void OnUploadButtonClicked();
    void OnAddProductButtonClicked();
private:
    Ui::NewProductItemDialog *ui;
    QString src;
};

#endif // NEWPRODUCTITEMDIALOG_HPP
