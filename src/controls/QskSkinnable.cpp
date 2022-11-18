/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskSkinnable.h"

#include "QskAnimationHint.h"
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
#include "QskShadowMetrics.h"
#include "QskBoxHints.h"
#include "QskGradient.h"
#include "QskTextOptions.h"

#include <qfont.h>
#include <qfontmetrics.h>
#include <map>

#define DEBUG_MAP 0
#define DEBUG_ANIMATOR 0
#define DEBUG_STATE 0

static inline bool qskIsControl( const QskSkinnable* skinnable )
{
    return skinnable->metaObject()->inherits( &QskControl::staticMetaObject );
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

static inline bool qskMoveMetric( QskSkinnable* skinnable,
    const QskAspect aspect, const QVariant& metric )
{
    return skinnable->moveSkinHint( aspect | QskAspect::Metric, metric );
}

template< typename T >
static inline bool qskSetMetric( QskSkinnable* skinnable,
    QskAspect aspect, const T& metric )
{
    return qskSetMetric( skinnable, aspect, QVariant::fromValue( metric ) );
}

template< typename T >
static inline bool qskMoveMetric( QskSkinnable* skinnable,
    QskAspect aspect, const T& metric )
{
    return qskMoveMetric( skinnable, aspect, QVariant::fromValue( metric ) );
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

static inline bool qskMoveColor( QskSkinnable* skinnable,
    const QskAspect aspect, const QVariant& color )
{
    return skinnable->moveSkinHint( aspect | QskAspect::Color, color );
}

template< typename T >
static inline bool qskSetColor( QskSkinnable* skinnable,
    const QskAspect aspect, const T& color )
{
    return qskSetColor( skinnable, aspect, QVariant::fromValue( color ) );
}

template< typename T >
static inline bool qskMoveColor( QskSkinnable* skinnable,
    const QskAspect aspect, const T& color )
{
    return qskMoveColor( skinnable, aspect, QVariant::fromValue( color ) );
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
#if 0
    if ( aspect.hasStates() )
    {
        qWarning() << "QskSkinnable::(re)setSkinHint: setting hints with states "
            "is discouraged - use QskSkinTableEditor if you are sure, that you need this.";

        qWarning() << "QskAspect:" << aspect.stateless()
                   << skinnable->skinStatesAsPrintable( aspect.states() );

#if 0
        aspect.clearStates();
#endif
    }
#endif

    aspect.setSubcontrol( skinnable->effectiveSubcontrol( aspect.subControl() ) );
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
    if ( subControl == QskAspect::NoSubcontrol )
        return; // nonsense, we ignore this

    if ( proxy == QskAspect::NoSubcontrol || subControl == proxy )
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

    return QskAspect::NoSubcontrol;
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

bool QskSkinnable::moveColor( const QskAspect aspect, const QColor& color )
{
    return qskMoveColor( this, aspect, color );
}

bool QskSkinnable::moveColor( const QskAspect aspect, Qt::GlobalColor color )
{
    return qskMoveColor( this, aspect, QColor( color ) );
}

bool QskSkinnable::moveColor( const QskAspect aspect, QRgb rgb )
{
    return qskMoveColor( this, aspect, QColor::fromRgba( rgb ) );
}

QColor QskSkinnable::color( const QskAspect aspect, QskSkinHintStatus* status ) const
{
    return qskColor< QColor >( this, aspect, status );
}

bool QskSkinnable::setMetric( const QskAspect aspect, qreal metric )
{
    return qskSetMetric( this, aspect, metric );
}

bool QskSkinnable::moveMetric( QskAspect aspect, qreal metric )
{
    return qskMoveMetric( this, aspect, metric );
}

qreal QskSkinnable::metric( const QskAspect aspect, QskSkinHintStatus* status ) const
{
    return qskMetric< qreal >( this, aspect, status );
}

bool QskSkinnable::setPositionHint( QskAspect aspect, qreal position )
{
    return qskSetMetric( this, aspect | QskAspect::Position, position );
}

bool QskSkinnable::movePositionHint( QskAspect aspect, qreal position )
{
    return qskMoveMetric( this, aspect | QskAspect::Position, position );
}

bool QskSkinnable::movePositionHint( QskAspect aspect, qreal from, qreal to )
{
    return moveSkinHint( aspect | QskAspect::Metric | QskAspect::Position,
        QVariant::fromValue( from ), QVariant::fromValue( to ) );
}

bool QskSkinnable::resetPositionHint( QskAspect aspect )
{
    return resetMetric( aspect | QskAspect::Position );
}

qreal QskSkinnable::positionHint( QskAspect aspect, QskSkinHintStatus* status ) const
{
    return qskMetric< qreal >( this, aspect | QskAspect::Position, status );
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

bool QskSkinnable::setShadowMetricsHint(
    QskAspect aspect, const QskShadowMetrics& metrics )
{
    return qskSetMetric( this, aspect | QskAspect::Shadow, metrics );
}

bool QskSkinnable::resetShadowMetricsHint( QskAspect aspect )
{
    return resetMetric( aspect | QskAspect::Shadow );
}

QskShadowMetrics QskSkinnable::shadowMetricsHint(
    QskAspect aspect, QskSkinHintStatus* status ) const
{
    return qskMetric< QskShadowMetrics >(
        this, aspect | QskAspect::Shadow, status );
}

bool QskSkinnable::setShadowColorHint( QskAspect aspect, const QColor& color )
{
    return qskSetColor( this, aspect | QskAspect::Shadow, color );
}

bool QskSkinnable::resetShadowColorHint( QskAspect aspect )
{
    return resetColor( aspect | QskAspect::Shadow );
}

QColor QskSkinnable::shadowColorHint( QskAspect aspect, QskSkinHintStatus* status ) const
{
    return qskColor< QColor >( this, aspect | QskAspect::Shadow, status );
}

QskBoxHints QskSkinnable::boxHints( QskAspect aspect ) const
{
    return QskBoxHints(
        boxShapeHint( aspect ), boxBorderMetricsHint( aspect ),
        boxBorderColorsHint( aspect ), gradientHint( aspect ),
        shadowMetricsHint( aspect ), shadowColorHint( aspect ) );
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

bool QskSkinnable::setTextOptionsHint(
    const QskAspect aspect, const QskTextOptions& options )
{
    return setSkinHint( aspect | QskAspect::Flag | QskAspect::Option,
        QVariant::fromValue( options ) );
}

bool QskSkinnable::resetTextOptionsHint( const QskAspect aspect )
{
    return resetFlagHint( aspect | QskAspect::Option );
}

QskTextOptions QskSkinnable::textOptionsHint(
    const QskAspect aspect, QskSkinHintStatus* status ) const
{
    return effectiveSkinHint(
        aspect | QskAspect::Flag | QskAspect::Option, status ).value< QskTextOptions >();
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

QFont QskSkinnable::effectiveFont( const QskAspect::Subcontrol subControl ) const
{
    return effectiveSkin()->font( fontRoleHint( subControl ) );
}

qreal QskSkinnable::effectiveFontHeight( const QskAspect::Subcontrol subControl ) const
{
    const QFontMetricsF fm( effectiveFont( subControl ) );
    return fm.height();
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

QskColorFilter QskSkinnable::effectiveGraphicFilter(
    const QskAspect::Subcontrol subControl ) const
{
    /*
        Usually we find the graphic role and return the related filter
        from the skin. But as we can't interpolate between graphic roles
        the corresponding animators interpolate the filters.
     */

    QskAspect aspect( effectiveSubcontrol( subControl ) | QskAspect::GraphicRole );
    aspect.setSection( section() );
    aspect.setPlacement( effectivePlacement() );

    QskSkinHintStatus status;

    const auto hint = storedHint( aspect | skinStates(), &status );
    if ( !status.isValid() )
        return QskColorFilter();

    aspect.setSubcontrol( status.aspect.subControl() );
    aspect.setSection( QskAspect::Body );
    aspect.setPlacement( QskAspect::NoPlacement );

    const auto v = animatedHint( aspect, nullptr );

    if ( v.canConvert< QskColorFilter >() )
        return v.value< QskColorFilter >();

    if ( auto control = owningControl() )
    {
        const auto graphicRole = hint.toInt();

        const auto v = QskSkinTransition::animatedGraphicFilter(
            control->window(), graphicRole );

        if ( v.canConvert< QskColorFilter >() )
        {
#if 1
            /*
                Design flaw: the animators for the skin transition do not
                know about the controls, that are affected from the color
                filter. As a workaround we schedule the update in the
                getter: TODO ...
             */
            control->update();
#endif
            return v.value< QskColorFilter >();
        }
    }

    return effectiveSkin()->graphicFilter( hint.toInt() );
}

bool QskSkinnable::setAnimationHint(
    QskAspect aspect, QskAnimationHint hint )
{
    aspect.setSubcontrol( effectiveSubcontrol( aspect.subControl() ) );
    return m_data->hintTable.setAnimation( aspect, hint );
}

QskAnimationHint QskSkinnable::animationHint(
    QskAspect aspect, QskSkinHintStatus* status ) const
{
    aspect.setAnimator( true );
    return effectiveSkinHint( aspect, status ).value< QskAnimationHint >();
}

bool QskSkinnable::hasAnimationHint( QskAspect aspect ) const
{
    return animationHint( aspect ).isValid();
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
    aspect.setSubcontrol( effectiveSubcontrol( aspect.subControl() ) );

    if ( !( aspect.isAnimator() || aspect.hasStates() ) )
    {
        const auto v = animatedHint( aspect, status );
        if ( v.isValid() )
            return v;
    }

    if ( aspect.section() == QskAspect::Body )
        aspect.setSection( section() );

    if ( aspect.placement() == QskAspect::NoPlacement )
        aspect.setPlacement( effectivePlacement() );

    if ( !aspect.hasStates() )
        aspect.setStates( skinStates() );

    if ( !aspect.isAnimator() && QskSkinTransition::isRunning() )
    {
        /*
            The skin has changed and the hints are interpolated
            between the old and the new one over time
         */
        const auto v = interpolatedHint( aspect, status );
        if ( v.isValid() )
            return v;
    }

    return storedHint( aspect, status );
}

QskSkinHintStatus QskSkinnable::hintStatus( QskAspect aspect ) const
{
    QskSkinHintStatus status;

    ( void ) effectiveSkinHint( aspect, &status );
    return status;
}

bool QskSkinnable::moveSkinHint( QskAspect aspect,
    const QVariant& oldValue, const QVariant& newValue )
{
    if ( aspect.isAnimator() )
        return false;

    const bool ok = setSkinHint( aspect, newValue );

    if ( ok && oldValue.isValid() && newValue.isValid() )
    {
        const auto animation = animationHint( aspect );
        if ( animation.isValid() )
        {
            if ( newValue != oldValue )
                startTransition( aspect, animation, oldValue, newValue );
        }
    }

    return ok;
}

bool QskSkinnable::moveSkinHint( QskAspect aspect, const QVariant& value )
{
    return moveSkinHint( aspect, effectiveSkinHint( aspect ), value );
}

QVariant QskSkinnable::animatedHint(
    QskAspect aspect, QskSkinHintStatus* status ) const
{
    QVariant v;

    if ( !m_data->animators.isEmpty() )
    {
        const int index = effectiveSkinlet()->animatorIndex();

        v = m_data->animators.currentValue( aspect, index );
        if ( !v.isValid() && index >= 0 )
            v = m_data->animators.currentValue( aspect, -1 );
    }

    if ( status && v.isValid() )
    {
        status->source = QskSkinHintStatus::Animator;
        status->aspect = aspect;
    }

    return v;
}

QVariant QskSkinnable::interpolatedHint(
    QskAspect aspect, QskSkinHintStatus* status ) const
{
    if ( !QskSkinTransition::isRunning() || m_data->hintTable.hasHint( aspect ) )
        return QVariant();

    const auto control = owningControl();
    if ( control == nullptr )
        return QVariant();

    QVariant v;

    auto a = aspect;

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

        if ( a.section() != QskAspect::Body )
        {
            // try to resolve with the default section

            a.setSection( QskAspect::Body );
            aspect = a;

            continue;
        }

        break;
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

    QskAspect resolvedAspect;

    const auto& localTable = m_data->hintTable;
    if ( localTable.hasHints() )
    {
        if ( const auto value = localTable.resolvedHint( aspect, &resolvedAspect ) )
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
        if ( const auto value = skinTable.resolvedHint( aspect, &resolvedAspect ) )
        {
            if ( status )
            {
                status->source = QskSkinHintStatus::Skin;
                status->aspect = resolvedAspect;
            }

            return *value;
        }

        if ( aspect.hasSubcontrol() )
        {
            // trying to resolve something from the skin default settings

            aspect.clearSubcontrol();
            aspect.clearStates();

            if ( const auto value = skinTable.resolvedHint( aspect, &resolvedAspect ) )
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
    QskAnimationHint animationHint, const QVariant& from, const QVariant& to )
{
    startTransition( aspect, -1, animationHint, from, to );
}

void QskSkinnable::startTransition( QskAspect aspect, int index,
    QskAnimationHint animationHint, const QVariant& from, const QVariant& to )
{
    aspect.setSubcontrol( effectiveSubcontrol( aspect.subControl() ) );
    startHintTransition( aspect, index, animationHint, from, to );
}

void QskSkinnable::startHintTransition( QskAspect aspect, int index,
    QskAnimationHint animationHint, const QVariant& from, const QVariant& to )
{
    if ( animationHint.duration <= 0 || ( from == to ) )
        return;

    auto control = this->owningControl();
    if ( control->window() == nullptr || !isTransitionAccepted( aspect ) )
        return;

    if ( !QskVariantAnimator::maybeInterpolate( from, to ) )
        return;

    auto v1 = from;
    auto v2 = to;

    if ( aspect.flagPrimitive() == QskAspect::GraphicRole )
    {
        const auto skin = effectiveSkin();

        v1.setValue( skin->graphicFilter( v1.toInt() ) );
        v2.setValue( skin->graphicFilter( v2.toInt() ) );
    }

    /*
        We do not need the extra bits that would slow down resolving
        the effective aspect in animatedHint.
     */

    aspect.clearStates();
    aspect.setSection( QskAspect::Body );
    aspect.setPlacement( QskAspect::NoPlacement );
    aspect.setAnimator( false );

#if DEBUG_ANIMATOR
    qDebug() << aspect << animationHint.duration;
#endif

    auto animator = m_data->animators.animator( aspect, index );
    if ( animator && animator->isRunning() )
        v1 = animator->currentValue();

    m_data->animators.start( control, aspect, index, animationHint, v1, v2 );
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
    const auto className = control ? control->className() : "QskSkinnable";

    qDebug() << className << ":"
        << skinStateAsPrintable( m_data->skinState ) << "->"
        << skinStateAsPrintable( newState );
#endif

    if ( control && control->window() )
    {
        if ( const auto skin = effectiveSkin() )
        {
            const auto mask = m_data->hintTable.states() | skin->hintTable().states();
            if ( ( newStates & mask ) != ( m_data->skinStates & mask ) )
            {
                /*
                    When there are no aspects for the changed state bits we know
                    that there won't be any animated transitions
                 */

                startHintTransitions( m_data->skinStates, newStates );
            }
        }

        if ( control->flags() & QQuickItem::ItemHasContents )
            control->update();
    }

    m_data->skinStates = newStates;
}

bool QskSkinnable::startHintTransitions(
    QskAspect::States oldStates, QskAspect::States newStates, int index )
{
    if ( !isTransitionAccepted( QskAspect() ) )
    {
        // the control does not like any animated transition at the moment
        return false;
    }

    bool started = false; // at least one transition has been started

    QskAspect aspect;
    aspect.setPlacement( effectivePlacement() );
    aspect.setSection( section() );

    const auto skin = effectiveSkin();
    const auto control = owningControl();

    const auto primitiveCount = QskAspect::primitiveCount();

    const auto subControls = control->subControls();
    for ( const auto subControl : subControls )
    {
        aspect.setSubcontrol( subControl );

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

                    const auto a1 = aspect | oldStates;
                    const auto a2 = aspect | newStates;

                    bool doTransition = true;

                    if ( m_data->hintTable.states() == QskAspect::NoState )
                    {
                        /*
                            In case we have no state aware aspects in the local
                            table we can avoid starting animators for aspects,
                            that are finally resolved from the same hint in
                            the skin table.
                         */

                        doTransition = !skinTable.isResolutionMatching( a1, a2 );
                    }

                    if ( doTransition )
                    {
                        startHintTransition( aspect, index, hint,
                            storedHint( a1 ), storedHint( a2 ) );

                        started = true;
                    }
                }
            }
        }
    }

    return started;
}

QskSkin* QskSkinnable::effectiveSkin() const
{
    QskSkin* skin = nullptr;

    if ( m_data->skinlet )
        skin = m_data->skinlet->skin();

    if ( skin == nullptr )
    {
        if ( const auto control = owningControl() )
            skin = qskEffectiveSkin( control->window() );
    }

    return skin ? skin : qskSetup->skin();
}

QskAspect::Placement QskSkinnable::effectivePlacement() const
{
    return QskAspect::NoPlacement;
}

QskAspect::Section QskSkinnable::section() const
{
    return QskAspect::Body;
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
