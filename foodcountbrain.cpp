#include "foodcountbrain.h"

#include "wormtank.h"
#include "worm.h"

// Register the brain creator
BrainCreator<FoodCountBrain> g_foodCountBrainCreator(
        "FoodCount",
        "Count food at ranges of 1, 10, 100 and infinity and if it's next to a worm or wall"
        );

/**
 * @brief Return the name of this brain type
 * @return name of brain type
 */
const std::string &FoodCountBrain::name() const
{
    return g_foodCountBrainCreator.name();
}

/**
 * @brief Clone a copy of this FoodRangeBrain
 * @param worm worm to attach clone to
 * @return new Brain copy
 */
Brain *FoodCountBrain::clone(Worm *worm)
{
    return new FoodCountBrain(*this, worm);
}

/**
 * @brief Fill the inputs with the food seen sensor data
 * @param pt worm head position
 * @param dir directin worm is facing
 */
void FoodCountBrain::fillInputs(QPoint pt, int dir)
{
	int ranges[4] = { 1,10,100, 99999 };
    m_worm->tank()->foodCount(pt, dir, ranges, 4, m_inputs);
    m_worm->tank()->wormNextTo(pt, dir, m_inputs + 16);
    m_worm->tank()->wallNextTo(pt, dir, m_inputs + 20);
}
