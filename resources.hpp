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


namespace utilities {
    static QString const USER_AGENT{ "Mozilla/5.0 (X11; Linux i586; rv:31.0) "
                                     "Gecko/20100101 Firefox/31.0" };
    class NetworkManager
    {
        NetworkManager() = default;
    public:
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

        static bool SetNetworkCookie( QNetworkCookieJar& jar, QNetworkReply* network_reply )
        {
            using CookieList = QList<QNetworkCookie>;
            auto const cookie_variant = network_reply->header( QNetworkRequest::SetCookieHeader );
            CookieList const cookies = qvariant_cast<CookieList>( cookie_variant );
            return jar.setCookiesFromUrl( cookies, network_reply->request().url() );
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
            static QSettings app_settings{ QSettings::UserScope, "Froist Inc.", "Tragel" };
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
            return endpoint_object.value( "company_id" ).toString();
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

        QString UpdateProduct() const {
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

        QString AddOrder() const {
            return endpoint_object.value( "add_order" ).toString();
        }

        QString RemoveOrder() const {
            return endpoint_object.value( "remove_order" ).toString();
        }

        QString PingAddress() const {
            return endpoint_object.value( "ping" ).toString();
        }

    public:
        Endpoint( Endpoint && ) = delete;
        Endpoint( Endpoint const & ) = delete;
        Endpoint& operator=( Endpoint const &)= delete;
        Endpoint& operator=( Endpoint && ) = delete;

        QJsonObject ToJson() const {
            return endpoint_object;
        }

        static void ParseEndpointsFromJson( Endpoint& endpoint, QJsonObject const & json_object )
        {
            endpoint.endpoint_object = json_object;
        }

        static Endpoint& GetEndpoints()
        {
            static Endpoint endpoints {};
            return endpoints;
        }
    };

    QNetworkRequest GetRequestInterface( QUrl const &address );
    QNetworkRequest PostRequestInterface( QUrl const &address );
    QJsonObject     GetJsonNetworkData(QNetworkReply *data, bool show_error_message = false );
}

#endif // RESOURCES_HPP
