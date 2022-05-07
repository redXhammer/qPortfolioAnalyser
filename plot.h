#ifndef PLOT_H
#define PLOT_H

#include <qwt_plot.h>
#include <qwt_series_data.h>
#include <QDate>

class Depot;
class DepotPos;

class Plot : public QwtPlot
{
public:
    Plot(Depot* depot, QWidget *parent = NULL );

//protected:
//    virtual void resizeEvent( QResizeEvent * );

private:
    void populate();
    //void updateGradient();
    Depot* m_depot;
};

class DepotPosSamples : public QwtSeriesData<QPointF>
{
public:
    DepotPosSamples(DepotPos*, QDate);
    size_t size() const override;
    QPointF sample( size_t i ) const override;
    QRectF boundingRect() const override;

private:
    DepotPos* m_depotPos;
    QDate     m_StartDate;
};

#endif // PLOT_H
