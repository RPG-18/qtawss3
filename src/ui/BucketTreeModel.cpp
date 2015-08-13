#include "BucketTreeModel.h"

#include <QtCore/QDebug>

#include <QtWidgets/QStyle>
#include <QtWidgets/QApplication>

using namespace ASSS;
namespace Gui
{
    BucketTreeModel::BucketTreeModel(QObject* parent) :
            QAbstractItemModel(parent)
    {
    }

    QHash<int, QByteArray> BucketTreeModel::roleNames() const
    {
        const static QHash<int, QByteArray> roles =
        { { Name, "name" } };

        return roles;
    }

    void BucketTreeModel::setTree(const BucketTreePtr& tree)
    {
        beginResetModel();
        m_tree = tree;
        endResetModel();
    }

    int BucketTreeModel::columnCount(const QModelIndex & parent) const
    {
        Q_UNUSED(parent);
        return 1;
    }

    QVariant BucketTreeModel::data(const QModelIndex & index, int role) const
    {
        if (role == Qt::DisplayRole)
        {
            const auto node =
                    static_cast<BucketTree::TreeNode*>(index.internalPointer());
            Q_ASSERT(node != nullptr);

            switch (node->type())
            {
            case BucketTree::TreeNode::Leaf:
            {
                ObjectInfo obj = m_tree->object(node->bucketPos());
                return obj.key();
            }
            case BucketTree::TreeNode::Node:
            {
                return node->segment();
            }
            default:
                return QString("Undef");
            }
        }
        else if (role == Qt::DecorationRole)
        {
            const auto node =
                    static_cast<BucketTree::TreeNode*>(index.internalPointer());
            Q_ASSERT(node != nullptr);

            switch (node->type())
            {
            case BucketTree::TreeNode::Leaf:
            {
                return qApp->style()->standardIcon(QStyle::SP_FileIcon);
            }
            case BucketTree::TreeNode::Node:
            {
                return qApp->style()->standardIcon(QStyle::SP_DirIcon);
            }
            default:
                return QVariant();
            }
        }

        return QVariant();
    }

    QModelIndex BucketTreeModel::index(int row, int column,
            const QModelIndex & parent) const
    {
        if (!parent.isValid())
        {
            //is root
            return createIndex(0, 0,
                    const_cast<BucketTree::TreeNode*>(m_tree->root()));
        }

        const auto node =
                static_cast<BucketTree::TreeNode*>(parent.internalPointer());

        Q_ASSERT(node != nullptr);

        return createIndex(row, column,
                const_cast<BucketTree::TreeNode*>(node->children(row)));
    }

    QModelIndex BucketTreeModel::parent(const QModelIndex & index) const
    {
        const auto node =
                static_cast<BucketTree::TreeNode*>(index.internalPointer());

        if (node == nullptr || node == m_tree->root())
        {
            return QModelIndex();
        }

        const auto parent = node->parent();
        if (parent == nullptr)
        {
            return QModelIndex();
        }

        if (parent == m_tree->root())
        {
            return createIndex(0, 0,
                    const_cast<BucketTree::TreeNode*>(m_tree->root()));
        }

        const auto granddad = parent->parent();
        if (granddad == nullptr)
        {
            return QModelIndex();
        }

        int pos = granddad->childrenPos(parent);
        if (pos == -1)
        {
            return QModelIndex();
        }

        return createIndex(pos, 0, const_cast<BucketTree::TreeNode*>(parent));
    }

    int BucketTreeModel::rowCount(const QModelIndex & parent) const
    {
        if(m_tree == nullptr)
        {
            return 0;
        }

        if (!parent.isValid())
        {
            const int count = m_tree->root()->childrens();
            return count?1:0;
        }

        const auto node =
                static_cast<BucketTree::TreeNode*>(parent.internalPointer());
        if (node != nullptr)
        {
            return node->childrens();
        }
        return 0;
    }

    BucketTree::TreeNode::Type BucketTreeModel::nodeType(const QModelIndex& index) const
    {
        if(!index.isValid())
        {
            return BucketTree::TreeNode::Undef;
        }
        const auto node =
                static_cast<BucketTree::TreeNode*>(index.internalPointer());

        if (node == nullptr)
        {
            return BucketTree::TreeNode::Undef;
        }

        return node->type();
    }

    int BucketTreeModel::objPosition(const QModelIndex& index) const
    {
        if(!index.isValid())
        {
            return -1;
        }
        const auto node =
                static_cast<BucketTree::TreeNode*>(index.internalPointer());

        if (node == nullptr)
        {
            return -1;
        }

        return node->bucketPos();
    }

    QVariant BucketTreeModel::headerData(int section, Qt::Orientation orientation, int role) const
    {
        if(role == Qt::DisplayRole)
        {
            return "Buckets";
        }
        return QVariant();
    }

    QString BucketTreeModel::path(const QModelIndex& index) const
    {
        if(!index.isValid())
        {
            return QString();
        }
        const auto node =
                static_cast<BucketTree::TreeNode*>(index.internalPointer());

        if (node == nullptr)
        {
            return QString();
        }

        return node->path();
    }

    void BucketTreeModel::insert(const QModelIndex& index, const ASSS::ObjectInfo& info)
    {
        if(!index.isValid())
        {
            return;
        }

        const auto node =
                static_cast<BucketTree::TreeNode*>(index.internalPointer());

        int to = 0;
        if(node != nullptr)
        {
            to = node->childrens() ? 1 : 0;
        }
        beginInsertRows(index,0,to);
        m_tree->insert(info);
        endInsertRows();
    }
}
