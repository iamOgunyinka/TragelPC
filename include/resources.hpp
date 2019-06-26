#ifndef RESOURCES_HPP
#define RESOURCES_HPP

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QJsonObject>
#include <QNetworkReply>
#include <QSettings>
#include <QNetworkCookieJar>
#include <QNetworkCookie>
#include <QList>
#include <QVector>
#include <QProgressDialog>

namespace utilities {
    static QString const USER_AGENT{ "Mozilla/5.0 (X11; Linux i586; rv:31.0) "
                                     "Gecko/20100101 Firefox/31.0" };
    class NetworkManager
    {
        NetworkManager() = default;
    public:
        static QNetworkAccessManager& GetNetworkWithCookie()
        {
            auto& network_manager{ GetNetwork() };
            auto& session_cookie{ GetSessionCookie() };
            network_manager.setCookieJar( &session_cookie );
            // don't take ownership of the session cookie
            session_cookie.setParent( nullptr );
            return network_manager;
        }

        static QNetworkAccessManager& GetNetwork()
        {
            static QNetworkAccessManager network_manager {};
            return network_manager;
        }

        static QNetworkCookieJar& GetSessionCookie()
        {
            static QNetworkCookieJar network_cookie {};
            return network_cookie;
        }

        static bool SetNetworkCookie( QNetworkCookieJar& jar,
                                      QNetworkReply* network_reply )
        {
            using CookieList = QList<QNetworkCookie>;
            auto const cookie_variant = network_reply->header(
                        QNetworkRequest::SetCookieHeader );
            CookieList const cookies = qvariant_cast<CookieList>(
                        cookie_variant );
            return jar.setCookiesFromUrl( cookies,
                                          network_reply->request().url() );
        }
    };

    class ApplicationSettings
    {
        ApplicationSettings() = default;
    public:
        ApplicationSettings( ApplicationSettings const & ) = delete;
        ApplicationSettings( ApplicationSettings && ) = delete;
        ApplicationSettings& operator=( ApplicationSettings const & ) = delete;
        ApplicationSettings& operator=( ApplicationSettings && ) = delete;

        static QSettings& GetAppSettings()
        {
            static QSettings app_settings{
                QSettings::UserScope, "Froist Inc.", "Tragel"
            };
            return app_settings;
        }
    };

    class Endpoint
    {
        QJsonObject endpoint_object;
        Endpoint() = default;
    public:
        QString CompanyToken() const
        {
            return endpoint_object.value( "company_token" ).toString();
        }

        QString CompanyID() const
        {
            return QString::number( endpoint_object.value( "company_id" )
                                    .toInt() );
        }

        QString LoginTo() const
        {
            return endpoint_object.value( "login" ).toString();
        }

        QString LogoutFrom() const
        {
            return endpoint_object.value( "logout" ).toString();
        }

        QString CreateUser() const {
            return endpoint_object.value( "create_user").toString();
        }

        QString DeleteUser() const {
            return endpoint_object.value( "delete_user" ).toString();
        }

        QString ResetPassword() const {
            return endpoint_object.value( "reset_password" ).toString();
        }

        QString ChangeRole() const {
            return endpoint_object.value( "change_role" ).toString();
        }

        QString ConfirmPayment() const {
            return endpoint_object.value( "confirm_order" ).toString();
        }

        QString GetSubscriptions() const {
            return endpoint_object.value( "get_subscriptions" ).toString();
        }

        QString AddSubscription() const {
            return endpoint_object.value( "add_subscription" ).toString();
        }

        QString GetProducts() const {
            return endpoint_object.value( "get_products" ).toString();
        }

        QString GetProductByID () const {
            return endpoint_object.value( "get_product" ).toString();
        }

        QString RemoveProduct() const {
            return endpoint_object.value( "remove_product" ).toString();
        }

        QString ListStaffs() const {
            return endpoint_object.value( "list_users" ).toString();
        }

        QString UpdateProducts() const {
            return endpoint_object.value( "update_product" ).toString();
        }

        QString AddProduct() const {
            return endpoint_object.value( "add_product" ).toString();
        }

        QString GetOrders() const {
            return endpoint_object.value( "get_orders" ).toString();
        }

        QString GetOrderByID() const {
            return endpoint_object.value( "get_customer_order" ).toString();
        }

        QString GetOrderCount() const {
            return endpoint_object.value( "count_orders" ).toString();
        }

        QString AddOrder() const {
            return endpoint_object.value( "add_order" ).toString();
        }

        QString RemoveOrder() const {
            return endpoint_object.value( "remove_order" ).toString();
        }

        QString PingAddress() const {
            return endpoint_object.value( "ping" ).toString();
        }

        QString UploadPhoto() const {
            return endpoint_object.value( "upload_images" ).toString();
        }

    public:
        Endpoint( Endpoint && ) = delete;
        Endpoint( Endpoint const & ) = delete;
        Endpoint& operator=( Endpoint const &)= delete;
        Endpoint& operator=( Endpoint && ) = delete;

        QJsonObject ToJson() const {
            return endpoint_object;
        }

        static void ParseEndpointsFromJson( Endpoint& endpoint,
                                            QJsonObject const & json_object )
        {
            endpoint.endpoint_object = json_object;
        }

        static Endpoint& GetEndpoints()
        {
            static Endpoint endpoints {};
            return endpoints;
        }
    };

    struct UrlData
    {
        QString next_url;
        QString previous_url;
        uint    page_number;
    };

    struct PageQuery
    {
        uint    number_of_pages {};
        uint    result_per_page {};
        uint    total_result {};
        QString first_url{};
        QString last_url{};
        UrlData other_url{};
    };

    struct ProductData
    {
        QString name;
        QString thumbnail_location;
        QString constant_url;
        double  price;
        qint64  product_id;

        QJsonObject ToJson() const;
    };

    bool operator==( ProductData const & a, ProductData const & b );

    enum PaymentType
    {
        Cash,
        E_Banking
    };

    struct PaymentConfirmationData
    {
        bool confirmed;
        QDateTime date_of_confirmation;
        QString   confirmed_by;
    };

    struct OrderData
    {
        qint64      order_id;
        int         payment_type;
        QString     confirmed_by;//empty by default, unless payment is confirmed
        QString     reference_id;
        QString     staff_username;
        QDateTime   order_date;
        struct Item {
            QString product_name;
            qint64  quantity;
            double  price;
        };
        QVector<Item> items;
        PaymentConfirmationData confirmation_data;
    };

    enum class SimpleRequestType
    {
        Get,
        Delete,
    };

    bool ParsePageUrls( QJsonObject const & page_url,
                        PageQuery& page_information );

    QNetworkRequest GetRequestInterface( QUrl const &address );
    QNetworkRequest PostRequestInterface( QUrl const &address );
    QJsonObject     GetJsonNetworkData( QNetworkReply *data,
                                        bool show_error_message = false );

    QPair<QNetworkRequest, QByteArray> PostImageRequestInterface(
            QUrl const &address, QVector<QString> const &data );

    template<typename FuncOnSuccess, typename FuncOnError>
    void SendNetworkRequest( QUrl const & address, FuncOnSuccess && on_success,
                             FuncOnError && on_error, QWidget *parent,
                             bool report_error = true,
                             SimpleRequestType type = SimpleRequestType::Get )
    {
        QNetworkRequest const request{ GetRequestInterface( address ) };
        auto& network_manager{ NetworkManager::GetNetworkWithCookie() };
        QProgressDialog* progress_dialog {
            new QProgressDialog( "Please wait", "Cancel", 1, 100, parent )
        };
        progress_dialog->show();
        QNetworkReply* reply{};
        if( type == SimpleRequestType::Get ){
            reply = network_manager.get( request );
        } else {
            reply = network_manager.deleteResource( request );
        }
        QObject::connect( reply, &QNetworkReply::downloadProgress,
                          [=]( qint64 const received, qint64 const total )
        {
            progress_dialog->setMaximum( total + ( total * 0.25 ));
            progress_dialog->setValue( received );
        });

        QObject::connect( reply, &QNetworkReply::sslErrors, reply,
                          QOverload<QList<QSslError> const &>::of(
                              &QNetworkReply::ignoreSslErrors ) );
        QObject::connect( progress_dialog, &QProgressDialog::canceled, reply,
                          &QNetworkReply::abort );
        QObject::connect( reply, &QNetworkReply::finished, progress_dialog,
                          &QProgressDialog::close );
        QObject::connect( reply, &QNetworkReply::finished, [=]
        {
            QJsonObject const result {
                utilities::GetJsonNetworkData( reply, report_error )
            };
            if( result.isEmpty() ) on_error();
            else on_success( result );
        });
    }

    template<typename FuncOnSuccess, typename FuncOnError>
    void SendPostNetworkRequest( QUrl const & address,
                                 FuncOnSuccess && on_success,
                                 FuncOnError && on_error, QWidget *parent,
                                 QByteArray const &payload,
                                 bool report_error = true )
    {
        QNetworkRequest const request{ PostRequestInterface( address ) };
        auto& network_manager{ NetworkManager::GetNetworkWithCookie() };
        QProgressDialog* progress_dialog {
            new QProgressDialog( "Please wait", "Cancel", 1, 100, parent )
        };
        progress_dialog->show();
        QNetworkReply* reply{ network_manager.post( request, payload )};
        QObject::connect( reply, &QNetworkReply::downloadProgress,
                          [=]( qint64 const received, qint64 const total )
        {
            progress_dialog->setMaximum( total + ( total * 0.25 ));
            progress_dialog->setValue( received );
        });

        QObject::connect( reply, &QNetworkReply::sslErrors, reply,
                          QOverload<QList<QSslError> const &>::of(
                              &QNetworkReply::ignoreSslErrors ) );
        QObject::connect( progress_dialog, &QProgressDialog::canceled, reply,
                          &QNetworkReply::abort );
        QObject::connect( reply, &QNetworkReply::finished, progress_dialog,
                          &QProgressDialog::close );
        QObject::connect( reply, &QNetworkReply::finished, [=]
        {
            QJsonObject const result {
                utilities::GetJsonNetworkData( reply, report_error )
            };
            if( result.isEmpty() ) on_error();
            else on_success( result, reply );
        });
    }
}

#endif // RESOURCES_HPP
