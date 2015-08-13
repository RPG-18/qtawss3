#pragma once

#include <QtWidgets/QItemDelegate>

namespace Gui
{
    class KeyDelegate: public QItemDelegate
    {
    public:
        KeyDelegate(QObject* parent = nullptr);

    protected:

        virtual void drawDisplay(QPainter * painter,
                const QStyleOptionViewItem & option, const QRect & rect,
                const QString & text) const;
    };
}
