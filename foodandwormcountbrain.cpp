#include "foodandwormcountbrain.h"

#include "wormtank.h"
#include "worm.h"

// Register the brain create
BrainCreator<FoodAndWormCountBrain> g_foodAndWormCountBrainCreator(
        "FoodAndWormCount",
        "Detect the amount of food and worms at ranges 1, 10, 100, infinity and if it's next to a wall"
        );

/**
 * @brief Return the name of this brain type
 * @return name of brain type
 */
const std::string &FoodAndWormCountBrain::name() const
{
    return g_foodAndWormCountBrainCreator.name();
}

/**
 * @brief Clone a copy of this SimpleBrain
 * @param worm worm to attach clone to
 * @return new Brain copy
 */
Brain *FoodAndWormCountBrain::clone(Worm *worm)
{
    return new FoodAndWormCountBrain(*this, worm);
}

/**
 * @brief Fill the inputs with the food seen sensor data
 * @param pt worm head position
 * @param dir directin worm is facing
 */
void FoodAndWormCountBrain::fillInputs(QPoint pt, int dir)
{
	int ranges[4] = { 1,10,100, 99999 };
    m_worm->tank()->foodCount(pt, dir, ranges, 4, m_inputs);
    m_worm->tank()->wormCount(pt, dir, ranges, 4, m_inputs + 16);
    m_worm->tank()->wallNextTo(pt, dir, m_inputs + 32);
}



