#include "wormtankmodelview.h"
#include "brain.h"
#include "wormcount.h"
#include <QSettings>
#include <QQuickItem>
#include <QQmlContext>
#include <QDesktopServices>

WormTankModelView::WormTankModelView(WormTank &wormTank, QObject *parent) :
    QObject(parent),
    m_wormTank(wormTank),
    m_wormListView(nullptr),
    m_leagueView(nullptr),
    m_historyView(nullptr),
    m_wormList(nullptr),
    m_finishedText(""),
    m_finishedTextVisible(false)
{

}

WormTankModelView::~WormTankModelView()
{
    if (m_wormListView) m_wormListView->destroy();
    delete m_leagueView;
    delete m_historyView;
    if (m_wormList)
    {
        for (auto wc : *m_wormList)
        {
            delete wc;
        }
        delete m_wormList;
    }
}

/**
 * @brief Show help on the game
 */
void WormTankModelView::showHelp()
{
    QDesktopServices::openUrl(QUrl("file:///C:/Misc/QtDev/WormTank/docs/index.html"));
}

/**
 * @brief Set text to overlay the tank image when a run has finished
 * @param newText new text to display
 */
void WormTankModelView::setFinishedText(QString newText)
{
    if (newText != m_finishedText)
    {
        m_finishedText = newText;
        emit finishedTextChanged(m_finishedText);
    }
}

/**
 * @brief Set to show/hide if the finish text should overlay the tank image
 * @param visible
 */
void WormTankModelView::setFinishedTextVisible(bool visible)
{
    if (visible != m_finishedTextVisible)
    {
        m_finishedTextVisible = visible;
        emit finishedTextVisibleChanged(m_finishedTextVisible);
    }
}

/**
 * @brief Set status text used in the status bar
 * @param newText new status text
 */
void WormTankModelView::setStatusText(QString newText)
{
    if (newText != m_statusText)
    {
        m_statusText = newText;
        emit statusTextChanged(m_statusText);
    }
}

/**
 * @brief List of worm types
 *
 * Used for worm type selection fields
 *
 * @return QStringList of wormTypes
 */
QStringList WormTankModelView::wormTypes()
{
    if (m_wormTypes.empty())
    {
        // Load only when asked for first time
        // Worms are named after their brains
        for (auto creator : BrainFactory::instance()->creators())
        {
            m_wormTypes.append(QString(creator.first.c_str()));
        }
    }

    return m_wormTypes;
}



/**
 * @brief Show dialog to choose worm type and start the tank running
 */
void WormTankModelView::start(QString wormType, bool reset)
{
    disconnectTankSignals();
    m_wormTank.save(); // save current
	m_wormTank.start(wormType, reset);
    setStatusText("Training " + wormType);
    startRun();
}

/**
* @brief Show dialog to choose worm type and start the tank running
*/
void WormTankModelView::startBattle(QString wormType1, QString wormType2, bool reset)
{
    disconnectTankSignals();
    m_wormTank.save(); // save current
    m_wormTank.startBattle(wormType1, wormType2, reset);
    m_battleEndConnection = QObject::connect(&m_wormTank, SIGNAL(matchEnded()), this, SLOT(battleEnded()));
    setStatusText(m_wormTank.name());
    startRun();
}

/**
 * @brief Battle has ended to show the result
 */
void WormTankModelView::battleEnded()
{
    disconnectTankSignals();    
    QString msg;
    if (m_wormTank.mode() == WormTank::BattleMode)
    {
        msg = "The battle has been won by ";
        msg += m_wormTank.battleWinner();
    } else
    {
        msg = "The arena has been won by ";
        msg += m_wormTank.arenaWinner();

    }
    setFinishedText(msg);
    setFinishedTextVisible(true);
    setStatusText(msg);
}

/**
 * @brief Start the league playing
 */
void WormTankModelView::startLeague()
{
    disconnectTankSignals();
    m_wormTank.save(); // save current
    m_wormTank.startLeague();
    m_leagueEndConnection = QObject::connect(&m_wormTank, SIGNAL(leagueFinished()), this, SLOT(leagueEnded()));
    m_leagueMatchEndConnection = QObject::connect(&m_wormTank, SIGNAL(matchEnded()), this, SLOT(leagueMatchEnded()));
    setStatusText(m_wormTank.name());
    startRun();
}

/**
 * @brief Called when a league match has just ended
 */
void WormTankModelView::leagueMatchEnded()
{
    QString msg("Last league game result: ");
    QString team1, team2;
    int score1, score2;
    m_wormTank.getLeagueMatchResult(team1, team2, score1, score2);
    if (score1 == score2)
    {
        msg += "a draw between " + team1 + team2;
    } else if (score1 > score2)
    {
        msg += team1 + " beat " + team2 + " " + QString::number(score1) + " to " + QString::number(score2);
    } else
    {
        msg += team2 + " beat " + team1 + " " + QString::number(score2) + " to " + QString::number(score1);
    }

    setStatusText(msg);
}

/**
 * @brief League has ended to show the result
 */
void WormTankModelView::leagueEnded()
{
    disconnectTankSignals();
    QString msg("The League has been won by ");
    msg += m_wormTank.leagueWinner();
    setStatusText(msg);
    setFinishedText(msg);
    setFinishedTextVisible(true);
}


/**
* @brief Start arena where all worms compete against each other
*/
void WormTankModelView::startArena()
{
    disconnectTankSignals();
    m_wormTank.save(); // save current
    m_wormTank.startArena();
    m_battleEndConnection = QObject::connect(&m_wormTank, SIGNAL(matchEnded()), this, SLOT(battleEnded()));
    setStatusText(m_wormTank.name());
    startRun();
}

/**
 * @brief Start a new run of a tank
 */
void WormTankModelView::startRun()
{
    setFinishedTextVisible(false);
	if (!m_timer.isActive())
	{
		QObject::connect(&m_timer, SIGNAL(timeout()), &m_wormTank, SLOT(step()));
		m_timer.start(20);
	}
}

/**
 * @brief Disconnect signals from worm tank
 */
void WormTankModelView::disconnectTankSignals()
{
    switch(m_wormTank.mode())
    {
    case WormTank::BattleMode:
    case WormTank::ArenaMode:
        QObject::disconnect(m_battleEndConnection);
        break;
    case WormTank::LeagueMode:
        QObject::disconnect(m_leagueEndConnection);
        QObject::disconnect(m_leagueMatchEndConnection);
        break;
    case WormTank::NormalMode:
        // No signals for normal mode
        break;
    }
}


/**
 * @brief Start the worm tank running when the last run closed
 */
void WormTankModelView::startLast()
{
    QSettings settings(QSettings::IniFormat, QSettings::UserScope,"BuckleySoft","WormTank");
    QString startType = settings.value("StartType", "Random").toString();
    WormTank::Mode mode = WormTank::Mode(settings.value("Mode", int(WormTank::NormalMode)).toInt());
    try
    {
        switch(mode)
        {
        case WormTank::NormalMode:
            start(startType, false);
            setStatusText("Continuing training " + startType);
            break;

        case WormTank::BattleMode:
            if (m_wormTank.loadBattle())
            {
                if (m_wormTank.battleWon())
                {
                    // Use a timer to reshow the battle ended message to allow ui
                    // to be setup
                    QTimer::singleShot(200,this, SLOT(battleEnded()));
                } else
                {
                    setStatusText("Continuing " + m_wormTank.name());
                    QObject::connect(&m_wormTank, SIGNAL(matchEnded()), this, SLOT(battleEnded()));
                }
                startRun();
            } else
            {
                setStatusText("Unable to load battle");
                throw "Unable to load battle";
            }
            break;

        case WormTank::LeagueMode:
            if (m_wormTank.loadLeague())
            {
                if (m_wormTank.leagueWon())
                {
                    // Use a timer to reshow the battle ended message to allow ui
                    // to be setup
                    QTimer::singleShot(200,this, SLOT(leagueEnded()));
                } else
                {
                    QObject::connect(&m_wormTank, SIGNAL(leagueFinished()), this, SLOT(leagueEnded()));
                    QObject::connect(&m_wormTank, SIGNAL(matchEnded()), this, SLOT(leagueMatchEnded()));
                    setStatusText("Continuing " + m_wormTank.name());
                }
                startRun();
            } else
            {
                setStatusText("Unable to load league");
                throw "Unable to load league";
            }
            break;

        case WormTank::ArenaMode:
            if (m_wormTank.loadArena())
            {
                if (m_wormTank.arenaWon())
                {
                    // Use a timer to reshow the battle ended message to allow ui
                    // to be setup
                    QTimer::singleShot(200,this, SLOT(battleEnded()));
                } else
                {
                    setStatusText("Continuing " + m_wormTank.name());
                    QObject::connect(&m_wormTank, SIGNAL(matchEnded()), this, SLOT(battleEnded()));
                }
                startRun();
            } else
            {
                setStatusText("Unable to load arena");
                throw "Unable to load arena";
            }
            break;


        }
    } catch(...)
    {
        start("Random", false);
    }
}

/**
 * @brief Call from QML when exiting the application
 *
 * This methods saves the current worm tank and remembers what
 * was running as you exit.
 */
void WormTankModelView::exiting()
{
    m_wormTank.save();
    QSettings settings(QSettings::IniFormat, QSettings::UserScope,"BuckleySoft","WormTank");
    if (m_wormTank.mode() == WormTank::NormalMode)
    {
        settings.setValue("StartType", m_wormTank.name());
    }
    settings.setValue("Mode", int (m_wormTank.mode()));

    qApp->quit();
}

/**
 * @brief Show a window to list the worms currently running
 */
void WormTankModelView::wormList()
{
	if (!m_wormListView)
	{
        m_wormListView = new QQuickView();
        m_wormListView->setSource(QUrl("qrc:/WormList.qml"));
        QQmlContext *ctx = m_wormListView->rootContext();
        ctx->setContextProperty("wormTankModelView", this);

		updateWormList();

        m_wormListView->setTitle("WormTank - list of alive worms");
        m_wormListView->setResizeMode(QQuickView::SizeRootObjectToView);
        m_wormListView->show();
        QObject::connect(m_wormListView, SIGNAL(visibleChanged(bool)),
                         this, SLOT(wormListHidden()));

        QObject::connect(&m_wormTank, SIGNAL(wormsLeftChanged(uint)),
			this, SLOT(updateWormList(void)));
    }
}

/**
 * @brief Update the worm list
 */
void WormTankModelView::updateWormList()
{
	if (!m_wormListView) return;

    if (m_wormList == nullptr) m_wormList = new QList<QObject *>;
	// Reset counts for worms found
    for (auto &wormCount : *m_wormList)
	{
        static_cast<WormCount *>(wormCount)->setCount(0);
	}

    bool resetList = false;

	// Update list from current set of worms
	for (auto worm : m_wormTank.worms())
	{
        QString fullName = worm->fullName().c_str();
        int colour = worm->colour();
		int insPos = 0;
		WormCount *wormCount;
		while (insPos < m_wormList->size())
		{
			wormCount = static_cast<WormCount *>(m_wormList->value(insPos));
            if (fullName <= wormCount->name()) break;
			insPos++;
		}
        if (insPos < m_wormList->size())
		{
            if (wormCount->name() == fullName)
            {
                while (insPos < m_wormList->size())
                {
                    wormCount = static_cast<WormCount *>(m_wormList->value(insPos));
                    if (wormCount->name() != fullName
                            || colour <= wormCount->colourIndex())
                    {
                       break;
                    }
                    insPos++;
                }
            }
        }
        if (insPos == m_wormList->size())
        {
            m_wormList->append(new WormCount(fullName, colour));
            resetList = true;
        } else if (wormCount->name() == fullName && wormCount->colourIndex() == colour)
		{
            wormCount->incCount();
        } else
        {
            m_wormList->insert(insPos, new WormCount(fullName, colour));
            resetList = true;
		}
	}

	// Remove and dead worms
	int delPos = 0;
	while (delPos < m_wormList->size())
	{
        WormCount *wormCount = static_cast<WormCount *>(m_wormList->value(delPos));
        if (wormCount->count() == 0)
        {
            m_wormList->removeAt(delPos);
            resetList = true;
        } else delPos++;
	}

    if (resetList)
    {
        // Reset context if number of items in the list changed
        QQmlContext *ctxt = m_wormListView->rootContext();
        ctxt->setContextProperty("worms", QVariant::fromValue(*m_wormList));
    }
}

/**
 * @brief Worm list window hidden so stop updates to the list
 */
void WormTankModelView::wormListHidden()
{
    QObject::disconnect(&m_wormTank, SIGNAL(wormsLeftChanged(uint)),
		this, SLOT(updateWormList(void)));

    m_wormListView->deleteLater();
	m_wormListView = nullptr;
    delete m_wormList;
    m_wormList = nullptr;
}

/**
 * @brief View the current league
 */
void WormTankModelView::viewLeague()
{
    if (!m_leagueView)
    {
        m_leagueView = new QQuickView();
        m_leagueView->setSource(QUrl("qrc:/LeagueView.qml"));
        QQmlContext *ctx = m_leagueView->rootContext();
        ctx->setContextProperty("wormTankModelView", this);
        ctx->setContextProperty("wormTank", &m_wormTank);

        m_leagueView->setTitle("WormTank - League");
        m_leagueView->setResizeMode(QQuickView::SizeRootObjectToView);
    }
    m_leagueView->show();
}

/**
 * @brief View the History of the current worm type
 */
void WormTankModelView::viewHistory()
{
    if (!m_historyView)
    {
        m_historyView = new QQuickView();
        m_historyView->setSource(QUrl("qrc:/HistoryView.qml"));
        QQmlContext *ctx = m_historyView->rootContext();
        ctx->setContextProperty("wormTankModelView", this);
        ctx->setContextProperty("wormTank", &m_wormTank);

        m_historyView->setTitle("WormTank - History");
        m_historyView->setResizeMode(QQuickView::SizeRootObjectToView);
    }
    m_historyView->show();
}
