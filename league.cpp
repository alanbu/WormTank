#include "league.h"
#include "brain.h"
#include <QFile>
#include <QDir>
#include <QDataStream>
#include <QStandardPaths>
#include <algorithm>

League::League(QObject *parent) : QObject(parent)
{

}

/**
 * @brief Return name of first team
 * @return name or "" if none
 */
QString League::currentTeam1() const
{
    return (m_current < (int)m_matches.size()) ? m_teams[m_matches[m_current].team1()] : QString("");
}

/**
 * @brief Return name of second team
 * @return name or "" if none
 */
QString League::currentTeam2() const
{
    return (m_current < (int)m_matches.size()) ? m_teams[m_matches[m_current].team2()] : QString("");
}

/**
 * @brief Return the current top of the League Table
 * @return
 */
QString League::topOfTableTeam() const
{
    return m_table.topTeamName();
}

/**
 * @brief Team 1 in previous game
 * @return team 1 name from previous game or "" it at beginning of the league
 */
QString League::previousTeam1() const
{
    return (m_current > 0) ? m_teams[m_matches[m_current-1].team1()] : QString("");
}

/**
 * @brief Team 2 in previous game
 * @return team 2 name from previous game or "" it at beginning of the league
 */
QString League::previousTeam2() const
{
    return (m_current > 0) ? m_teams[m_matches[m_current-1].team2()] : QString("");
}

/**
 * @brief Team 1 score in previous game
 * @return team 1 score from previous game or 0 if at beginning of the league
 */
int League::previousTeam1Score() const
{
    return (m_current > 0) ? m_matches[m_current-1].score1() : 0;
}

/**
 * @brief Team 2 score in previous game
 * @return team 2 score from previous game or 0 if at beginning of the league
 */
int League::previousTeam2Score() const
{
    return (m_current > 0) ? m_matches[m_current-1].score2() : 0;
}

/**
 * @brief Check if the last match has finished
 *
 * This call is only valid after the first generation of a match has been
 * scored. It will also return true after the match has started, but not
 * yet been scored.
 *
 * @return true if the last match has finished
 */
bool League::matchFinished() const
{
    if (m_current >= (int)m_matches.size()) return true;
    return (m_team1Score == 0 || m_team2Score == 0);
}

/**
 * @brief Get result of current game
 * @param score1 team1 score
 * @param score2 team2 score
 */
void League::getCurrentScore(int &score1, int &score2)
{
    if (m_team1Score == 0 && m_team2Score == 0)
    {
        if (m_matches[m_current].played())
        {
            score1 = m_matches[m_current].score1();
            score2 = m_matches[m_current].score2();
        } else if (m_current > 0)
        {
            score1 = m_matches[m_current-1].score1();
            score2 = m_matches[m_current-1].score2();
        } else
        {
            score1 = score2 = 0;
        }
    } else
    {
        score1 = m_team1Score;
        score2 = m_team2Score;
    }
}

/**
 * @brief Advance to next match in the league
 * @return true if there is another match or false if league finished
 */
bool League::nextMatch()
{
    if (m_current < (int)m_matches.size())
    {
        m_current++;
        emit previousTeam1Changed(previousTeam1());
        emit previousTeam2Changed(previousTeam2());
        emit previousTeam1ScoreChanged(previousTeam1Score());
        emit previousTeam2ScoreChanged(previousTeam2Score());
    }

    if (m_current < (int)m_matches.size())
    {
        emit currentTeam1Changed(currentTeam1());
        emit currentTeam2Changed(currentTeam2());
        return true;
    }
    return false;
}

/**
  * @brief Load the current league or create a new one if it hasn't been saved
  */
void League::load()
{
	 QFile leagueFile(leagueFileName());
     if (leagueFile.exists() && leagueFile.open(QIODevice::ReadOnly))
	 {
         QDataStream in(&leagueFile);
		 int fileType, fileVersion;
		 in >> fileType >> fileVersion;
		 int numTeams;
		 in >> numTeams;
		 for (int j = 0; j < numTeams; j++)
		 {
			 QString name;
			 in >> name;
			 m_teams.push_back(name);
			 m_table.addTeam(j, name);
		 }
		 int numMatches;
		 in >> numMatches;
		 in >> m_team1Score;
		 in >> m_team2Score;
		 m_current = -1;
         for (int j = 0; j < numMatches; j++)
		 {
			 Match match;
			 in >> match;
			 if (m_current == -1 && !match.played()) m_current = j;
             if (m_current == -1) m_table.addResult(match);
			 m_matches.push_back(match);
		 }
         // No games found to play so at end of season
         if (m_current == -1) m_current = numMatches;
	 }
	 else
	 {
		 newSeason();
	 }
 }

/**
  * @brief Save the current league
  */
void League::save()
{
    QString fileName = leagueFileName();
     QDir saveDir(fileName);
     saveDir.cdUp();
     saveDir.mkpath(".");
     QFile leagueFile(fileName);

     if (leagueFile.open(QIODevice::WriteOnly))
     {
         QDataStream out(&leagueFile);
         int fileType = 100, fileVersion = 1;
         out << fileType << fileVersion;
         out << (int)m_teams.size();
         for (auto name : m_teams)
         {
             out << name;
         }
         out << (int)m_matches.size();
         out << m_team1Score;
         out << m_team2Score;
         for (auto match : m_matches)
         {
             out << match;
         }
     }
 }

 /**
 * @brief get path to file to load/save league details
 * @return full path to league file
 */
QString League::leagueFileName() const
{
    QDir latestDir(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation));
    return latestDir.absoluteFilePath(QLatin1String("league"));
}


/**
  * @brief Start a new league season
  *
  * This resets all the teams and results
  */
 void League::newSeason()
 {
	 m_teams.clear();
	 m_matches.clear();
	 m_table.clear();
     m_current = -1;
     m_team1Score = 0;
     m_team2Score = 0;
	 int numTeams = 0;
     for (auto creator : BrainFactory::instance()->creators())
     {
         QString wormName(creator.first.c_str());
		 m_teams.push_back(wormName);
		 m_table.addTeam(numTeams++, wormName);
	 }

	 for (int j = 0; j < numTeams; j++)
	 {
		 for (int k = j + 1; k < numTeams; k++)
		 {
			 m_matches.push_back(Match(j, k));
			 m_matches.push_back(Match(k, j));
		 }
	 }
     std::random_shuffle(m_matches.begin(), m_matches.end());
 }

 /**
  * @brief update the score in the current game
  * @param score team1Score
  */
 void League::updateTeamScore(int team1Score)
 {
     m_team1Score += team1Score;
     m_team2Score += 10 - team1Score;
 }

 /**
  * @brief Called if team 1 wins
  * @param generation generation of the victory
  */
 void League::team1Victory(int generation)
 {
     m_team1Score += (4 - generation) * 10;
	 endOfMatch();
 }

 /**
  * @brief Called if team 2 wins
  * @param generation generation of the victory
  */
 void League::team2Victory(int generation)
 {
     m_team2Score += (4 - generation) * 10;
	 endOfMatch();
 }

 /**
  * @brief Current match has ended
  *
  * Match ends when one or the other teams have all the worms in the
  * top ten or 4 generations have passed.
  */
 void League::endOfMatch()
 {
     m_matches[m_current].setScore(m_team1Score, m_team2Score);
     m_table.addResult(m_matches[m_current]);
     m_team1Score = 0;
     m_team2Score = 0;     
 }
