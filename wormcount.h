#ifndef WORMCOUNT_H
#define WORMCOUNT_H

#include <QObject>
#include <QColor>
#include "worm.h"

class WormCount : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name NOTIFY nameChanged)
    Q_PROPERTY(QString colourName READ colourName NOTIFY colourNameChanged)
    Q_PROPERTY(QColor colour READ colour NOTIFY colourChanged)
    Q_PROPERTY(uint count READ count NOTIFY countChanged)

public:
    explicit WormCount(QString name, int colourIndex, QObject *parent = 0) :
        QObject(parent),
        m_name(name),
        m_colourIndex(colourIndex),
        m_count(1)
    {

    }

    QString name() const {return m_name;}
    QString colourName() const { return PositionNames[m_colourIndex]; }
    QColor colour() const {return PositionColours[m_colourIndex];}
    int colourIndex() const {return m_colourIndex;}
    uint count() const {return m_count;}

    void setName(QString name)
    {
        if (name != m_name)
        {
            m_name = name;
            emit nameChanged(m_name);
        }
    }

    void setColourIndex(int colourIndex)
    {
        if (colourIndex != m_colourIndex)
        {
            m_colourIndex = colourIndex;
            emit colourChanged(colour());
			emit colourNameChanged(colourName());
        }
    }

    void setCount(uint count)
    {
        if (count != m_count)
        {
            m_count = count;
            emit countChanged(m_count);
        }
    }

    void incCount()
    {
        m_count++;
        emit countChanged(m_count);
    }

signals:
    void nameChanged(QString name);
    void countChanged(uint changed);
    void colourChanged(QColor colour);
    void colourNameChanged(QString colourName);

public slots:

private:
    QString m_name;
    int m_colourIndex;
    uint m_count;
};

#endif // WORMCOUNT_H
