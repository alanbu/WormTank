#ifndef WORM_H
#define WORM_H

#include "brain.h"

#include <QPoint>
#include <QColor>
#include <QDataStream>
#include <deque>

class WormTank;

// Background colour
const int GROUND_COLOUR = 0;
// Food colour
const int FOOD_COLOUR = 1;
// Worm colours
const int WORM_COLOUR = 2;
const int LEADER_COLOUR = 10;
const int CHILD_OFFSET = 10;
const int CHILD_COLOUR = 20;
const int AVERAGE_COLOUR = 30;
const int MIX_COLOUR = 31;

const int NUM_POSITIONS = MIX_COLOUR + 1;

// Reuse some colours for Team games
const int TEAM1_COLOUR = LEADER_COLOUR;
const int TEAM2_COLOUR = LEADER_COLOUR+1;

extern const char *PositionNames[NUM_POSITIONS];
extern QRgb PositionColours[NUM_POSITIONS];

/**
 * @brief The Worm that represents a worm
 */
class Worm
{
public:
    Worm(WormTank *tank, const char *brainName, const QPoint &start, int dir);
    Worm(WormTank *tank);
    ~Worm();

    std::string name() const {return m_brain->name();}
    std::string fullName() const {return m_brain->fullName();}

    int colour() const {return m_colour;}
    void setColour(int colour) {m_colour = colour;}

    void write(QDataStream &out);
    void read(QDataStream &in);

    WormTank *tank() {return m_tank;}

    void planMove(QPoint &pt, int &dir);
    void OKtoMove(QPoint pt, int dir);

	void clearTail();
	void resetEnergy();
	void resetPosition(QPoint pt, int dir);

    uint energy() const {return m_energy;}
	void useEnergy();
    void feed(uint amount);
	bool dead() const { return (m_energy == 0); }

    QPoint headPos() const {return m_body.front();}
	bool getPointToErase(QPoint &pt);

    const std::deque<QPoint> &body() const { return m_body; }

	Worm *randomChild(int chance, int weight);
    Worm *averageChild(Worm *mate, int chance, int weight);
    Worm *mixChild(Worm *mate, int chance, int weight);

private:
	Worm(const Worm &other);

private:
    WormTank *m_tank;
    int m_colour;
    Brain *m_brain;
	int m_dir;
    std::deque<QPoint> m_body;
    uint m_energy;
};

inline QDataStream &operator<<(QDataStream &out, Worm &worm)
{
    worm.write(out);
    return out;
}

inline QDataStream &operator>>(QDataStream &in, Worm &worm)
{
    worm.read(in);
    return in;
}

// comparisons for sorting etc
inline bool less_energy(Worm *w1, Worm *w2)
{
    return w1->energy() < w2->energy();
}

inline bool more_energy(Worm *w1, Worm *w2)
{
    return w1->energy() > w2->energy();
}


#endif // WORM_H
