#pragma once

#include <memory>

#include <QtCore/QList>
#include <QtCore/QString>

#include "ObjectInfo.h"

class QTextStream;

namespace Gui
{
    class ObjectTree
    {
    public:

        class TreeNode;

        ObjectTree();
        ~ObjectTree();

        ObjectTree(const ObjectTree&) = delete;
        ObjectTree& operator=(const ObjectTree&) = delete;

        void build(const ASSS::ObjectInfoList& bucket);
        void insert(const ASSS::ObjectInfo& object);

        void setBucketName(const QString& name);

        /*!
         * Export to dot
         * @see http://www.graphviz.org/
         * @param[in] path path to file
         */
        bool toDot(const QString& path) const;

        const TreeNode* root() const;

        const ASSS::ObjectInfo& object(int pos) const;

    private:

        void makeRoot();

        void appendNode(const ASSS::ObjectInfo& info, int pos);

        void add(const QStringList& segments, int pos);

        void addInRoot(int pos);

        /*!
         * Save node to dot file
         */
        void toDot(const TreeNode* node, QTextStream& stream) const;

    private:

        TreeNode* m_root;
        ASSS::ObjectInfoList m_objects;
    };

    class ObjectTree::TreeNode
    {
        typedef QList<TreeNode*> NodeList;

    public:

        enum Type
        {
            Undef,
            Leaf,
            Node
        };

        TreeNode();
        ~TreeNode();

        TreeNode(Type type,
                 TreeNode* parent,
                 int pos,
                 const QString& segment = QString());

        TreeNode& operator=(const TreeNode&) = delete;
        TreeNode(const TreeNode&) = delete;

        Type type() const;
        int objectPos() const;
        const QString& segment() const;

        const TreeNode* children(int pos) const;
        int childrens(Type type = Undef) const;
        int childrenPos(const TreeNode* child) const;

        const TreeNode* parent() const;
        QString path() const;

    private:

        friend class ObjectTree;

        Type m_type;
        TreeNode* m_parent;
        NodeList m_children;
        int m_objectPos;
        QString m_segment;
        bool m_isRoot;
    };

    typedef std::shared_ptr<ObjectTree> ObjectTreePtr;
}
