#include "worm.h"
#include <cstdlib>
#include "brain.h"

/*
 Name of positions for WormList table
*/
const char *PositionNames[MIX_COLOUR + 1] =
{
	"Ground", // Background colour
	"Food",   // Food colour
	"Worm",   // Basic worm colour
	"","","","","","","", // Unused
	"Leader", // Leader
	"Second", "Third", "Fourth", "Fifth",
	"Sixth", "Seventh", "Eigth", "Nineth", "Tenth",
	"Child1", "Child2", "Child3", "Child4", "Child5", // Random children of positions 1 to 10
	"Child6", "Child7", "Child8", "Child9", "Child10",
	"Average", "Mix"  // Average and mix children of the leader
};

/**
  Colours used for the above positions
*/
QRgb PositionColours[MIX_COLOUR + 1] = 
{
	qRgb(0,0,0),   // Ground - black
	qRgb(0,255,0), // Food   - green
    qRgb(240, 248, 255), // Basic worm - aliceblue
	0,0,0,0,0,0,0,       // Unused

    qRgb(65, 105, 225),  // Leader - royalblue
	qRgb(255, 215, 0),   // Second - gold
	qRgb(0, 0, 255),     // Third - blue
	qRgb(0, 255, 255),   // Forth - cyan
	qRgb(255, 255, 0),   // Fifth - yellow
	qRgb(255, 0, 255),   // Sixth - magenta
	qRgb(220, 20, 60),   // Seventh - crimson
	qRgb(255, 165, 0),   // Eigth - orange
	qRgb(250, 128, 114), // Nineth - salmon
	qRgb(238, 130, 238), // Tenth - violet

	qRgb(30, 144, 255),  // Leader random child -  dodgerblue
	qRgb(218, 165, 32),  // Child2 - goldenrod
	qRgb(0, 0, 205),     // Child3 - mediumblue
	qRgb(72, 209, 204),  // Child4 - mediumturquoise				  
	qRgb(240, 230, 140), // Child5 - khaki
	qRgb(218, 112, 214), // Child6 - orchid
	qRgb(178, 34, 34),   // Child7 - firebrick
	qRgb(244, 164, 96),  // Child8 - sandybrown
	qRgb(233, 150, 122), // Child9 - darksalmon
	qRgb(148, 0, 211),   // Child1- - darkviolet

	qRgb(0, 191, 255),  //  Leeader average child - deepskyblue
	qRgb(100, 149, 237) // Leader mix child - cornflowerblue
};


Worm::Worm(WormTank *tank, const char *brainName, const QPoint &start, int dir) :
    m_tank(tank),
    m_colour(WORM_COLOUR),
    m_dir(dir), m_energy(1000)
{
    m_brain = BrainFactory::create(this, brainName);
    m_body.push_front(start);
}

Worm::Worm(WormTank *tank) :
    m_tank(tank),
    m_colour(WORM_COLOUR),
    m_brain(nullptr),
    m_dir(0),
    m_energy(0)
{
}

Worm::Worm(const Worm &other) :
	m_tank(other.m_tank),
    m_colour(other.m_colour),
	m_dir(other.m_dir),
	m_energy(other.m_energy)
{
    m_brain = other.m_brain->clone(this);
	for (auto pt : other.m_body)
	{
		m_body.push_back(pt);
	}
}

Worm::~Worm()
{
    delete m_brain;
}

/**
 * @brief Save worm to data stream
 * @param out data stream to save to
 */
void Worm::write(QDataStream &out)
{
    out << m_colour;
    out << m_dir;
    out << m_energy;
    out << (quint32)m_body.size();
    for (auto pt : m_body)
    {
        out << pt;
    }
    out << QString(m_brain->name().c_str());
    m_brain->write(out);
}

/**
 * @brief Load worm from data stream
 * @param in data stream to load from
 */
void Worm::read(QDataStream &in)
{
    in >> m_colour;
    in >> m_dir;
    in >> m_energy;
    int bodySize;
    in >> bodySize;
    m_body.clear();
    for (int j = 0; j < bodySize; j++)
    {
        QPoint pt;
        in >> pt;
        m_body.push_back(pt);
    }
    QString brainName;
    in >> brainName;
    delete [] m_brain;
    m_brain = nullptr;
    m_brain = BrainFactory::create(this, brainName.toStdString().c_str());
    m_brain->read(in);
}

void Worm::clearTail()
{
	while (m_body.size() > 1) m_body.pop_back();
}

void Worm::resetEnergy()
{
	m_energy = 1000;
}

void Worm::resetPosition(QPoint pt, int dir)
{
	m_body.clear();
	m_body.push_back(pt);
	m_dir = dir;
}

/**
 * @brief Plan the next move for the worm
 * @param pt the next point to go to
 * @param dir the direction taken to get to this point
 */
void Worm::planMove(QPoint &pt, int &dir)
{
    dir = m_dir;
    pt = m_body.front();

    m_brain->planMove(pt, dir);
}

void Worm::OKtoMove(QPoint pt, int dir)
{
    m_body.push_front(pt);
    m_dir = dir;
}

void Worm::feed(uint amount)
{
	m_energy += amount;
}

void Worm::useEnergy()
{
	if (m_energy > 0) m_energy--;
}

bool Worm::getPointToErase(QPoint &pt)
{
    unsigned int bodyLength = m_energy / 100 + 2;
	if (m_body.size() > bodyLength)
	{
		pt = m_body.back();
        m_body.pop_back();
		return true;
	}
	return false;
}

Worm *Worm::randomChild(int chance, int weight)
{
	Worm *child = new Worm(*this);
	child->m_brain->randomise(chance, weight);
    if (m_colour >= LEADER_COLOUR && m_colour <= LEADER_COLOUR+10)
        child->m_colour = m_colour+CHILD_OFFSET;
	return child;
}

Worm *Worm::averageChild(Worm *mate, int chance, int weight)
{
    Worm *child = new Worm(*this);
    child->m_brain->average(mate->m_brain);
    if (chance > 0 && weight > 0) child->m_brain->randomise(chance, weight);
    child->m_colour = AVERAGE_COLOUR;
    return child;
}

Worm *Worm::mixChild(Worm *mate, int chance, int weight)
{
    Worm *child = new Worm(*this);
    child->m_brain->mix(mate->m_brain);
    if (chance > 0 && weight > 0) child->m_brain->randomise(chance, weight);
    child->m_colour = MIX_COLOUR;
    return child;
}
