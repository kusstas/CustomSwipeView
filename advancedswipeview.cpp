#include "advancedswipeview.h"
#include <cmath>
#include <QDebug>

AdvancedSwipeView::AdvancedSwipeView(QQuickItem* parent)
    : QQuickItem(parent), m_orientation(Qt::Horizontal), m_loop(false),
      m_distanceReturn(0), m_visibleRelitivePos(0.0f),
      m_thresholdSwitch(0.5f), m_minVelocitySwitch(5.0f),
      m_maxPullingOutOnEnd(0.25f), m_durationSnap(100),
      m_seized(false), m_currentIndex(0),
      m_getterPos(&QQuickItem::x), m_setterPos(&QQuickItem::setX),
      m_getterLength(&QQuickItem::width)
{
    setClip(true);
    setAcceptedMouseButtons(Qt::LeftButton);
    m_timer.setInterval(10);

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

int AdvancedSwipeView::durationSnap() const
{
    return m_durationSnap;
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
    return m_content.at(currentIndex());
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

    SetterPos setterPosNull = nullptr;
    if (orientation == Qt::Horizontal) {
        setterPosNull = &QQuickItem::setY;
        m_setterPos = &QQuickItem::setX;
        m_getterPos = &QQuickItem::x;
        m_getterLength = &QQuickItem::width;
    }
    else if (orientation == Qt::Vertical) {
        setterPosNull = &QQuickItem::setX;
        m_setterPos = &QQuickItem::setY;
        m_getterPos = &QQuickItem::y;
        m_getterLength = &QQuickItem::height;
    }

    m_orientation = orientation;
    emit orientationChanged(m_orientation);

    if (isComponentComplete()){
        // reshow content
        for (auto item : m_content) {
            (item->*setterPosNull)(0);
        }
        showVisibleCurrentItem();
    }
}

void AdvancedSwipeView::setLoop(bool loop)
{
    if (m_loop == loop) {
        return;
    }

    auto nItem = nextItem();
    auto pItem = prevItem();

    m_loop = loop;
    emit loopChanged(m_loop);

    if (isComponentComplete()) {
        // limit content
        if (nullptr == nextItem() && nullptr != nItem && m_visibleRelitivePos < 0.0f) {
            nItem->setVisible(false);
            m_visibleRelitivePos = 0.0f;
        }
        else if (nullptr == prevItem() && nullptr != pItem  && m_visibleRelitivePos > 0.0f) {
            pItem->setVisible(false);
            m_visibleRelitivePos = 0.0f;
        }
        showVisibleCurrentItem();
    }
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

void AdvancedSwipeView::setDurationSnap(int durationSnap)
{
    assert(durationSnap >= 0);
    if (m_durationSnap == durationSnap)
        return;

    m_durationSnap = durationSnap;
    emit durationSnapChanged(m_durationSnap);
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
    if (!seized()) {
        runReturn();
    }

    emit currentIndexChanged(m_currentIndex);
}

// ------------------------------------------------------

void AdvancedSwipeView::onWidthChanged()
{
    for (auto item : m_content) {
        item->setWidth(width());
    }
    if (orientation() == Qt::Horizontal && isComponentComplete()) {
        showVisibleCurrentItem();
    }
}

void AdvancedSwipeView::onHeightChanged()
{
    for (auto item : m_content) {
        item->setHeight(height());
    }
    if (orientation() == Qt::Vertical && isComponentComplete()) {
        showVisibleCurrentItem();
    }
}

void AdvancedSwipeView::onTimerOut()
{
    qreal distance = m_speedReturn * m_timer.interval();
    if (fabs(distance) > fabs(m_distanceReturn)) {
        distance = m_distanceReturn;
    }
    m_distanceReturn -= distance;

    if (distance < -1.0f || distance > 1.0f) {
        int shift = static_cast<int>(distance);
        auto pItem = prevItem();
        auto nItem = nextItem();
        if (nullptr != pItem) {
            pItem->setVisible(false);
        }
        if (nullptr != nItem) {
            nItem->setVisible(false);
        }
        visibleCurrentItem()->setVisible(false);

        m_visibleCurrentIndex = shiftIndex(m_visibleCurrentIndex, shift);
        distance -= shift;
    }
    m_visibleRelitivePos += distance;
    checkOutOfThreshold();

    if (m_visibleCurrentIndex == currentIndex() && qFuzzyIsNull(m_distanceReturn)) {
        m_timer.stop();
        m_distanceReturn = 0.0f;
        m_visibleRelitivePos = 0.0f;
    }

    showVisibleCurrentItem();
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
    setCurrentIndex(m_visibleCurrentIndex);
    m_timer.stop();
}

void AdvancedSwipeView::mouseMoveEvent(QMouseEvent* event)
{
    QPointF locPos = event->localPos();

    // compute velocity
    m_velMouse = locPos - m_posPrevMouse;

    // offset
    QPointF offset = locPos - m_posPrevMouse;
    m_posPrevMouse = locPos;

    shiftVisibleContent(offset);
    setCurrentIndex(m_visibleCurrentIndex);
}

void AdvancedSwipeView::mouseReleaseEvent(QMouseEvent* event)
{
    QQuickItem::mouseReleaseEvent(event);
    qreal velMouse;
    if (orientation() == Qt::Horizontal) {
        velMouse = m_velMouse.x();
    }
    else if (orientation() == Qt::Vertical) {
        velMouse = m_velMouse.y();
    }

    if (m_visibleCurrentIndex == currentIndex()) {
        if (velMouse >= minVelocitySwitch() && m_visibleRelitivePos > 0.0f && nullptr != prevItem()) {
            setCurrentIndex(prevIndex());
        }
        else if (velMouse <= -minVelocitySwitch() && m_visibleRelitivePos < 0.0f && nullptr != nextItem()) {
            setCurrentIndex(nextIndex());
        }
    }

    setSeized(false);
    m_velMouse = QPoint(0.0f, 0.0f);
    runReturn();
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

void AdvancedSwipeView::computeDistanceReturn()
{
    int d = m_visibleCurrentIndex - currentIndex();
    if (loop()) {
        if (d > 0) {
            int n_d = m_visibleCurrentIndex - count() - currentIndex();
            if (-n_d < d) {
                d = n_d;
            }
        }
        else if (d < 0) {
            int n_d = m_visibleCurrentIndex + count() - currentIndex();
            if (n_d < -d) {
                d = n_d;
            }
        }
    }
    m_distanceReturn = d - m_visibleRelitivePos;
}

void AdvancedSwipeView::limitVisibleCurrentItem()
{
    auto pItem = visiblePrevItem();
    auto nItem = visibleNextItem();
    if (m_visibleRelitivePos > maxPullingOutOnEnd() && nullptr == pItem) {
        m_visibleRelitivePos = maxPullingOutOnEnd();
    }
    else if (m_visibleRelitivePos < -maxPullingOutOnEnd() && nullptr == nItem) {
        m_visibleRelitivePos = -maxPullingOutOnEnd();
    }
}

void AdvancedSwipeView::checkOutOfThreshold()
{
    if (m_visibleRelitivePos > thresholdSwitch() && nullptr != visiblePrevItem()) {
        m_visibleCurrentIndex = computePrevIndex(m_visibleCurrentIndex);
        m_visibleRelitivePos += -1.0f;
    }
    else if (m_visibleRelitivePos < -thresholdSwitch() && nullptr != visibleNextItem()) {
        m_visibleCurrentIndex = computeNextIndex(m_visibleCurrentIndex);
        m_visibleRelitivePos += 1.0f;
    }
}

void AdvancedSwipeView::showVisibleCurrentItem()
{
    auto cItem = visibleCurrentItem();
    auto pItem = visiblePrevItem();
    auto nItem = visibleNextItem();
    qreal length = (this->*m_getterLength)();

    cItem->setVisible(true);
    (cItem->*m_setterPos)(m_visibleRelitivePos * length);

    if (m_visibleRelitivePos < 0.0f) { // show next and hide prev, if they exists
        if (nullptr != nItem) {
            nItem->setVisible(true);
            (nItem->*m_setterPos)((m_visibleRelitivePos + 1.0f) * length);
        }
        if (nullptr != pItem && pItem != nItem) {
            pItem->setVisible(false);
        }
    }
    else if (m_visibleRelitivePos > 0.0f) { // show prev and hide next, if they exists
        if (nullptr != pItem) {
            pItem->setVisible(true);
            (pItem->*m_setterPos)((m_visibleRelitivePos - 1.0f) * length);
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
    qreal length = (this->*m_getterLength)();
    qreal offsetAxis = 0;

    // chose parameters by orientation
    if (orientation() == Qt::Horizontal) {
        offsetAxis = offset.x();
    }
    else if (orientation() == Qt::Vertical) {
        offsetAxis = offset.y();
    }

    // limit offset
    if (offsetAxis > length) {
        offsetAxis = length;
    }
    else if (offsetAxis < -length) {
        offsetAxis = -length;
    }
    m_visibleRelitivePos += offsetAxis / length;

    // show current item
    checkOutOfThreshold();
    limitVisibleCurrentItem();
    showVisibleCurrentItem();
}

void AdvancedSwipeView::runReturn()
{
    computeDistanceReturn();
    if (!qFuzzyIsNull(m_distanceReturn)) {
        m_speedReturn = m_distanceReturn / m_durationSnap;
        m_timer.start();
    }
}
