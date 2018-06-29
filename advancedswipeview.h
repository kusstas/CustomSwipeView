#ifndef ADVANCEDSWIPEVIEW_H
#define ADVANCEDSWIPEVIEW_H

#include <QQuickItem>
#include <QTimer>

class AdvancedSwipeView : public QQuickItem
{
    Q_OBJECT
    // parameters of swipe
    Q_PROPERTY(Qt::Orientation orientation READ orientation WRITE setOrientation NOTIFY orientationChanged)
    Q_PROPERTY(bool loop READ loop WRITE setLoop NOTIFY loopChanged)

    // control swipe
    Q_PROPERTY(qreal thresholdSwitch READ thresholdSwitch WRITE setThresholdSwitch NOTIFY thresholdSwitchChanged)
    Q_PROPERTY(qreal minVelocitySwitch READ minVelocitySwitch WRITE setMinVelocitySwitch NOTIFY minVelocitySwitchChanged)
    Q_PROPERTY(qreal maxPullingOutOnEnd READ maxPullingOutOnEnd WRITE setMaxPullingOutOnEnd NOTIFY maxPullingOutOnEndChanged)

    // status
    Q_PROPERTY(bool seized READ seized NOTIFY seizedChanged)

    // content info
    Q_PROPERTY(int count READ count)
    Q_PROPERTY(int prevIndex READ prevIndex)
    Q_PROPERTY(int nextIndex READ nextIndex)
    Q_PROPERTY(int currentIndex READ currentIndex WRITE setCurrentIndex NOTIFY currentIndexChanged)
    Q_PROPERTY(QQuickItem* prevItem READ prevItem)
    Q_PROPERTY(QQuickItem* nextItem READ nextItem)
    Q_PROPERTY(QQuickItem* currentItem READ currentItem)

public:
    AdvancedSwipeView(QQuickItem* parent = nullptr);

    Qt::Orientation orientation() const;
    bool loop() const;

    qreal thresholdSwitch() const;
    qreal minVelocitySwitch() const;
    qreal maxPullingOutOnEnd() const;

    bool seized() const;
    int count() const;
    int prevIndex() const;
    int nextIndex() const;
    int currentIndex() const;
    QQuickItem* currentItem() const;
    QQuickItem* prevItem() const;
    QQuickItem* nextItem() const;

public slots:
    void setOrientation(Qt::Orientation orientation);
    void setLoop(bool loop);
    void setThresholdSwitch(qreal thresholdSwitch);
    void setMinVelocitySwitch(qreal minVelocitySwitch);
    void setMaxPullingOutOnEnd(qreal maxPullingOutOnEnd);
    void setCurrentIndex(int currentIndex);

private slots:
    void onWidthChanged();
    void onHeightChanged();
    void onTimerOut();

signals:
    void orientationChanged(Qt::Orientation orientation);
    void loopChanged(bool loop);
    void thresholdSwitchChanged(qreal thresholdSwitch);
    void maxPullingOutOnEndChanged(qreal maxPullingOutOnEnd);
    void minVelocitySwitchChanged(qreal minVelocitySwitch);
    void seizedChanged(bool seized);
    void currentIndexChanged(int currentIndex);

protected:
    void componentComplete() override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void setSeized(bool seized);

private:
    // paramaters of swipe
    Qt::Orientation m_orientation;
    bool m_loop;

    // control swipe
    int m_visibleCurrentIndex;
    qreal m_visibleRelitivePos;
    qreal m_thresholdSwitch;
    qreal m_minVelocitySwitch;
    qreal m_maxPullingOutOnEnd;
    bool m_seized;
    QTimer m_timer;

    // content info
    QList<QQuickItem*> m_content;
    int m_currentIndex;

    // mouse variables
    QPointF m_posPrevMouse;
    QPointF m_midleVelMouse;
    QPointF m_velMouse;

    // custom types
    using SetterPos = void (QQuickItem::*) (qreal);
    using GetterPos = qreal (QQuickItem::*) () const;

    // helper functions
    int shiftIndex(int index, int shift) const;
    bool indexValid(int index) const;
    int computePrevIndex(int index) const;
    int computeNextIndex(int index) const;
    QQuickItem* computePrevItem(int index) const;
    QQuickItem* computeNextItem(int index) const;

    QQuickItem* visibleCurrentItem() const;
    QQuickItem* visiblePrevItem() const;
    QQuickItem* visibleNextItem() const;

    void limitVisibleCurrentItem(qreal length, SetterPos setterPos);
    void showVisibleCurrentItem(qreal length, SetterPos setterPos, GetterPos getterPos);
    void shiftVisibleContent(QPointF const& offset);
};

#endif // ADVANCEDSWIPEVIEW_H
