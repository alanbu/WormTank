#ifndef BRAIN_H
#define BRAIN_H

#include <QPoint>
#include <QDataStream>
#include <map>
#include <string>
class Worm;
class Brain;

/**
 * @brief The interface to the worms brain
 */
class Brain
{
public:
    Brain(Worm *worm) : m_worm(worm) {}
    virtual ~Brain() {}

	/**
	 * Return the brain name
	 */
    virtual const std::string &name() const = 0;

    /**
     * @brief Return the full name of the brain
     *
     * If there are variants of a brain type this allows a modified
     * name to be returned for each type.
     *
     * If overridden it's recommended to prefix it with name() "_"
     *
     * @return The full name as a string
     */
    virtual std::string fullName() const {return name();}


    /**
     * @brief plan then next move to make
     * @param pt position after move (initialised to current head before call)
     * @param dir direction taken (initialised to current direction before call)
     */
    virtual void planMove(QPoint &pt, int &dir) = 0;
	/**
	 * @brief Make copy of the brain
	*/
    virtual Brain *clone(Worm *worm) = 0;
	/**
	 * @brief Randomise the brain
	 * @param chance percentage chance that each element of the brain will change
	 * @param weight the amount of change
	 */
	virtual void randomise(int chance, int weight) = 0;

    /**
     * @brief average take average of two brains
     * @param other Brain to average with
     */
    virtual void average(Brain *other) = 0;
    /**
     * @brief mix mix this brain with the other
     *
     * This randomly takes 50% of each brain and puts them together
     * @param other other brain to mix with
     */
    virtual void mix(Brain *other) = 0;

	/**
	 * @brief save brain to byte stream
	 * @param out QDataStream to save to
	 */
    virtual void write(QDataStream &out) = 0;
	/**
	* @brief load brain from byte stream
	* @param in QDataStream to load from
	*/
    virtual void read(QDataStream &in) = 0;

	// Helper functions for derived brains
	inline int turnLeft(int dir) const { return (dir) ? dir - 1 : 3; }
	inline int turnRight(int dir) const { return (dir < 3) ? dir + 1 : 0; }

protected:
    Worm *m_worm;

};

/**
 * @brief Base class for the Brain creation objects
 */
class BrainCreatorBase
{
public:
    BrainCreatorBase(const char *name, const char *summary);
    virtual ~BrainCreatorBase() {}

    const std::string &name() const {return m_name;}
    const std::string &summary() const {return m_summary;}

    virtual Brain *create(Worm *worm) = 0;

protected:
    std::string m_name;
    std::string m_summary;
};

/**
 * Template class to make it easy to create and register the brain creators for
 * a brain type.
 */
template<class T> class BrainCreator : public BrainCreatorBase
{
public:
    /**
     * @brief Register a new brain type
     *
     * e.g. In the source file for brain type MyBrain add
     * BrainCreator<MyBrain> g_MyBrainCreator("MyBrain", "Example brain");
     *
     * @param name Brain name
     * @param summary Summary of how the brain works.
     */
    BrainCreator(const char *name, const char *summary) : BrainCreatorBase(name, summary) {}
    virtual ~BrainCreator() {}

    virtual Brain *create(Worm *worm) {return new T(worm);}
};


class BrainFactory
{
    static BrainFactory *s_instance;
public:
    BrainFactory() { s_instance = this; }
    static BrainFactory *instance() {return s_instance;}

    static Brain *create(Worm *worm, const char *name);
    static void registerBrainType(BrainCreatorBase *creator);

    const std::map<std::string, BrainCreatorBase *> &creators() const {return m_brainCreators;}

private:
    std::map<std::string, BrainCreatorBase *> m_brainCreators;
};

#endif // BRAIN_H
