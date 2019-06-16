#ifndef WORMTANK_H
#define WORMTANK_H

#include <QObject>
#include <QImage>
#include <vector>
#include <string>
#include "QQuickImageProvider"
#include "worm.h"
#include "braincell.h"

class League;

/**
 * @brief The WormTank that encapsulates the whole environment
 */
class WormTank : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name NOTIFY nameChanged)
    Q_PROPERTY(uint generation READ generation NOTIFY generationChanged)
    Q_PROPERTY(uint tick READ tick NOTIFY tickChanged)
    Q_PROPERTY(uint wormsLeft READ wormsLeft NOTIFY wormsLeftChanged)
    Q_PROPERTY(uint foodLeft READ foodLeft NOTIFY foodLeftChanged)
    Q_PROPERTY(uint maxEnergy READ maxEnergy NOTIFY maxEnergyChanged)
    Q_PROPERTY(uint leaderAge READ leaderAge NOTIFY leaderAgeChanged)
    Q_PROPERTY(uint leaderAtTop READ leaderAtTop NOTIFY leaderAtTopChanged)
    Q_PROPERTY(uint lastGenerationTicks READ lastGenerationTicks NOTIFY lastGenerationTicksChanged)
    Q_PROPERTY(uint lastGenerationFoodLeft READ lastGenerationFoodLeft NOTIFY lastGenerationFoodLeftChanged)
    Q_PROPERTY(uint lastGenerationMaxEnergy READ lastGenerationMaxEnergy NOTIFY lastGenerationMaxEnergyChanged)
    Q_PROPERTY(QObject *league READ league NOTIFY leagueChanged)

public:
    explicit WormTank(QObject *parent = 0);
    ~WormTank();

	void start(QString wormType, bool reset);
	void startBattle(QString wormType1, QString wormType2, bool reset);
    void startLeague();

    bool battleWon() const;
    bool leagueWon() const;

    QString battleWinner() const;
    QString leagueWinner() const;
    void getLeagueMatchResult(QString &team1, QString &team2, int &score1, int &score2);

    void clear();
    bool loadLatest(QString wormType);
    bool loadBattle();
    bool loadLeague();
    void reset(QString wormType);

    void save();

    QString name() const {return m_name;}
    enum Mode {NormalMode, BattleMode, LeagueMode};
    Mode mode() const {return m_mode;}

    uint generation() const {return m_generation;}
    uint tick() const {return m_tick;}
    unsigned int wormsLeft() const {return m_wormsLeft;}
    unsigned int foodLeft() const {return m_foodLeft;}
    unsigned int maxEnergy() const {return m_maxEnergy;}

    const std::vector<Worm *> &worms() const {return m_worms;}

    QObject *league();

    // Stats from last generation
    unsigned int leaderAge() const {return m_leaderAge;}
    unsigned int leaderAtTop() const {return m_leaderAtTop;}
    uint lastGenerationTicks() const {return m_lastGenerationTicks;}
    uint lastGenerationFoodLeft() const {return m_lastGenerationFoodLeft;}
    uint lastGenerationMaxEnergy() const {return m_lastGenerationMaxEnergy;}


    unsigned int maxDistance() const {return m_maxDistance;}

    QImage tankImage() const {return m_tankImage;}
    QColor colourForIndex(int index);

    // Sensors
    void foodSeen(QPoint pt, int dir, InputBrainCell *inputs);
    void wormSeen(QPoint pt, int dir, InputBrainCell *inputs);
    void foodNextTo(QPoint pt, int dir, InputBrainCell *inputs);
    void wormNextTo(QPoint pt, int dir, InputBrainCell *inputs);
    void wallNextTo(QPoint pt, int dir, InputBrainCell *inputs);

    void foodSeen(QPoint pt, int dir, int *ranges, int numRanges, InputBrainCell *inputs);
    void wormSeen(QPoint pt, int dir, int *ranges, int numRanges, InputBrainCell *inputs);
    void wallSeen(QPoint pt, int dir, int *ranges, int numRanges, InputBrainCell *inputs);

    void foodCount(QPoint pt, int dir, int *ranges, int numRanges, InputBrainCell *inputs);
    void wormCount(QPoint pt, int dir, int *ranges, int numRanges, InputBrainCell *inputs);

    // Helpers
    /**
     * @brief Check if a point is in the tank
     * @return true if point is in the tank
     */
    inline bool pointInTank(QPoint pt) const {return pt.x() >= 0 && pt.x() < m_width && pt.y() >= 0 && pt.y() < m_height;}

protected:
	void nextGeneration();
    void endOfLeagueGeneration();
    void setName(QString name);
    void setWormsLeft(unsigned int left);
    void setFoodLeft(unsigned int food);
    void setMaxEnergy(unsigned int energy);
    void setLeaderAge(unsigned int age);
    void setLeaderAtTop(unsigned int atTop);
    void setLastGenerationTicks(uint ticks);
    void setLastGenerationFoodLeft(uint left);
    void setLastGenerationMaxEnergy(uint energy);

    void save(QString fileName);
    bool load(QString fileName);

    void addNewWorms(QString wormType, int colour, uint count);
    bool loadWorms(QString wormType, int colour, uint count);
    void addFood(uint count);
	void moveFoodFromEdges();

    void setFirstGenerationVars();
    void newGeneration();
    void nextLeagueMatch();

    void setFoundRanges(QPoint ptRelative, int dir, int *rangesSquared, int numRanges, InputBrainCell *inputs);
    void setNotFoundRanges(int numRanges, InputBrainCell *inputs);
    void countFoundRanges(QPoint ptRelative, int dir, int *rangesSquared, int numRanges, InputBrainCell *inputs);

    QString latestFileName(QString wormName);
    QString battleFileName();
    QString leagueFileName();


signals:
    void generationChanged(uint generation);
    void tickChanged(uint tick);
    void nameChanged(QString name);
    void wormsLeftChanged(uint left);
    void foodLeftChanged(uint left);
    void maxEnergyChanged(uint energy);
    void leaderAgeChanged(uint age);
    void leaderAtTopChanged(uint atTop);
    void lastGenerationTicksChanged(uint ticks);
    void lastGenerationFoodLeftChanged(uint left);
    void lastGenerationMaxEnergyChanged(uint energy);
    void matchEnded();
    void leagueChanged();
    void leagueFinished();

public slots:
    void step();

protected:
	QPoint getEmptyPos();

        void setupTank();
        
private:
	Mode m_mode;
    QString m_name;
    uint m_generation;
    uint m_tick;
    int m_width;
    int m_height;
    unsigned int m_startFood;
    unsigned int m_startWorms;
    unsigned int m_foodEnergy;
    unsigned int m_survivors;
    QImage m_tankImage;
    std::vector<QPoint> m_food;
    std::vector<Worm *> m_worms;
    unsigned int m_wormsLeft;
    unsigned int m_foodLeft;
    unsigned int m_maxEnergy;
    unsigned int m_maxDistance;
    uint m_leaderAge;
    uint m_leaderAtTop;
    uint m_lastGenerationTicks;
    uint m_lastGenerationFoodLeft;
    uint m_lastGenerationMaxEnergy;
    League *m_league;
};

/**
 * @brief Image provider to give the wormtanks image to QtQuick
 */
class WormTankImageProvider : public QQuickImageProvider
{
    const WormTank &m_wormTank;
public:
   WormTankImageProvider(WormTank &wormTank);

   virtual QImage requestImage(const QString & id, QSize * size, const QSize & requestedSize);
};

#endif // WORMTANK_H
