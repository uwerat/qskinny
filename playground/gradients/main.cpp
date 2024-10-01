/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "GradientView.h"

#include <SkinnyNamespace.h>
#include <SkinnyShortcut.h>

#include <QskGradient.h>
#include <QskGradientDirection.h>
#include <QskLinearBox.h>
#include <QskTextInput.h>
#include <QskTextLabel.h>
#include <QskWindow.h>
#include <QskRgbValue.h>
#include <QskSkinManager.h>

#include <QDoubleValidator>
#include <QFontMetricsF>
#include <QGuiApplication>

namespace
{
    class InputValidator : public QDoubleValidator
    {
      public:
        InputValidator( QObject* parent = nullptr )
            : QDoubleValidator( parent )
        {
            setRange( -9.99, 9.99 );
            setDecimals( 2 );
            setNotation( QDoubleValidator::StandardNotation );

            auto locale = this->locale();
            locale.setNumberOptions( QLocale::RejectGroupSeparator );

            setLocale( locale );
        }
    };

    class NumberInput : public QskLinearBox
    {
        Q_OBJECT

      public:
        NumberInput( const QString& label, qreal value, QQuickItem* parent = nullptr )
            : QskLinearBox( Qt::Horizontal, parent )
        {
            new QskTextLabel( label, this );

            m_input = new QskTextInput( this );
            m_input->setValidator( new InputValidator( m_input ) );
            m_input->setInputText( QString::number( value ) );

            const QFontMetricsF fm( m_input->font() );
            m_input->setFixedWidth( fm.horizontalAdvance( "-0.000" ) );

            connect( m_input, &QskTextInput::editingChanged,
                this, [ this ]( bool on ) { if ( !on ) Q_EMIT valueChanged(); } );

            setSizePolicy( QskSizePolicy::Fixed, QskSizePolicy::Fixed );
        }

        qreal value() const
        {
            return m_input->inputText().toDouble();
        }

      Q_SIGNALS:
        void valueChanged();

      private:
        QskTextInput* m_input;
    };

    class VectorBox : public QskLinearBox
    {
        Q_OBJECT

      public:
        VectorBox( QQuickItem* parent = nullptr )
            : QskLinearBox( Qt::Horizontal, parent )
        {
            setSizePolicy( Qt::Vertical, QskSizePolicy::Fixed );
            setExtraSpacingAt( Qt::BottomEdge | Qt::RightEdge );

            setSpacing( 10 );

            m_inputs[0] = new NumberInput( "X1", 0, this );
            m_inputs[1] = new NumberInput( "Y1", 0, this );
            m_inputs[2] = new NumberInput( "X2", 1, this );
            m_inputs[3] = new NumberInput( "Y2", 1, this );

            for ( auto input : m_inputs )
            {
                connect( input, &NumberInput::valueChanged,
                    this, [this] { Q_EMIT vectorChanged( vector() ); } );
            }
        }

        QLineF vector() const
        {
            const auto x1 = m_inputs[0]->value();
            const auto y1 = m_inputs[1]->value();
            const auto x2 = m_inputs[2]->value();
            const auto y2 = m_inputs[3]->value();

            return QLineF( x1, y1, x2, y2 );
        }


      Q_SIGNALS:
        void vectorChanged( const QLineF& );

      private:
        NumberInput* m_inputs[4];
    };

    class GradientBox : public QskLinearBox
    {
        Q_OBJECT

      public:
        GradientBox( QQuickItem* parent = nullptr )
            : QskLinearBox( Qt::Horizontal, 2, parent )
        {
            setSpacing( 10 );

            for ( int i = 0; i < GradientView::NumNodeTypes; i++ )
            {
                const auto nodeType = static_cast< GradientView::NodeType >( i );
                m_views[i] = new GradientView( nodeType, this );
            }

            setVector( QLineF( 0, 0, 1, 0 ) );
            setColors( { Qt::green, Qt::red, Qt::yellow, Qt::cyan, Qt::darkCyan } );
        }

      public Q_SLOTS:
        void setVector( const QLineF& vector )
        {
            m_gradient.setLinearDirection( vector );
            updateViews();
        }

        void setStops( const QskGradientStops& stops )
        {
            m_gradient.setStops( stops );
            updateViews();
        }

      private:
        void setColors( const QVector< QColor >& colors )
        {
            const auto step = 1.0 / colors.size();

            QskGradientStops stops;

            for ( int i = 0; i < colors.size(); i++ )
            {
                stops += { i * step, colors[i] };
                stops += { ( i + 1 ) * step, colors[i] };
            }

            setStops( stops );
        }

        void updateViews()
        {
            for ( auto view : m_views )
            {
                if ( view )
                    view->setGradient( m_gradient );
            }
        }

        QskGradient m_gradient;
        GradientView* m_views[ GradientView::NumNodeTypes ];
    };

    class MainView : public QskLinearBox
    {
      public:
        MainView( QQuickItem* parent = nullptr )
            : QskLinearBox( Qt::Vertical, parent )
        {
            auto vectorBox = new VectorBox( this );
            vectorBox->setMargins( 10, 10, 10, 0 );

            auto gradientBox = new GradientBox( this );
            gradientBox->setMargins( 10 );

            gradientBox->setVector( vectorBox->vector() );

            connect( vectorBox, &VectorBox::vectorChanged,
                gradientBox, &GradientBox::setVector );
        }
    };
}

int main( int argc, char** argv )
{
    qputenv( "QT_IM_MODULE", QByteArray() ); // no virtual keyboard

    QGuiApplication app( argc, argv );

    Skinny::init(); // we need a skin
    SkinnyShortcut::enable( SkinnyShortcut::Quit | SkinnyShortcut::DebugBackground );
    qskSkinManager->setSkin( "fusion" );

    QskWindow window;
    window.setColor( QskRgb::Wheat );
    window.addItem( new MainView() );
    window.resize( 800, 600 );
    window.show();

    return app.exec();
}

#include "main.moc"
