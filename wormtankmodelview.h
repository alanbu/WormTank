#ifndef WORMTANKMODELVIEW_H
#define WORMTANKMODELVIEW_H

#include "wormtank.h"
#include "wormcount.h"
#include <QObject>
#include <QTimer>
#include <QStringList>
#include <QQuickView>

class WormTankModelView : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QStringList wormTypes READ wormTypes NOTIFY wormTypesChanged)
    Q_PROPERTY(QString finishedText READ finishedText NOTIFY finishedTextChanged)
    Q_PROPERTY(bool finishedTextVisible READ finishedTextVisible NOTIFY finishedTextVisibleChanged)
    Q_PROPERTY(QString statusText READ statusText NOTIFY statusTextChanged)

public:
    explicit WormTankModelView(WormTank &wormTank, QObject *parent = 0);
    virtual ~WormTankModelView();

    void startLast();

    QStringList wormTypes();
    QString finishedText() const {return m_finishedText;}
    bool finishedTextVisible() const {return m_finishedTextVisible;}
    QString statusText() const {return m_statusText;}

    void setFinishedText(QString newText);
    void setFinishedTextVisible(bool visible);
    void setStatusText(QString newText);

signals:
    void wormTypesChanged(QStringList wormTypes);
    void finishedTextChanged(QString text);
    void finishedTextVisibleChanged(bool visible);
    void statusTextChanged(QString text);

public slots:
    void start(QString wormType, bool reset);
	void startBattle(QString wormType1, QString wormType2, bool reset);
    void startLeague();

    void battleEnded();
    void leagueEnded();
    void leagueMatchEnded();
    void exiting();
    void showHelp();

    void wormList();
    void updateWormList();
    void wormListHidden();
    void viewLeague();

protected:
    void startRun();
    void disconnectTankSignals();

protected:
    WormTank &m_wormTank;
    QTimer m_timer;
    QStringList m_wormTypes;
    QQuickView *m_wormListView;
    QQuickView *m_leagueView;
    QList<QObject *> *m_wormList;
    QString m_finishedText;
    bool m_finishedTextVisible;
    QString m_statusText;
    QMetaObject::Connection m_battleEndConnection;
    QMetaObject::Connection m_leagueEndConnection;
    QMetaObject::Connection m_leagueMatchEndConnection;
};

#endif // WORMTANKMODELVIEW_H
