#ifndef PLOT_H
#define PLOT_H

#include <qwt_plot.h>

class Depot;

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

#endif // PLOT_H
