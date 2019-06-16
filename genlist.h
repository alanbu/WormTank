#ifndef GENLIST_H
#define GENLIST_H

#include <QAbstractListModel>
class WormTank;

/**
 * @brief One item for GenList
 */
class GenListItem
{
public:
    GenListItem(QString wormType, uint generation) :
        m_wormType(wormType),
        m_generation(generation)
    {
    }

    GenListItem() : m_generation(0)
    {
    }

    QString wormType() const {return m_wormType;}
    uint generation() const {return m_generation;}
    void setGeneration(uint newGen) {m_generation = newGen;}

private:
    QString m_wormType;
    uint m_generation;
};

/**
 * @brief List of worm types and their current generation
 */
class GenList : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit GenList(QObject *parent = 0);
    void setup(WormTank *wormTank);

    void updateGeneration(QString wormType, uint newGen);

    enum Roles
    {
       WormTypeRole = Qt::UserRole + 0,
       GenerationRole
    };

    // Abstract table model overrides
    int rowCount(const QModelIndex &/*parent = QModelIndex()*/) const {return (int)m_items.size();}
    QHash<int, QByteArray> roleNames() const;
    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;


signals:

public slots:

private:
    std::vector<GenListItem> m_items;
};

#endif // GENLIST_H
