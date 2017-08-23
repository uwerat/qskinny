/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskSkinnable.h"

#include "QskAspect.h"
#include "QskSetup.h"
#include "QskSkin.h"
#include "QskSkinHintTable.h"
#include "QskSkinlet.h"
#include "QskAnimationHint.h"
#include "QskMargins.h"
#include "QskHintAnimator.h"
#include "QskControl.h"
#include "QskColorFilter.h"
#include "QskSkinTransition.h"

#include <QFont>
#include <QElapsedTimer>
#include <QMarginsF>
#include <QtAlgorithms>

#include <unordered_map>
#include <set>

static inline bool qskIsControl( const QskSkinnable* skinnable )
{
#if QT_VERSION >= QT_VERSION_CHECK(5, 7, 0)
    return skinnable->metaObject()->inherits( &QskControl::staticMetaObject );
#else
    for ( auto mo = skinnable->metaObject();
        mo != nullptr; mo = mo->superClass() )
    {
        if ( mo == &QskControl::staticMetaObject )
            return true;
    }

    return false;
#endif
}

static inline const QVariant& qskResolvedHint( QskAspect::Aspect aspect,
    const std::unordered_map< QskAspect::Aspect, QVariant >* controlHints,
    const std::unordered_map< QskAspect::Aspect, QVariant >* skinHints,
    QskSkinHintStatus* status )
{
    if ( controlHints )
    {
        auto it = controlHints->find( aspect );
        if ( it != controlHints->cend() )
        {
            if ( status )
            {
                status->source = QskSkinHintStatus::Skinnable;
                status->aspect = aspect;
            }

            return it->second;
        }
    }

    if ( skinHints )
    {
        auto it = skinHints->find( aspect );
        if ( it != skinHints->cend() )
        {
            if ( status )
            {
                status->source = QskSkinHintStatus::Skin;
                status->aspect = aspect;
            }

            return it->second;
        }
    }

    const auto topState = aspect.topState();

    if ( aspect.isBoxPrimitive() )
    {
        // progressively strip state flags
        if ( topState )
        {
            aspect.clearState( topState );
            return qskResolvedHint( aspect, controlHints, skinHints, status );
        }

        // fall back from specific edge to no edge
        if ( qPopulationCount( static_cast< quint8 >( aspect.edge() ) ) == 1 )
        {
            aspect.clearEdge();
            return qskResolvedHint( aspect, controlHints, skinHints, status );
        }
    }
    else if ( topState ) // Fundamental elements (no edges/corners)
    {
        aspect.clearState( topState );
        return qskResolvedHint( aspect, controlHints, skinHints, status );
    }

    if ( aspect.subControl() != QskAspect::Control )
    {
        // fall back from subcontrol to control

        aspect.setSubControl( QskAspect::Control );
        return qskResolvedHint( aspect, controlHints, skinHints, status );
    }

    // Nothing left to try...
    if ( status )
    {
        status->source = QskSkinHintStatus::NoSource;
        status->aspect = QskAspect::Aspect();
    }

    static QVariant hintInvalid;
    return hintInvalid;
}

class QskSkinnable::PrivateData
{
public:
    PrivateData():
        hintTable( false ),
        skinlet( nullptr ),
        skinState( QskAspect::NoState ),
        hasLocalSkinlet( false )
    {
    }

    ~PrivateData()
    {
        if ( skinlet && skinlet->isOwnedBySkinnable() )
            delete skinlet;
    }

    QskSkinHintTable hintTable;

    QskHintAnimatorTable animators;

    const QskSkinlet* skinlet;

    QMetaObject::Connection connection;

    QskAspect::State skinState;
    bool hasLocalSkinlet : 1;
};

QskSkinnable::QskSkinnable():
    m_data( new PrivateData() )
{
    auto skinChangedHandler =
        [ this ]( QskSkin* )
        {
            if ( !m_data->hasLocalSkinlet )
                setSkinlet( nullptr );

            QEvent event( QEvent::StyleChange );
            QCoreApplication::sendEvent( owningControl(), &event );
        };

    m_data->connection = QObject::connect(
        qskSetup, &QskSetup::skinChanged, skinChangedHandler );
}

QskSkinnable::~QskSkinnable()
{
    QObject::disconnect( m_data->connection );
}

void QskSkinnable::setSkinlet( const QskSkinlet* skinlet )
{
    if ( skinlet == m_data->skinlet )
    {
        if ( skinlet )
        {
            // now we don't depend on global skin changes anymore
            m_data->hasLocalSkinlet = true;
        }
        return;
    }

    if ( m_data->skinlet && m_data->skinlet->isOwnedBySkinnable() )
        delete m_data->skinlet;

    m_data->skinlet = skinlet;
    m_data->hasLocalSkinlet = ( skinlet != nullptr );

    owningControl()->update();
}

const QskSkinlet* QskSkinnable::skinlet() const
{
    return m_data->hasLocalSkinlet ? m_data->skinlet : nullptr;
}

const QskSkinlet* QskSkinnable::effectiveSkinlet() const
{
    if ( m_data->skinlet == nullptr )
    {
        m_data->skinlet = qskSetup->skin()->skinlet( this );
        m_data->hasLocalSkinlet = false;
    }

    return m_data->skinlet;
}

QskSkinHintTable &QskSkinnable::hintTable()
{
    return m_data->hintTable;
}

const QskSkinHintTable &QskSkinnable::hintTable() const
{
    return m_data->hintTable;
}

void QskSkinnable::setFlagHint( QskAspect::Aspect aspect, int flag )
{
    m_data->hintTable.setSkinHint( aspect, QVariant( flag ) );
}

int QskSkinnable::flagHint( QskAspect::Aspect aspect ) const
{
    return effectiveHint( aspect ).toInt();
}

void QskSkinnable::setColor( QskAspect::Aspect aspect, const QColor& color )
{
    m_data->hintTable.setColor( aspect, color );
}

void QskSkinnable::setColor( QskAspect::Aspect aspect, Qt::GlobalColor color )
{
    m_data->hintTable.setColor( aspect, color );
}

void QskSkinnable::setColor( QskAspect::Aspect aspect, QRgb rgb )
{
    m_data->hintTable.setColor( aspect, rgb );
}

QColor QskSkinnable::color( QskAspect::Aspect aspect, QskSkinHintStatus* status ) const
{
    return effectiveHint( aspect | QskAspect::Color, status ).value< QColor >();
}

void QskSkinnable::setMetric( QskAspect::Aspect aspect, qreal metric )
{
    m_data->hintTable.setMetric( aspect, metric );
}

qreal QskSkinnable::metric( QskAspect::Aspect aspect, QskSkinHintStatus* status ) const
{
    return effectiveHint( aspect | QskAspect::Metric, status ).toReal();
}

void QskSkinnable::setMarginsHint( QskAspect::Aspect aspect, const QMarginsF& margins )
{
    m_data->hintTable.setMargins( aspect, margins );
}

QMarginsF QskSkinnable::marginsHint(
    QskAspect::Aspect aspect, QskSkinHintStatus* status ) const
{
    return effectiveHint( aspect | QskAspect::Metric, status ).value< QskMargins >();
}

QMarginsF QskSkinnable::borderMetrics( QskAspect::Subcontrol subControl ) const
{
    using namespace QskAspect;

    const Aspect aspect = subControl | Border;

    return QMarginsF(
        metric( aspect | LeftEdge ),
        metric( aspect | TopEdge ),
        metric( aspect | RightEdge ),
        metric( aspect | BottomEdge ) );
}

void QskSkinnable::setFontRole( QskAspect::Aspect aspect, int role )
{
    m_data->hintTable.setFontRole( aspect, role );
}

int QskSkinnable::fontRole( QskAspect::Aspect aspect ) const
{
    return effectiveHint( aspect | QskAspect::FontRole ).toInt();
}

QFont QskSkinnable::effectiveFont( QskAspect::Aspect aspect ) const
{
    return effectiveSkin()->font( fontRole( aspect ) );
}

void QskSkinnable::setGraphicRole( QskAspect::Aspect aspect, int role )
{
    m_data->hintTable.setGraphicRole( aspect, role );
}

int QskSkinnable::graphicRole( QskAspect::Aspect aspect ) const
{
    return effectiveHint( aspect | QskAspect::GraphicRole ).toInt();
}

QskColorFilter QskSkinnable::effectiveGraphicFilter(
    QskAspect::Aspect aspect ) const
{
    aspect.setSubControl( effectiveSubcontrol( aspect.subControl() ) );
    aspect = aspect | QskAspect::GraphicRole;

#if 1
    QskSkinHintStatus status;

    const QVariant hint = storedHint( aspect | skinState(), &status );
    if ( status.isValid() )
    {
        // we need to know about how the aspect gets resolved
        // before checking for animators

        aspect.setSubControl( status.aspect.subControl() );
    }
#endif

    if ( !aspect.isAnimator() )
    {
        const QVariant v = animatedValue( aspect, nullptr );
        if ( v.canConvert< QskColorFilter >() )
            return v.value< QskColorFilter >();
    }

    return effectiveSkin()->graphicFilter( hint.toInt() );
}

void QskSkinnable::setAnimation(
    QskAspect::Aspect aspect, QskAnimationHint animation )
{
    m_data->hintTable.setAnimation( aspect, animation );
}

QskAnimationHint QskSkinnable::animation(
    QskAspect::Aspect aspect, QskSkinHintStatus* status  ) const
{
    aspect.setAnimator( true );
    return effectiveHint( aspect, status ).value< QskAnimationHint >();
}

QVariant QskSkinnable::effectiveHint(
    QskAspect::Aspect aspect, QskSkinHintStatus* status ) const
{
    aspect.setSubControl( effectiveSubcontrol( aspect.subControl() ) );

    if ( aspect.isAnimator() )
        return storedHint( aspect, status );

    const QVariant v = animatedValue( aspect, status );
    if ( v.isValid() )
        return v;

    if ( aspect.state() == QskAspect::NoState )
        aspect = aspect | skinState();

    return storedHint( aspect, status );
}

QskSkinHintStatus QskSkinnable::hintStatus( QskAspect::Aspect aspect ) const
{
    QskSkinHintStatus status;

    (void)effectiveHint( aspect, &status );
    return status;
}


QVariant QskSkinnable::animatedValue(
    QskAspect::Aspect aspect, QskSkinHintStatus* status ) const
{
    QVariant v;

    if ( aspect.state() == QskAspect::NoState )
    {
        /*
            The local animators were invented to be stateless
            and we never have an aspect with a state here.
            But that might change ...
         */

        v = m_data->animators.currentValue( aspect );
    }

    if ( !v.isValid() )
    {
        if ( QskSkinTransition::isRunning() )
        {
            /*
               Next we check for values from the skin. Those
               animators are usually from global skin/color changes
               and are state aware
             */

            if ( aspect.state() == QskAspect::NoState )
                aspect = aspect | skinState();

            Q_FOREVER
            {
                v = QskSkinTransition::animatedHint( aspect );
                if ( v.isValid() || aspect.state() == QskAspect::NoState )
                    break;

                aspect.clearState( aspect.topState() );
            }
        }
    }

    if ( status && v.isValid() )
    {
        status->source = QskSkinHintStatus::Animator;
        status->aspect = aspect;
    }

    return v;
}

const QVariant& QskSkinnable::storedHint(
    QskAspect::Aspect aspect, QskSkinHintStatus* status ) const
{
    const auto& localHints = m_data->hintTable.hints();
    const QskSkin* skin = effectiveSkin();

    return qskResolvedHint( aspect, 
        !localHints.empty() ? &localHints : nullptr,
        skin ? &skin->hintTable().hints() : nullptr,
        status );
}

QskAspect::State QskSkinnable::skinState() const
{
    return m_data->skinState;
}

const char* QskSkinnable::skinStateAsPrintable() const
{
    return skinStateAsPrintable( skinState() );
}

const char* QskSkinnable::skinStateAsPrintable( QskAspect::State state ) const
{
    QString tmp;

    QDebug debug( &tmp );
    qskDebugState( debug, metaObject(), state );

    // we should find a better way
    static QByteArray bytes[10];
    static int counter = 0;

    counter = ( counter + 1 ) % 10;

    bytes[counter] = tmp.toUtf8();
    return bytes[counter].constData();
}

static inline qreal qskMax( qreal v1, qreal v2, qreal v3 )
{
    return std::max( std::max( v1, v2 ), v3 );
}

static inline qreal qskPaddingInner( const QskSkinnable* skinnable,
    QskAspect::Aspect aspect, qreal length )
{
    qreal radius = skinnable->metric( aspect );
    if ( radius <= 0.0 )
        return 0.0;

    const auto mode = skinnable->flagHint< Qt::SizeMode >(
        aspect | QskAspect::SizeMode, Qt::AbsoluteSize );

    if ( mode == Qt::RelativeSize )
    {
        if ( radius > 100.0 )
            radius = 100.0;

        radius *= 0.5 * length / 100.0;
    }

    constexpr double sin45 = 0.70710678; // sin 45° ceiled
    return radius * ( 1.0 - sin45 );
}

static inline qreal qskPaddingOuter( const QskSkinnable* skinnable,
    QskAspect::Aspect aspect, qreal length )
{
    qreal radius = skinnable->metric( aspect );
    if ( radius <= 0.0 )
        return 0.0;

    const auto mode = skinnable->flagHint< Qt::SizeMode >(
        aspect | QskAspect::SizeMode, Qt::AbsoluteSize );

    constexpr double sin45 = 0.70710677; // sin 45° floored

    qreal padding;
    if ( mode == Qt::RelativeSize )
    {
        if ( radius > 100.0 )
            radius = 100.0;
#if 1
        // not correct as the radius is relative to the outer length
        // expanded at both ends. TODO ...

        const qreal t = 0.5 * radius / 100.0 * ( 1.0 - sin45 );
        padding = length * ( t / ( 1.0 - t ) );
#endif
    }
    else
    {
        padding = radius * ( 1.0 - sin45 );
    }

    return padding;
}


static inline QMarginsF qskMarginsInner( const QskSkinnable* skinnable,
    QskAspect::Aspect aspect, const QSizeF& size )
{
    using namespace QskAspect;

    qreal left = skinnable->metric( aspect | Border | LeftEdge );
    qreal right = skinnable->metric( aspect | Border | RightEdge );
    qreal top = skinnable->metric( aspect | Border | TopEdge );
    qreal bottom = skinnable->metric( aspect | Border | BottomEdge );

    const QMarginsF padding = skinnable->marginsHint( aspect | Padding );

    const qreal w = size.width() - 0.5 * ( left + right );
    const qreal h = size.height() - 0.5 * ( top + bottom );

    left += qskMax(
        qskPaddingInner( skinnable, aspect | RadiusX | TopLeftCorner, w ),
        qskPaddingInner( skinnable, aspect | RadiusX | BottomLeftCorner, w ),
        padding.left() );

    right += qskMax(
        qskPaddingInner( skinnable, aspect | RadiusX | TopRightCorner, w ),
        qskPaddingInner( skinnable, aspect | RadiusX | BottomRightCorner, w ),
        padding.right() );

    top += qskMax(
        qskPaddingInner( skinnable, aspect | RadiusY | TopLeftCorner, h ),
        qskPaddingInner( skinnable, aspect | RadiusY | TopRightCorner, h ),
        padding.top() );

    bottom += qskMax(
        qskPaddingInner( skinnable, aspect | RadiusY | BottomLeftCorner, h ),
        qskPaddingInner( skinnable, aspect | RadiusY | BottomRightCorner, h ),
        padding.bottom() );

    return QMarginsF( left, top, right, bottom );
}

static inline QMarginsF qskMarginsOuter( const QskSkinnable* skinnable,
    QskAspect::Aspect aspect, const QSizeF& size )
{
    // calculation is not yet correct, but to get something started

    using namespace QskAspect;

    qreal left = skinnable->metric( aspect | Border | LeftEdge );
    qreal right = skinnable->metric( aspect | Border | RightEdge );
    qreal top = skinnable->metric( aspect | Border | TopEdge );
    qreal bottom = skinnable->metric( aspect | Border | BottomEdge );

    const QMarginsF padding = skinnable->marginsHint( aspect | Padding );

    const qreal w = size.width() + 0.5 * ( left + right );
    const qreal h = size.height() + 0.5 * ( top + bottom );

    left += qskMax(
        qskPaddingOuter( skinnable, aspect | RadiusX | TopLeftCorner, w ),
        qskPaddingOuter( skinnable, aspect | RadiusX | BottomLeftCorner, w ),
        padding.left() );

    right += qskMax(
        qskPaddingOuter( skinnable, aspect | RadiusX | TopRightCorner, w ),
        qskPaddingOuter( skinnable, aspect | RadiusX | BottomRightCorner, w ),
        padding.right() );

    top += qskMax(
        qskPaddingOuter( skinnable, aspect | RadiusY | TopLeftCorner, h ),
        qskPaddingOuter( skinnable, aspect | RadiusY | TopRightCorner, h ),
        padding.top() );

    bottom += qskMax(
        qskPaddingOuter( skinnable, aspect | RadiusY | BottomLeftCorner, h ),
        qskPaddingOuter( skinnable, aspect | RadiusY | BottomRightCorner, h ),
        padding.bottom() );

    return QMargins( left, top, right, bottom );
}

QSizeF QskSkinnable::innerBoxSize(
    QskAspect::Aspect aspect, const QSizeF& outerBoxSize ) const
{
    const QMarginsF m = qskMarginsInner( this, aspect, outerBoxSize );

    return QSizeF( outerBoxSize.width() - m.left() - m.right(),
        outerBoxSize.height() - m.top() - m.bottom() );
}

QRectF QskSkinnable::innerBox(
    QskAspect::Aspect aspect, const QRectF& outerBox ) const
{
    const QMarginsF m = qskMarginsInner( this, aspect, outerBox.size() );
    return outerBox.marginsRemoved( m );
}

QSizeF QskSkinnable::outerBoxSize(
    QskAspect::Aspect aspect, const QSizeF& innerBoxSize ) const
{
    const QMarginsF m = qskMarginsOuter( this, aspect, innerBoxSize );

    return QSizeF( innerBoxSize.width() + m.left() + m.right(),
        innerBoxSize.height() + m.top() + m.bottom() );
}

QRectF QskSkinnable::outerBox(
    QskAspect::Aspect aspect, const QRectF& innerBox ) const
{
    const QMarginsF m = qskMarginsOuter( this, aspect, innerBox.size() );
    return innerBox.marginsAdded( m );
}

void QskSkinnable::startTransition( QskAspect::Aspect aspect,
    QskAnimationHint animationHint, QVariant from, QVariant to )
{
    if ( animationHint.duration <= 0 || ( from == to ) )
        return;

    QskControl* control = this->owningControl();
    if ( control->window() == nullptr || !control->isInitiallyPainted() )
        return;

    if ( from == to )
    {
        return;
    }
    else
    {
        // We might be invalid for one of the values, when an aspect
        // has not been defined for all states ( f.e. metrics are expected
        // to fallback to 0.0 ). In this case we create a default one.

        if ( !from.isValid() )
        {
            from = QVariant( to.type() );
        }
        else if ( !to.isValid() )
        {
            to = QVariant( from.type() );
        }
        else if ( from.type() != to.type() )
        {
            return;
        }
    }

    if( aspect.flagPrimitive() == QskAspect::GraphicRole  )
    {
        const auto skin = effectiveSkin();

        from.setValue( skin->graphicFilter( from.toInt() ) );
        to.setValue( skin->graphicFilter( to.toInt() ) );
    }

    aspect.clearStates();
    aspect.setAnimator( false );

    m_data->animators.start( control, aspect, animationHint, from, to );
}

void QskSkinnable::startTransition( QskAspect::Aspect aspect,
    QskAspect::State oldState, QskAspect::State newState )
{
    const auto animationHint = storedHint( aspect ).value< QskAnimationHint >();
    if ( animationHint.duration <= 0 )
        return;

    aspect.clearStates();
    aspect.setAnimator( false );

    const auto from = storedHint( aspect | oldState, nullptr );
    const auto to = storedHint( aspect | newState, nullptr );

    startTransition( aspect, animationHint, from, to );
}

void QskSkinnable::setSkinStateFlag( QskAspect::State state, bool on )
{
    const auto newState = on ? ( m_data->skinState | state )
        : ( m_data->skinState & ~state );

    if ( m_data->skinState == newState )
        return;

    QskControl* control = owningControl();
    if ( control->window() && control->isInitiallyPainted() )
    {
        for ( const auto entry : m_data->hintTable.hints() )
        {
            const auto aspect = entry.first;
            if ( aspect.isAnimator() )
            {
                if ( !aspect.state() || aspect.state() == newState )
                    startTransition( aspect, m_data->skinState, newState );
            }
        }

        const QskSkin* skin = effectiveSkin();

        auto subControls = control->subControls();
#if 1
        subControls += QskAspect::Control;
#endif

        const auto& hintTable = skin->hintTable();

        for ( const auto subControl : subControls )
        {
            const auto& animatorAspects = hintTable.animatorAspects( subControl );

            for ( QskAspect::Aspect aspect : animatorAspects )
            {
                // ignore animators from the skin, when we have others
                // specifically defined for the skinnable

                if ( !m_data->hintTable.hasHint( aspect ) )
                {
                    if ( !aspect.state() || aspect.state() == newState )
                        startTransition( aspect, m_data->skinState, newState );
                }
            }
        }
    }

    m_data->skinState = newState;
    control->update();
}

QskSkin* QskSkinnable::effectiveSkin() const
{
    auto skin = effectiveSkinlet()->skin();
    return skin ? skin : qskSetup->skin();
}

void QskSkinnable::updateNode( QSGNode* parentNode )
{
    effectiveSkinlet()->updateNode( this, parentNode );
}

QskAspect::Subcontrol QskSkinnable::effectiveSubcontrol(
    QskAspect::Subcontrol subControl ) const
{
    // derived classes might want to redirect a sub-control
    return subControl;
}

QskControl* QskSkinnable::controlCast()
{
    return qskIsControl( this )
        ? static_cast< QskControl* >( this ) : nullptr;
}

const QskControl* QskSkinnable::controlCast() const
{
    return qskIsControl( this )
        ? static_cast< const QskControl* >( this ) : nullptr;
}

void QskSkinnable::debug( QDebug debug, QskAspect::Aspect aspect ) const
{
    qskDebugAspect( debug, metaObject(), aspect );
}

void QskSkinnable::debug( QDebug debug, QskAspect::State state ) const
{
    qskDebugState( debug, metaObject(), state );
}

void QskSkinnable::debug( QskAspect::Aspect aspect ) const
{
    qskDebugAspect( qDebug(), metaObject(), aspect );
}

void QskSkinnable::debug( QskAspect::State state ) const
{
    qskDebugState( qDebug(), metaObject(), state );
}

