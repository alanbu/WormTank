#include "genlist.h"
#include "brain.h"
#include "wormtank.h"

GenList::GenList(QObject *parent) : QAbstractListModel(parent)
{

}

/**
 * @brief Set up the list
 * @param wormTank wormTank to read latest generation from
 */
void GenList::setup(WormTank *wormTank)
{
    for (auto creator : BrainFactory::instance()->creators())
    {
        QString name(creator.first.c_str());
        m_items.push_back(GenListItem(name, wormTank->latestGeneration(name)));
    }
}

/**
 * @brief Update   generaton for a specific worm type
 * @param wormType worm type to update
 * @param newGen   new generation count
 */
void GenList::updateGeneration(QString wormType, uint newGen)
{
    for (uint j = 0; j < m_items.size(); j++)
    {
        if (wormType == m_items[j].wormType())
        {
            m_items[j].setGeneration(newGen);
            QModelIndex index = createIndex(j,0);
            QVector<int> roles;
            roles.push_back(GenerationRole);
            emit dataChanged(index, index, roles);
            return;
        }
    }
}


/**
 * @brief Return role names for QML
 * @return role names with has to id
 */
QHash<int, QByteArray> GenList::roleNames() const
{
    QHash<int, QByteArray> roles = QAbstractListModel::roleNames();
    roles.insert(WormTypeRole, QByteArray("wormType"));
    roles.insert(GenerationRole, QByteArray("generation"));

    return roles;
}

/**
 * @brief return the data for the role
 * @param index row index
 * @param role role to return data for
 * @return variant with correct data
 */
QVariant GenList::data(const QModelIndex & index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role >= WormTypeRole && role <= GenerationRole)
    {
        switch(Roles(role))
        {
        case WormTypeRole: return m_items[index.row()].wormType(); break;
        case GenerationRole: return m_items[index.row()].generation(); break;
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
QVariant GenList::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
            return QVariant();
    if (orientation == Qt::Horizontal)
    {
        switch(Roles(section))
        {
        case WormTypeRole: return "Worm Type"; break;
        case GenerationRole: return "Generation"; break;
        }
    } else
    {
        return section+1;
    }

    return QVariant();
}
