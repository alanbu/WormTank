#include "trendchart.h"
#include <QPen>
#include <QPainter>

TrendChart::TrendChart(QQuickItem *parent) :
    QQuickPaintedItem(parent),
    m_model(nullptr),
    m_background("white"),
    m_colour("red"),
    m_roleName("display"),
    m_role(-1)
{
}

/**
 * @brief Set the list model to get the points from
 * @param model new model
 */
void TrendChart::setModel(QAbstractListModel *model)
{
    if (model != m_model)
    {
        if (m_model)
        {
            // disconnect old
            disconnect();
        }
        m_model = model;
        m_role = -1;
        // Connect new to force an update
        QObject::connect(m_model, &QAbstractListModel::modelReset, this, &TrendChart::refresh);
        QObject::connect(m_model, &QAbstractListModel::rowsInserted, this, &TrendChart::refresh);
        QObject::connect(m_model, &QAbstractListModel::rowsRemoved, this, &TrendChart::refresh);
        update();
        emit modelChanged(m_model);
    }
}

/**
 * @brief Role name to retrieve the data for
 * @param name role name from model
 */
void TrendChart::setRole(QString name)
{
    if (name != m_roleName)
    {
        m_roleName = name;
        m_role = -1;
        emit roleChanged(m_roleName);
        update();
    }
}

/**
 * @brief Line colour for chart
 * @param colour new colour
 */
void TrendChart::setColour(QColor colour)
{
    if (colour != m_colour)
    {
        m_colour = colour;
        emit colourChanged(m_colour);
        update();
    }
}

/**
 * @brief Background colour for the chart
 * @param background new background colour
 */
void TrendChart::setBackground(QColor background)
{
    if (background != m_background)
    {
        m_background = background;
        emit backgroundChanged(m_background);
        update();
    }
}


/**
 * @brief paint the item
 * @param painter painter to paint to
 */
void TrendChart::paint(QPainter *painter)
{
    painter->fillRect(boundingRect(), m_background);
    // Need two generations before we can draw anything
    if (m_model == nullptr || m_model->rowCount() < 2) return;

    QRectF drawRect = boundingRect().adjusted(2,2,-2,-2);
    QPen pen(m_colour, 2);
    painter->setPen(pen);

    if (m_role == -1)
    {
        QByteArray lookFor(m_roleName.toLatin1());
        auto hash = m_model->roleNames();
        auto i = hash.constBegin();
        while (i != hash.constEnd())
        {
            if (i.value() == lookFor)
            {
                m_role = i.key();
                break;
            }
            ++i;
        }
        if (m_role == -1) return;
    }

    painter->setRenderHints(QPainter::Antialiasing, true);
    int numPoints = m_model->rowCount();
    QPoint pts[numPoints];

    qreal xStep = drawRect.width() / (numPoints-1);
    qreal x = drawRect.left();
    int y;
    int minY = INT_MAX;
    int maxY = 0;

    for (int j = 0; j < numPoints; j++)
    {
        QModelIndex index = m_model->index(j,0);
        y = m_model->data(index, m_role).toInt();
        // History is from last to newest so reverse it
        pts[numPoints - j - 1].setY(y);
        if (y < minY) minY = y;
        if (y > maxY) maxY = y;
        pts[j].setX(x);
        x += xStep;
    }

    // Round results to make ranges end on 10s
    minY = minY / 10;
    minY *= 10;
    maxY = maxY + 10 - (maxY % 10);

    double multY = double(drawRect.height())/double(maxY - minY);
    for (int j = 0; j < numPoints; j++)
    {
        pts[j].setY(drawRect.bottom() - int((pts[j].y() - minY) * multY));
    }
    for (int j = 1; j < numPoints; j++)
    {
        painter->drawLine(pts[j-1], pts[j]);
    }
}
