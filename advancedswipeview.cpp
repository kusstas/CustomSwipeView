#include "advancedswipeview.h"

AdvancedSwipeView::AdvancedSwipeView(QQuickItem* parent)
    : QQuickItem(parent), m_currentIndex(0)
{
    setClip(true);
}

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

void AdvancedSwipeView::setCurrentIndex(int currentIndex)
{
    assert(currentIndex >= 0 && currentIndex < count());
    if (m_currentIndex == currentIndex)
        return;

    m_currentIndex = currentIndex;
    emit currentIndexChanged(m_currentIndex);
}

void AdvancedSwipeView::componentComplete()
{
    QQuickItem::componentComplete();
    assert(height() > 0 && width() > 0);

    if (!children().empty()) {
        for (auto const& child : children()) {
            QQuickItem* item = qobject_cast<QQuickItem*>(child);
            if (nullptr != item) {
                m_content << item;
            }
        }

        // TODO: handle visibility and position of objects
    }
}

void AdvancedSwipeView::snapOneItem()
{
    for (int i = 0; i < currentIndex(); i++) {
        m_content[i]->setVisible(false);
    }

    int p = 0;
}
