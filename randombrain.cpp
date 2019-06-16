#include "randombrain.h"
#include <cstdlib>

// Register the Random Brain type
BrainCreator<RandomBrain> g_randomBrainCreator(
     "Random",
     "A completely random direction is chosen for every move"
     );

RandomBrain::RandomBrain(Worm *worm) : Brain(worm)
{
}

const std::string &RandomBrain::name() const
{
    return g_randomBrainCreator.name();
}


void RandomBrain::write(QDataStream &out)
{
	Q_UNUSED(out)
	// Nothing to save
}

void RandomBrain::read(QDataStream &in)
{
	Q_UNUSED(in)
	// Nothing to load
}

/**
 * @brief Pick a random direction to move in
 * @param pt location after move
 * @param dir direction moved in
 */
void RandomBrain::planMove(QPoint &pt, int &dir)
{
    dir = (dir + (std::rand() & 3) + 3) % 4;
    switch (dir)
    {
    case 0: pt.ry()--; break;
    case 1: pt.rx()++; break;
    case 2: pt.ry()++; break;
    case 3: pt.rx()--; break;
    }
}


/**
* @brief Make copy of the brain
*/
Brain *RandomBrain::clone(Worm *worm)
{
    return new RandomBrain(worm);
}

void RandomBrain::average(Brain *other)
{
    Q_UNUSED(other)
    // Random can't change
}

void RandomBrain::mix(Brain *other)
{
    Q_UNUSED(other)
    // Random can't change
}

/**
* @brief Randomise the brain
* @param chance percentage chance that each element of the brain will change
* @param weight the amount of change
*/
void RandomBrain::randomise(int chance, int weight)
{
	Q_UNUSED(chance);
	Q_UNUSED(weight);
	// Nothing we can change as this brain is completey random
}
