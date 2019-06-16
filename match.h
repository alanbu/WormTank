#ifndef MATCH_H
#define MATCH_H

#include <QDataStream>

/**
 * @brief class for details of one match
 */
class Match
{
    uint m_match;
public:
    Match(int team1, int team2) { m_match = (team1 << 24) | (team2 << 16); }
    Match() {m_match = 0;}

    inline int team1() const { return m_match >> 24; }
    inline int team2() const { return (m_match >> 16) & 0xFF; }
    inline int score1() const {return (m_match >> 8) & 0xFF;}
    inline int score2() const { return m_match & 0xFF; }
    inline bool played() const { return (m_match & 0xFFFF) != 0; }
    inline void setScore(int team1Score, int team2Score) { m_match |= (team1Score << 8) | team2Score; }

    inline void save(QDataStream & out) { out << m_match;}
    inline void load(QDataStream &in) { in >> m_match;}
 };

inline QDataStream &operator<<(QDataStream &out, Match &match)
{
    match.save(out);
    return out;
}

inline QDataStream &operator>>(QDataStream &in, Match &match)
{
    match.load(in);
    return in;
}


#endif // MATCH_H

