#ifndef ALLSUBSCRIPTIONSDIALOG_HPP
#define ALLSUBSCRIPTIONSDIALOG_HPP

#include <QDialog>
#include "allsubscriptionmodel.hpp"

namespace Ui {
class AllSubscriptionsDialog;
}

class AllSubscriptionsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AllSubscriptionsDialog(QWidget *parent = nullptr);
    ~AllSubscriptionsDialog();
    void GetSubscriptions();
private:
    void ParseSubscriptionResult( QJsonArray const & );
    void OnResultObtained( QJsonObject const & );
    Ui::AllSubscriptionsDialog *ui;
    utilities::SubscriptionList subscriptions_{};
};

#endif // ALLSUBSCRIPTIONSDIALOG_HPP
