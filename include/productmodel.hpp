#ifndef PRODUCTMODEL_HPP
#define PRODUCTMODEL_HPP

#include <QObject>
#include <QAbstractTableModel>
#include <QVariant>
#include <QVector>
#include "resources.hpp"

class ProductModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit ProductModel( QVector<utilities::ProductData> &products,
                           QObject *parent = nullptr );
    Qt::ItemFlags flags( QModelIndex const &index ) const override;
    QVariant data( QModelIndex const& index, int role) const override;
    QVariant headerData( int section, Qt::Orientation orientation,
                         int role ) const override;
    int rowCount( QModelIndex const& parent = QModelIndex{} ) const override;
    int columnCount( QModelIndex const& parent) const override;
    bool removeRows( int row, int count,
                     QModelIndex const &parent = QModelIndex() ) override;
signals:

public slots:
private:
    QVector<utilities::ProductData> &products_;
};


class OrderingProductModel: public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit OrderingProductModel( QVector<utilities::ProductData> & products,
                                   QObject* parent = nullptr );
    Qt::ItemFlags flags( QModelIndex const & )const override;
    int rowCount( QModelIndex const &parent ) const override;
    int columnCount( QModelIndex const &parent ) const override;
    QVariant headerData( int section, Qt::Orientation orientation,
                         int role ) const override;
    QVariant data( QModelIndex const &index, int role ) const override;
    QMimeData* mimeData( QModelIndexList const &indexes ) const override;
    QStringList mimeTypes() const override;
private:
    QVector<utilities::ProductData>& products_;
};

#endif // PRODUCTMODEL_HPP
