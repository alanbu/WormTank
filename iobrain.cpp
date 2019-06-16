#include "iobrain.h"
#include "random.h"
#include "direction.h"

/**
 * @brief Create the IOBrain base class
 *
 * @param worm Worm that contains the brain
 * @param numInputs number of inputs in the brain
 */
IOBrain::IOBrain(Worm *worm, uint numInputs) :
    Brain(worm),
    m_numInputs(numInputs),
    m_inputs(nullptr)
{
    m_inputs = new InputBrainCell[numInputs];
    // Initialise the brain with random weights
    for (int j = 0; j < 2; j++)
    {
        for (uint k = 0; k < m_numInputs; k++)
        {
            m_outputs[j].addLink(&m_inputs[k], randBetween(-5, 5));
        }
    }
}

/**
 * @brief Create brain as copy of another but attach it to another worm
 * @param other brain to copy
 * @param worm worm to attach it
 */
IOBrain::IOBrain(const IOBrain &other, Worm *worm) :
    Brain(worm),
    m_numInputs(other.m_numInputs),
    m_inputs(nullptr)
{
    m_inputs = new InputBrainCell[m_numInputs];
    // Initialise the brain with random weights
    for (int j = 0; j < 2; j++)
    {
        for (uint k = 0; k < m_numInputs; k++)
        {
            m_outputs[j].addLink(&m_inputs[k], other.m_outputs[j].weight(k));
        }
    }
}

IOBrain::~IOBrain()
{
    delete [] m_inputs;
}

void IOBrain::write(QDataStream &out)
{
    for (int j = 0; j < 2; j++) m_outputs[j].writeWeights(out);
}

void IOBrain::read(QDataStream &in)
{
    for (int j = 0; j < 2; j++) m_outputs[j].readWeights(in);
}


/**
 * @brief Plan the move
 *
 * Clears the inputs and calls the sub class fillInputs to set the
 * initial input, then gets the output values and set the move/direction
 * accordingly
 *
 * @param pt point of worms head
 * @param dir direction worm is travelling in
 */
void IOBrain::planMove(QPoint &pt, int &dir)
{
    fillInputs(pt, dir);

    bool left = (m_outputs[0].value() > 0);
    bool right = (m_outputs[1].value() > 0);
    if (left && !right)	dir = turnLeft(dir);
    if (right && !left) dir = turnRight(dir);
    movePoint(pt, dir);
}

void IOBrain::average(Brain *other)
{
    IOBrain *otherBrain = dynamic_cast<IOBrain *>(other);
    if (otherBrain == nullptr) return; // Can only work with another SimpleBrain

    for (int j = 0; j < 2; j++)
    {
       m_outputs[j].average(otherBrain->m_outputs[j]);
    }
}

void IOBrain::mix(Brain *other)
{
    IOBrain *otherBrain = dynamic_cast<IOBrain *>(other);
    if (otherBrain == nullptr) return; // Can only work with another SimpleBrain

    for (int j = 0; j < 2; j++)
    {
       m_outputs[j].mix(otherBrain->m_outputs[j]);
    }
}

void IOBrain::randomise(int chance, int weight)
{
    for (int j = 0; j < 2; j++)
    {
        m_outputs[j].randomise(chance, weight);
    }
}

