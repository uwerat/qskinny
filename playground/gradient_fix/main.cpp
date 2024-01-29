/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include <QskBoxBorderColors.h>
#include <QskBoxBorderMetrics.h>
#include <QskBoxNode.h>
#include <QskBoxShadowNode.h>
#include <QskBoxShapeMetrics.h>
#include <QskGradientDirection.h>
#include <QskLinearBox.h>
#include <QskObjectCounter.h>
#include <QskPushButton.h>
#include <QskSGNode.h>
#include <QskSkinlet.h>
#include <QskTabBar.h>
#include <QskTabView.h>
#include <QskTextNode.h>
#include <QskWindow.h>

#include <QGuiApplication>
#include <SkinnyShortcut.h>
#include <qnamespace.h>

double qskMapValueRange( double value, double srcMin, double srcMax, double dstMin, double dstMax )
{
    value = std::min( std::max( value, srcMin ), srcMax );
    double percentage = ( value - srcMin ) / ( srcMax - srcMin );
    double mappedValue = dstMin + percentage * ( dstMax - dstMin );
    return mappedValue;
}

QColor extracted( const QColor& from, const QColor& to, qreal ratio )
{
    // Ensure the factor is within the [0, 1] range
    ratio = qBound< qreal >( 0.0, ratio, 1.0 );

    // Extract RGB components of the start and end colors
    int startRed, startGreen, startBlue, startAlpha;
    from.getRgb( &startRed, &startGreen, &startBlue, &startAlpha );

    int endRed, endGreen, endBlue, endAlpha;
    to.getRgb( &endRed, &endGreen, &endBlue, &endAlpha );

    // Linearly interpolate each color component
    int interpolatedRed = static_cast< int >( startRed + ratio * ( endRed - startRed ) );
    int interpolatedGreen = static_cast< int >( startGreen + ratio * ( endGreen - startGreen ) );
    int interpolatedBlue = static_cast< int >( startBlue + ratio * ( endBlue - startBlue ) );
    int interpolatedAlpha = static_cast< int >( startAlpha + ratio * ( endAlpha - startAlpha ) );

    // Create and return the interpolated color
    return QColor( interpolatedRed, interpolatedGreen, interpolatedBlue, interpolatedAlpha );
}

QskGradient extracted( const QskGradient& gradient, qreal from, qreal to )
{
    const auto stops = gradient.stops();

    if ( stops.count() == 0 )
    {
        return {};
    }

    if ( stops.count() == 1 )
    {
        return stops[ 0 ].color();
    }

    from = qBound( 0.0, from, 1.0 );
    to = qBound( 0.0, to, 1.0 );

    // returns the stop indexes that contain 'from'
    const auto fromIndex = [ & ]() {
        for ( int i = 0; i < stops.count() - 1; ++i )
        {
            if ( stops[ i ].position() <= from && from <= stops[ i + 1 ].position() )
            {
                return std::make_pair( i, i + 1 );
            }
        }
        return std::pair< int, int >{ -1, -1 };
    }();

    // returns the stop indexes that contain 'to'
    const auto toIndex = [ & ]() {
        for ( int i = stops.count() - 1; i > 0; --i )
        {
            if ( stops[ i - 1 ].position() <= to && to <= stops[ i ].position() )
            {
                return std::make_pair( i - 1, i );
            }
        }
        return std::pair< int, int >{ -1, -1 };
    }();

    Q_ASSERT( fromIndex.first < toIndex.second );

    const auto fromColor = [ & ]() {
        const auto p = qskMapValueRange( from, stops[ fromIndex.first ].position(),
            stops[ fromIndex.second ].position(), 0.0, 1.0 );
        return extracted( stops[ fromIndex.first ].color(), stops[ fromIndex.second ].color(), p );
    }();

    const auto toColor = [ & ]() {
        const auto p = qskMapValueRange(
            to, stops[ toIndex.first ].position(), stops[ toIndex.second ].position(), 0.0, 1.0 );
        return extracted( stops[ toIndex.first ].color(), stops[ toIndex.second ].color(), p );
    }();

    QskGradient newGradient;
    newGradient.setLinearDirection( Qt::Horizontal );
    QskGradientStops newStops;
    newStops << QskGradientStop{ 0.0, fromColor };

    for ( int i = fromIndex.second; i < toIndex.second; ++i )
    {
        const auto p = qskMapValueRange( stops[ i ].position(), from, to, 0.0, 1.0 );
        newStops << QskGradientStop{ p, stops[ i ].color() };
    }

    newStops << QskGradientStop{ 1.0, toColor };
    newGradient.setStops( newStops );

    Q_ASSERT( newGradient.stops().count() <= gradient.stops().count() );

    return newGradient;
}

class Control : public QskControl
{
  public:
    QSK_SUBCONTROLS( Gradient, GradientStart, GradientStop )

    Control( QQuickItem* parent = nullptr )
        : QskControl( parent )
    {
        setAcceptedMouseButtons( Qt::LeftButton | Qt::RightButton );
        setPositionHint( GradientStart, 0.0 );
        setPositionHint( GradientStop, 1.0 );
    }

    void mouseMoveEvent( QMouseEvent* event ) override
    {
        if ( event->buttons() == Qt::LeftButton )
            setPositionHint( GradientStart, event->pos().x() / width() );
        if ( event->buttons() == Qt::RightButton )
            setPositionHint( GradientStop, event->pos().x() / width() );
    }

    void mousePressEvent( QMouseEvent* event ) override
    {
    }

    void mouseReleaseEvent( QMouseEvent* event ) override
    {
    }
};

QSK_SUBCONTROL( Control, Gradient )
QSK_SUBCONTROL( Control, GradientStart )
QSK_SUBCONTROL( Control, GradientStop )

class Skinlet : public QskSkinlet
{
  public:
    enum NodeRoles
    {
        A,
        B,
        C
    };
    Skinlet( QskSkin* skin = nullptr )
        : QskSkinlet( skin )
    {
        setNodeRoles( { A, B, C } );
    }

    QSGNode* updateSubNode(
        const QskSkinnable* skinnable, quint8 nodeRole, QSGNode* node ) const override
    {
        struct Node : QSGNode
        {
            Node()
            {
                appendChildNode( new QskBoxNode );
                appendChildNode( new QskTextNode );
            }
        };

        const auto r = static_cast< const QskControl* >( skinnable )->contentsRect();
        auto g = skinnable->gradientHint( Control::Gradient );
        g.setLinearDirection( Qt::Horizontal );

        const auto from = skinnable->positionHint( Control::GradientStart );
        const auto to = skinnable->positionHint( Control::GradientStop );

        const auto x1 = qskMapValueRange( from, 0.0, 1.0, r.left(), r.right() );
        const auto x2 = qskMapValueRange( to, 0.0, 1.0, r.left(), r.right() );
        const auto y1 = 0.0;
        const auto y2 = r.height() / 3;
        const auto rect = QRectF{ QPointF{ x1, y1 }, QPointF{ x2, y2 } };
        const auto dy = r.height() / 3;

        if ( nodeRole == A )
        {
            const auto rect = QRectF{ r.topLeft(), QSizeF{ r.width(), r.height() / 3 } };
            auto* const n = QskSGNode::ensureNode< Node >( node );
            updateBoxNode( skinnable, n->firstChild(), rect, {}, {}, {}, g );
            updateTextNode( skinnable, n->lastChild(), rect, Qt::AlignTop | Qt::AlignLeft,
                "Reference", QskControl::Background );
            return n;
        }
        if ( nodeRole == B )
        {
            const auto rectB = rect.adjusted( 0, dy, 0, dy );
            auto* const n = QskSGNode::ensureNode< Node >( node );
            updateBoxNode(
                skinnable, n->firstChild(), rectB, {}, {}, {}, extracted( g, from, to ) );
            updateTextNode( skinnable, n->lastChild(), rectB, Qt::AlignTop | Qt::AlignLeft,
                "Expected", QskControl::Background );
            return n;
        }
        if ( nodeRole == C )
        {
            const auto rectC = rect.adjusted( 0, 2 * dy, 0, 2 * dy );
            auto* const n = QskSGNode::ensureNode< Node >( node );
            updateBoxNode( skinnable, n->firstChild(), rectC, {}, {}, {}, g.extracted( from, to ) );
            updateTextNode( skinnable, n->lastChild(), rectC, Qt::AlignTop | Qt::AlignLeft,
                "Actual", QskControl::Background );
            return n;
        }

        return nullptr;
    }
};

int main( int argc, char* argv[] )
{
#ifdef ITEM_STATISTICS
    QskObjectCounter counter( true );
#endif

    QGuiApplication app( argc, argv );

    SkinnyShortcut::enable( SkinnyShortcut::AllShortcuts );

    auto* const layout = new QskLinearBox( Qt::Vertical );
    auto* const row = new QskLinearBox( Qt::Vertical, layout );
    auto* const control = new Control( layout );
    auto* const skinlet = new Skinlet;
    control->setSkinlet( skinlet );
    skinlet->setOwnedBySkinnable( true );

    auto qskGradient = []( Qt::Orientation orientation, QskGradient gradient ) {
        gradient.setLinearDirection( orientation );
        return gradient;
    };

    {
        auto* const button = new QskPushButton( "Click", row );
        button->setGradientHint( QskPushButton::Panel, {} );
        QObject::connect( button, &QskPushButton::clicked, control, [ control, button ]() {
            control->setGradientHint(
                Control::Gradient, button->gradientHint( QskPushButton::Panel ) );
        } );
    }
    {
        auto* const button = new QskPushButton( "Click", row );
        button->setGradientHint( QskPushButton::Panel, Qt::red );
        QObject::connect( button, &QskPushButton::clicked, control, [ control, button ]() {
            control->setGradientHint(
                Control::Gradient, button->gradientHint( QskPushButton::Panel ) );
        } );
    }
    {
        auto* const button = new QskPushButton( "Click", row );
        button->setGradientHint(
            QskPushButton::Panel, qskGradient( Qt::Horizontal, { Qt::red, Qt::green } ) );
        QObject::connect( button, &QskPushButton::clicked, control, [ control, button ]() {
            control->setGradientHint(
                Control::Gradient, button->gradientHint( QskPushButton::Panel ) );
        } );
    }
    {
        auto* const button = new QskPushButton( "Click", row );
        button->setGradientHint( QskPushButton::Panel,
            qskGradient(
                Qt::Horizontal, { { { 0.0, Qt::red }, { 0.5, Qt::green }, { 1.0, Qt::blue } } } ) );
        QObject::connect( button, &QskPushButton::clicked, control, [ control, button ]() {
            control->setGradientHint(
                Control::Gradient, button->gradientHint( QskPushButton::Panel ) );
        } );
    }
    {
        auto* const button = new QskPushButton( "Click", row );
        button->setGradientHint( QskPushButton::Panel,
            qskGradient( Qt::Horizontal, { { { 0.0, Qt::red }, { 0.5, Qt::red }, { 0.5, Qt::blue },
                                             { 1.0, Qt::blue } } } ) );
        QObject::connect( button, &QskPushButton::clicked, control, [ control, button ]() {
            control->setGradientHint(
                Control::Gradient, button->gradientHint( QskPushButton::Panel ) );
        } );
    }
    {
        auto* const button = new QskPushButton( "Click", row );
        button->setGradientHint(
            QskPushButton::Panel, qskGradient( Qt::Horizontal,
                                      { { { 0.0, Qt::red }, { 0.25, Qt::red }, { 0.25, Qt::green },
                                          { 0.5, Qt::green }, { 0.5, Qt::blue }, { 0.75, Qt::blue },
                                          { 0.75, Qt::yellow }, { 1.0, Qt::yellow } } } ) );
        QObject::connect( button, &QskPushButton::clicked, control, [ control, button ]() {
            control->setGradientHint(
                Control::Gradient, button->gradientHint( QskPushButton::Panel ) );
        } );
    }

    QskWindow window;
    window.addItem( layout );
    window.resize( 600, 600 );
    window.show();

    return app.exec();
}

// #include "moc_MainWindow.cpp"