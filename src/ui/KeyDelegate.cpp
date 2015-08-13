#include <QtCore/QDebug>

#include "ObjectInfo.h"
#include "KeyDelegate.h"

using namespace ASSS;

namespace Gui
{
    KeyDelegate::KeyDelegate(QObject* parent) :
            QItemDelegate(parent)
    {
    }

    void KeyDelegate::drawDisplay(QPainter * painter,
            const QStyleOptionViewItem & option, const QRect & rect,
            const QString & text) const
    {
        const QString name = ObjectInfo::extractFileName(text);
        QItemDelegate::drawDisplay(painter, option, rect,name);
    }
}
