#include "foodandwormrangebrain.h"

#include "wormtank.h"
#include "worm.h"

// Register the simple brain creator
BrainCreator<FoodAndWormRangeBrain> g_foodAndWormRangeBrainCreator(
        "FoodAndWormRange",
        "Detect food and worms at ranges 1, 10, 100, infinity and if it's next to a wall"
        );

/**
 * @brief Return the name of this brain type
 * @return name of brain type
 */
const std::string &FoodAndWormRangeBrain::name() const
{
    return g_foodAndWormRangeBrainCreator.name();
}

/**
 * @brief Clone a copy of this SimpleBrain
 * @param worm worm to attach clone to
 * @return new Brain copy
 */
Brain *FoodAndWormRangeBrain::clone(Worm *worm)
{
    return new FoodAndWormRangeBrain(*this, worm);
}

/**
 * @brief Fill the inputs with the food seen sensor data
 * @param pt worm head position
 * @param dir directin worm is facing
 */
void FoodAndWormRangeBrain::fillInputs(QPoint pt, int dir)
{
	int ranges[4] = { 1,10,100, 99999 };
	m_worm->tank()->foodSeen(pt, dir, ranges, 4, m_inputs);
	m_worm->tank()->wormSeen(pt, dir, ranges, 4, m_inputs + 16);
    m_worm->tank()->wallNextTo(pt, dir, m_inputs + 32);
}



