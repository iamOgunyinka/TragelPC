#ifndef RESOURCES_HPP
#define RESOURCES_HPP

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QJsonObject>
#include <QNetworkReply>

namespace utilities {
    class NetworkManager
    {
        NetworkManager() = default;
    public:
        static QNetworkAccessManager& GetNetwork()
        {
            static QNetworkAccessManager network_manager;
            return network_manager;
        }
    };

    class Endpoint
    {
        QJsonObject endpoint_object;
        Endpoint() = default;
    public:
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

    QNetworkRequest GetRequestInterface( QString const & address );
    QNetworkRequest PostRequestInterface( QString const & address );
    QJsonObject     GetJsonNetworkData( QNetworkReply *data );
}

#endif // RESOURCES_HPP
