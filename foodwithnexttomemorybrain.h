#ifndef FOODWITHNEXTTOMEMORYBRAIN_H
#define FOODWITHNEXTTOMEMORYBRAIN_H

#include "brain.h"
#include "braincell.h"
#include <map>

class FoodWithNextToMemoryBrain : public Brain
{
public:
    FoodWithNextToMemoryBrain(Worm *worm);
    FoodWithNextToMemoryBrain(const FoodWithNextToMemoryBrain &other, Worm *worm);
    virtual ~FoodWithNextToMemoryBrain() {}

    virtual const std::string &name() const;
    virtual std::string fullName() const;
    virtual void planMove(QPoint &pt, int &dir);
    virtual Brain *clone(Worm *worm);
    virtual void randomise(int chance, int weight);
    virtual void average(Brain *other);
    virtual void mix(Brain *other);
    virtual void write(QDataStream &out);
    virtual void read(QDataStream &in);

private:
   static const uint NumInputs = 16;
   static const uint MaxMemories = 40;
   static const uint MaxNewMemories = 5;
   InputBrainCell m_inputs[NumInputs];
   std::map<uint,int> m_memories;
   uint m_newMemories[MaxNewMemories];
};

#endif // FOODWITHNEXTTOMEMORYBRAIN_H
