#include "foodwithnexttomemorybrain.h"
#include "worm.h"
#include "wormtank.h"
#include "direction.h"
#include "random.h"
#include <sstream>

// Register the simple brain creator
BrainCreator<FoodWithNextToMemoryBrain> g_foodWithNextToMemoryBrainCreator(
        "FoodWithNextToMemory",
        "Detects direction food is in and if it's next to food, a worm or wall"
        "and remembers up to 40 combinations which it uses to make a decision."
        );


/**
 * @brief Construct a new brain with no memories
 * @param worm worm the brain is attached too
 */
FoodWithNextToMemoryBrain::FoodWithNextToMemoryBrain(Worm *worm) : Brain(worm)
{
    for (uint j = 0; j < MaxNewMemories; j++) m_newMemories[j] = 0;
}

/**
 * @brief Construct a copy of the brain and attach it to a different worm
 * @param other brain to copy
 * @param worm worm to attach the new version to
 */
FoodWithNextToMemoryBrain::FoodWithNextToMemoryBrain(const FoodWithNextToMemoryBrain &other, Worm *worm) : Brain(worm)
{
    for (auto memory : other.m_memories)
    {
        m_memories[memory.first] = memory.second;
    }
    for (uint j = 0; j < MaxNewMemories; j++)
    {
        m_newMemories[j] = other.m_newMemories[j];
    }
}

/**
 * @brief Return the name of this brain type
 * @return name of brain type
 */
const std::string &FoodWithNextToMemoryBrain::FoodWithNextToMemoryBrain::name() const
{
    return g_foodWithNextToMemoryBrainCreator.name();
}

/**
 * @brief Clone a copy of this brain
 * @param worm worm to attach clone to
 * @return new Brain copy
 */
Brain *FoodWithNextToMemoryBrain::FoodWithNextToMemoryBrain::clone(Worm *worm)
{
    return new FoodWithNextToMemoryBrain(*this, worm);
}


/**
 * @brief Return the full name of the brain
 *
 * Use current number of memories as the sub type
 *
 * @return The full name as a string
 */
std::string FoodWithNextToMemoryBrain::fullName() const
{
    std::ostringstream os;
    os << name() << "_M" << m_memories.size();
    return os.str();
}

/**
 * @brief plan then next move to make
 * @param pt position after move (initialised to current head before call)
 * @param dir direction taken (initialised to current direction before call)
 */
void FoodWithNextToMemoryBrain::planMove(QPoint &pt, int &dir)
{
    m_worm->tank()->foodSeen(pt, dir, m_inputs);
    m_worm->tank()->foodNextTo(pt, dir, m_inputs+4);
    m_worm->tank()->wormNextTo(pt, dir, m_inputs+8);
    m_worm->tank()->wallNextTo(pt, dir, m_inputs+12);
    uint bits = 0;
    uint bit = 1;
    for (uint j = 0; j < NumInputs; j++)
    {
        if (m_inputs[j].value()) bits |= bit;
        bit <<=1;
    }
    auto found = m_memories.find(bits);
    if (found != m_memories.end())
    {
        dir = (dir + found->second + 4) % 4;
    } else
    {
        // Randomly replace new memory
        m_newMemories[randBelow(MaxNewMemories)] = bits;
    }
    movePoint(pt, dir);
}

/**
 * @brief Randomise the brain
 * @param chance percentage chance that each element of the brain will change
 * @param weight the amount of change used as percentage chance to discard memory here
 */
void FoodWithNextToMemoryBrain::randomise(int chance, int weight)
{
    auto memory = m_memories.begin();
    while (memory != m_memories.end())
    {
        if (randPercent() <= chance)
        {
            // Change direction
            memory->second = randBelow(3);
            memory++;
        } else if (randPercent() <= weight)
        {
            // Discard the memory
            memory = m_memories.erase(memory);
        } else
        {
            // Leave memory alone
            memory++;
        }
    }

    // Copy memories from new memories if there is space
    uint j = 0;
    while (m_memories.size() < MaxMemories && j < MaxNewMemories)
    {
        if (m_newMemories[j] != 0)
        {
            m_memories[m_newMemories[j]] = randBelow(3);
            m_newMemories[j] = 0;
        }
        j++;
    }
}

/**
 * @brief average take average of two brains
 *
 * Average keeps memory if in both brains, or on 50% change if not in the other brain
 *
 * @param other Brain to average with
 */
void FoodWithNextToMemoryBrain::average(Brain *other)
{
    FoodWithNextToMemoryBrain *otherBrain = dynamic_cast<FoodWithNextToMemoryBrain *>(other);
    if (otherBrain == nullptr) return; // Can only mix with brains of the same type
    auto memory = m_memories.begin();

    while (memory != m_memories.end())
    {
        auto found = otherBrain->m_memories.find(memory->first);
        if (found != otherBrain->m_memories.end())
        {
            if (std::rand() & 1) memory->second = found->second;
            memory++;
        } else if (std::rand() & 1)
        {
            memory = m_memories.erase(memory);
        } else
        {
            memory++;
        }
    }
    for (uint j = 0; j < MaxNewMemories; j++)
    {
        if (std::rand() & 1) m_newMemories[j] = otherBrain->m_newMemories[j];
    }
}

/**
 * @brief mix mix this brain with the other
 *
 * This randomly takes 50% of each brain and puts them together
 * @param other other brain to mix with
 */
void FoodWithNextToMemoryBrain::mix(Brain *other)
{
    FoodWithNextToMemoryBrain *otherBrain = dynamic_cast<FoodWithNextToMemoryBrain *>(other);
    if (otherBrain == nullptr) return; // Can only mix with brains of the same type
    auto memory = m_memories.begin();
    auto otherMemory = otherBrain->m_memories.begin();
    // Erase half memories
    while (memory != m_memories.end())
    {
        if (std::rand() & 1)
        {
            memory = m_memories.erase(memory);
        } else
        {
            memory++;
        }
    }
    // Add half of other brain memories
    while (otherMemory != otherBrain->m_memories.end() && m_memories.size() < MaxMemories)
    {
        if (std::rand() & 1) m_memories[otherMemory->first] = otherMemory->second;
        otherMemory++;
    }
}

/**
 * @brief save brain to byte stream
 * @param out QDataStream to save to
 */
void FoodWithNextToMemoryBrain::write(QDataStream &out)
{
    out << m_memories.size();
    for (auto memory : m_memories)
    {
        out << memory.first << memory.second;
    }
    out << MaxNewMemories;
    for (uint j = 0; j < MaxNewMemories; j++)
    {
        out << m_newMemories[j];
    }
}

/**
* @brief load brain from byte stream
* @param in QDataStream to load from
*/
void FoodWithNextToMemoryBrain::read(QDataStream &in)
{
    uint numMemories;
    in >> numMemories;
    uint bits;
    int dir;
    for (uint j = 0; j < numMemories; j++)
    {
        in >> bits >> dir;
        m_memories[bits] = dir;
    }
    in >> numMemories;
    uint newIdx = 0;
    for (uint j = 0; j < numMemories; j++)
    {
        in >> bits;
        if (newIdx < MaxNewMemories) m_newMemories[newIdx++] = bits;
    }
}
