/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskSkinnable.h"

#include "QskAnimationHint.h"
#include "QskAspect.h"
#include "QskColorFilter.h"
#include "QskControl.h"
#include "QskHintAnimator.h"
#include "QskMargins.h"
#include "QskSetup.h"
#include "QskSkin.h"
#include "QskSkinHintTable.h"
#include "QskSkinTransition.h"
#include "QskSkinlet.h"

#include <qfont.h>

#define DEBUG_MAP 0
#define DEBUG_ANIMATOR 0
#define DEBUG_STATE 0

static inline bool qskIsControl( const QskSkinnable* skinnable )
{
#if QT_VERSION >= QT_VERSION_CHECK( 5, 7, 0 )
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

static inline bool qskCompareResolvedStates(
    QskAspect& aspect1, QskAspect& aspect2, const QskSkinHintTable& table )
{
    if ( !table.hasStates() )
        return false;

    const auto a1 = aspect1;
    const auto a2 = aspect2;

    Q_FOREVER
    {
        const auto s1 = aspect1.topState();
        const auto s2 = aspect2.topState();

        if ( s1 > s2 )
        {
            if ( table.hasHint( aspect1 ) )
                return false;

            aspect1.clearState( s1 );
        }
        else if ( s2 > s1 )
        {
            if ( table.hasHint( aspect2 ) )
                return false;

            aspect2.clearState( s2 );
        }
        else
        {
            if ( aspect1 == aspect2 )
            {
                if ( table.hasHint( aspect1 ) )
                    return true;

                if ( s1 == 0 )
                {
                    if ( aspect1.placement() == QskAspect::NoPlacement )
                        return true;

                    // clear the placement bits and restart with the initial state
                    aspect1 = a1;
                    aspect1.setPlacement( QskAspect::NoPlacement );

                    aspect2 = a2;
                    aspect2.setPlacement( QskAspect::NoPlacement );
                }
            }
            else
            {
                if ( table.hasHint( aspect1 ) || table.hasHint( aspect2 ) )
                    return false;
            }

            aspect1.clearState( s1 );
            aspect2.clearState( s2 );
        }
    }
}

static inline QVariant qskTypedNullValue( const QVariant& value )
{
#if QT_VERSION >= QT_VERSION_CHECK( 6, 0, 0 )
    const auto vType = static_cast< QMetaType >( value.userType() );
#else
    const auto vType = value.userType();
#endif

    return QVariant( vType, nullptr );
}

static inline void qskSetFlag( QskSkinnable* skinnable,
     const QskAspect aspect, int flag )
{
    skinnable->setSkinHint( aspect | QskAspect::Flag, QVariant( flag ) );
}

static inline int qskFlag( const QskSkinnable* skinnable,
    const QskAspect aspect, QskSkinHintStatus* status = nullptr )
{
    return skinnable->effectiveSkinHint( aspect | QskAspect::Flag, status ).toInt();
}

static inline void qskSetMetric( QskSkinnable* skinnable,
     const QskAspect aspect, const QVariant& metric )
{
    skinnable->setSkinHint( aspect | QskAspect::Metric, metric );
}

template< typename T >
static inline void qskSetMetric( QskSkinnable* skinnable,
    QskAspect aspect, const T& metric )
{
    qskSetMetric( skinnable, aspect, QVariant::fromValue( metric ) );
}

template< typename T >
static inline T qskMetric( const QskSkinnable* skinnable,
    QskAspect aspect, QskSkinHintStatus* status = nullptr )
{
    return skinnable->effectiveSkinHint(
        aspect | QskAspect::Metric, status ).value< T >();
}

static inline void qskSetColor( QskSkinnable* skinnable,
    const QskAspect aspect, const QVariant& color )
{
    skinnable->setSkinHint( aspect | QskAspect::Color, color );
}

template< typename T >
static inline void qskSetColor( QskSkinnable* skinnable,
    const QskAspect aspect, const T& color )
{
    qskSetColor( skinnable, aspect, QVariant::fromValue( color ) );
}

template< typename T >
static inline T qskColor( const QskSkinnable* skinnable,
    QskAspect aspect, QskSkinHintStatus* status = nullptr )
{
    return skinnable->effectiveSkinHint(
        aspect | QskAspect::Color, status ).value< T >();
}

class QskSkinnable::PrivateData
{
  public:
    PrivateData()
        : skinlet( nullptr )
        , skinState( QskAspect::NoState )
        , hasLocalSkinlet( false )
    {
    }

    ~PrivateData()
    {
        if ( hasLocalSkinlet )
        {
            if ( skinlet && skinlet->isOwnedBySkinnable() )
                delete skinlet;
        }
    }

    QskSkinHintTable hintTable;
    QskHintAnimatorTable animators;

    const QskSkinlet* skinlet;

    QskAspect::State skinState;
    bool hasLocalSkinlet : 1;
};

QskSkinnable::QskSkinnable()
    : m_data( new PrivateData() )
{
}

QskSkinnable::~QskSkinnable()
{
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

QskSkinHintTable& QskSkinnable::hintTable()
{
    return m_data->hintTable;
}

const QskSkinHintTable& QskSkinnable::hintTable() const
{
    return m_data->hintTable;
}

void QskSkinnable::setFlagHint( const QskAspect aspect, int flag )
{
    qskSetFlag( this, aspect, flag );
}

int QskSkinnable::flagHint( const QskAspect aspect ) const
{
    return effectiveSkinHint( aspect ).toInt();
}

void QskSkinnable::setAlignmentHint( const QskAspect aspect, Qt::Alignment alignment )
{
    qskSetFlag( this, aspect | QskAspect::Alignment, alignment );
}

bool QskSkinnable::resetAlignmentHint( const QskAspect aspect )
{
    return resetFlagHint( aspect | QskAspect::Alignment );
}

void QskSkinnable::setColor( const QskAspect aspect, const QColor& color )
{
    qskSetColor( this, aspect, color );
}

void QskSkinnable::setColor( const QskAspect aspect, Qt::GlobalColor color )
{
    qskSetColor( this, aspect, QColor( color ) );
}

void QskSkinnable::setColor( const QskAspect aspect, QRgb rgb )
{
    qskSetColor( this, aspect, QColor::fromRgba( rgb ) );
}

QColor QskSkinnable::color( const QskAspect aspect, QskSkinHintStatus* status ) const
{
    return qskColor< QColor >( this, aspect, status );
}

void QskSkinnable::setMetric( const QskAspect aspect, qreal metric )
{
    qskSetMetric( this, aspect, metric );
}

qreal QskSkinnable::metric( const QskAspect aspect, QskSkinHintStatus* status ) const
{
    return qskMetric< qreal >( this, aspect, status );
}

void QskSkinnable::setStrutSizeHint(
    const QskAspect aspect, qreal width, qreal height )
{
    qskSetMetric( this, aspect, QSizeF( width, height ) );
}

void QskSkinnable::setStrutSizeHint( const QskAspect aspect, const QSizeF& strut )
{
    qskSetMetric( this, aspect, strut );
}

bool QskSkinnable::resetStrutSizeHint( const QskAspect aspect )
{
    return resetMetric( aspect | QskAspect::StrutSize );
}

QSizeF QskSkinnable::strutSizeHint(
    const QskAspect aspect, QskSkinHintStatus* status ) const
{
    return qskMetric< QSizeF >( this, aspect | QskAspect::StrutSize, status );
}

void QskSkinnable::setMarginHint( const QskAspect aspect, qreal margins )
{
    qskSetMetric( this, aspect | QskAspect::Margin, QskMargins( margins ) );
}

void QskSkinnable::setMarginHint( const QskAspect aspect, const QMarginsF& margins )
{
    qskSetMetric( this, aspect | QskAspect::Margin, QskMargins( margins ) );
}

bool QskSkinnable::resetMarginHint( const QskAspect aspect )
{
    return resetMetric( aspect | QskAspect::Margin );
}

QMarginsF QskSkinnable::marginHint(
    const QskAspect aspect, QskSkinHintStatus* status ) const
{
    return qskMetric< QskMargins >( this, aspect | QskAspect::Margin, status );
}

void QskSkinnable::setPaddingHint( const QskAspect aspect, qreal padding )
{
    qskSetMetric( this, aspect | QskAspect::Padding, QskMargins( padding ) );
}

void QskSkinnable::setPaddingHint( const QskAspect aspect, const QMarginsF& padding )
{
    qskSetMetric( this, aspect | QskAspect::Padding, QskMargins( padding ) );
}

bool QskSkinnable::resetPaddingHint( const QskAspect aspect )
{
    return resetMetric( aspect | QskAspect::Padding );
}

QMarginsF QskSkinnable::paddingHint(
    const QskAspect aspect, QskSkinHintStatus* status ) const
{
    return qskMetric< QskMargins >( this, aspect | QskAspect::Padding, status );
}

void QskSkinnable::setGradientHint(
    const QskAspect aspect, const QskGradient& gradient )
{
    qskSetColor( this, aspect, gradient );
}

QskGradient QskSkinnable::gradientHint(
    const QskAspect aspect, QskSkinHintStatus* status ) const
{
    return qskColor< QskGradient >( this, aspect, status );
}

void QskSkinnable::setBoxShapeHint(
    const QskAspect aspect, const QskBoxShapeMetrics& shape )
{
    qskSetMetric( this, aspect | QskAspect::Shape, shape );
}

bool QskSkinnable::resetBoxShapeHint( const QskAspect aspect )
{
    return resetMetric( aspect | QskAspect::Shape );
}

QskBoxShapeMetrics QskSkinnable::boxShapeHint(
    const QskAspect aspect, QskSkinHintStatus* status ) const
{
    return qskMetric< QskBoxShapeMetrics >(
        this, aspect | QskAspect::Shape, status );
}

void QskSkinnable::setBoxBorderMetricsHint(
    const QskAspect aspect, const QskBoxBorderMetrics& border )
{
    qskSetMetric( this, aspect | QskAspect::Border, border );
}

bool QskSkinnable::resetBoxBorderMetricsHint( const QskAspect aspect )
{
    return resetMetric( aspect | QskAspect::Border );
}

QskBoxBorderMetrics QskSkinnable::boxBorderMetricsHint(
    const QskAspect aspect, QskSkinHintStatus* status ) const
{
    return qskMetric< QskBoxBorderMetrics >(
        this, aspect | QskAspect::Border, status );
}

void QskSkinnable::setBoxBorderColorsHint(
    const QskAspect aspect, const QskBoxBorderColors& colors )
{
    qskSetColor( this, aspect | QskAspect::Border, colors );
}

bool QskSkinnable::resetBoxBorderColorsHint( const QskAspect aspect )
{
    return resetColor( aspect | QskAspect::Border );
}

QskBoxBorderColors QskSkinnable::boxBorderColorsHint(
    const QskAspect aspect, QskSkinHintStatus* status ) const
{
    return qskColor< QskBoxBorderColors >(
        this, aspect | QskAspect::Border, status );
}

void QskSkinnable::setSpacingHint( const QskAspect aspect, qreal spacing )
{
    qskSetMetric( this, aspect | QskAspect::Spacing, spacing );
}

bool QskSkinnable::resetSpacingHint( const QskAspect aspect )
{
    return resetMetric( aspect | QskAspect::Spacing );
}

qreal QskSkinnable::spacingHint(
    const QskAspect aspect, QskSkinHintStatus* status ) const
{
    return qskMetric< qreal >( this, aspect | QskAspect::Spacing, status );
}

void QskSkinnable::setFontRole( const QskAspect aspect, int role )
{
    qskSetFlag( this, aspect | QskAspect::FontRole, role );
}

int QskSkinnable::fontRole(
    const QskAspect aspect, QskSkinHintStatus* status ) const
{
    return qskFlag( this, aspect | QskAspect::FontRole, status );
}

QFont QskSkinnable::effectiveFont( const QskAspect aspect ) const
{
    return effectiveSkin()->font( fontRole( aspect ) );
}

void QskSkinnable::setGraphicRole( const QskAspect aspect, int role )
{
    qskSetFlag( this, aspect | QskAspect::GraphicRole, role );
}

int QskSkinnable::graphicRole(
    const QskAspect aspect, QskSkinHintStatus* status ) const
{
    return qskFlag( this, aspect | QskAspect::GraphicRole, status );
}

QskColorFilter QskSkinnable::effectiveGraphicFilter( QskAspect aspect ) const
{
    aspect.setSubControl( effectiveSubcontrol( aspect.subControl() ) );
    aspect.setPlacement( effectivePlacement() );
    aspect = aspect | QskAspect::GraphicRole;

    QskSkinHintStatus status;

    const auto hint = storedHint( aspect | skinState(), &status );
    if ( status.isValid() )
    {
        // we need to know about how the aspect gets resolved
        // before checking for animators

        aspect.setSubControl( status.aspect.subControl() );
    }

    if ( !aspect.isAnimator() )
    {
        auto v = animatedValue( aspect, nullptr );
        if ( v.canConvert< QskColorFilter >() )
            return v.value< QskColorFilter >();

        if ( auto control = owningControl() )
        {
            v = QskSkinTransition::animatedGraphicFilter(
                    control->window(), hint.toInt() );

            if ( v.canConvert< QskColorFilter >() )
            {
                /*
                    As it is hard to find out which controls depend
                    on the animated graphic filters we reschedule
                    our updates here.
                 */
                control->update();
                return v.value< QskColorFilter >();
            }
        }
    }

    return effectiveSkin()->graphicFilter( hint.toInt() );
}

void QskSkinnable::setAnimationHint(
    QskAspect aspect, QskAnimationHint animation )
{
    aspect.setSubControl( effectiveSubcontrol( aspect.subControl() ) );
    m_data->hintTable.setAnimation( aspect, animation );
}

QskAnimationHint QskSkinnable::animationHint(
    QskAspect aspect, QskSkinHintStatus* status ) const
{
    aspect.setAnimator( true );
    return effectiveSkinHint( aspect, status ).value< QskAnimationHint >();
}

QskAnimationHint QskSkinnable::effectiveAnimation(
    QskAspect::Type type, QskAspect::Subcontrol subControl,
    QskAspect::State state, QskSkinHintStatus* status ) const
{
    auto aspect = subControl | type | state;
    aspect.setAnimator( true );

    QskAnimationHint hint;

    const auto a = m_data->hintTable.resolvedAnimator( aspect, hint );
    if ( a.isAnimator() )
    {
        if ( status )
        {
            status->source = QskSkinHintStatus::Skinnable;
            status->aspect = a;
        }

        return hint;
    }

    if ( auto skin = effectiveSkin() )
    {
        const auto a = skin->hintTable().resolvedAnimator( aspect, hint );
        if ( a.isAnimator() )
        {
            if ( status )
            {
                status->source = QskSkinHintStatus::Skin;
                status->aspect = a;
            }

            return hint;
        }
    }

    if ( status )
    {
        status->source = QskSkinHintStatus::NoSource;
        status->aspect = QskAspect();
    }

    return hint;
}

void QskSkinnable::setSkinHint( QskAspect aspect, const QVariant& skinHint )
{
    aspect.setSubControl( effectiveSubcontrol( aspect.subControl() ) );
    m_data->hintTable.setHint( aspect, skinHint );
}

bool QskSkinnable::resetSkinHint( QskAspect aspect )
{
    aspect.setSubControl( effectiveSubcontrol( aspect.subControl() ) );

    if ( !m_data->hintTable.hasHint( aspect ) )
        return false;

    /*
        To be able to indicate, when the resolved value has changed
        we retrieve the value before and after removing the hint from
        the local table. An implementation with less lookups
        should be possible, but as reset is a low frequently called
        operation, we prefer to keep the implementation simple.
     */

    auto a = aspect;
    a.setPlacement( effectivePlacement() );

    if ( !a.hasState() )
        a.setState( skinState() );

    const auto oldHint = storedHint( a );

    m_data->hintTable.removeHint( aspect );

    return oldHint != storedHint( a );
}

QVariant QskSkinnable::effectiveSkinHint(
    QskAspect aspect, QskSkinHintStatus* status ) const
{
    aspect.setSubControl( effectiveSubcontrol( aspect.subControl() ) );
    aspect.setPlacement( effectivePlacement() );

    if ( aspect.isAnimator() )
        return storedHint( aspect, status );

    const auto v = animatedValue( aspect, status );
    if ( v.isValid() )
        return v;

    if ( !aspect.hasState() )
        aspect.setState( skinState() );

    return storedHint( aspect, status );
}

QskSkinHintStatus QskSkinnable::hintStatus( QskAspect aspect ) const
{
    QskSkinHintStatus status;

    ( void ) effectiveSkinHint( aspect, &status );
    return status;
}


QVariant QskSkinnable::animatedValue(
    QskAspect aspect, QskSkinHintStatus* status ) const
{
    QVariant v;

    if ( !aspect.hasState() )
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
        if ( QskSkinTransition::isRunning() &&
            !m_data->hintTable.hasHint( aspect ) )
        {
            /*
               Next we check for values from the skin. Those
               animators are usually from global skin/color changes
               and are state aware
             */

            if ( const auto control = owningControl() )
            {
                if ( aspect.state() == QskAspect::NoState )
                    aspect = aspect | skinState();

                const auto a = aspect;

                Q_FOREVER
                {
                    v = QskSkinTransition::animatedHint( control->window(), aspect );

                    if ( !v.isValid() )
                    {
                        if ( const auto topState = aspect.topState() )
                        {
                            aspect.clearState( aspect.topState() );
                            continue;
                        }

                        if ( aspect.placement() )
                        {
                            // clear the placement bits and restart
                            aspect = a;
                            aspect.setPlacement( QskAspect::NoPlacement );

                            continue;
                        }
                    }

                    break;
                }
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
    QskAspect aspect, QskSkinHintStatus* status ) const
{
    const auto skin = effectiveSkin();

    // clearing all state bits not being handled from the skin
    aspect.clearState( ~skin->stateMask() );

    QskAspect resolvedAspect;

    const auto& localTable = m_data->hintTable;
    if ( localTable.hasHints() )
    {
        auto a = aspect;

        if ( !localTable.hasStates() )
        {
            // we don't need to clear the state bits stepwise
            a.clearStates();
        }

        if ( const QVariant* value = localTable.resolvedHint( a, &resolvedAspect ) )
        {
            if ( status )
            {
                status->source = QskSkinHintStatus::Skinnable;
                status->aspect = resolvedAspect;
            }
            return *value;
        }
    }

    // next we try the hints from the skin

    const auto& skinTable = skin->hintTable();
    if ( skinTable.hasHints() )
    {
        auto a = aspect;

        const QVariant* value = skinTable.resolvedHint( a, &resolvedAspect );
        if ( value )
        {
            if ( status )
            {
                status->source = QskSkinHintStatus::Skin;
                status->aspect = resolvedAspect;
            }

            return *value;
        }

        if ( aspect.subControl() != QskAspect::Control )
        {
            // trying to resolve something from the skin default settings

            aspect.setSubControl( QskAspect::Control );
            aspect.clearStates();

            value = skinTable.resolvedHint( aspect, &resolvedAspect );
            if ( value )
            {
                if ( status )
                {
                    status->source = QskSkinHintStatus::Skin;
                    status->aspect = resolvedAspect;
                }

                return *value;
            }
        }
    }

    if ( status )
    {
        status->source = QskSkinHintStatus::NoSource;
        status->aspect = QskAspect();
    }

    static QVariant hintInvalid;
    return hintInvalid;
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
    static QByteArray bytes[ 10 ];
    static int counter = 0;

    counter = ( counter + 1 ) % 10;

    bytes[ counter ] = tmp.toUtf8();
    return bytes[ counter ].constData();
}

static inline QskMargins qskEffectivePadding( const QskSkinnable* skinnable,
    QskAspect aspect, const QSizeF& size, bool inner )
{
    const auto shape = skinnable->boxShapeHint( aspect ).toAbsolute( size );
    const auto borderMetrics = skinnable->boxBorderMetricsHint( aspect );

    const qreal left = qMax( shape.radius( Qt::TopLeftCorner ).width(),
        shape.radius( Qt::BottomLeftCorner ).width() );

    const qreal top = qMax( shape.radius( Qt::TopLeftCorner ).height(),
        shape.radius( Qt::TopRightCorner ).height() );

    const qreal right = qMax( shape.radius( Qt::TopRightCorner ).width(),
        shape.radius( Qt::BottomRightCorner ).width() );

    const qreal bottom = qMax( shape.radius( Qt::BottomLeftCorner ).height(),
        shape.radius( Qt::BottomRightCorner ).height() );

    QskMargins padding( left, top, right, bottom );

    // half of the border goes to the inner side
    const auto borderMargins = borderMetrics.toAbsolute( size ).widths() * 0.5;

    if ( inner )
    {
        padding -= borderMargins;
    }
    else
    {
        // not correct, but to get things started. TODO ...
        padding += borderMargins;
    }

    // sin 45° ceiled : 0.70710678;
    padding *= 1.0 - 0.70710678;

    const auto paddingHint = skinnable->paddingHint( aspect );

    return QskMargins(
        qMax( padding.left(), paddingHint.left() ),
        qMax( padding.top(), paddingHint.top() ),
        qMax( padding.right(), paddingHint.right() ),
        qMax( padding.bottom(), paddingHint.bottom() )
        );
}

QMarginsF QskSkinnable::innerPadding(
    QskAspect aspect, const QSizeF& outerBoxSize ) const
{
    return qskEffectivePadding( this, aspect, outerBoxSize, true );
}

QSizeF QskSkinnable::innerBoxSize(
    QskAspect aspect, const QSizeF& outerBoxSize ) const
{
    const auto pd = qskEffectivePadding( this, aspect, outerBoxSize, true );

    return QSizeF( outerBoxSize.width() - pd.width(),
        outerBoxSize.height() - pd.height() );
}

QRectF QskSkinnable::innerBox(
    QskAspect aspect, const QRectF& outerBox ) const
{
    const auto pd = qskEffectivePadding( this, aspect, outerBox.size(), true );
    return outerBox.marginsRemoved( pd );
}

QSizeF QskSkinnable::outerBoxSize(
    QskAspect aspect, const QSizeF& innerBoxSize ) const
{
    const auto pd = qskEffectivePadding( this, aspect, innerBoxSize, false );

    // since Qt 5.14 we would have QSizeF::grownBy !
    return QSizeF( innerBoxSize.width() + pd.width(),
        innerBoxSize.height() + pd.height() );
}

QRectF QskSkinnable::outerBox(
    QskAspect aspect, const QRectF& innerBox ) const
{
    const auto m = qskEffectivePadding( this, aspect, innerBox.size(), false );
    return innerBox.marginsAdded( m );
}

bool QskSkinnable::isTransitionAccepted( QskAspect aspect ) const
{
    Q_UNUSED( aspect )

    /*
        Usually we only need smooth transitions, when state changes
        happen while the skinnable is visible. There are few exceptions
        like QskPopup::Closed, that is used to slide/fade in.
     */
    if ( auto control = owningControl() )
        return control->isInitiallyPainted();

    return false;
}

void QskSkinnable::startTransition( QskAspect aspect,
    QskAnimationHint animationHint, QVariant from, QVariant to )
{
    aspect.setSubControl( effectiveSubcontrol( aspect.subControl() ) );
    startHintTransition( aspect, animationHint, from, to );
}

void QskSkinnable::startHintTransition( QskAspect aspect,
    QskAnimationHint animationHint, QVariant from, QVariant to )
{
    if ( animationHint.duration <= 0 || ( from == to ) )
        return;

    auto control = this->owningControl();
    if ( control->window() == nullptr || !isTransitionAccepted( aspect ) )
        return;

    /*
        We might be invalid for one of the values, when an aspect
        has not been defined for all states ( f.e. metrics are expected
        to fallback to 0.0 ). In this case we create a default one.
     */

    if ( !from.isValid() )
    {
        from = qskTypedNullValue( to );
    }
    else if ( !to.isValid() )
    {
        to = qskTypedNullValue( from );
    }
    else if ( from.userType() != to.userType() )
    {
        return;
    }

    if ( aspect.flagPrimitive() == QskAspect::GraphicRole )
    {
        const auto skin = effectiveSkin();

        from.setValue( skin->graphicFilter( from.toInt() ) );
        to.setValue( skin->graphicFilter( to.toInt() ) );
    }

    aspect.clearStates();
    aspect.setAnimator( false );
    aspect.setPlacement( effectivePlacement() );

#if DEBUG_ANIMATOR
    qDebug() << aspect << animationHint.duration;
#endif

    auto animator = m_data->animators.animator( aspect );
    if ( animator && animator->isRunning() )
        from = animator->currentValue();

    m_data->animators.start( control, aspect, animationHint, from, to );
}

void QskSkinnable::setSkinStateFlag( QskAspect::State stateFlag, bool on )
{
    const auto newState = on
        ? ( m_data->skinState | stateFlag )
        : ( m_data->skinState & ~stateFlag );

    setSkinState( newState );
}

void QskSkinnable::setSkinState( QskAspect::State newState, bool animated )
{
    if ( m_data->skinState == newState )
        return;

    auto control = owningControl();

#if DEBUG_STATE
    qDebug() << control->className() << ":"
        << skinStateAsPrintable( m_data->skinState ) << "->"
        << skinStateAsPrintable( newState );
#endif

    const auto skin = effectiveSkin();

    if ( skin )
    {
        const auto mask = skin->stateMask();
        if ( ( newState & mask ) == ( m_data->skinState & mask ) )
        {
            // the modified bits are not handled by the skin

            m_data->skinState = newState;
            return;
        }
    }

    if ( control->window() && animated && isTransitionAccepted( QskAspect() ) )
    {
        const auto placement = effectivePlacement();

        const auto subControls = control->subControls();
        for ( const auto subControl : subControls )
        {
            auto aspect = subControl | placement;

            const auto& skinTable = skin->hintTable();

            for ( uint i = 0; i < QskAspect::typeCount; i++ )
            {
                const auto type = static_cast< QskAspect::Type >( i );

                const auto hint = effectiveAnimation( type, subControl, newState );

                if ( hint.duration > 0 )
                {
                    /*
                        Starting an animator for all primitives,
                        that differ between the states
                     */

                    const auto primitiveCount = QskAspect::primitiveCount( type );

                    for ( uint primitive = 0; primitive < primitiveCount; primitive++ )
                    {
                        aspect.setPrimitive( type, primitive );

                        auto a1 = aspect | m_data->skinState;
                        auto a2 = aspect | newState;

                        bool doTransition = true;

                        if ( !m_data->hintTable.hasStates() )
                        {
                            /*
                                The hints are found by stripping the state bits one by
                                one until a lookup into the hint table is successful.
                                So for deciding whether two aspects lead to the same hint
                                we can stop as soon as the aspects have the same state bits.
                                This way we can reduce the number of lookups significantly
                                for skinnables with many state bits.

                             */
                            doTransition = !qskCompareResolvedStates( a1, a2, skinTable );
                        }

                        if ( doTransition )
                        {
                            startHintTransition( aspect, hint,
                                storedHint( a1 ), storedHint( a2 ) );
                        }
                    }
                }
            }
        }
    }

    m_data->skinState = newState;

    if ( control->flags() & QQuickItem::ItemHasContents )
        control->update();
}

QskSkin* QskSkinnable::effectiveSkin() const
{
    QskSkin* skin = nullptr;

    if ( m_data->skinlet )
        skin = m_data->skinlet->skin();

    return skin ? skin : qskSetup->skin();
}

QskAspect::Placement QskSkinnable::effectivePlacement() const
{
    return QskAspect::NoPlacement;
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

void QskSkinnable::debug( QDebug debug, QskAspect aspect ) const
{
    qskDebugAspect( debug, metaObject(), aspect );
}

void QskSkinnable::debug( QDebug debug, QskAspect::State state ) const
{
    qskDebugState( debug, metaObject(), state );
}

void QskSkinnable::debug( QskAspect aspect ) const
{
    qskDebugAspect( qDebug(), metaObject(), aspect );
}

void QskSkinnable::debug( QskAspect::State state ) const
{
    qskDebugState( qDebug(), metaObject(), state );
}

#ifndef QT_NO_DEBUG_STREAM

#include <qdebug.h>

QDebug operator<<( QDebug debug, const QskSkinHintStatus& status )
{
    QDebugStateSaver saver( debug );
    debug.nospace();

    switch( status.source )
    {
        case QskSkinHintStatus::Skinnable:
            debug << "Skinnable";
            break;
        case QskSkinHintStatus::Skin:
            debug << "Skin";
            break;
        case QskSkinHintStatus::Animator:
            debug << "Animator";
            break;
        default:
            debug << "None";
            break;
    }

    debug << ": " << status.aspect;

    return debug;
}

#endif
