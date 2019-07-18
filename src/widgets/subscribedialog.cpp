#include <QJsonDocument>
#include <QJsonObject>
#include <QMessageBox>
#include "widgets/subscribedialog.hpp"
#include "utils/resources.hpp"

#include "ui_subscribedialog.h"

SubscribeDialog::SubscribeDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SubscribeDialog)
{
    ui->setupUi( this );
    QObject::connect( ui->subscribe_button, &QPushButton::clicked, this,
                      &SubscribeDialog::OnSubscribeButtonClicked );
}

SubscribeDialog::~SubscribeDialog()
{
    delete ui;
}

void SubscribeDialog::OnSubscribeButtonClicked()
{
    QString const key { ui->code_edit->toPlainText().trimmed() };
    if( key.isEmpty() ) return;
    QUrl const url{ utilities::Endpoint::GetEndpoints().AddSubscription() };
    QJsonObject const code { { "key", key } };
    QByteArray const payload{ QJsonDocument{ code }.toJson() };
    auto on_success = [=]( QJsonObject const &, QNetworkReply* ){
        ui->code_edit->clear();
        QMessageBox::information( this, "Subscription", "Your subscription code"
                                  " was accepted! Congratulations" );
    };
    utilities::SendPostNetworkRequest( url, on_success, []{}, this, payload );
}
