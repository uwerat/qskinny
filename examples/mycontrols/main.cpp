/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "MySkin.h"
#include "MyToggleButton.h"

#include <SkinnyShapeProvider.h>
#include <SkinnyShortcut.h>

#include <QskWindow.h>
#include <QskObjectCounter.h>
#include <QskFocusIndicator.h>
#include <QskBox.h>
#include <QskLinearBox.h>
#include <QskSkinManager.h>
#include <QskAnimationHint.h>
#include <QskSkinTransition.h>
#include <QskSetup.h>
#include <QskSkin.h>
#include <QskSkinlet.h>
#include <QskSGNode.h>
#include <QskTickmarksMetrics.h>
#include <QskTickmarksNode.h>
#include <QskScaleTickmarks.h>
#include <QskTextLabel.h>
#include <QskAnimator.h>
#include <QskTickmarksMetrics.h>

#include <QGuiApplication>

#include <qmath.h>

/*
    This example is intended to show how to:

        - implement custom controls
        - set up skins, customizing these controls

    For demonstration purposes we limit the set of controls
    being used:

        - QskPushButton
        - QskBox
        - QskFocusIndicator
        - MyToggleButton
 */

class ContentBox : public QskBox
{
  public:
    ContentBox( QQuickItem* parent = nullptr )
        : QskBox( parent )
    {
        setAutoLayoutChildren( true );

        auto layout = new QskLinearBox( Qt::Horizontal, 2, this );
        layout->setMargins( 10 );
        layout->setSpacing( 20 );
        layout->setDefaultAlignment( Qt::AlignCenter );
        layout->setExtraSpacingAt( Qt::BottomEdge );

        const QskSizePolicy sizePolicy(
            QskSizePolicy::MinimumExpanding, QskSizePolicy::MinimumExpanding );

        {
            auto button = new MyToggleButton( layout );
            button->setSizePolicy( sizePolicy );

            button->setText( false, "Off" );
            button->setText( true, "On" );
        }

        {
            auto button = new MyToggleButton( layout );
            button->setSizePolicy( sizePolicy );

            button->setIcon( false, "image://shapes/Diamond/Khaki" );
            button->setIcon( true, "image://shapes/Ring/Khaki" );
        }
    }
};

struct Tickmarks final : public QskControl
{
  public:
      QSK_SUBCONTROLS(Ticks)
      using QskControl::QskControl;
};

QSK_SUBCONTROL(Tickmarks, Ticks)

class TickmarksSkinlet final : public QskSkinlet
{
    Q_GADGET
  public:
    enum NodeRoles
    {
        TickmarksRole
    };

    Q_INVOKABLE TickmarksSkinlet(QskSkin* skin = nullptr) : QskSkinlet(skin)
    {
        setNodeRoles( { TickmarksRole } );
    }

    QSGNode* updateSubNode( const QskSkinnable* const skinnable, const quint8 nodeRole, QSGNode* const node ) const override
    {
        const auto* const q = static_cast< const Tickmarks* >( skinnable );

        if(nodeRole == TickmarksRole) 
        {
            auto* const tickmarksNode = QskSGNode::ensureNode<QskTickmarksNode>(node);
            
            const auto rect = q->contentsRect();

            QskScaleTickmarks tickmarks;
            QVector<qreal> major, medium, minor;
            for ( int i = -100; i <= +100; ++i ) 
            {
                if(i % 50 == 0) major << i;
                else if(i % 10 == 0) medium << i;
                else minor << i;
            }
            tickmarks.setMajorTicks(major);
            tickmarks.setMediumTicks(medium);
            tickmarks.setMinorTicks(minor);

            const auto metric = q->tickmarksMetricsHint(Tickmarks::Ticks);
            tickmarksNode->update(
                Qt::darkGray, rect, { -100, +100 }, tickmarks, 1, Qt::Horizontal, Qt::AlignBottom, metric );

            return tickmarksNode;
        }

        return nullptr;
    }
};

class Window : public QskWindow
{
  public:
    Window()
    {
        auto* const layout = new QskLinearBox(Qt::Vertical, contentItem());
        {
            (void) new QskTextLabel("Default", layout);
            auto* const tickmarks = new Tickmarks(layout);
            auto* const skinlet = new TickmarksSkinlet;
            tickmarks->setSkinlet(skinlet);
            skinlet->setOwnedBySkinnable(true);
        }
        {
            (void) new QskTextLabel("Custom", layout);
            auto* const tickmarks = new Tickmarks(layout);
            tickmarks->setTickmarksMetricsHint(Tickmarks::Ticks, { 0.2, 0.5, 1.0 } );
            auto* const skinlet = new TickmarksSkinlet;
            tickmarks->setSkinlet(skinlet);
            skinlet->setOwnedBySkinnable(true);
        }
        {
            (void) new QskTextLabel("Custom (Major only)", layout);
            auto* const tickmarks = new Tickmarks(layout);
            tickmarks->setTickmarksMetricsHint(Tickmarks::Ticks, { 0.0, 0.0, 1.0 } );
            auto* const skinlet = new TickmarksSkinlet;
            tickmarks->setSkinlet(skinlet);
            skinlet->setOwnedBySkinnable(true);
        }
        {
            (void) new QskTextLabel("Custom (Major + Minor only)", layout);
            auto* const tickmarks = new Tickmarks(layout);
            tickmarks->setTickmarksMetricsHint(Tickmarks::Ticks, { 0.2, 0.0, 1.0 } );
            auto* const skinlet = new TickmarksSkinlet;
            tickmarks->setSkinlet(skinlet);
            skinlet->setOwnedBySkinnable(true);
        }
        {
            (void) new QskTextLabel("Custom (Animated)", layout);
            auto* const tickmarks = new Tickmarks(layout);
            auto* const skinlet = new TickmarksSkinlet;
            tickmarks->setSkinlet(skinlet);
            skinlet->setOwnedBySkinnable(true);

            class Animator final : public QskAnimator
            { 
            public:
                explicit Animator(Tickmarks& tickmarks) : tickmarks(tickmarks)
                {
                    setAutoRepeat(true);
                    setDuration(2000);
                }

                void advance( qreal value ) override
                {
                    const QskTickmarksMetrics metrics {
                        ( 0.1 + 0.1 * qFastSin( value * 2 * M_PI ) ),
                        ( 0.25 + 0.25 * qFastSin( value * 2 * M_PI ) ),
                        ( 0.5 + 0.5 * qFastSin( value * 2 * M_PI ) )
                    };
                    tickmarks.setTickmarksMetricsHint(Tickmarks::Ticks, metrics);
                }

                Tickmarks& tickmarks;
            };

            auto* const animator = new Animator(*tickmarks);
            animator->setWindow(this);
            animator->start();
        }
    }

  private:
    void setAlternativeSkin( bool on )
    {
        auto oldSkin = qskSetup->skin();
        if ( oldSkin->parent() == qskSetup )
            oldSkin->setParent( nullptr ); // otherwise setSkin deletes it

        auto newSkin = qskSetup->setSkin( alternativeSkin( on ) );

        QskSkinTransition transition;

        transition.setSourceSkin( oldSkin );
        transition.setTargetSkin( newSkin );
        transition.setAnimation( QskAnimationHint( 600, QEasingCurve::Linear ) );

        transition.process();

        if ( oldSkin->parent() == nullptr )
            delete oldSkin;
    }

    QString alternativeSkin( bool on ) const
    {
        const auto skinNames = qskSkinManager->skinNames();
        return skinNames[ on ];
    }
};

int main( int argc, char* argv[] )
{
#ifdef ITEM_STATISTICS
    QskObjectCounter counter( true );
#endif

    QGuiApplication app( argc, argv );

    SkinnyShortcut::enable( SkinnyShortcut::DebugBackground |
        SkinnyShortcut::DebugStatistics | SkinnyShortcut::Quit );

    Qsk::addGraphicProvider( "shapes", new SkinnyShapeProvider() );

    qskSkinManager->setPluginPaths( QStringList() ); // no plugins
    qskSkinManager->unregisterFactories(); // get rid of the factories of the support lib

    qskSkinManager->registerFactory(
        QStringLiteral( "MySkinFactory" ), new MySkinFactory() );

    qskSetup->setItemUpdateFlag( QskQuickItem::PreferRasterForTextures, true );

    Window window;
    window.resize( 480, 360 );
    window.show();

    return app.exec();
}

#include "main.moc"