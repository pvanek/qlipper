#ifndef QLIPPERMODEL_H
#define QLIPPERMODEL_H

#include <QAbstractListModel>
#include <QtGui/QClipboard>


class QlipperModel : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit QlipperModel(QObject *parent = 0);

protected:
    int rowCount(const QModelIndex&) const;
    QVariant data(const QModelIndex&, int) const;
    Qt::ItemFlags flags(const QModelIndex & index) const;

private:
    QList<QString> m_data;
    QClipboard *m_clipboard;

private slots:
    void clipboard_changed(QClipboard::Mode);
};

#endif // QLIPPERMODEL_H
