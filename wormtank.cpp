#include "wormtank.h"
#include "random.h"
#include "direction.h"
#include "league.h"
#include <cstdlib>
#include <algorithm>
#include <QDir>
#include <QFile>
#include <QStandardPaths>
#include <QMessageBox>


WormTank::WormTank(QObject *parent) :
    QObject(parent),
    m_mode(WormTank::NormalMode),
    m_name("Simple"),
    m_generation(0),
    m_width(200),
    m_height(200),
    m_startFood(400),
    m_startWorms(100),
    m_foodEnergy(400),
    m_survivors(10),
    m_tankImage(m_width, m_height, QImage::Format_Indexed8),
    m_league(nullptr)
{
    for (int j = 0; j < NUM_POSITIONS; j++)
	{
		m_tankImage.setColor(j, PositionColours[j]);
	}
    m_tankImage.fill(GROUND_COLOUR);

    // Cache the maximum distance for speed
    m_maxDistance = int(std::sqrt(m_width * m_width + m_height * m_height)) + 1;
}

WormTank::~WormTank()
{
    for (auto worm : m_worms)
    {
        delete worm;
    }
    delete m_league;
}

/**
 * @brief Get QColour for an index
 * @param index of colour to return (use _COLOUR constants)
 * @return QColour for index
 */
QColor WormTank::colourForIndex(int index)
{
    return m_tankImage.color(index);
}


/**
 * @brief start normal run of the tank
 * @param wormType type of worm to use
 * @param resetTank true to reset to first generation, otherwise loads last save
*/
void WormTank::start(QString wormType, bool resetTank)
{
    m_mode = NormalMode;
    setName(wormType);
    if (resetTank || !loadLatest(wormType)) reset(wormType);
    m_survivors = 10;
}

/**
 * @brief start a battle between two worm types
 *
 * @param wormType1 first side of battle
 * @param wormType2 second side of battle
 * @param reset use new first generations for both sides
 */
void WormTank::startBattle(QString wormType1, QString wormType2, bool reset)
{    
	QString name("Battle: ");
	name += wormType1;
	name += " v ";
	name += wormType2;

    m_mode = BattleMode;
    setName(name);
	clear();
	if (reset)
	{
        addNewWorms(wormType1, LEADER_COLOUR+1, 10);
        addNewWorms(wormType2, LEADER_COLOUR+2, 10);
    } else
    {
        if (!loadWorms(wormType1, LEADER_COLOUR+1, 10)) addNewWorms(wormType1, LEADER_COLOUR+1, 10);
        if (!loadWorms(wormType2, LEADER_COLOUR+2, 10)) addNewWorms(wormType2, LEADER_COLOUR+2, 10);
    }

    // Mix worms so each type alternates
    for (uint j = 1; j < 10; j+=2)
    {
        Worm *worm = m_worms[j];
        m_worms[j] = m_worms[j+10];
        m_worms[j+10] = worm;
    }

    addFood(160); // 8 bits of food each

    setFirstGenerationVars();
    m_survivors = 1;
}

/**
 * @brief Check if the battle has finished
 * @return true if ended, false it running or not a battle
 */
bool WormTank::battleWon() const
{
    if (m_mode != BattleMode) return false;
    if (m_survivors <= 1) return true;
    std::string firstType;
    for (auto worm : m_worms)
    {
        if (firstType.empty()) firstType = worm->name();
        else if (firstType != worm->name()) return false;
    }
    return true;
}

/**
 * @brief Check if the current league is finished
 * @return true if it's finished
 */
bool WormTank::leagueWon() const
{
    if (m_league == nullptr) return false;
    return !m_league->hasNextMatch();
}

/**
 * @brief Return the last battle winner
 *
 * Only valid in battle mode after a battle has been won
 *
 * @return battle winner or ""
 */
QString WormTank::battleWinner() const
{
    QString winner;
    if (m_mode == BattleMode) winner = m_worms[0]->name().c_str();
    return winner;
}

/**
 * @brief Return the league winner
 *
 * Only valid in league mode, returns the top of the table
 *
 * @return league winner or ""
 */
QString WormTank::leagueWinner() const
{
    QString winner;
    if (m_mode == LeagueMode) winner = m_league->topOfTableTeam();
    return winner;
}


/**
 * @brief Start/Continue the league
 */
void WormTank::startLeague()
{
    m_mode = LeagueMode;
    m_survivors = 10;
    if (!loadLeague() || leagueWon())
    {
        m_league->newSeason();
        nextLeagueMatch();
    }
}

/**
 * @brief Get details on the current league
 * @return details on the league
 */
QObject *WormTank::league()
{
    if (m_league == nullptr)
    {
        m_league = new League();
        m_league->load();
    }

    return m_league;
}

/**
 * Clear down the worm tank
 */
void WormTank::clear()
{
    m_tankImage.fill(GROUND_COLOUR);
    for (auto worm : m_worms)
    {
        delete worm;
    }
    m_worms.clear();
    m_food.clear();
}

/**
 * @brief Set up the image and the variables from a loaded tank
 */
void WormTank::setupTank()
{
    for (auto foodPos : m_food)
    {
        m_tankImage.setPixel(foodPos, FOOD_COLOUR);
    }
    // Draw worms still alive
    for (auto worm : m_worms)
    {
        for (auto wormPos : worm->body())
        {
            m_tankImage.setPixel(wormPos, worm->colour());
        }
    }

    // Ensure variables read are update in UI
    emit generationChanged(m_generation);
    emit tickChanged(m_tick);
    emit maxEnergyChanged(m_maxEnergy);
    m_foodLeft = 0; // Ensure food and worms get's updated
    m_wormsLeft = 0;
    setFoodLeft(m_food.size());
    setWormsLeft(m_worms.size());
    emit leaderAgeChanged(m_leaderAge);
    emit leaderAtTopChanged(m_leaderAtTop);
    emit lastGenerationTicksChanged(m_lastGenerationTicks);
    emit lastGenerationFoodLeftChanged(m_lastGenerationFoodLeft);
    emit lastGenerationMaxEnergyChanged(m_lastGenerationMaxEnergy);
}

/**
 * @brief Load latest worm tank for a given worm type
 * @param wormType name of worm type
 * @return true if successfull
 */
bool WormTank::loadLatest(QString wormType)
{
    clear();

    QString loadFileName = latestFileName(wormType);
    QFile loadFile(loadFileName);
    bool fileLoaded = false;
    if (loadFile.exists())
    {
        try
        {
            if (load(loadFileName))
            {
                setName(wormType);

                // Draw tank
                setupTank();

                fileLoaded = true;
            }
        } catch(...)
        {
            fileLoaded = false;
        }
    }

    return fileLoaded;
}

/**
 * @brief load battle in progressm
 * @return true if successful
 */
bool WormTank::loadBattle()
{
    if (load(battleFileName()))
    {
        emit nameChanged(m_name);
        setupTank();
        m_survivors = 1;
        if (battleWon()) m_survivors = 0;
        return true;
    }
    return false;
}

/**
 * @brief load league in progress
 * @return true if successful
 */
bool WormTank::loadLeague()
{
    league(); // Ensures league table is loaded
    if (load(leagueFileName()))
    {
        emit nameChanged(m_name);
        setupTank();
        m_survivors = 10;
        if (leagueWon()) m_survivors = 0;
        return true;
    }
    return false;
}

/**
 * @brief Add new randomly generated worms to the worm tank
 *
 * @param wormType the name of the worm type
 * @param colour the colour to use
 * @param count the number to add
 */
void WormTank::addNewWorms(QString wormType, int colour, uint count)
{
    std::string brainName = wormType.toStdString();

    for (uint j = 0; j < count; j++)
    {
        QPoint wormPos = getEmptyPos();
        Worm *worm = new Worm(this, brainName.c_str(), wormPos, randBetween(0, 3));
        worm->setColour(colour);
        m_tankImage.setPixel(wormPos, colour);
        m_worms.push_back(worm);
    }
}

/**
 * @brief Add Food to the worm tank
 * @param count amount of food to add
 */
void WormTank::addFood(uint count)
{
    for (uint j = 0; j < count; j++)
    {
        QPoint foodPos = getEmptyPos();
        m_food.push_back(foodPos);
        m_tankImage.setPixel(foodPos, FOOD_COLOUR);
    }
}

/**
 * @brief Move food that's near the edges back to the centre
 */
void WormTank::moveFoodFromEdges()
{
	for (auto &foodPos : m_food)
	{
		if (foodPos.x() < 10 || foodPos.x() > m_width - 10
			|| foodPos.y() < 10 || foodPos.y() > m_height - 10)
		{
			m_tankImage.setPixel(foodPos, GROUND_COLOUR);
			foodPos = getEmptyPos();
			m_tankImage.setPixel(foodPos, FOOD_COLOUR);
		}
	}
}


/**
 * @brief Reset worm tank to first generation of a given worm type
 *
 * @param wormType type of worm to reset
 */
void WormTank::reset(QString wormType)
{
    clear();
    setName(wormType);

    // Generate food for initial generation
    for (uint j = 0; j < m_startFood; j++)
    {
        QPoint foodPos = getEmptyPos();
        m_tankImage.setPixel(foodPos, FOOD_COLOUR);
        m_food.push_back(foodPos);
    }

    addNewWorms(wormType, WORM_COLOUR, m_startWorms);
    setFirstGenerationVars();
}

/**
 * @brief Set all the variable for the first generation
 */
void WormTank::setFirstGenerationVars()
{
    m_generation = 1;
    m_tick = 0;
    emit generationChanged(m_generation);
    emit tickChanged(m_tick);
    setMaxEnergy(0);
    setFoodLeft(m_food.size());
    setWormsLeft(m_worms.size());
    setLeaderAge(0);
    setLeaderAtTop(0);
    setLastGenerationTicks(0);
    setLastGenerationFoodLeft(m_startFood);
    setLastGenerationMaxEnergy(0);
}


/**
 * @brief Save the latest version of the tank to the "latest" directory
 */
void WormTank::save()
{
    QString fileName;
    switch(m_mode)
    {
    case NormalMode:
        fileName = latestFileName(m_name);
        break;

    case BattleMode:
        fileName = battleFileName();
        break;

    case LeagueMode:
        fileName = leagueFileName();
        m_league->save();
        break;
    }

    QDir saveDir(fileName);
    saveDir.cdUp();
    saveDir.mkpath(".");
    save(fileName);
}

/**
 * @brief Get file name to load/save latest generation
 * @param wormName name of worm
 * @return file name to full path
 */
QString WormTank::latestFileName(QString wormName)
{
    QDir latestDir(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation));
    latestDir.cd("latest");
    return latestDir.absoluteFilePath(wormName);
}

/**
 * @brief Get file name to load/save current battle
 * @return file name to full path
 */
QString WormTank::battleFileName()
{
    QDir latestDir(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation));
    return latestDir.absoluteFilePath(QLatin1String("BattleTank"));
}

/**
 * @brief Get file name to load/save current league
 * @return file name to full path
 */
QString WormTank::leagueFileName()
{
    QDir latestDir(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation));
    return latestDir.absoluteFilePath(QLatin1String("LeagueTank"));
}

/**
 * @brief Save the current worm tanl to the given file
 * @param fileName file name to save to
 */
void WormTank::save(QString fileName)
{
    QFile saveFile(fileName);
    if (saveFile.open(QIODevice::WriteOnly))
    {
        QDataStream out(&saveFile);
        int fileVersion = 1;
        out << fileVersion;
        out << int(m_mode);
        out << m_name;
        out << m_generation;
        out << m_tick;
        out << m_maxEnergy;
        int wormCount = m_worms.size();
        out << wormCount;
        for (auto worm : m_worms)
        {
            out << *worm;
        }
        int foodCount = m_food.size();
        out << foodCount;
        for (auto food : m_food)
        {
            out << food;
        }
        out << m_leaderAge;
        out << m_leaderAtTop;
        out << m_lastGenerationTicks;
        out << m_lastGenerationFoodLeft;
        out << m_lastGenerationMaxEnergy;
    } else
    {
        //TODO: Error handling
    }
}

/**
 * @brief Load the worm tank from the given file
 * @param fileName file name to load from
 */
bool WormTank::load(QString fileName)
{
    QFile loadFile(fileName);
    if (loadFile.open(QIODevice::ReadOnly))
    {
        QDataStream in(&loadFile);
        int fileVersion, fileType;
        in >> fileVersion;       
        in >> fileType;
        m_mode = Mode(fileType);
        in >> m_name;
        in >> m_generation;
        in >> m_tick;
        in >> m_maxEnergy;
        in >> m_wormsLeft;
        m_worms.clear();
        for (uint j = 0; j < m_wormsLeft; j++)
        {
            Worm *worm = new Worm(this);
            in >> *worm;
            m_worms.push_back(worm);
        }
        in >> m_foodLeft;
        m_food.clear();
        for (uint j = 0; j < m_foodLeft; j++)
        {
            QPoint foodPos;
            in >> foodPos;
            m_food.push_back(foodPos);
        }
        in >> m_leaderAge;
        in >> m_leaderAtTop;
        in >> m_lastGenerationTicks;
        in >> m_lastGenerationFoodLeft;
        in >> m_lastGenerationMaxEnergy;

        return true;
    } else
    {
        return false;
    }
}

/**
 * @brief Load worms from a worm tank
 *
 * Adds them to the current tank, does not remove any existing worms
 *
 * @param wormType the name of the worm type
 * @param colour the colour to use for the loaded worms
 * @param count the number to load (note can only load up to maximum in file)
 * @return true if the worms were loaded
 */
bool WormTank::loadWorms(QString wormType, int colour, uint count)
{
    QString loadFileName = latestFileName(wormType);
    QFile loadFile(loadFileName);
    bool fileLoaded = false;
    if (loadFile.exists() && loadFile.open(QIODevice::ReadOnly))
    {
        QDataStream in(&loadFile);
        int fileVersion, fileType;
        in >> fileVersion;
        in >> fileType;
        QString name;
        in >> name; // Skip name
        uint dum;
        in >> dum; // m_generation;
        in >> dum; // m_tick;
        in >> dum; // m_maxEnergy;
        uint wormsLeft;
        in >> wormsLeft;
        if (count > wormsLeft) count = wormsLeft;
        for (uint j = 0; j < count; j++)
        {
            Worm *worm = new Worm(this);
            in >> *worm;
            worm->setColour(colour);
            worm->clearTail();
            worm->resetEnergy();
            m_worms.push_back(worm);
            if (m_tankImage.pixelIndex(worm->headPos()))
            {
                // Reposition so it's not loaded on top of something else
                worm->resetPosition(getEmptyPos(), std::rand() & 3);
            }
            m_tankImage.setPixel(worm->headPos(), worm->colour());
        }

        fileLoaded = true;
    }

    return fileLoaded;
}


void WormTank::setName(QString name)
{
    if (name != m_name)
    {
        m_name = name;
        emit nameChanged(m_name);
    }
}

void WormTank::setWormsLeft(unsigned int left)
{
    if (left != m_wormsLeft)
    {
        m_wormsLeft = left;
        emit wormsLeftChanged(left);
    }
}

void WormTank::setFoodLeft(unsigned int food)
{
    if (food != m_foodLeft)
    {
        m_foodLeft = food;
        emit foodLeftChanged(food);
    }
}

void WormTank::setMaxEnergy(unsigned int energy)
{
   if (m_maxEnergy != energy)
    {
        m_maxEnergy = energy;
        emit maxEnergyChanged(energy);
    }
}

void WormTank::setLeaderAge(unsigned int age)
{
    if (m_leaderAge != age)
    {
        m_leaderAge = age;
        emit leaderAgeChanged(age);
    }
}

void WormTank::setLeaderAtTop(unsigned int atTop)
{
    if (m_leaderAtTop != atTop)
    {
        m_leaderAtTop = atTop;
        emit leaderAtTopChanged(atTop);
    }
}

void WormTank::setLastGenerationTicks(uint ticks)
{
    if (ticks != m_lastGenerationTicks)
    {
        m_lastGenerationTicks = ticks;
        emit lastGenerationTicksChanged(ticks);
    }
}

void WormTank::setLastGenerationFoodLeft(uint left)
{
    if (left != m_lastGenerationFoodLeft)
    {
        m_lastGenerationFoodLeft = left;
        emit lastGenerationFoodLeftChanged(left);
    }
}

void WormTank::setLastGenerationMaxEnergy(uint energy)
{
    if (energy != m_lastGenerationMaxEnergy)
    {
        m_lastGenerationMaxEnergy = energy;
        emit lastGenerationMaxEnergyChanged(energy);
    }
}


/**
 * @brief Step through one update of all the worms
 */
void WormTank::step()
{
    std::vector<Worm *> died;
    QPoint pt;
    int dir = 0;

    if (m_survivors == 0 && m_wormsLeft == 1)
    {
       // Lap of honour for victor of a battle or arena
       if (m_worms[0]->energy() < 500) m_worms[0]->feed(500);
       if (m_foodLeft < 10) addFood(10);
    }

	for (auto worm : m_worms)
	{
        worm->planMove(pt, dir);
        if (pt.x() >= 0 && pt.x() < m_width && pt.y() >= 0 && pt.y() < m_height)
		{
            int pixel = m_tankImage.pixelIndex(pt);
            if (pixel == GROUND_COLOUR || pixel == FOOD_COLOUR)
			{
                worm->OKtoMove(pt,dir);
                m_tankImage.setPixel(pt, worm->colour());
				if (pixel == FOOD_COLOUR)
				{
                    auto pos = std::find(m_food.begin(), m_food.end(), pt);
                    m_food.erase(pos);
					worm->feed(m_foodEnergy);
                    if (worm->energy() > m_maxEnergy) setMaxEnergy(worm->energy());
                }
            } else
            {
                // Collistions cost an extra unit of energy
                worm->useEnergy();
            }

		}
		worm->useEnergy();
        while (worm->getPointToErase(pt))
        {
            m_tankImage.setPixel(pt, GROUND_COLOUR);
        }
        if (worm->dead())
        {
            died.push_back(worm);
            // Exit loop when we have only the specified number of survivors left
            if (m_worms.size() - died.size() <= m_survivors) break;
        }
	}

	// Get rid of dead worms and convert them to food
	for (auto worm : died)
	{
        auto pos = std::find(m_worms.begin(), m_worms.end(), worm);
        m_worms.erase(pos);
		// Convert dead body to food
		for (auto pt : worm->body())
		{
			m_tankImage.setPixel(pt, FOOD_COLOUR);
			m_food.push_back(pt);
		}
        delete worm;
	}

    setWormsLeft(m_worms.size());
    setFoodLeft(m_food.size());
    m_tick++;

    bool genFinished = false;
    if (m_wormsLeft <= m_survivors)
    {
        genFinished = true;
    } else if (m_mode != NormalMode && !died.empty())
    {
        // All other types exit the generation when there is only one
        // type of worm left
        std::string firstType;
        genFinished = true;
        for (auto worm : m_worms)
        {
            if (firstType.empty()) firstType = worm->name();
            else if (firstType != worm->name())
            {
                genFinished = false;
                break;
            }
        }
    }

    if (genFinished)
	{
		nextGeneration();
	}
    else
	{
		emit tickChanged(m_tick);
	}
}

/**
 * @brief start next generation
*/
void WormTank::nextGeneration()
{
    // Sort survivors by energy
    std::sort(m_worms.begin(), m_worms.end(), more_energy);

    if (m_worms[0]->colour() == LEADER_COLOUR) setLeaderAtTop(m_leaderAtTop+1);
    if (m_worms[0]->colour() < LEADER_COLOUR + 10) setLeaderAge(m_leaderAge + 1);
    setLastGenerationTicks(m_tick);
    setLastGenerationFoodLeft(m_foodLeft);
    setLastGenerationMaxEnergy(m_maxEnergy);

    switch(m_mode)
    {
    case NormalMode:
        newGeneration();
        break;
    case BattleMode:
        m_survivors = 0; // Set last worm to do a lap of honour
        emit matchEnded();
        break;
    case LeagueMode:
        endOfLeagueGeneration();
        break;
    }
}

/**
 * @brief Create a new generation of worms based on the top 10 survivors
 */
void WormTank::newGeneration()
{
    Worm *worm;

    // Reset surviviors
    for (int j = 0; j < 10; j++)
    {
        m_worms[j]->resetEnergy();
        m_worms[j]->setColour(LEADER_COLOUR+j);
        // Delete tail
        for (auto bodyIter = m_worms[j]->body().begin() + 1; bodyIter != m_worms[j]->body().end(); ++bodyIter)
        {
            m_tankImage.setPixel(*bodyIter, GROUND_COLOUR);
        }
        m_worms[j]->clearTail();
    }

	// leader gets most children
	Worm *parent = m_worms[0];
    // 9 - Random variants
    for (int chance = 20; chance <= 60; chance+=20)
	{
        for (int weight = 5; weight <= 15; weight += 5)
        {
            worm = parent->randomChild(chance, weight);
            worm->resetPosition(getEmptyPos(), std::rand() & 3);
            m_worms.push_back(worm);
            m_tankImage.setPixel(worm->headPos(), WORM_COLOUR);
        }
	}
    // 9 mating with others in top ten using average
    for (int j = 1; j < 10; j++)
	{
        worm = parent->averageChild(m_worms[j], 20, 10);
		worm->resetPosition(getEmptyPos(), std::rand() & 3);
		m_worms.push_back(worm);
        m_tankImage.setPixel(worm->headPos(), WORM_COLOUR);
    }

    // 9 mating with others in top ten using mix
    for (int j = 1; j < 10; j++)
    {
        worm = parent->mixChild(m_worms[j], 20, 10);
        worm->resetPosition(getEmptyPos(), std::rand() & 3);
        m_worms.push_back(worm);
        m_tankImage.setPixel(worm->headPos(), WORM_COLOUR);
    }

    // 63 = 9 x (3 x 2 + 1) Random children based on the rest of the top ten
	for (int k = 1; k < 10; k++)
	{
        parent = m_worms[k];
        for (int chance = 20; chance <= 60; chance+=20)
		{
            for (int weight = 5; weight <= 15; weight += 10)
            {
                worm = parent->randomChild(chance, weight);
                worm->resetPosition(getEmptyPos(), std::rand() & 3);
                m_worms.push_back(worm);
                m_tankImage.setPixel(worm->headPos(), WORM_COLOUR);
            }
        }
        worm = parent->randomChild(50, 20);
        worm->resetPosition(getEmptyPos(), std::rand() & 3);
        m_worms.push_back(worm);
        m_tankImage.setPixel(worm->headPos(), WORM_COLOUR);
    }

	moveFoodFromEdges();

    // Bring food up to start level
    while (m_food.size() < (unsigned)m_startFood)
    {
        QPoint foodPos = getEmptyPos();
        m_food.push_back(foodPos);
        m_tankImage.setPixel(foodPos, FOOD_COLOUR);
    }

    // Reward survivors so that children have to be better to replace them
    int reward = m_survivors * 10;
    for (uint j = 0; j < m_survivors; j++)
    {
        m_worms[j]->feed(reward);
        reward -= 10;
    }

	// Start up next generation
	m_generation++;
	m_tick = 0;
	emit generationChanged(m_generation);
	emit tickChanged(m_tick);
    setMaxEnergy(0);
    setWormsLeft(m_worms.size());
    setFoodLeft(m_food.size());
}

/**
 * @brief End of generation when in league mode
 *
 * Checks for the end of the match and moves to next match
 * or sets up next round of current match
 */
void WormTank::endOfLeagueGeneration()
{
    int numTeam1 = 0;
    for (auto worm : m_worms)
    {
       if (worm->colour() == LEADER_COLOUR + 1) numTeam1++;
    }
    if (numTeam1 > 10) numTeam1 = 10;
    m_league->updateTeamScore(numTeam1);
    if (numTeam1 == 10)
    {
        m_league->team1Victory(m_generation);
    }
    else if (numTeam1 == 0) m_league->team2Victory(m_generation);
    else if (m_generation == 4) m_league->endOfMatch();

    if (m_league->matchFinished())
    {
        emit matchEnded();
        nextLeagueMatch();
    }
    else
    {
        // Recolour top ten to adult colours, then generate a new child
        // for every top ten survivor
        for (int j = 0; j  < 10; j++)
        {
            Worm *worm = m_worms[j];
            if (worm->colour() >= LEADER_COLOUR + CHILD_OFFSET) worm->setColour(worm->colour()-CHILD_OFFSET);
            Worm *child = worm->randomChild(20, 5);
            child->clearTail();
            child->resetPosition(getEmptyPos(), std::rand() & 3);
            m_worms.push_back(child);
            m_tankImage.setPixel(child->headPos(), worm->colour());
        }

        m_generation++;
        m_tick = 0;
    }

    emit generationChanged(m_generation);
    emit tickChanged(m_tick);
}

/**
 * @brief Start of next match in the league
 */
void WormTank::nextLeagueMatch()
{
   if (m_league->nextMatch())
   {
      QString wormType1 = m_league->currentTeam1();
      QString wormType2 = m_league->currentTeam2();
      QString name("League: ");
      name += wormType1;
      name += " v ";
      name += wormType2;

      setName(name);
      clear();

      if (!loadWorms(wormType1, LEADER_COLOUR+1, 10)) addNewWorms(wormType1, LEADER_COLOUR+1, 10);
      if (!loadWorms(wormType2, LEADER_COLOUR+2, 10)) addNewWorms(wormType2, LEADER_COLOUR+2, 10);

      // Mix worms so each type alternates
      for (uint j = 1; j < 10; j+=2)
      {
          Worm *worm = m_worms[j];
          m_worms[j] = m_worms[j+10];
          m_worms[j+10] = worm;
      }

      addFood(160); // 8 bits of food each

      setFirstGenerationVars();
   }
   else
   {
       emit leagueFinished();
       m_survivors = 0; // Leave last worm wandering around
   }
}

/**
 * @brief Get the result of the current league match
 * @param team1 returns first team name
 * @param team2 returns second team name
 * @param score1 returns first team score
 * @param score2 returns second team score
 */
void WormTank::getLeagueMatchResult(QString &team1, QString &team2, int &score1, int &score2)
{
    team1 = m_league->currentTeam1();
    team2 = m_league->currentTeam2();
    m_league->getCurrentScore(score1, score2);
}

/**
 * @brief Find an unused space in the tank
 *
 * @return new unused position
 */
QPoint WormTank::getEmptyPos()
{
	int x, y;
	do
	{
		x = randBetween(10, m_width - 10);
		y = randBetween(10, m_height - 10);
	} while (m_tankImage.pixelIndex(x, y));

	return QPoint(x, y);
}

/**
 * @brief Fill in the inputs with the food seen sensor
 *
 * This sensor detects if food is in the given direction
 *
 * @param pt Worm head position
 * @param dir direction worm is facing
 * @param inputs 4 InputBrainCell array for left, ahead, right and nothing seen
 */
void WormTank::foodSeen(QPoint pt, int dir, InputBrainCell *inputs)
{
    for (int j = 0; j < 4; j++) inputs[j].setValue(0);
    int left = (dir + 3) % 4;
    int ahead = dir;
    int right = (dir + 1) % 4;

	for (auto foodPt : m_food)
	{
		int xDiff = foodPt.x() - pt.x();
		int yDiff = foodPt.y() - pt.y();
        if (xDiff != 0 && yDiff != 0)
        {
            int foodDir = direction(xDiff, yDiff);
            if (foodDir == left) inputs[0].setValue(1);
            else if (foodDir == ahead) inputs[1].setValue(1);
            else if (foodDir == right) inputs[2].setValue(1);
        }
	}
    // Set nothing seens if no inputs
    inputs[3].setValue(!(inputs[0].value() | inputs[1].value() | inputs[2].value()));
}

/**
 * @brief Fill in the inputs with the food seen sensor
 *
 * This sensor detects if a worm is in the given direction
 *
 * @param pt Worm head position
 * @param dir direction worm is facing
 * @param inputs 4 InputBrainCell array for left, ahead, right and nothing seen
 */
void WormTank::wormSeen(QPoint pt, int dir, InputBrainCell *inputs)
{
    for (int j = 0; j < 4; j++) inputs[j].setValue(0);
    int left = (dir + 3) % 4;
    int ahead = dir;
    int right = (dir + 1) % 4;

    for (auto worm : m_worms)
    {
        for (auto wormPt : worm->body())
        {
            int xDiff = wormPt.x() - pt.x();
            int yDiff = wormPt.y() - pt.y();
            if (xDiff != 0 && yDiff != 0)
            {
                int wormDir = direction(xDiff, yDiff);
                if (wormDir == left) inputs[0].setValue(1);
                else if (wormDir == ahead) inputs[1].setValue(1);
                else if (wormDir == right) inputs[2].setValue(1);
            }
        }
    }
    // Set nothing seens if no inputs
    inputs[3].setValue(!(inputs[0].value() | inputs[1].value() | inputs[2].value()));
}


/**
 * @brief Fill in the inputs with the next to food sensor
 *
 * This sensor detects if food is next to a point
 *
 * @param pt Worm head position
 * @param dir direction worm is facing
 * @param inputs 4 InputBrainCell array for left, ahead, right and nothing seen
 */
void WormTank::foodNextTo(QPoint pt, int dir, InputBrainCell *inputs)
{
    for (int j = 0; j < 4; j++) inputs[j].setValue(0);

    int left = (dir + 3) % 4;
    int ahead = dir;
    int right = (dir + 1) % 4;
    QPoint check(pt);
    movePoint(pt, left);
    if (pointInTank(pt) && m_tankImage.pixelIndex(pt) == FOOD_COLOUR)
    {
        inputs[0].setValue(1);
    }
    check = pt;
    movePoint(pt, ahead);
    if (pointInTank(pt) && m_tankImage.pixelIndex(pt) == FOOD_COLOUR)
    {
        inputs[1].setValue(1);
    }
    check = pt;
    movePoint(pt, right);
    if (pointInTank(pt) && m_tankImage.pixelIndex(pt) == FOOD_COLOUR)
    {
        inputs[2].setValue(1);
    }

    // Set nothing seens if no inputs
    inputs[3].setValue(!(inputs[0].value() | inputs[1].value() | inputs[2].value()));
}

/**
 * @brief Fill in the inputs with the next to worm sensor
 *
 * This sensor detects if worm is next to a point
 *
 * @param pt Worm head position
 * @param dir direction worm is facing
 * @param inputs 4 InputBrainCell array for left, ahead, right and nothing seen
 */
void WormTank::wormNextTo(QPoint pt, int dir, InputBrainCell *inputs)
{
    for (int j = 0; j < 4; j++) inputs[j].setValue(0);

    int left = (dir + 3) % 4;
    int ahead = dir;
    int right = (dir + 1) % 4;
    QPoint check(pt);
    movePoint(pt, left);
    if (pointInTank(pt) && m_tankImage.pixelIndex(pt) >= WORM_COLOUR)
    {
        inputs[0].setValue(1);
    }
    check = pt;
    movePoint(pt, ahead);
    if (pointInTank(pt) && m_tankImage.pixelIndex(pt) >= WORM_COLOUR)
    {
        inputs[1].setValue(1);
    }
    check = pt;
    movePoint(pt, right);
    if (pointInTank(pt) && m_tankImage.pixelIndex(pt) >= WORM_COLOUR)
    {
        inputs[2].setValue(1);
    }

    // Set nothing seen if no inputs
    inputs[3].setValue(!(inputs[0].value() | inputs[1].value() | inputs[2].value()));
}

/**
 * @brief Fill in the inputs with the next to wall sensor
 *
 * This sensor detects if worm is next to a wall
 *
 * @param pt Worm head position
 * @param dir direction worm is facing
 * @param inputs 4 InputBrainCell array for left, ahead, right and nothing seen
 */
void WormTank::wallNextTo(QPoint pt, int dir, InputBrainCell *inputs)
{
    int wall[4];
    wall[0] = (pt.y() == m_height - 1);
    wall[1] = (pt.x() == m_width - 1);
    wall[2] = (pt.y() == 1);
    wall[3] = (pt.x() == 1);

    int left = (dir + 3) % 4;
    int ahead = dir;
    int right = (dir + 1) % 4;

    inputs[0].setValue(wall[left]);
    inputs[1].setValue(wall[ahead]);
    inputs[2].setValue(wall[right]);

    // Set nothing seen if no inputs
    inputs[3].setValue(!(inputs[0].value() | inputs[1].value() | inputs[2].value()));
}


/**
* @brief Set the input to 1 for the ranges given
* @param ptRelative point relative to the worm head (origin)
* @param dir direction the worm is facing
* @param rangesSquared squared values of the ranges to check
* @param numRanges number of ranges to check
* @param inputs inputs to update
*/
void WormTank::setFoundRanges(QPoint ptRelative, int dir, int *rangesSquared, int numRanges, InputBrainCell *inputs)
{
	int xDiff = ptRelative.x();
	int yDiff = ptRelative.y();

	if (xDiff != 0 && yDiff != 0)
	{
		int ptDir = direction(xDiff, yDiff);
		int behind = (dir + 2) % 4;

		if (ptDir != behind)
		{
			int distSquared = xDiff * xDiff + yDiff * yDiff;
            int j = 0;
			while (j < numRanges && distSquared > rangesSquared[j]) j++;
			if (j < numRanges)
			{
                InputBrainCell *setInputs = inputs + 4 * j;
				int left = (dir + 3) % 4;
				int ahead = dir;
				int right = (dir + 1) % 4;

				if (ptDir == left) setInputs[0].setValue(1);
				else if (ptDir == ahead) setInputs[1].setValue(1);
				else if (ptDir == right) setInputs[2].setValue(1);
			}
		}
	}
}

/**
 * @brief set not found at range inputs values
 *
 * Call after setting the ahead, left and right inputs for the tnages
 */
void WormTank::setNotFoundRanges(int numRanges, InputBrainCell *inputs)
{
	for (int j = 0; j < numRanges; j++)
	{
		inputs[3].setValue(inputs[0].value() == 0 && inputs[1].value() == 0 && inputs[2].value() == 0);
		inputs += 4;
	}
}

/**
* @brief Increment the input value for the input for the range this point is in
* @param ptRelative point relative to the worm head (origin)
* @param dir direction the worm is facing
* @param rangesSquared squared values of the ranges to check
* @param numRanges number of ranges to check
* @param inputs inputs to update
*/
void WormTank::countFoundRanges(QPoint ptRelative, int dir, int *rangesSquared, int numRanges, InputBrainCell *inputs)
{
	int xDiff = ptRelative.x();
	int yDiff = ptRelative.y();

	if (xDiff != 0 && yDiff != 0)
	{
		int ptDir = direction(xDiff, yDiff);
		int behind = (dir + 2) % 4;

		if (ptDir != behind)
		{
			int distSquared = xDiff * xDiff + yDiff * yDiff;
            int j = 0;
			while (j < numRanges && distSquared > rangesSquared[j]) j++;
			if (j < numRanges)
			{
                InputBrainCell *setInputs = inputs + 4 * j;
				int left = (dir + 3) % 4;
				int ahead = dir;
				int right = (dir + 1) % 4;

				if (ptDir == left) setInputs[0].setValue(setInputs[0].value()+1);
                else if (ptDir == ahead) setInputs[1].setValue(setInputs[1].value()+1);
                else if (ptDir == right) setInputs[2].setValue(setInputs[2].value()+1);
			}
		}
	}
}

/**
 * @brief check if food has been seen within certain ranges
 *
 * The ranges should be from smallest to largest. The first distance to
 * the food that is less than or equal to the range is set.
 *
 * @param pt Worm head position
 * @param dir direction worm is facing
 * @param ranges pointer to the first range
 * @param numRanges number of ranges
 * @param inputs 4 * number of ranges InputBrainCell array for left, ahead, right and nothing seen
 */
void WormTank::foodSeen(QPoint pt, int dir, int *ranges, int numRanges , InputBrainCell *inputs)
{
	int rangesSquared[numRanges];
	int j;

	for (j = 0; j < numRanges; j++) rangesSquared[j] = ranges[j] * ranges[j];
	for (j = 0; j < numRanges * 4; j++) inputs[j].setValue(0);
	for (auto foodPos : m_food)
	{
		foodPos -= pt; // Make point relative to worm head
		setFoundRanges(foodPos, dir, rangesSquared, numRanges, inputs);
	}
	setNotFoundRanges(numRanges, inputs);
}

/**
* @brief check if a worm has been seen within certain ranges
*
* The ranges should be from smallest to largest. The first distance to
* the worm that is less than or equal to the range is set.
*
* @param pt Worm head position
* @param dir direction worm is facing
* @param ranges pointer to the first range
* @param numRanges number of ranges
* @param inputs 4 * number of ranges InputBrainCell array for left, ahead, right and nothing seen
*/
void WormTank::wormSeen(QPoint pt, int dir, int *ranges, int numRanges, InputBrainCell *inputs)
{
	int rangesSquared[numRanges];
	int j;

	for (j = 0; j < numRanges; j++) rangesSquared[j] = ranges[j] * ranges[j];
	for (j = 0; j < numRanges * 4; j++) inputs[j].setValue(0);
	for (auto worm : m_worms)
	{
		for (auto bodyPos : worm->body())
		{
			bodyPos -= pt; // Make relative to observing worm head
			setFoundRanges(bodyPos, dir, rangesSquared, numRanges, inputs);
		}
	}
	setNotFoundRanges(numRanges, inputs);
}


/**
* @brief check if a wall has been seen within certain ranges
*
* The ranges should be from smallest to largest. The first distance to
* the wall that is less than or equal to the range is set.

* @param pt Worm head position
* @param dir direction worm is facing
* @param ranges pointer to the first range
* @param numRanges number of ranges
* @param inputs 4 * number of ranges InputBrainCell array for left, ahead, right and nothing seen
*/
void WormTank::wallSeen(QPoint pt, int dir, int *ranges, int numRanges, InputBrainCell *inputs)
{
	int rangesSquared[numRanges];
	int j;

	for (j = 0; j < numRanges; j++) rangesSquared[j] = ranges[j] * ranges[j];
	for (j = 0; j < numRanges * 4; j++) inputs[j].setValue(0);

	QPoint westWall(-1, pt.y());
	QPoint eastWall(m_width, pt.y());
	QPoint northWall(pt.x(), m_height);
	QPoint southWall(pt.x(), -1);

	// Make relative to worm head
	westWall -= pt;
	eastWall -= pt;
	northWall -= pt;
	southWall -= pt;

	setFoundRanges(westWall, dir, rangesSquared, numRanges, inputs);
	setFoundRanges(eastWall, dir, rangesSquared, numRanges, inputs);
	setFoundRanges(northWall, dir, rangesSquared, numRanges, inputs);
	setFoundRanges(southWall, dir, rangesSquared, numRanges, inputs);

	setNotFoundRanges(numRanges, inputs);
}

/**
* @brief count the food within certain ranges
*
* The ranges should be from smallest to largest. The first distance to
* the food that is less than or equal to the range is set.
*
* @param pt Worm head position
* @param dir direction worm is facing
* @param ranges pointer to the first range
* @param numRanges number of ranges
* @param inputs 4 * number of ranges InputBrainCell array for left, ahead, right and nothing seen
*/
void WormTank::foodCount(QPoint pt, int dir, int *ranges, int numRanges, InputBrainCell *inputs)
{
	int rangesSquared[numRanges];
	int j;

	for (j = 0; j < numRanges; j++) rangesSquared[j] = ranges[j] * ranges[j];
	for (j = 0; j < numRanges * 4; j++) inputs[j].setValue(0);
	for (auto foodPos : m_food)
	{
		foodPos -= pt; // Make point relative to worm head
		countFoundRanges(foodPos, dir, rangesSquared, numRanges, inputs);
	}
	setNotFoundRanges(numRanges, inputs);
}

/**
* @brief Count the number of worms within certain ranges
*
* The ranges should be from smallest to largest. The first distance to
* the worm that is less than or equal to the range is set.
*
* @param pt Worm head position
* @param dir direction worm is facing
* @param ranges pointer to the first range
* @param numRanges number of ranges
* @param inputs 4 * number of ranges InputBrainCell array for left, ahead, right and nothing seen
*/
void WormTank::wormCount(QPoint pt, int dir, int *ranges, int numRanges, InputBrainCell *inputs)
{
	int rangesSquared[numRanges];
	int j;

	for (j = 0; j < numRanges; j++) rangesSquared[j] = ranges[j] * ranges[j];
	for (j = 0; j < numRanges * 4; j++) inputs[j].setValue(0);
	for (auto worm : m_worms)
	{
		for (auto bodyPos : worm->body())
		{
			bodyPos -= pt; // Make relative to observing worm head
			countFoundRanges(bodyPos, dir, rangesSquared, numRanges, inputs);
		}
	}
	setNotFoundRanges(numRanges, inputs);
}

/**
 * @brief WormTankImageProvider construct
 * @param wormTank Worm tank to provide image of
 */
WormTankImageProvider::WormTankImageProvider(WormTank &wormTank) :
    QQuickImageProvider(QQmlImageProviderBase::Image),
    m_wormTank(wormTank)
{

}

/**
 * @brief Get current worm tank image
 * @param id id of image - not used
 * @param size - size of image
 * @param requestedSize - size requested
 * @return
 */
QImage WormTankImageProvider::requestImage(const QString & id, QSize * size, const QSize & requestedSize)
{
    Q_UNUSED(id)
    QImage result;

    if (requestedSize.isValid()) {
       result = m_wormTank.tankImage().scaled(requestedSize, Qt::KeepAspectRatio);
    } else {
       result = m_wormTank.tankImage();
    }
    *size = result.size();
    return result;
}
