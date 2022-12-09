/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#include <SkinnyNamespace.h>
#include <QskControl.h>
#include <QskGradient.h>

#include <QWidget>
#include <QApplication>
#include <QQuickWidget>
#include <QHBoxLayout>
#include <QPainter>

class BoxQsk: public QQuickWidget
{
  public:
    BoxQsk( QWidget* parent = nullptr )
        : QQuickWidget( parent )
    {
        setSizePolicy( QSizePolicy::Ignored, QSizePolicy::Ignored );

        setContentsMargins( QMargins() );
        setResizeMode( QQuickWidget::SizeRootObjectToView );

        m_control = new QskControl();
        setContent( QUrl(), nullptr, m_control );
    }

    void showGradient( const QGradient& gradient )
    {
        QskGradient qskGradient( gradient );

        /*
            Eliminate the useless offsets that have been added to work around
            QGradients limitation to have stops at the same position
         */

        auto stops = qskGradient.stops();
        for ( int i = 1; i < stops.count(); i++ )
        {
            if ( stops[i].position() - stops[i-1].position() < 1e-5 )
                stops[i].setPosition( stops[i-1].position() );
        }

        qskGradient.setStops( stops );

        m_control->setBackground( qskGradient );
    }

  private:
    QskControl* m_control;
};

class BoxQt: public QWidget
{ 
  public:
    BoxQt( QWidget* parent = nullptr )
        : QWidget( parent )
    {
        setContentsMargins( QMargins() );
        setSizePolicy( QSizePolicy::Ignored, QSizePolicy::Ignored );
    }

    void showGradient( const QGradient& gradient )
    {
        m_gradient = gradient;
        m_gradient.setCoordinateMode( QGradient::ObjectMode );

        update();
    }

  protected:
    void paintEvent( QPaintEvent* ) override
    {
        QPainter painter( this );
        painter.fillRect( contentsRect(), m_gradient );
    }

  private:
    QGradient m_gradient;
};

class GradientBox: public QWidget
{
    Q_OBJECT

  public:
    GradientBox( QWidget* parent = nullptr )
        : QWidget( parent )
    {
        m_qtBox = new BoxQt();
        m_qskBox = new BoxQsk();

        auto layout = new QHBoxLayout( this );
        layout->addWidget( m_qskBox );
        layout->addWidget( m_qtBox );

        showGradient( { Qt::green, Qt::red, Qt::yellow, Qt::cyan, Qt::darkCyan } );
    }

    void showGradient( const QVector< QColor >& colors )
    {
        const auto step = 1.0 / colors.size();

        QGradientStops stops;

        for ( int i = 0; i < colors.size(); i++ )
        {
            auto pos = i * step;
            if ( i > 0 )
                pos += 1e-6; // QGradient does not support stops at the same position

            stops += { pos, colors[i] };
            stops += { ( i + 1 ) * step, colors[i] };
        }

        QLinearGradient gradient( 0.0, 0.0, 0.0, 1.0 );
        gradient.setStops( stops );

        showGradient( gradient );
    }

  public Q_SLOTS:
    void showGradient( const QGradient& gradient )
    {
        m_qtBox->showGradient( gradient );
        m_qskBox->showGradient( gradient );
    }

  private:
    BoxQt* m_qtBox;
    BoxQsk* m_qskBox;
};

int main( int argc, char** argv )
{
    QApplication app( argc, argv );
    Skinny::init(); // we need a skin

    GradientBox box;

    box.resize( 600, 600 );
    box.show();

    return app.exec();
}

#include "main.moc"
