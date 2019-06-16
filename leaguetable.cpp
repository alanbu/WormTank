#include "leaguetable.h"

LeagueTable::LeagueTable()
{

}

/**
 * @brief Free league line memory
 */
LeagueTable::~LeagueTable()
{
    for (auto line : m_lines)
    {
        delete line;
    }
}

/**
 * @brief Add a new team to the league table
 * @param id   unique team id
 * @param name name fo the team
 */
void LeagueTable::addTeam(int id, QString name)
{
    beginInsertRows(QModelIndex(), m_lines.size(), m_lines.size());
    m_lines.push_back(new LeagueLine(id, name));
    endInsertRows();
}

/**
 * @brief Clear the league table
 */
void LeagueTable::clear()
{
    beginResetModel();
    m_lines.clear();
    endResetModel();
}

/**
 * @brief Add the result of a match
 * @param match result
 */
void LeagueTable::addResult(Match match)
{
	addResult(match.team1(), match.score1(), match.score2());
    addResult(match.team2(), match.score2(), match.score1());
}

/**
 * @brief Get name of team at top of the table
 * @return name of team
 */
QString LeagueTable::topTeamName() const
{
    return m_lines[0]->teamName();
}

void LeagueTable::addResult(int team, int scoreFor, int scoreAgainst)
{
	uint j = 0;
    while (team != m_lines[j]->id()) j++;
    m_lines[j]->incPlayed();
    m_lines[j]->addScoreFor(scoreFor);
    m_lines[j]->addScoreAgainst(scoreAgainst);
	if (scoreFor < scoreAgainst)
	{
        m_lines[j]->incLost();
	}
	else if (scoreFor == scoreAgainst)
	{
        m_lines[j]->incDrew();
	}
	else
	{
        m_lines[j]->incWon();
	}
    uint k = 0;
    while (k < m_lines.size()
           && (m_lines[j]->points() < m_lines[k]->points()
            || (m_lines[j]->points() == m_lines[k]->points()
                && m_lines[j]->scoreDifference() < m_lines[k]->scoreDifference())
			)
        ) k++;

    QVector<int> rolesChanged;
    for (int role = PlayedRole; role <= PointsRole; role++)
    {
        rolesChanged.push_back(role);
    }

    if (j == k)
	{
        // Position didn't change, just the values
        QModelIndex index = createIndex(j,0);
        emit dataChanged(index, index, rolesChanged);
    } else if (j == k + 1 || (j == k - 1 && k < m_lines.size()))
    {
        // Can just swap data if lines next to each other
        LeagueLine *line = m_lines[j];
        m_lines[j] = m_lines[k];
        m_lines[k] = line;
        rolesChanged.push_front(TeamNameRole);
        QModelIndex index = createIndex(j,0);
        emit dataChanged(index, index, rolesChanged);
        index = createIndex(k,0);
        emit dataChanged(index, index, rolesChanged);
    } else
    {
        // Need to delete it and move it to the new spot
        LeagueLine *line = m_lines[j];
        beginRemoveRows(QModelIndex(), j, j);
        m_lines.erase(m_lines.begin()+j);
        endRemoveRows();
        if (j < k) k--;
        beginInsertRows(QModelIndex(), k, k);
        m_lines.insert(m_lines.begin()+k, line);
        endInsertRows();
    }
}


/**
 * @brief QAbstractTableModel row count
 * @return number of rows
 */
int LeagueTable::rowCount(const QModelIndex & /* parent */) const
{
    return m_lines.size();
}

/**
 * @brief QAbstractTableModel column Count
 * @return number of columns
 */
int LeagueTable::columnCount(const QModelIndex &/* parent = QModelIndex()*/) const
{
    return 9;
}

/**
 * @brief Role names for QML
 * @return
 */
QHash<int, QByteArray> LeagueTable::roleNames() const
{
    QHash<int, QByteArray> roles = QAbstractTableModel::roleNames();
    roles.insert(TeamNameRole, QByteArray("teamName"));
    roles.insert(PlayedRole, QByteArray("played"));
    roles.insert(WonRole, QByteArray("won"));
    roles.insert(DrewRole, QByteArray("drew"));
    roles.insert(LostRole, QByteArray("lost"));
    roles.insert(ScoreForRole, QByteArray("scoreFor"));
    roles.insert(ScoreAgainstRole, QByteArray("scoreAgainst"));
    roles.insert(ScoreDifferenceRole, QByteArray("scoreDifference"));
    roles.insert(PointsRole, QByteArray("points"));
    return roles;
}

/**
 * @brief Get data for an index
 * @param index index to fetch data for
 * @param role type of data
 * @return data
 */
QVariant LeagueTable::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    int col = -1;
    if (role == Qt::DisplayRole)
    {
        col = index.column() + Qt::UserRole;
    } else if (role >= Qt::UserRole && role <= Qt::UserRole+8)
    {
        col = role;
    }
    if (col != -1)
    {
        switch(Roles(col))
        {
        case TeamNameRole: return m_lines[index.row()]->teamName(); break;
        case PlayedRole: return m_lines[index.row()]->played(); break;
        case WonRole: return m_lines[index.row()]->won(); break;
        case DrewRole: return m_lines[index.row()]->drew(); break;
        case LostRole: return m_lines[index.row()]->lost(); break;
        case ScoreForRole: return m_lines[index.row()]->scoreFor(); break;
        case ScoreAgainstRole: return m_lines[index.row()]->scoreAgainst(); break;
        case ScoreDifferenceRole: return m_lines[index.row()]->scoreDifference(); break;
        case PointsRole: return m_lines[index.row()]->points(); break;
        }
    }
    return QVariant();
}

/**
 * @brief QAbstractTableMode Header data
 * @param section       header row or column
 * @param orientation   if its row or column headers
 * @param role          role
 * @return header information
 */
QVariant LeagueTable::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
            return QVariant();
    if (orientation == Qt::Horizontal)
    {
        switch(Roles(section))
        {
        case TeamNameRole: return "Team"; break;
        case PlayedRole: return "Played"; break;
        case WonRole: return "Won"; break;
        case DrewRole: return "Drew"; break;
        case LostRole: return "Lost"; break;
        case ScoreForRole: return "Score For"; break;
        case ScoreAgainstRole: return "Score Against"; break;
        case ScoreDifferenceRole: return "Score Difference"; break;
        case PointsRole: return "Points"; break;
        }
    } else
    {
        return section+1;
    }

    return QVariant();
}
