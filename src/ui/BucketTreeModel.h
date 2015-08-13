#pragma once

#include <QtCore/QHash>
#include <QtCore/QByteArray>
#include <QtCore/QAbstractItemModel>

#include "BucketTree.h"

namespace Gui
{
    class BucketTreeModel: public QAbstractItemModel
    {
    Q_OBJECT

    public:

        enum DirModelRole
        {
            Name = Qt::UserRole + 1
        };

        BucketTreeModel(QObject* parent = nullptr);

        void setTree(const BucketTreePtr& tree);
        BucketTree::TreeNode::Type nodeType(const QModelIndex& index) const;
        int objPosition(const QModelIndex& index) const;

        QString path(const QModelIndex& index) const;
        void insert(const QModelIndex& index, const ASSS::ObjectInfo& info);

        virtual QHash<int, QByteArray> roleNames() const;
        virtual int columnCount(const QModelIndex & parent = QModelIndex()) const;
        virtual QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;
        virtual QModelIndex index(int row, int column, const QModelIndex & parent = QModelIndex()) const;
        virtual QModelIndex parent(const QModelIndex & index) const;
        virtual int rowCount(const QModelIndex & parent = QModelIndex()) const;
        virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

    private:

        BucketTreePtr m_tree;
    };
}
