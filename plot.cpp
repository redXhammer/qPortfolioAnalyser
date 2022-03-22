#include "plot.h"

#include <qwt_plot_curve.h>
#include <qwt_legend.h>
#include <qwt_plot_canvas.h>
#include <qwt_plot_panner.h>
#include <qwt_plot_magnifier.h>
#include <qwt_date_scale_draw.h>
#include <qwt_date_scale_engine.h>
#include <depot.h>

Plot::Plot(Depot *depot, QWidget *parent ):
    QwtPlot( parent ), m_depot(depot)
{
    setAutoFillBackground( true );
    setPalette( QPalette( QColor( 165, 193, 228 ) ) );
    //updateGradient();

    setTitle( "A Simple QwtPlot Demonstration" );
    insertLegend( new QwtLegend(), QwtPlot::RightLegend );

    // axes
    //setAxisTitle( xBottom, "x -->" );
    //setAxisScale( xBottom, 0.0, 10.0 );
    setAxisScaleDraw( xBottom, new QwtDateScaleDraw() );
    setAxisScaleEngine(xBottom, new QwtDateScaleEngine() );

    //setAxisTitle( yLeft, "y -->" );
    //setAxisScale( yLeft, -1.0, 1.0 );
    setAxisTitle( yLeft, QString( "Price [EUR]" ) );

    // canvas
    QwtPlotCanvas *canvas = new QwtPlotCanvas();
    canvas->setLineWidth( 1 );
    canvas->setFrameStyle( QFrame::Box | QFrame::Plain );
    //canvas->setBorderRadius( 15 );

    QPalette canvasPalette( Qt::white );
    canvasPalette.setColor( QPalette::Foreground, QColor( 133, 190, 232 ) );
    canvas->setPalette( canvasPalette );

    setCanvas( canvas );

    // panning with the left mouse button
    ( void ) new QwtPlotPanner( canvas );

    // zoom in/out with the wheel
    ( void ) new QwtPlotMagnifier( canvas );

    populate();
}

void Plot::populate()
{

    QList<DepotPos*>::iterator itList = m_depot->begin();
    for (;itList != m_depot->end();itList++)
    {
        if (*itList == nullptr) continue;
        //if ((*itList)->iDepotNr == iAt) return *itList;

        // Insert new curves
        QwtPlotCurve *cSin = new QwtPlotCurve( (*itList)->GetName() );
        cSin->setRenderHint( QwtPlotItem::RenderAntialiased );
        cSin->setLegendAttribute( QwtPlotCurve::LegendShowLine, true );
        cSin->setPen( Qt::red );
        cSin->attach( this );
        //cSin->setSamples()

        QPolygonF points;
        points << QPointF( 0.0, 4.4 ) << QPointF( 1.0, 3.0 )
            << QPointF( 2.0, 4.5 ) << QPointF( 3.0, 6.8 )
            << QPointF( 4.0, 7.9 ) << QPointF( 5.0, 7.1 );
        cSin->setSamples( points );


    }






    //QwtPlotCurve *cCos = new QwtPlotCurve( "y = cos(x)" );
    //cCos->setRenderHint( QwtPlotItem::RenderAntialiased );
    //cCos->setLegendAttribute( QwtPlotCurve::LegendShowLine, true );
    //cCos->setPen( Qt::blue );
    //cCos->attach( this );
}
