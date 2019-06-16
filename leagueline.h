#ifndef LEAGUELINE_H
#define LEAGUELINE_H

struct LeagueLine
{
public:
    LeagueLine(int id, QString teamName) :
        m_id(id),
        m_teamName(teamName),
        m_played(0),
        m_won(0),
        m_drew(0),
        m_lost(0),
        m_scoreFor(0),
        m_scoreAgainst(0)
    {
    }

    int id() const {return m_id;}
	QString teamName() const { return m_teamName; }
	int played() const { return m_played; }
	int won() const { return m_won; }
	int drew() const { return m_drew; }
	int lost() const { return m_lost; }
	int scoreFor() const { return m_scoreFor; }
	int scoreAgainst() const { return m_scoreAgainst; }
	int scoreDifference() const { return m_scoreFor - m_scoreAgainst; }
    int points() const {return m_won * 2 + m_drew;}

    void incPlayed() { m_played++;}
    void incWon() {m_won++;}
    void incDrew() { m_drew++;}
    void incLost() { m_lost++;}
    void addScoreFor(int score) {m_scoreFor += score;}
    void addScoreAgainst(int score) {m_scoreAgainst += score;}

private:
	int m_id;
	QString m_teamName;
	int m_played;
	int m_won;
	int m_drew;
	int m_lost;
	int m_scoreFor;
	int m_scoreAgainst;
};

#endif // LEAGUELINE_H
