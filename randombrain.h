#ifndef RANDOMBRAIN_H
#define RANDOMBRAIN_H

#include "brain.h"

/**
 * @brief Simple brain that just chooses a random direction to travel in
 */
class RandomBrain : public Brain
{
public:
    RandomBrain(Worm *worm);
	static Brain *create(Worm *worm) { return new RandomBrain(worm); }
    virtual void write(QDataStream &out);
    virtual void read(QDataStream &in);

    virtual const std::string &name() const;
	virtual void planMove(QPoint &pt, int &dir);

    virtual Brain *clone(Worm *worm);

    virtual void average(Brain *other);
    virtual void mix(Brain *other);
	virtual void randomise(int chance, int weight);	
};

#endif // RANDOMBRAIN_H
