#ifndef BRAINCELL_H
#define BRAINCELL_H

#include <QDataStream>
#include <vector>
#include <algorithm>
#include <map>

/**
 * @brief Base class for brain cells
 */
class BrainCell
{
public:
    BrainCell() {}
    virtual ~BrainCell() {}

    virtual int value() = 0;
};

class InputBrainCell : public BrainCell
{
public:
    virtual ~InputBrainCell() {}
    virtual int value() {return m_value;}
    void setValue(int value) {m_value = value;}
private:
    int m_value;
};

class LinkedBrainCell : public BrainCell
{
public:
    virtual ~LinkedBrainCell() {}
    void addLink(BrainCell *cell, int weight);
    void removeLinks(BrainCell *first, BrainCell *last);
    void renameLinks(BrainCell *to, BrainCell *first, BrainCell *last);

    virtual int value();

    int getWeight(BrainCell *cell);
    void setWeight(BrainCell *cell, int weight);
    void averageWeight(BrainCell *cell, int weight);

	void randomise(int chance, int weight);

    int weight(int idx) const {return m_links[idx].second;}
    void writeWeights(QDataStream &out);
    void readWeights(QDataStream &in);
    void writeLinks(QDataStream &out, std::map<BrainCell *, int> &linkMap);
    void readLinks(QDataStream &in, BrainCell **linkArray);

    void average(const LinkedBrainCell &other);
    void mix(const LinkedBrainCell &other);

private:
    std::vector< std::pair<BrainCell *, int> > m_links;
};

#endif // BRAINCELL_H
