#include "braincell.h"
#include "random.h"

void LinkedBrainCell::addLink(BrainCell *cell, int weight)
{    
	m_links.push_back(std::make_pair(cell, weight));
}

/**
 * @brief Remove links from the brain cell
 *
 * begin, end must be part of the same array
 *
 * @param first first link to remove
 * @param last last link to remove
 */
void LinkedBrainCell::removeLinks(BrainCell *first, BrainCell *last)
{
    auto link = m_links.begin();
    while (link != m_links.end())
    {
        if (link->first >= first && link->first <= last) link = m_links.erase(link);
        else ++link;
    }
}

/**
 * @brief Rename links to a range of brain cells
 *
 * from, end must be part of the same array
 *
 * @param to   base cell to copy to
 * @param first first cell to rename
 * @param last  last cell to rename
 */
void LinkedBrainCell::renameLinks(BrainCell *to, BrainCell *first, BrainCell *last)
{
    for (auto &link : m_links)
    {
        if (link.first >= first && link.first <= last)
        {
            link.first = to + (link.first - first);
        }
    }
}

int LinkedBrainCell::value()
{
	int val = 0;
	for (auto link : m_links)
	{
        val += link.first->value() * link.second;
	}

	return val;
}

/**
 * @brief Get weight for link to given brain cell
 * @param cell Linked cell
 * @return weight
 */
int LinkedBrainCell::getWeight(BrainCell *cell)
{
    for (auto link : m_links)
    {
        if (link.first == cell) return link.second;
    }

    // Shouldn't get here
    Q_ASSERT(false);
    return 0;
}

/**
 * @brief Set weight for link to given brain cell
 * @param cell Linked cell
 * @param weight new weight
 */
void LinkedBrainCell::setWeight(BrainCell *cell, int weight)
{
    for (auto &link : m_links)
    {
        if (link.first == cell)
        {
            link.second = weight;
            return;
        }
    }

    // Shouldn't get here
    Q_ASSERT(false);
}

/**
 * @brief Replace weight for link to given brain cell with average
 * between it's current value and the given weight.
 *
 * @param cell Linked cell
 * @param weight weight to average with
 */
void LinkedBrainCell::averageWeight(BrainCell *cell, int weight)
{
    for (auto &link : m_links)
    {
        if (link.first == cell)
        {
            link.second = (link.second + weight)/2;
            return;
        }
    }

    // Shouldn't get here
    Q_ASSERT(false);
}


/**
 * @brief save weights from the links in this cell
 * @param out QDataStream to save them too
*/
void LinkedBrainCell::writeWeights(QDataStream &out)
{
	for (auto link : m_links)
	{
		out << link.second;
	}
}

/**
 * @brief load weight from the links in this cell
 *
 * Note: The correct number of links to load must
 * be created before calling this function
 *
 * @param QDataStream to load weight from
 */
void LinkedBrainCell::readWeights(QDataStream &in)
{
	for (auto link : m_links)
	{
		in >> link.second;
	}
}

/**
 * @brief Write links to the data stream
 * @param out data stream to write to
 * @param linkMap map to provide indexes for the links for restoring
 */
void LinkedBrainCell::writeLinks(QDataStream &out, std::map<BrainCell *, int> &linkMap)
{
    out << m_links.size();
    for (auto link : m_links)
    {
        auto found = linkMap.find(link.first);
        if (found != linkMap.end()) out << found->second;
        else
        {
            int newId = linkMap.size();
            linkMap[link.first] = newId;
            out << newId;
        }
        out << link.second;
    }
}

/**
 * @brief Read links from data stream
 * @param in stream to read from
 * @param linkArray map read id index to a BrainCell
 */
void LinkedBrainCell::readLinks(QDataStream &in, BrainCell **linkArray)
{
    int numLinks;
    in >> numLinks;
    m_links.clear();
    for (int j = 0; j < numLinks; j++)
    {
        int linkId, weight;
        in >> linkId >> weight;
        addLink(linkArray[linkId], weight);
    }
}

/**
 * @brief Randomise the links for this cell
 * @param chance percentage chance a link will change
 * @param weight maximum change in weight
*/
void LinkedBrainCell::randomise(int chance, int weight)
{
	for (auto link : m_links)
	{
        if (randPercent() <= chance) link.second += randBetween(-weight, weight);
	}
}

void LinkedBrainCell::average(const LinkedBrainCell &other)
{
    for (uint j = 0; j < m_links.size(); j++)
    {
        uint otherWeight = (j < other.m_links.size()) ? other.m_links[j].second : 0;
        m_links[j].second = (m_links[j].second + otherWeight)/2;
    }
}

void LinkedBrainCell::mix(const LinkedBrainCell &other)
{
    for (uint j = 0; j < m_links.size(); j++)
    {
        if (j < other.m_links.size() && (std::rand() & 1))
        {
            m_links[j].second = other.m_links[j].second;
        }
    }
}
