#include "simplewithnexttofood.h"

#include "wormtank.h"
#include "worm.h"

// Register the simple brain creator
BrainCreator<SimpleWithNextToFoodBrain> g_simpleWithNextToFoodBrainCreator(
        "SimpleWithNextToFood",
        "Simple brain that can detect which direction food is in"
        " and if it is next to food."
        );

/**
 * @brief Return the name of this brain type
 * @return name of brain type
 */
const std::string &SimpleWithNextToFoodBrain::name() const
{
    return g_simpleWithNextToFoodBrainCreator.name();
}

/**
 * @brief Clone a copy of this SimpleBrain
 * @param worm worm to attach clone to
 * @return new Brain copy
 */
Brain *SimpleWithNextToFoodBrain::clone(Worm *worm)
{
    return new SimpleWithNextToFoodBrain(*this, worm);
}

/**
 * @brief Fill the inputs with the food seen sensor data
 * @param pt worm head position
 * @param dir directin worm is facing
 */
void SimpleWithNextToFoodBrain::fillInputs(QPoint pt, int dir)
{
    m_worm->tank()->foodSeen(pt, dir, m_inputs);
    m_worm->tank()->foodNextTo(pt, dir, m_inputs+4);
}


