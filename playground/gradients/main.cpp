/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#include "GradientView.h"

#include <SkinnyNamespace.h>

#include <QskGradient.h>
#include <QskLinearBox.h>
#include <QskWindow.h>

#include <QGuiApplication>

namespace
{
    class MainView : public QskLinearBox
    {
        Q_OBJECT

      public:
        MainView( QQuickItem* parent = nullptr )
            : QskLinearBox( Qt::Horizontal, 2, parent )
        {
            for ( int i = 0; i < GradientView::NumNodeTypes; i++ )
            {
                const auto nodeType = static_cast< GradientView::NodeType >( i );
                m_views[i] = new GradientView( nodeType, this );
            }

            showColors( { Qt::green, Qt::red, Qt::yellow, Qt::cyan, Qt::darkCyan } );
        }

        void showColors( const QVector< QColor >& colors )
        {
            const auto step = 1.0 / colors.size();

            QskGradientStops stops;

            for ( int i = 0; i < colors.size(); i++ )
            {
                stops += { i * step, colors[i] };
                stops += { ( i + 1 ) * step, colors[i] };
            }

            QskGradient gradient;
#if 1
            gradient.setLinearDirection( 0.0, 0.0, 1.0, 1.0 );
#endif
#if 0
            gradient.setLinearDirection( 0.2, 0.2, 0.7, 0.5 );
            gradient.setSpreadMode( QskGradient::PadSpread );
#endif
#if 0
            gradient.setRadialDirection( 0.25, 0.75, 0.25 );
            gradient.setSpreadMode( QskGradient::ReflectSpread );
#endif
#if 0
            gradient.setConicDirection( 0.25, 0.75, 0, 90 );
            gradient.setSpreadMode( QskGradient::ReflectSpread );
#endif

            gradient.setStops( stops );

            showGradient( gradient );
        }

      public Q_SLOTS:
        void showGradient( const QskGradient& gradient )
        {
            for ( auto view : m_views )
            {
                if ( view )
                    view->setGradient( gradient );
            }
        }

      private:
        GradientView* m_views[ GradientView::NumNodeTypes ];
    };
}

int main( int argc, char** argv )
{
    QGuiApplication app( argc, argv );
    Skinny::init(); // we need a skin

    QskWindow window;
    window.addItem( new MainView() );
    window.resize( 600, 600 );
    window.show();

    return app.exec();
}

#include "main.moc"
