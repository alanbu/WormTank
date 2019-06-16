#include "simplewithnexttobrain.h"
#include "wormtank.h"
#include "worm.h"

// Register the simple brain creator
BrainCreator<SimpleWithNextToBrain> g_simpleWithNextToBrainCreator(
        "SimpleWithNextTo",
        "Simple brain that can detect which direction food is in"
        " and if it is next to food/worms or a wall."
        );

/**
 * @brief Return the name of this brain type
 * @return name of brain type
 */
const std::string &SimpleWithNextToBrain::name() const
{
    return g_simpleWithNextToBrainCreator.name();
}

/**
 * @brief Clone a copy of this SimpleBrain
 * @param worm worm to attach clone to
 * @return new Brain copy
 */
Brain *SimpleWithNextToBrain::clone(Worm *worm)
{
    return new SimpleWithNextToBrain(*this, worm);
}

/**
 * @brief Fill the inputs with the food seen and all next to sensor data
 * @param pt worm head position
 * @param dir directin worm is facing
 */
void SimpleWithNextToBrain::fillInputs(QPoint pt, int dir)
{
    m_worm->tank()->foodSeen(pt, dir, m_inputs);
    m_worm->tank()->foodNextTo(pt, dir, m_inputs+4);
    m_worm->tank()->wormNextTo(pt, dir, m_inputs+8);
    m_worm->tank()->wallNextTo(pt, dir, m_inputs+12);
}


