#include "history.h"
#include "worm.h"

History::History(QObject *parent) : QAbstractListModel(parent)
{

}

/**
 * @brief Save history
 * @param out data stream to save to
 */
void History::save(QDataStream &out)
{
    out << (int)m_items.size();
    for(auto item : m_items)
    {
        item.save(out);
    }
}

/**
 * @brief Load history
 * @param in data stream to load from
 */
void History::load(QDataStream &in)
{
    if (!m_items.empty()) clear();
    int numItems = 0;
    in >> numItems;
    if (numItems > 0)
    {
        beginInsertRows(QModelIndex(), 0, numItems-1);
        for (int j = 0; j < numItems; j++)
        {
            HistoryItem item;
            item.load(in);
            m_items.push_back(item);
        }
        endInsertRows();
    }
}

/**
 * @brief Add a new item of history
 *
 * Removes last item if MaxHistory would be exceeded
 *
 * @param item to add
 */
void History::add(const HistoryItem &item)
{
    beginInsertRows(QModelIndex(), 0, 0);
    m_items.insert(m_items.begin(), item);
    endInsertRows();
    if (m_items.size() > MaxHistory)
    {
        beginRemoveRows(QModelIndex(), m_items.size()-1,m_items.size()-1);
        m_items.pop_back();
        endRemoveRows();
    }
}

/**
 * @brief Clear the history
 */
void History::clear()
{
    beginResetModel();
    m_items.clear();
    endResetModel();
}


/**
 * @brief Return role names for QML
 * @return role names with has to id
 */
QHash<int, QByteArray> History::roleNames() const
{
    QHash<int, QByteArray> roles = QAbstractListModel::roleNames();
    roles.insert(TicksRole, QByteArray("ticks"));
    roles.insert(LeaderRole, QByteArray("leader"));    
    roles.insert(FoodLeftRole, QByteArray("foodLeft"));
    roles.insert(MaxEnergyRole, QByteArray("maxEnergy"));
    roles.insert(LeaderNameRole, QByteArray("leaderName"));
    roles.insert(LeaderColourRole, QByteArray("leaderColour"));

    return roles;
}

/**
 * @brief return the data for the role
 * @param index row index
 * @param role role to return data for
 * @return variant with correct data
 */
QVariant History::data(const QModelIndex & index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role >= TicksRole && role <= LeaderColourRole)
    {
        switch(Roles(role))
        {
        case TicksRole: return m_items[index.row()].ticks(); break;
        case LeaderRole: return m_items[index.row()].leader(); break;
        case FoodLeftRole: return m_items[index.row()].foodLeft(); break;
        case MaxEnergyRole: return m_items[index.row()].maxEnergy(); break;
        case LeaderNameRole: return QString(PositionNames[m_items[index.row()].leader()]); break;
        case LeaderColourRole: return QColor(PositionColours[m_items[index.row()].leader()]); break;
        }
    }
    return QVariant();
}

/**
 * @brief Return headerData for a role
 * @param section row or column
 * @param orientation direction required
 * @param role role name
 * @return Header text
 */
QVariant History::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
            return QVariant();
    if (orientation == Qt::Horizontal)
    {
        switch(Roles(section))
        {
        case TicksRole: return "Ticks"; break;
        case LeaderRole: return "Leader"; break;
        case FoodLeftRole: return "Food Left"; break;
        case MaxEnergyRole: return "Max Energy"; break;
        case LeaderNameRole: return "Leader Name"; break;
        case LeaderColourRole: return "Leader Colour"; break;
        }
    } else
    {
        return section+1;
    }

    return QVariant();
}

