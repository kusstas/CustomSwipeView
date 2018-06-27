#include "advancedswipeview.h"
#include <QDebug>

AdvancedSwipeView::AdvancedSwipeView(QQuickItem* parent)
    : QQuickItem(parent), m_orientation(Qt::Horizontal), m_currentIndex(0)
{
    setClip(true);
    setAcceptedMouseButtons(Qt::AllButtons);
}

// ------------------------------------------------------

int AdvancedSwipeView::count() const
{
    return m_content.count();
}

int AdvancedSwipeView::prevIndex() const
{
    return currentIndex() == 0 ? count() - 1 : currentIndex() - 1;
}

int AdvancedSwipeView::nextIndex() const
{
    return currentIndex() == count() - 1 ? 0 : currentIndex() + 1;
}

int AdvancedSwipeView::currentIndex() const
{
    return m_currentIndex;
}

QQuickItem* AdvancedSwipeView::currentItem() const
{
    return m_content.at(currentIndex());
}

Qt::Orientation AdvancedSwipeView::orientation() const
{
    return m_orientation;
}

// ------------------------------------------------------

void AdvancedSwipeView::setCurrentIndex(int currentIndex)
{
    if (m_currentIndex == currentIndex)
        return;

    m_currentIndex = currentIndex;
    emit currentIndexChanged(m_currentIndex);
}

void AdvancedSwipeView::setOrientation(Qt::Orientation orientation)
{
    assert(orientation == Qt::Vertical || orientation == Qt::Horizontal);
    if (m_orientation == orientation)
        return;

    m_orientation = orientation;
    emit orientationChanged(m_orientation);
}

// ------------------------------------------------------

void AdvancedSwipeView::componentComplete()
{    
    QQuickItem::componentComplete();
    if (!children().empty()) {
        for (auto const& child : children()) {
            QQuickItem* item = qobject_cast<QQuickItem*>(child);
            if (nullptr != item) {
                m_content << item;
            }
        }
    }
}

void AdvancedSwipeView::mousePressEvent(QMouseEvent* event)
{
    QQuickItem::mousePressEvent(event);
    qDebug() << "ok";
}

void AdvancedSwipeView::mouseMoveEvent(QMouseEvent* event)
{
    QQuickItem::mouseMoveEvent(event);
    qDebug() << "move";
}

void AdvancedSwipeView::mouseReleaseEvent(QMouseEvent* event)
{
    QQuickItem::mouseReleaseEvent(event);
    qDebug() << "event release";
}

// ------------------------------------------------------

void AdvancedSwipeView::snapOneItemInstantly()
{
    // choice of parameter snap
    using PropertyRead = qreal (QQuickItem::*)() const;
    using PropertyWrite = void (QQuickItem::*)(qreal);
    PropertyRead length;
    PropertyWrite setterPos;
    if (Qt::Horizontal == orientation()) {
        setterPos = &QQuickItem::setX;
        length = &QQuickItem::width;
    }
    else if (Qt::Vertical == orientation()) {
        setterPos = &QQuickItem::setY;
        length = &QQuickItem::height;
    }

    // snap
    qreal p = 0;
    for (int i = currentIndex() - 1; i >= 0; i--) {
        p -= (m_content[i]->*length)();
        (m_content[i]->*setterPos)(p);
    }
    p = 0;
    for (int i = currentIndex(); i < count(); i++) {
        (m_content[i]->*setterPos)(p);
         p += (m_content[i]->*length)();
    }
}
