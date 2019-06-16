#include "simplebrain.h"
#include "wormtank.h"
#include "worm.h"

// Register the simple brain creator
BrainCreator<SimpleBrain> g_simpleBrainCreator(
        "Simple",
        "Simple brain that can just detect which direction food is in"
        );

/**
 * @brief Return the name of this brain type
 * @return name of brain type
 */
const std::string &SimpleBrain::name() const
{
    return g_simpleBrainCreator.name();
}

/**
 * @brief Clone a copy of this SimpleBrain
 * @param worm worm to attach clone to
 * @return new Brain copy
 */
Brain *SimpleBrain::clone(Worm *worm)
{
    return new SimpleBrain(*this, worm);
}

/**
 * @brief Fill the inputs with the food seen sensor data
 * @param pt worm head position
 * @param dir directin worm is facing
 */
void SimpleBrain::fillInputs(QPoint pt, int dir)
{
    m_worm->tank()->foodSeen(pt, dir, m_inputs);
}

