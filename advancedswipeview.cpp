#include "advancedswipeview.h"
#include <QDebug>

AdvancedSwipeView::AdvancedSwipeView(QQuickItem* parent)
    : QQuickItem(parent), m_orientation(Qt::Horizontal), m_loop(false), m_visibleRelitivePos(0.0f),
      m_thresholdSwitch(0.5f), m_minVelocitySwitch(8.0f),
      m_maxPullingOutOnEnd(0.25f), m_seized(false),
      m_currentIndex(0)
{
    setClip(true);
    setAcceptedMouseButtons(Qt::LeftButton);

    connect(this, &QQuickItem::widthChanged, this, &AdvancedSwipeView::onWidthChanged);
    connect(this, &QQuickItem::heightChanged, this, &AdvancedSwipeView::onHeightChanged);
    connect(&m_timer, &QTimer::timeout, this, &AdvancedSwipeView::onTimerOut);
}

// ------------------------------------------------------

Qt::Orientation AdvancedSwipeView::orientation() const
{
    return m_orientation;
}

bool AdvancedSwipeView::loop() const
{
    return m_loop;
}

// ------------------------------------------------------

qreal AdvancedSwipeView::thresholdSwitch() const
{
    return m_thresholdSwitch;
}

qreal AdvancedSwipeView::minVelocitySwitch() const
{
    return m_minVelocitySwitch;
}

qreal AdvancedSwipeView::maxPullingOutOnEnd() const
{
    return m_maxPullingOutOnEnd;
}

// ------------------------------------------------------

bool AdvancedSwipeView::seized() const
{
    return m_seized;
}

int AdvancedSwipeView::count() const
{
    return m_content.count();
}

int AdvancedSwipeView::prevIndex() const
{
    return computePrevIndex(currentIndex());
}

int AdvancedSwipeView::nextIndex() const
{
    return computeNextIndex(currentIndex());
}

int AdvancedSwipeView::currentIndex() const
{
    return m_currentIndex;
}

// ------------------------------------------------------

QQuickItem* AdvancedSwipeView::currentItem() const
{
    return count() > 0 ? m_content.at(currentIndex()) : nullptr;
}

QQuickItem* AdvancedSwipeView::prevItem() const
{
    return computePrevItem(currentIndex());
}

QQuickItem* AdvancedSwipeView::nextItem() const
{
    return computeNextItem(currentIndex());
}

// ------------------------------------------------------

void AdvancedSwipeView::setOrientation(Qt::Orientation orientation)
{
    assert(orientation == Qt::Vertical || orientation == Qt::Horizontal);
    if (m_orientation == orientation) {
        return;
    }

    m_orientation = orientation;
    emit orientationChanged(m_orientation);
}

void AdvancedSwipeView::setLoop(bool loop)
{
    if (m_loop == loop) {
        return;
    }

    m_loop = loop;
    emit loopChanged(m_loop);
}

void AdvancedSwipeView::setThresholdSwitch(qreal thresholdSwitch)
{
    assert(thresholdSwitch >= 0.0f && thresholdSwitch <= 1.0f);
    if (qFuzzyCompare(m_thresholdSwitch, thresholdSwitch)) {
        return;
    }

    m_thresholdSwitch = thresholdSwitch;
    emit thresholdSwitchChanged(m_thresholdSwitch);
}

void AdvancedSwipeView::setMinVelocitySwitch(qreal minVelocitySwitch)
{
    assert(minVelocitySwitch > 0.0f);
    if (qFuzzyCompare(m_minVelocitySwitch, minVelocitySwitch)) {
        return;
    }

    m_minVelocitySwitch = minVelocitySwitch;
    emit minVelocitySwitchChanged(m_minVelocitySwitch);
}

void AdvancedSwipeView::setMaxPullingOutOnEnd(qreal maxPullingOutOnEnd)
{
    assert(maxPullingOutOnEnd >= 0.0f && maxPullingOutOnEnd <= 1.0f);
    if (qFuzzyCompare(m_maxPullingOutOnEnd, maxPullingOutOnEnd)) {
        return;
    }

    m_maxPullingOutOnEnd = maxPullingOutOnEnd;
    emit maxPullingOutOnEndChanged(m_maxPullingOutOnEnd);
}

void AdvancedSwipeView::setCurrentIndex(int currentIndex)
{
    if (m_currentIndex == currentIndex) {
        return;
    }

    if (isComponentComplete()) {
        assert(indexValid(currentIndex));
    }

    m_currentIndex = currentIndex;
    emit currentIndexChanged(m_currentIndex);
}

// ------------------------------------------------------

void AdvancedSwipeView::onWidthChanged()
{
    for (auto item : m_content) {
        item->setWidth(width());
    }
    if (orientation() == Qt::Horizontal) {
        // save ratio
        currentItem()->setX(m_visibleRelitivePos * width());
        showVisibleCurrentItem(width(), &QQuickItem::setX, &QQuickItem::x);
    }
}

void AdvancedSwipeView::onHeightChanged()
{
    for (auto item : m_content) {
        item->setHeight(height());
    }
    if (orientation() == Qt::Vertical) {
        // save ratio
        currentItem()->setY(m_visibleRelitivePos * height());
        showVisibleCurrentItem(height(), &QQuickItem::setY, &QQuickItem::y);
    }
}

void AdvancedSwipeView::onTimerOut()
{

}

// ------------------------------------------------------

void AdvancedSwipeView::componentComplete()
{    
    QQuickItem::componentComplete();
    if (!children().empty()) {
        for (auto const& child : children()) {
            // add only QQuickItem from children
            QQuickItem* item = qobject_cast<QQuickItem*>(child);
            if (nullptr != item) {
                m_content << item;
                item->setSize(size());
                item->setVisible(false);
            }
        }
        assert(!m_content.empty());
        assert(indexValid(currentIndex()));
        currentItem()->setVisible(true);
        m_visibleCurrentIndex = currentIndex();
    }
}

void AdvancedSwipeView::mousePressEvent(QMouseEvent* event)
{
    setSeized(true);
    m_posPrevMouse = event->localPos();
}

void AdvancedSwipeView::mouseMoveEvent(QMouseEvent* event)
{
    QPointF locPos = event->localPos();

    // compute velocity
    m_velMouse = locPos - m_posPrevMouse;
    m_midleVelMouse += m_velMouse;
    m_midleVelMouse /= 2.0f;

    // offset
    QPointF offset = locPos - m_posPrevMouse;
    m_posPrevMouse = locPos;

    shiftVisibleContent(offset);
    setCurrentIndex(m_visibleCurrentIndex);
}

void AdvancedSwipeView::mouseReleaseEvent(QMouseEvent* event)
{
    setSeized(false);
    m_velMouse = QPoint(0, 0);
    m_midleVelMouse = QPoint(0, 0);
}

void AdvancedSwipeView::setSeized(bool seized)
{
    if (m_seized == seized)
        return;

    m_seized = seized;
    emit seizedChanged(m_seized);
}

// ------------------------------------------------------

bool AdvancedSwipeView::indexValid(int index) const
{
    return index >= 0 && index < count();
}

int AdvancedSwipeView::shiftIndex(int index, int shift) const
{
    index += shift;
    if (loop() && count() > 1) {
        index %= count();
        if (index < 0) {
            index = count() + index;
        }
    }
    else {
        if (index < 0) {
            index = 0;
        }
        else if (index >= count()) {
            index = count() - 1;
        }
    }
    return index;
}

int AdvancedSwipeView::computePrevIndex(int index) const
{
    return index == 0 && loop() && count() > 1 ? count() - 1 : index - 1;
}

int AdvancedSwipeView::computeNextIndex(int index) const
{
    return index >= count() - 1 && loop() && count() > 1 ? 0 : index + 1;
}

QQuickItem* AdvancedSwipeView::computePrevItem(int index) const
{
    int i = computePrevIndex(index);
    return i >= 0 && i < count() && count() > 0 ? m_content.at(i) : nullptr;
}

QQuickItem*AdvancedSwipeView::computeNextItem(int index) const
{
    int i = computeNextIndex(index);
    return i >= 0 && i < count() && count() > 0 ? m_content.at(i) : nullptr;
}

// ------------------------------------------------------

QQuickItem* AdvancedSwipeView::visibleCurrentItem() const
{
    return m_content[m_visibleCurrentIndex];
}

QQuickItem* AdvancedSwipeView::visiblePrevItem() const
{
    return computePrevItem(m_visibleCurrentIndex);
}

QQuickItem* AdvancedSwipeView::visibleNextItem() const
{
    return computeNextItem(m_visibleCurrentIndex);
}

// ------------------------------------------------------

void AdvancedSwipeView::limitVisibleCurrentItem(qreal length, SetterPos setterPos)
{
    auto cItem = visibleCurrentItem();
    auto pItem = visiblePrevItem();
    auto nItem = visibleNextItem();
    if (m_visibleRelitivePos > maxPullingOutOnEnd() && nullptr == pItem) {
        m_visibleRelitivePos = maxPullingOutOnEnd();
        (cItem->*setterPos)(m_visibleRelitivePos * length);
    }
    else if (m_visibleRelitivePos < -maxPullingOutOnEnd() && nullptr == nItem) {
        m_visibleRelitivePos = -maxPullingOutOnEnd();
        (cItem->*setterPos)(m_visibleRelitivePos * length);
    }
}

void AdvancedSwipeView::showVisibleCurrentItem(qreal length, SetterPos setterPos, GetterPos getterPos)
{
    auto cItem = visibleCurrentItem();
    auto pItem = visiblePrevItem();
    auto nItem = visibleNextItem();
    qreal currentItemCoord = (cItem->*getterPos)();
    if (currentItemCoord < 0) { // show next and hide prev, if they exists
        if (nullptr != nItem) {
            nItem->setVisible(true);
            (nItem->*setterPos)(currentItemCoord + length);
        }
        if (nullptr != pItem && pItem != nItem) {
            pItem->setVisible(false);
        }
    }
    else if (currentItemCoord > 0) { // show prev and hide next, if they exists
        if (nullptr != pItem) {
            pItem->setVisible(true);
            (pItem->*setterPos)(currentItemCoord - length);
        }
        if (nullptr != nItem && nItem != pItem) {
            nItem->setVisible(false);
        }
    }
    else { // hide prev and next, if they exist
        if (nullptr != pItem) {
            pItem->setVisible(false);
        }
        if (nullptr != nItem) {
            nItem->setVisible(false);
        }
    }
}

void AdvancedSwipeView::shiftVisibleContent(QPointF const& offset)
{
    qreal length = 0;
    qreal offsetAxis = 0;
    SetterPos setterPos = nullptr;
    GetterPos getterPos = nullptr;

    // chose parameters by orientation
    if (orientation() == Qt::Horizontal) {
        length = width();
        offsetAxis = offset.x();
        setterPos = &QQuickItem::setX;
        getterPos = &QQuickItem::x;
    }
    else if (orientation() == Qt::Vertical) {
        length = height();
        offsetAxis = offset.y();
        setterPos = &QQuickItem::setY;
        getterPos = &QQuickItem::y;
    }

    // limit offset
    if (offsetAxis > length) {
        offsetAxis = length;
    }
    else if (offsetAxis < -length) {
        offsetAxis = -length;
    }

    // set visible current item
    (visibleCurrentItem()->*setterPos)((visibleCurrentItem()->*getterPos)() + offsetAxis);
    if (m_visibleRelitivePos > thresholdSwitch() && nullptr != visiblePrevItem()) {
        m_visibleCurrentIndex = computePrevIndex(m_visibleCurrentIndex);
    }
    else if (m_visibleRelitivePos < -thresholdSwitch() && nullptr != visibleNextItem()) {
        m_visibleCurrentIndex = computeNextIndex(m_visibleCurrentIndex);
    }
    m_visibleRelitivePos = (visibleCurrentItem()->*getterPos)() / length;

    // show current item
    limitVisibleCurrentItem(length, setterPos);
    showVisibleCurrentItem(length, setterPos, getterPos);
}
