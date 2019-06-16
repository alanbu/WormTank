#include "beambrain.h"
#include "worm.h"
#include "wormtank.h"

// Register the beam brain creator
BrainCreator<BeamBrain> g_beamBrainCreator(
        "Beam",
        "Sends out beams in three directions to detect nearest of food, worm or wall"
        );

/**
 * @brief Return the name of this brain type
 * @return name of brain type
 */
const std::string &BeamBrain::name() const
{
    return g_beamBrainCreator.name();
}

/**
 * @brief Clone a copy of this SimpleBrain
 * @param worm worm to attach clone to
 * @return new Brain copy
 */
Brain *BeamBrain::clone(Worm *worm)
{
    return new BeamBrain(*this, worm);
}

/**
 * @brief Fill the inputs with the food seen sensor data
 * @param pt worm head position
 * @param dir directin worm is facing
 */
void BeamBrain::fillInputs(QPoint pt, int dir)
{
    m_worm->tank()->foodBeam(pt, dir, m_inputs);
    m_worm->tank()->wormBeam(pt, dir, m_inputs+4);
    m_worm->tank()->wallBeam(pt, dir, m_inputs+8);
}
