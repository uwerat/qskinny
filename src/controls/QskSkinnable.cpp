/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskSkinnable.h"

#include "QskAnimationHint.h"
#include "QskArcBorderColors.h"
#include "QskArcBorderMetrics.h"
#include "QskArcMetrics.h"
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
#include "QskWindow.h"

#include "QskBoxShapeMetrics.h"
#include "QskBoxBorderMetrics.h"
#include "QskBoxBorderColors.h"
#include "QskGradient.h"

#include <qfont.h>
#include <map>

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

static inline QVariant qskTypedNullValue( const QVariant& value )
{
#if QT_VERSION >= QT_VERSION_CHECK( 6, 0, 0 )
    const auto vType = static_cast< QMetaType >( value.userType() );
#else
    const auto vType = value.userType();
#endif

    return QVariant( vType, nullptr );
}

static inline bool qskSetFlag( QskSkinnable* skinnable,
    const QskAspect aspect, int flag )
{
    return skinnable->setSkinHint( aspect | QskAspect::Flag, QVariant( flag ) );
}

static inline int qskFlag( const QskSkinnable* skinnable,
    const QskAspect aspect, QskSkinHintStatus* status = nullptr )
{
    return skinnable->effectiveSkinHint( aspect | QskAspect::Flag, status ).toInt();
}

static inline bool qskSetMetric( QskSkinnable* skinnable,
    const QskAspect aspect, const QVariant& metric )
{
    return skinnable->setSkinHint( aspect | QskAspect::Metric, metric );
}

template< typename T >
static inline bool qskSetMetric( QskSkinnable* skinnable,
    QskAspect aspect, const T& metric )
{
    return qskSetMetric( skinnable, aspect, QVariant::fromValue( metric ) );
}

template< typename T >
static inline T qskMetric( const QskSkinnable* skinnable,
    QskAspect aspect, QskSkinHintStatus* status = nullptr )
{
    return skinnable->effectiveSkinHint(
        aspect | QskAspect::Metric, status ).value< T >();
}

static inline bool qskSetColor( QskSkinnable* skinnable,
    const QskAspect aspect, const QVariant& color )
{
    return skinnable->setSkinHint( aspect | QskAspect::Color, color );
}

template< typename T >
static inline bool qskSetColor( QskSkinnable* skinnable,
    const QskAspect aspect, const T& color )
{
    return qskSetColor( skinnable, aspect, QVariant::fromValue( color ) );
}

template< typename T >
static inline T qskColor( const QskSkinnable* skinnable,
    QskAspect aspect, QskSkinHintStatus* status = nullptr )
{
    return skinnable->effectiveSkinHint(
        aspect | QskAspect::Color, status ).value< T >();
}

static inline void qskTriggerUpdates( QskAspect aspect, QskControl* control )
{
    /*
        To put the hint into effect we have to call the usual suspects:

            - resetImplicitSize
            - polish
            - update

        The following code decides about these calls based on type/primitive
        of the aspect. It can be expected, that it results in more calls
        than what would be mandatory and in rare cases we might even miss necessary
        calls. This has to be fixed by doing the call manually in the specific
        controls.
     */

    if ( control == nullptr || aspect.isAnimator() )
        return;

    bool maybeLayout = false;

    switch( aspect.type() )
    {
        using A = QskAspect;

        case A::Metric:
        {
            if ( aspect.metricPrimitive() != A::Position )
            {
                control->resetImplicitSize();
                maybeLayout = true;
            }

            break;
        }

        case A::Color:
        {
            break;
        }

        case A::Flag:
        {
            switch( aspect.flagPrimitive() )
            {
                case A::GraphicRole:
                case A::FontRole:
                {
                    break;
                }
                case A::Alignment:
                {
                    maybeLayout = true;
                    break;
                }
                default:
                {
                    control->resetImplicitSize();
                    maybeLayout = true;
                }
            }
        }
    }

    control->update(); // always

    if ( maybeLayout && control->hasChildItems() )
    {
        if ( control->polishOnResize() || control->autoLayoutChildren() )
            control->polish();
    }
}

static inline QskAspect qskSubstitutedAspect(
    const QskSkinnable* skinnable, QskAspect aspect )
{
    if ( aspect.hasStates() )
    {
        qWarning() << "QskSkinnable::(re)setSkinHint: setting hints with states "
                      "is discouraged - use QskSkinTableEditor if you are "
                      "sure, that you need this.";

        qWarning() << "QskAspect:" << aspect.stateless()
                   << skinnable->skinStatesAsPrintable( aspect.states() );

#if 0
        aspect.clearStates();
#endif
    }

    aspect.setSubControl( skinnable->effectiveSubcontrol( aspect.subControl() ) );
    return aspect;
}

class QskSkinnable::PrivateData
{
  public:
    ~PrivateData()
    {
        if ( hasLocalSkinlet )
        {
            if ( skinlet && skinlet->isOwnedBySkinnable() )
                delete skinlet;
        }

        delete subcontrolProxies;
    }

    QskSkinHintTable hintTable;
    QskHintAnimatorTable animators;

    typedef std::map< QskAspect::Subcontrol, QskAspect::Subcontrol > ProxyMap;
    ProxyMap* subcontrolProxies = nullptr;

    const QskSkinlet* skinlet = nullptr;

    QskAspect::States skinStates;
    bool hasLocalSkinlet = false;
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

    if ( auto control = owningControl() )
    {
        control->resetImplicitSize();
        control->polish();

        if ( control->flags() & QQuickItem::ItemHasContents )
            control->update();
    }
}

const QskSkinlet* QskSkinnable::skinlet() const
{
    return m_data->hasLocalSkinlet ? m_data->skinlet : nullptr;
}

const QskSkinlet* QskSkinnable::effectiveSkinlet() const
{
    if ( m_data->skinlet == nullptr )
    {
        m_data->skinlet = effectiveSkin()->skinlet( metaObject() );
        m_data->hasLocalSkinlet = false;
    }

    return m_data->skinlet;
}

void QskSkinnable::setSubcontrolProxy(
    QskAspect::Subcontrol subControl, QskAspect::Subcontrol proxy )
{
    if ( subControl == QskAspect::Control )
        return; // nonsense, we ignore this

    if ( proxy == QskAspect::Control || subControl == proxy )
    {
        resetSubcontrolProxy( subControl );
        return;
    }

    if ( m_data->subcontrolProxies == nullptr )
        m_data->subcontrolProxies = new PrivateData::ProxyMap();

    ( *m_data->subcontrolProxies )[ subControl ] = proxy;
}

void QskSkinnable::resetSubcontrolProxy( QskAspect::Subcontrol subcontrol )
{
    if ( auto& proxies = m_data->subcontrolProxies )
    {
        auto it = proxies->find( subcontrol );
        if ( it != proxies->end() )
        {
            proxies->erase( it );
            if ( proxies->empty() )
            {
                delete proxies;
                proxies = nullptr;
            }
        }
    }
}

QskAspect::Subcontrol QskSkinnable::subcontrolProxy( QskAspect::Subcontrol subControl ) const
{
    if ( const auto proxies = m_data->subcontrolProxies )
    {
        auto it = proxies->find( subControl );
        if ( it != proxies->end() )
            return it->second;
    }

    return QskAspect::Control;
}

QskSkinHintTable& QskSkinnable::hintTable()
{
    return m_data->hintTable;
}

const QskSkinHintTable& QskSkinnable::hintTable() const
{
    return m_data->hintTable;
}

bool QskSkinnable::setFlagHint( const QskAspect aspect, int flag )
{
    return qskSetFlag( this, aspect, flag );
}

int QskSkinnable::flagHint( const QskAspect aspect ) const
{
    return effectiveSkinHint( aspect ).toInt();
}

bool QskSkinnable::setAlignmentHint( const QskAspect aspect, Qt::Alignment alignment )
{
    return qskSetFlag( this, aspect | QskAspect::Alignment, alignment );
}

bool QskSkinnable::resetAlignmentHint( const QskAspect aspect )
{
    return resetFlagHint( aspect | QskAspect::Alignment );
}

bool QskSkinnable::setColor( const QskAspect aspect, const QColor& color )
{
    return qskSetColor( this, aspect, color );
}

bool QskSkinnable::setColor( const QskAspect aspect, Qt::GlobalColor color )
{
    return qskSetColor( this, aspect, QColor( color ) );
}

bool QskSkinnable::setColor( const QskAspect aspect, QRgb rgb )
{
    return qskSetColor( this, aspect, QColor::fromRgba( rgb ) );
}

QColor QskSkinnable::color( const QskAspect aspect, QskSkinHintStatus* status ) const
{
    return qskColor< QColor >( this, aspect, status );
}

bool QskSkinnable::setMetric( const QskAspect aspect, qreal metric )
{
    return qskSetMetric( this, aspect, metric );
}

qreal QskSkinnable::metric( const QskAspect aspect, QskSkinHintStatus* status ) const
{
    return qskMetric< qreal >( this, aspect, status );
}

bool QskSkinnable::setStrutSizeHint(
    const QskAspect aspect, qreal width, qreal height )
{
    return qskSetMetric( this, aspect | QskAspect::StrutSize, QSizeF( width, height ) );
}

bool QskSkinnable::setStrutSizeHint( const QskAspect aspect, const QSizeF& size )
{
    return qskSetMetric( this, aspect | QskAspect::StrutSize, size );
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

bool QskSkinnable::setMarginHint( const QskAspect aspect, qreal margins )
{
    return qskSetMetric( this, aspect | QskAspect::Margin, QskMargins( margins ) );
}

bool QskSkinnable::setMarginHint( const QskAspect aspect, const QMarginsF& margins )
{
    return qskSetMetric( this, aspect | QskAspect::Margin, QskMargins( margins ) );
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

bool QskSkinnable::setPaddingHint( const QskAspect aspect, qreal padding )
{
    return qskSetMetric( this, aspect | QskAspect::Padding, QskMargins( padding ) );
}

bool QskSkinnable::setPaddingHint( const QskAspect aspect, const QMarginsF& padding )
{
    return qskSetMetric( this, aspect | QskAspect::Padding, QskMargins( padding ) );
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

bool QskSkinnable::setGradientHint(
    const QskAspect aspect, const QskGradient& gradient )
{
    return qskSetColor( this, aspect, gradient );
}

QskGradient QskSkinnable::gradientHint(
    const QskAspect aspect, QskSkinHintStatus* status ) const
{
    return qskColor< QskGradient >( this, aspect, status );
}

bool QskSkinnable::setBoxShapeHint(
    const QskAspect aspect, const QskBoxShapeMetrics& shape )
{
    return qskSetMetric( this, aspect | QskAspect::Shape, shape );
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

bool QskSkinnable::setBoxBorderMetricsHint(
    const QskAspect aspect, const QskBoxBorderMetrics& border )
{
    return qskSetMetric( this, aspect | QskAspect::Border, border );
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

bool QskSkinnable::setBoxBorderColorsHint(
    const QskAspect aspect, const QskBoxBorderColors& colors )
{
    return qskSetColor( this, aspect | QskAspect::Border, colors );
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

bool QskSkinnable::setArcMetricsHint(
    const QskAspect aspect, const QskArcMetrics& arc )
{
    return qskSetMetric( this, aspect | QskAspect::Shape, arc );
}

bool QskSkinnable::resetArcMetricsHint( const QskAspect aspect )
{
    return resetMetric( aspect | QskAspect::Shape );
}

QskArcMetrics QskSkinnable::arcMetricsHint(
    const QskAspect aspect, QskSkinHintStatus* status ) const
{
    return qskMetric< QskArcMetrics >(
        this, aspect | QskAspect::Shape, status );
}

bool QskSkinnable::setArcBorderMetricsHint(
    const QskAspect aspect, const QskArcBorderMetrics& arc )
{
    return qskSetMetric( this, aspect | QskAspect::Border, arc );
}

bool QskSkinnable::resetArcBorderMetricsHint( const QskAspect aspect )
{
    return resetMetric( aspect | QskAspect::Border );
}

QskArcBorderMetrics QskSkinnable::arcBorderMetricsHint(
    const QskAspect aspect, QskSkinHintStatus* status ) const
{
    return qskMetric< QskArcBorderMetrics >(
        this, aspect | QskAspect::Border, status );
}

bool QskSkinnable::setArcBorderColorsHint(
    const QskAspect aspect, const QskArcBorderColors& colors )
{
    return qskSetColor( this, aspect | QskAspect::Border, colors );
}

bool QskSkinnable::resetArcBorderColorsHint( const QskAspect aspect )
{
    return resetColor( aspect | QskAspect::Border );
}

QskArcBorderColors QskSkinnable::arcBorderColorsHint(
    const QskAspect aspect, QskSkinHintStatus* status ) const
{
    return qskColor< QskArcBorderColors >(
        this, aspect | QskAspect::Border, status );
}

bool QskSkinnable::setSpacingHint( const QskAspect aspect, qreal spacing )
{
    return qskSetMetric( this, aspect | QskAspect::Spacing, spacing );
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

bool QskSkinnable::setFontRoleHint( const QskAspect aspect, int role )
{
    return qskSetFlag( this, aspect | QskAspect::FontRole, role );
}

bool QskSkinnable::resetFontRoleHint( const QskAspect aspect )
{
    return resetFlagHint( aspect | QskAspect::FontRole );
}

int QskSkinnable::fontRoleHint(
    const QskAspect aspect, QskSkinHintStatus* status ) const
{
    return qskFlag( this, aspect | QskAspect::FontRole, status );
}

QFont QskSkinnable::effectiveFont( const QskAspect aspect ) const
{
    return effectiveSkin()->font( fontRoleHint( aspect ) );
}

bool QskSkinnable::setGraphicRoleHint( const QskAspect aspect, int role )
{
    return qskSetFlag( this, aspect | QskAspect::GraphicRole, role );
}

bool QskSkinnable::resetGraphicRoleHint( const QskAspect aspect )
{
    return resetFlagHint( aspect | QskAspect::GraphicRole );
}

int QskSkinnable::graphicRoleHint(
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

    const auto hint = storedHint( aspect | skinStates(), &status );
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

bool QskSkinnable::setAnimationHint(
    QskAspect aspect, QskAnimationHint hint )
{
    aspect.setSubControl( effectiveSubcontrol( aspect.subControl() ) );
    return m_data->hintTable.setAnimation( aspect, hint );
}

QskAnimationHint QskSkinnable::animationHint(
    QskAspect aspect, QskSkinHintStatus* status ) const
{
    aspect.setAnimator( true );
    return effectiveSkinHint( aspect, status ).value< QskAnimationHint >();
}

QskAnimationHint QskSkinnable::effectiveAnimation(
    QskAspect::Type type, QskAspect::Subcontrol subControl,
    QskAspect::States states, QskSkinHintStatus* status ) const
{
#if 0
    // TODO ...
    subControl = effectiveSubcontrol( aspect.subControl() );
#endif

    auto aspect = subControl | type | states;
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

bool QskSkinnable::setSkinHint( QskAspect aspect, const QVariant& hint )
{
    aspect = qskSubstitutedAspect( this, aspect );

    if ( m_data->hintTable.setHint( aspect, hint ) )
    {
        qskTriggerUpdates( aspect, owningControl() );
        return true;
    }

    return false;
}

bool QskSkinnable::resetSkinHint( QskAspect aspect )
{
    aspect = qskSubstitutedAspect( this, aspect );

    if ( m_data->hintTable.removeHint( aspect ) )
    {
        qskTriggerUpdates( aspect, owningControl() );
        return true;
    }

    return false;
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

    if ( !aspect.hasStates() )
        aspect.setStates( skinStates() );

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

    if ( !aspect.hasStates() )
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
                if ( !aspect.hasStates() )
                    aspect.setStates( skinStates() );

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
    aspect.clearStates( ~skin->stateMask() );

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

bool QskSkinnable::hasSkinState( QskAspect::State state ) const
{
    return ( m_data->skinStates & state ) == state;
}

QskAspect::States QskSkinnable::skinStates() const
{
    return m_data->skinStates;
}

const char* QskSkinnable::skinStatesAsPrintable() const
{
    return skinStatesAsPrintable( skinStates() );
}

const char* QskSkinnable::skinStatesAsPrintable( QskAspect::States states ) const
{
    QString tmp;

    QDebug debug( &tmp );
    qskDebugStates( debug, metaObject(), states );

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

    return padding.expandedTo( skinnable->paddingHint( aspect ) );
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

QRectF QskSkinnable::subControlRect(
    const QRectF& contentsRect, QskAspect::Subcontrol subControl ) const
{
    return effectiveSkinlet()->subControlRect( this, contentsRect, subControl );
}

QRectF QskSkinnable::subControlContentsRect(
    const QRectF& contentsRect, QskAspect::Subcontrol subControl ) const
{
    return innerBox( subControl, subControlRect( contentsRect, subControl ) );
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
        ? ( m_data->skinStates | stateFlag )
        : ( m_data->skinStates & ~stateFlag );

    setSkinStates( newState );
}

void QskSkinnable::replaceSkinStates( QskAspect::States newStates )
{
    m_data->skinStates = newStates;
}

void QskSkinnable::addSkinStates( QskAspect::States states )
{
    setSkinStates( m_data->skinStates | states );
}

void QskSkinnable::clearSkinStates( QskAspect::States states )
{
    setSkinStates( m_data->skinStates & ~states );
}

void QskSkinnable::setSkinStates( QskAspect::States newStates )
{
    if ( m_data->skinStates == newStates )
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
        if ( ( newStates & mask ) == ( m_data->skinStates & mask ) )
        {
            // the modified bits are not handled by the skin

            m_data->skinStates = newStates;
            return;
        }
    }

    if ( control->window() && isTransitionAccepted( QskAspect() ) )
    {
        const auto placement = effectivePlacement();
        const auto primitiveCount = QskAspect::primitiveCount();

        const auto subControls = control->subControls();
        for ( const auto subControl : subControls )
        {
            auto aspect = subControl | placement;

            const auto& skinTable = skin->hintTable();

            for ( uint i = 0; i < QskAspect::typeCount; i++ )
            {
                const auto type = static_cast< QskAspect::Type >( i );

                const auto hint = effectiveAnimation( type, subControl, newStates );

                if ( hint.duration > 0 )
                {
                    /*
                        Starting an animator for all primitives,
                        that differ between the states
                     */

                    for ( uint i = 0; i < primitiveCount; i++ )
                    {
                        const auto primitive = static_cast< QskAspect::Primitive >( i );
                        aspect.setPrimitive( type, primitive );

                        auto a1 = aspect | m_data->skinStates;
                        auto a2 = aspect | newStates;

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
                            doTransition = !skinTable.isResolutionMatching( a1, a2 );
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

    m_data->skinStates = newStates;

    if ( control->flags() & QQuickItem::ItemHasContents )
        control->update();
}

QskSkin* QskSkinnable::effectiveSkin() const
{
    QskSkin* skin = nullptr;

    if ( m_data->skinlet )
        skin = m_data->skinlet->skin();

    if ( skin == nullptr )
    {
        if ( const auto control = owningControl() )
        {
            if ( auto window = qobject_cast< const QskWindow* >( control->window() ) )
            {
                skin = window->skin();
            }
        }
    }

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
    if ( const auto proxies = m_data->subcontrolProxies )
    {
        auto it = proxies->find( subControl );
        if ( it != proxies->end() )
            return it->second;
    }

    return substitutedSubcontrol( subControl );
}

QskAspect::Subcontrol QskSkinnable::substitutedSubcontrol(
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
    qskDebugStates( debug, metaObject(), state );
}

void QskSkinnable::debug( QskAspect aspect ) const
{
    qskDebugAspect( qDebug(), metaObject(), aspect );
}

void QskSkinnable::debug( QskAspect::State state ) const
{
    qskDebugStates( qDebug(), metaObject(), state );
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
