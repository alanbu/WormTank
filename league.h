#ifndef LEAGUE_H
#define LEAGUE_H

#include "leaguetable.h"

#include <vector>


/**
 * @brief class to hold all the details of the league
 */
class League : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString currentTeam1 READ currentTeam1 NOTIFY currentTeam1Changed)
    Q_PROPERTY(QString currentTeam2 READ currentTeam2 NOTIFY currentTeam2Changed)
    Q_PROPERTY(QObject *table READ table NOTIFY tableChanged)
    Q_PROPERTY(QString previousTeam1 READ previousTeam1 NOTIFY previousTeam1Changed)
    Q_PROPERTY(QString previousTeam2 READ previousTeam2 NOTIFY previousTeam2Changed)
    Q_PROPERTY(int previousTeam1Score READ previousTeam1Score NOTIFY previousTeam1ScoreChanged)
    Q_PROPERTY(int previousTeam2Score READ previousTeam2Score NOTIFY previousTeam2ScoreChanged)


public:
    explicit League(QObject *parent = 0);

	void load();
    void save();
	void newSeason();

    void updateTeamScore(int team1Score);
	void team1Victory(int generation);
	void team2Victory(int generation);
	void endOfMatch();

    QString currentTeam1() const;
    QString currentTeam2() const;
    QString topOfTableTeam() const;

    QString previousTeam1() const;
    QString previousTeam2() const;
    int previousTeam1Score() const;
    int previousTeam2Score() const;

    void getCurrentScore(int &score1, int &score2);


    QObject *table() {return &m_table;}

    bool matchFinished() const;
    bool hasNextMatch() const {return m_current < (int)m_matches.size();}
    bool nextMatch();

    QString leagueFileName() const;

signals:
    void currentTeam1Changed(QString teamName);
    void currentTeam2Changed(QString teamName);
    void tableChanged(QObject *table);
    void previousTeam1Changed(QString teamName);
    void previousTeam2Changed(QString teamName);
    void previousTeam1ScoreChanged(int score);
    void previousTeam2ScoreChanged(int score);

public slots:

private:
	std::vector<QString> m_teams;
	std::vector<Match> m_matches;
	int m_current;
	int m_team1Score;
	int m_team2Score;
	LeagueTable m_table;
};

#endif // LEAGUE_H
