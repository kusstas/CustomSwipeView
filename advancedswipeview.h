#ifndef ADVANCEDSWIPEVIEW_H
#define ADVANCEDSWIPEVIEW_H

#include <QQuickItem>

class AdvancedSwipeView : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(int count READ count)
    Q_PROPERTY(int prevIndex READ prevIndex)
    Q_PROPERTY(int nextIndex READ nextIndex)
    Q_PROPERTY(int currentIndex READ currentIndex WRITE setCurrentIndex NOTIFY currentIndexChanged)
    Q_PROPERTY(QQuickItem* currentItem READ currentItem NOTIFY currentItemChanged)
    Q_PROPERTY(Qt::Orientation orientation READ orientation WRITE setOrientation NOTIFY orientationChanged)

public:
    AdvancedSwipeView(QQuickItem* parent = nullptr);

    int count() const;
    int prevIndex() const;
    int nextIndex() const;
    int currentIndex() const;
    QQuickItem* currentItem() const;
    Qt::Orientation orientation() const;

public slots:
    void setCurrentIndex(int currentIndex);
    void setOrientation(Qt::Orientation orientation);

signals:

    void currentIndexChanged(int currentIndex);
    void currentItemChanged(QQuickItem* currentItem);
    void orientationChanged(Qt::Orientation orientation);

protected:
    void componentComplete() override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;

private:
    QList<QQuickItem*> m_content;
    Qt::Orientation m_orientation;
    int m_currentIndex;

    void alignBefore();
    void alignAfter();
    void snapOneItemInstantly();
};

#endif // ADVANCEDSWIPEVIEW_H
