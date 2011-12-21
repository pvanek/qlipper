#ifndef QLIPPERMODEL_H
#define QLIPPERMODEL_H

#include <QAbstractListModel>
#include <QtGui/QClipboard>

#include "qlipperitem.h"


class QlipperModel : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit QlipperModel(QObject *parent = 0);

public slots:
    void clearHistory();
    void indexTriggered(const QModelIndex &);

protected:
    int rowCount(const QModelIndex&) const;
    QVariant data(const QModelIndex&, int) const;
    Qt::ItemFlags flags(const QModelIndex & index) const;

private:
    QList<QlipperItem> m_sticky;
    QList<QlipperItem> m_data;
    QClipboard *m_clipboard;

    QList<QlipperItem> getList(int &row) const;

private slots:
    void clipboard_changed(QClipboard::Mode);
};

#endif // QLIPPERMODEL_H
