#ifndef LEAGUETABLE_H
#define LEAGUETABLE_H

#include "match.h"
#include "leagueline.h"
#include <vector>
#include <QAbstractTableModel>

/**
 * @brief Class to maintain the league table
 */
class LeagueTable : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit LeagueTable();
    ~LeagueTable();

	void addTeam(int id, QString name);
	void clear();
	void addResult(Match match);

    QString topTeamName() const;

    enum Roles
    {
       TeamNameRole = Qt::UserRole + 0,
       PlayedRole,
       WonRole,
       DrewRole,
       LostRole,
       ScoreForRole,
       ScoreAgainstRole,
       ScoreDifferenceRole,
       PointsRole
    };

    // Abstract table model overrides
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QHash<int, QByteArray> roleNames() const;
    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

signals:

public slots:

protected:
	void addResult(int team, int scoreFor, int scoreAgainst);

private:
    std::vector<LeagueLine *> m_lines;

};

#endif // LEAGUETABLE_H
