#ifndef TRENDCHART_H
#define TRENDCHART_H

#include <QQuickPaintedItem>
#include <QAbstractListModel>

class TrendChart : public QQuickPaintedItem
{
    Q_OBJECT
    Q_PROPERTY(QAbstractListModel *model READ model WRITE setModel NOTIFY modelChanged)
    Q_PROPERTY(QString role READ role WRITE setRole NOTIFY roleChanged)
    Q_PROPERTY(QColor colour READ colour WRITE setColour NOTIFY colourChanged)
    Q_PROPERTY(QColor background READ background WRITE setBackground NOTIFY backgroundChanged)
public:
    explicit TrendChart(QQuickItem *parent = 0);

    QAbstractListModel *model() const {return m_model;}
    void setModel(QAbstractListModel *model);
    QString role() const {return m_roleName;}
    void setRole(QString name);
    QColor colour() const {return m_colour;}
    void setColour(QColor colour);
    QColor background() const {return m_background;}
    void setBackground(QColor background);

    void paint(QPainter *painter);

signals:
     void modelChanged(QAbstractListModel *model);
     void roleChanged(QString role);
     void colourChanged(QColor colour);
     void backgroundChanged(QColor background);

public slots:
     // Used as update needs a parameter so can't be connected to ListModel easily
     void refresh() {update();}

private:
     QAbstractListModel *m_model;
     QColor m_background;
     QColor m_colour;
     QString m_roleName;
     int m_role;
};

#endif // TRENDCHART_H
