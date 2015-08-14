#include "ObjectTree.h"

#include <algorithm>

#include <QtCore/QFile>
#include <QtCore/QDebug>
#include <QtCore/QTextStream>
#include <QtCore/QStringList>
#include <QtCore/QtAlgorithms>
#include <QtCore/QRegularExpression>


namespace Gui
{
    ObjectTree::ObjectTree() :
            m_root(nullptr)
    {
        makeRoot();
    }

    ObjectTree::~ObjectTree()
    {
        delete m_root;
        m_root = nullptr;
    }

    void ObjectTree::makeRoot()
    {
        m_root = new TreeNode;
        m_root->m_type = TreeNode::Node;
        m_root->m_segment = "root";
        m_root->m_isRoot = true;
    }

    void ObjectTree::build(const ASSS::ObjectInfoList& bucket)
    {
        m_bucket = bucket;
        int pos = 0;
        for (const auto& object : m_bucket)
        {
            appendNode(object, pos);
            ++pos;
        }
    }

    void ObjectTree::insert(const ASSS::ObjectInfo& object)
    {
        int pos = m_bucket.size();
        m_bucket.push_back(object);
        appendNode(object, pos);
    }

    void ObjectTree::appendNode(const ASSS::ObjectInfo& info, int pos)
    {
        const auto key = info.key();

        QRegularExpression delimiter("/|(:/)");
        if (key.contains(delimiter) )
        {
            auto segmets = key.split(delimiter);
            if (segmets.first().isEmpty())
            {
                segmets.first() = "/";
            }
            add(segmets, pos);
        }
        else
        {
            addInRoot(pos);
        }
    }

    bool ObjectTree::toDot(const QString& path) const
    {
        QFile dotFile(path);

        if (!dotFile.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            return false;
        }

        QTextStream stream(&dotFile);

        stream << "graph G {" << endl;
        stream << "    rankdir=LR" << endl;
        stream << "    node [shape=rect]" << endl;

        toDot(m_root, stream);

        stream << "}" << endl;

        stream.flush();
        dotFile.close();
        return true;
    }

    void ObjectTree::toDot(const ObjectTree::TreeNode* node,
            QTextStream& stream) const
    {
        for (auto child : node->m_children)
        {
            switch (child->m_type)
            {
            case TreeNode::Leaf:
                stream << "    \"" << node->m_segment << "\"--\""
                       << m_bucket[child->m_bucketPos].key() << "\";" << endl;
                break;
            case TreeNode::Node:
                stream << "    \"" << node->m_segment << "\"--\""
                       << child->m_segment << "\";" << endl;
                break;
            default:
                qDebug() << "Unknown node type";
            }
            toDot(child, stream);
        }
    }

    void ObjectTree::add(const QStringList& segments, int pos)
    {
        TreeNode* parent = m_root;
        for (auto iter = segments.cbegin(); iter != segments.end(); ++iter)
        {
            if (iter->isEmpty())
            {
                //this directory
                //skip
                return;
            }

            if (parent->segment() == *iter)
            {
                const auto nextSegment = *(iter + 1);
                const auto find = std::find_if(parent->m_children.begin(),
                        parent->m_children.end(), [nextSegment](TreeNode* node)
                        {
                            return node->m_segment == nextSegment;
                        });
                if (find != parent->m_children.end())
                {
                    //to next segment
                    parent = *find;
                }
                else
                {
                    //insert in tree
                    const auto type =
                            (iter + 1) != segments.end() ?
                                    TreeNode::Node : TreeNode::Leaf;
                    auto node = new TreeNode(type, parent, pos, *iter);
                    parent = node;
                }
            }
            else
            {
                const auto find = std::find_if(parent->m_children.begin(),
                        parent->m_children.end(), [iter](TreeNode* node)
                        {
                            return node->m_segment == *iter;
                        });
                if (find != parent->m_children.end())
                {
                    parent = *find;
                    //to next segment
                    continue;
                }

                //insert in tree
                const auto type =
                        (iter + 1) != segments.end() ?
                                TreeNode::Node : TreeNode::Leaf;
                auto node = new TreeNode(type, parent, pos, *iter);
                parent = node;
            }
        }
    }

    void ObjectTree::addInRoot(int pos)
    {
        new TreeNode(TreeNode::Leaf, m_root, pos);

    }

    const ObjectTree::TreeNode* ObjectTree::root() const
    {
        return m_root;
    }

    const ASSS::ObjectInfo& ObjectTree::object(int pos) const
    {
        Q_ASSERT(pos >= 0);
        Q_ASSERT(pos < m_bucket.size());

        return m_bucket[pos];
    }

    void ObjectTree::setBucketName(const QString& name)
    {
        m_root->m_segment = name;
    }

////////////////////////////////////////////////////////////////////////////////

    ObjectTree::TreeNode::TreeNode() :
            m_type(Undef),
            m_parent(nullptr),
            m_bucketPos(-1),
            m_nodePos(-1),
            m_isRoot(false)
    {
    }

    ObjectTree::TreeNode::TreeNode(Type type, TreeNode* parent, int pos,
            const QString& segment) :
            m_type(type),
            m_parent(parent),
            m_bucketPos(pos),
            m_segment(segment),
            m_nodePos(-1),
            m_isRoot(false)

    {
        m_nodePos = m_parent->m_children.count();
        m_parent->m_children.push_back(this);
    }

    ObjectTree::TreeNode::~TreeNode()
    {
        qDeleteAll(m_children.begin(), m_children.end());
        m_children.clear();
        m_bucketPos = -1;
    }

    ObjectTree::TreeNode::Type ObjectTree::TreeNode::type() const
    {
        return m_type;
    }

    int ObjectTree::TreeNode::bucketPos() const
    {
        return m_bucketPos;
    }

    const QString& ObjectTree::TreeNode::segment() const
    {
        return m_segment;
    }

    int ObjectTree::TreeNode::childrens(Type type) const
    {
        if (type != Undef)
        {
            return std::count_if(m_children.cbegin(), m_children.cend(),
                    [type](const TreeNode* node)
                    {
                        return node->m_type == type;

                    });
        }

        return m_children.size();
    }

    const ObjectTree::TreeNode* ObjectTree::TreeNode::children(int pos) const
    {
        Q_ASSERT(pos >= 0);
        Q_ASSERT(pos < m_children.size());

        return m_children[pos];
    }

    const ObjectTree::TreeNode* ObjectTree::TreeNode::parent() const
    {
        return m_parent;
    }

    int ObjectTree::TreeNode::childrenPos(
            const ObjectTree::TreeNode* child) const
    {
        int pos = 0;
        for (const auto item : m_children)
        {
            if (item == child)
            {
                return pos;
            };
            ++pos;
        }
        return -1;
    }

    QString ObjectTree::TreeNode::path() const
    {
        QStringList out;

        const TreeNode* cur = this;
        while(cur!=nullptr && !cur->m_isRoot)
        {
            out.push_front(cur->m_segment);
            cur = cur->m_parent;
        }

        return out.join('/');
    }
}
