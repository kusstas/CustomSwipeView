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

public:
    AdvancedSwipeView(QQuickItem* parent = nullptr);

    int count() const;
    int prevIndex() const;
    int nextIndex() const;
    int currentIndex() const;
    QQuickItem* currentItem() const;

public slots:
    void setCurrentIndex(int currentIndex);

signals:

    void currentIndexChanged(int currentIndex);
    void currentItemChanged(QQuickItem* currentItem);

protected:
    void componentComplete() override;

private:
    QList<QQuickItem*> m_content;
    int m_currentIndex;

    void snapOneItem();
};

#endif // ADVANCEDSWIPEVIEW_H
