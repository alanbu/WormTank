#ifndef HISTORY_H
#define HISTORY_H

#include <QObject>
#include <QAbstractListModel>
#include <QDataStream>
#include <vector>

/**
 * @brief One item of history
 */
class HistoryItem
{
public:
    HistoryItem(uint ticks, uint leader, uint foodLeft, uint maxEnergy) :
        m_ticks(ticks),
        m_leader(leader),
        m_foodLeft(foodLeft),
        m_maxEnergy(maxEnergy)
    {
    }

    HistoryItem() : m_ticks(0), m_leader(0), m_foodLeft(0), m_maxEnergy(0)
    {
    }

    uint ticks() const {return m_ticks;}
    uint leader() const {return m_leader;}
    uint foodLeft() const {return m_foodLeft;}
    uint maxEnergy() const {return m_maxEnergy;}

    void load(QDataStream &in)
    {
        in >> m_ticks >> m_leader >> m_foodLeft >> m_maxEnergy;
    }
    void save(QDataStream &out)
    {
        out << m_ticks << m_leader << m_foodLeft << m_maxEnergy;
    }

private:
    uint m_ticks;
    uint m_leader;
    uint m_foodLeft;
    uint m_maxEnergy;
};

/**
 * @brief Store history of last few generations
 */
class History : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit History(QObject *parent = 0);

    void add(const HistoryItem &item);
    void clear();

    uint numItems() const {return m_items.size();}
    const std::vector<HistoryItem> &items() const {return m_items;}

    void save(QDataStream &out);
    void load(QDataStream &in);

    enum Roles
    {
       TicksRole = Qt::UserRole + 0,
       LeaderRole,
       FoodLeftRole,
       MaxEnergyRole,
       LeaderNameRole,
       LeaderColourRole
    };

    // Abstract table model overrides
    int rowCount(const QModelIndex &/*parent = QModelIndex()*/) const {return (int)m_items.size();}
    QHash<int, QByteArray> roleNames() const;
    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;


signals:

public slots:

private:
    const uint MaxHistory = 10;
    std::vector<HistoryItem> m_items;
};

#endif // HISTORY_H
