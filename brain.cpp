#include "brain.h"

// Map to functions to create brains
BrainFactory *BrainFactory::s_instance = nullptr;


/**
 * @brief Register a new brain type
 * @param name Name of the brain type
 * @param func
 */
void BrainFactory::registerBrainType(BrainCreatorBase *creator)
{
    if (!s_instance) new BrainFactory();
    s_instance->m_brainCreators[creator->name()] = creator;
}

/**
 * @brief Factory to create the various brain type
 * @param brainName name of brain to create
 * @return new brain object
 */
Brain *BrainFactory::create(Worm *worm, const char *brainName)
{
    BrainCreatorBase *creator = s_instance->m_brainCreators[brainName];
    if (creator == nullptr) throw std::invalid_argument("Invalid brain name passed to BrainFactory::create");
    return creator->create(worm);
}

/**
 * @brief Construct the brain creator and register it.
 *
 * @param name name of Brain type
 * @param summary Summary of its functions
 */
BrainCreatorBase::BrainCreatorBase(const char *name, const char *summary) :
    m_name(name),
    m_summary(summary)
{
    BrainFactory::registerBrainType(this);
}
