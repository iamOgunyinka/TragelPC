#ifndef PRODUCTTHUMBNAILDELEGATE_HPP
#define PRODUCTTHUMBNAILDELEGATE_HPP

#include <QAbstractItemDelegate>

class ProductThumbnailDelegate : public QAbstractItemDelegate
{
    Q_OBJECT

public:
    ProductThumbnailDelegate( QObject *parent = nullptr );
    void paint( QPainter *painter, QStyleOptionViewItem const &option,
               QModelIndex const &index) const;
    QSize sizeHint( QStyleOptionViewItem const &option, QModelIndex const &index ) const;
};

class LocalProductUploadDelegate: public QAbstractItemDelegate
{
    Q_OBJECT

public:
    LocalProductUploadDelegate( QObject *parent = nullptr );
    void paint( QPainter *painter, QStyleOptionViewItem const &option,
               QModelIndex const &index) const;
    QSize sizeHint( QStyleOptionViewItem const &option, QModelIndex const &index ) const;
};
#endif // PRODUCTTHUMBNAILDELEGATE_HPP
