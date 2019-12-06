#ifndef USERMODEL_H
#define USERMODEL_H

#include <QAbstractTableModel>

namespace utilities
{
struct UserData
{
    qint64   user_id;
    int      user_role;
    QString  fullname;
    QString  username;
    QString  email;
    QJsonObject ToJson() const;
};
}

class UserModel: public QAbstractTableModel
{
    Q_OBJECT
    enum UserRole {
        BasicUser = 0x4A,
        Administrator = 0x4B
    };
    QString RoleToString( int const role ) const;
public:
    UserModel( QVector<utilities::UserData>& users, QObject* parent = nullptr );
    int rowCount( QModelIndex const &parent ) const override;
    int columnCount( QModelIndex const &parent ) const override;
    QVariant data( QModelIndex const &index, int role ) const override;
    QVariant headerData( int section, Qt::Orientation orientation, int role )
        const override;
    Qt::ItemFlags flags( QModelIndex const &index ) const override;
    bool removeRows( int row, int count,
                     QModelIndex const & parent = QModelIndex{} ) override;

    bool setData( QModelIndex const &index, QVariant const &value,
                 int role = Qt::EditRole ) override;
private:
    QVector<utilities::UserData> &users_;
};


#endif // USERMODEL_H
