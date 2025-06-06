/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#ifndef QSK_SKINNABLE_H
#define QSK_SKINNABLE_H

#include "QskAspect.h"
#include <memory>

typedef unsigned int QRgb;
class QSizeF;
class QRectF;
class QColor;
class QFont;
class QMarginsF;
struct QMetaObject;
class QVariant;
class QDebug;

class QSGNode;
class QQuickItem;

class QskArcHints;
class QskArcMetrics;
class QskControl;
class QskAnimationHint;
class QskHintAnimator;
class QskColorFilter;
class QskBoxShapeMetrics;
class QskBoxBorderMetrics;
class QskBoxBorderColors;
class QskShadowMetrics;
class QskStippleMetrics;
class QskTextOptions;
class QskBoxHints;
class QskGradient;
class QskGraphic;
class QskFontRole;

class QskSkin;
class QskSkinlet;
class QskSkinHintTable;
class QskSkinStateChanger;

class QSK_EXPORT QskSkinHintStatus
{
  public:
    enum Source : quint8
    {
        NoSource = 0,

        Skinnable,
        Skin,
        Animator
    };

    QskSkinHintStatus()
        : source( NoSource )
    {
    }

    inline bool isValid() const
    {
        return source > 0;
    }

    Source source;
    QskAspect aspect;
};

#ifndef QT_NO_DEBUG_STREAM

class QDebug;
QSK_EXPORT QDebug operator<<( QDebug, const QskSkinHintStatus& );

#endif

class QSK_EXPORT QskSkinnable
{
  public:
    QskSkinnable();
    virtual ~QskSkinnable();

    void setSkinlet( const QskSkinlet* );
    const QskSkinlet* skinlet() const;

    QFont effectiveFont( QskAspect ) const;
    qreal effectiveFontHeight( QskAspect ) const;
    QskColorFilter effectiveGraphicFilter( QskAspect::Subcontrol ) const;

    void setSubcontrolProxy( QskAspect::Subcontrol, QskAspect::Subcontrol proxy );
    void resetSubcontrolProxy( QskAspect::Subcontrol );
    QskAspect::Subcontrol subcontrolProxy( QskAspect::Subcontrol ) const;

    bool setAnimationHint( QskAspect, QskAnimationHint );
    QskAnimationHint animationHint( QskAspect, QskSkinHintStatus* = nullptr ) const;
    bool hasAnimationHint( QskAspect ) const;

    bool setSkinHint( QskAspect, const QVariant& );
    bool resetSkinHint( QskAspect );

    bool moveSkinHint( QskAspect, const QVariant& );
    bool moveSkinHint( QskAspect, const QVariant&, const QVariant& );

    QskAnimationHint effectiveAnimation( QskAspect::Type, QskAspect::Subcontrol,
        QskAspect::States, QskSkinHintStatus* status = nullptr ) const;

    QVariant effectiveSkinHint( QskAspect, QskSkinHintStatus* = nullptr ) const;
    virtual QskAspect::Variation effectiveVariation() const;

    virtual QskAspect::Section section() const;

    QskSkinHintStatus hintStatus( QskAspect ) const;

    QRectF subControlRect( const QRectF&, QskAspect::Subcontrol ) const;
    QRectF subControlContentsRect( const QRectF&, QskAspect::Subcontrol ) const;

    QSizeF outerBoxSize( QskAspect, const QSizeF& innerBoxSize ) const;
    QSizeF innerBoxSize( QskAspect, const QSizeF& outerBoxSize ) const;

    QRectF innerBox( QskAspect, const QRectF& outerBox ) const;
    QRectF outerBox( QskAspect, const QRectF& innerBox ) const;

    QMarginsF innerPadding( QskAspect, const QSizeF& ) const;

    const QskSkinlet* effectiveSkinlet() const;
    QskSkin* effectiveSkin() const;

    void startTransition( QskAspect,
        QskAnimationHint, const QVariant& from, const QVariant& to );

    void startTransition( QskAspect, int index,
        QskAnimationHint, const QVariant& from, const QVariant& to );

    QskAspect::Subcontrol effectiveSubcontrol( QskAspect::Subcontrol ) const;

    QskControl* controlCast();
    const QskControl* controlCast() const;

    virtual QQuickItem* owningItem() const = 0;
    virtual const QMetaObject* metaObject() const = 0;

    void debug( QskAspect ) const;
    void debug( QskAspect::State ) const;
    void debug( QDebug, QskAspect ) const;
    void debug( QDebug, QskAspect::State ) const;

    void setSkinStates( QskAspect::States );
    void setSkinStateFlag( QskAspect::State, bool on = true );

    void addSkinStates( QskAspect::States );
    void clearSkinStates( QskAspect::States );

    bool hasSkinState( QskAspect::State ) const;
    QskAspect::States skinStates() const;

    const char* skinStatesAsPrintable() const;
    const char* skinStatesAsPrintable( QskAspect::States ) const;

    // type aware methods for accessing skin hints

    bool setColor( QskAspect, Qt::GlobalColor );
    bool setColor( QskAspect, QRgb );
    bool setColor( QskAspect, const QColor& );

    bool moveColor( QskAspect, Qt::GlobalColor );
    bool moveColor( QskAspect, QRgb );
    bool moveColor( QskAspect, const QColor& );

    bool resetColor( QskAspect );
    QColor color( QskAspect, QskSkinHintStatus* = nullptr ) const;

    bool setMetric( QskAspect, qreal );
    bool moveMetric( QskAspect, qreal );
    bool resetMetric( QskAspect );
    qreal metric( QskAspect, QskSkinHintStatus* = nullptr ) const;
    qreal metric( QskAspect, qreal defaultValue ) const;

    bool setFlagHint( QskAspect, int flag );
    template< typename T > T flagHint( QskAspect, T = T() ) const;

    bool setPositionHint( QskAspect, qreal );
    bool movePositionHint( QskAspect, qreal );
    bool movePositionHint( QskAspect, qreal from, qreal to );
    bool resetPositionHint( QskAspect );
    qreal positionHint( QskAspect, QskSkinHintStatus* = nullptr ) const;

    bool setStrutSizeHint( QskAspect, const QSizeF& );
    bool setStrutSizeHint( QskAspect, qreal width, qreal height );
    bool resetStrutSizeHint( QskAspect );
    QSizeF strutSizeHint( QskAspect, QskSkinHintStatus* = nullptr ) const;

    bool setMarginHint( QskAspect, qreal );
    bool setMarginHint( QskAspect, const QMarginsF& );
    bool resetMarginHint( QskAspect );
    QMarginsF marginHint( QskAspect, QskSkinHintStatus* = nullptr ) const;

    bool setPaddingHint( QskAspect, qreal );
    bool setPaddingHint( QskAspect, const QMarginsF& );
    bool resetPaddingHint( QskAspect );
    QMarginsF paddingHint( QskAspect, QskSkinHintStatus* = nullptr ) const;

    bool setGradientHint( QskAspect, const QskGradient& );
    QskGradient gradientHint( QskAspect, QskSkinHintStatus* = nullptr ) const;

    bool setBoxShapeHint( QskAspect, const QskBoxShapeMetrics& );
    bool resetBoxShapeHint( QskAspect );
    QskBoxShapeMetrics boxShapeHint( QskAspect, QskSkinHintStatus* = nullptr ) const;

    bool setBoxBorderMetricsHint( QskAspect, const QskBoxBorderMetrics& );
    bool resetBoxBorderMetricsHint( QskAspect );
    QskBoxBorderMetrics boxBorderMetricsHint(
        QskAspect, QskSkinHintStatus* = nullptr ) const;

    bool setBoxBorderColorsHint( QskAspect, const QskBoxBorderColors& );
    bool resetBoxBorderColorsHint( QskAspect );
    QskBoxBorderColors boxBorderColorsHint( QskAspect, QskSkinHintStatus* = nullptr ) const;

    bool setShadowMetricsHint( QskAspect, const QskShadowMetrics& );
    bool resetShadowMetricsHint( QskAspect );
    QskShadowMetrics shadowMetricsHint( QskAspect, QskSkinHintStatus* = nullptr ) const;

    bool setShadowColorHint( QskAspect, const QColor& );
    bool resetShadowColorHint( QskAspect );
    QColor shadowColorHint( QskAspect, QskSkinHintStatus* = nullptr ) const;

    QskBoxHints boxHints( QskAspect ) const;
    QskArcHints arcHints( QskAspect ) const;

    bool setArcMetricsHint( QskAspect, const QskArcMetrics& );
    bool resetArcMetricsHint( QskAspect );
    QskArcMetrics arcMetricsHint( QskAspect, QskSkinHintStatus* = nullptr ) const;

    bool setStippleMetricsHint( QskAspect, const QskStippleMetrics& );
    bool resetStippleMetricsHint( QskAspect );
    QskStippleMetrics stippleMetricsHint( QskAspect, QskSkinHintStatus* = nullptr ) const;

    bool setSpacingHint( QskAspect, qreal );
    bool resetSpacingHint( QskAspect );
    qreal spacingHint( QskAspect, QskSkinHintStatus* = nullptr ) const;

    bool setAlignmentHint( QskAspect, Qt::Alignment );
    bool resetAlignmentHint( QskAspect );
    Qt::Alignment alignmentHint( QskAspect, Qt::Alignment = Qt::Alignment() ) const;

    bool setTextOptionsHint( QskAspect, const QskTextOptions& );
    bool resetTextOptionsHint( QskAspect );
    QskTextOptions textOptionsHint( QskAspect, QskSkinHintStatus* = nullptr ) const;

    bool setFontRoleHint( QskAspect, const QskFontRole& );
    bool resetFontRoleHint( QskAspect );
    QskFontRole fontRoleHint( QskAspect, QskSkinHintStatus* = nullptr ) const;

    bool setGraphicRoleHint( QskAspect, int role );
    bool resetGraphicRoleHint( QskAspect );
    int graphicRoleHint( QskAspect, QskSkinHintStatus* = nullptr ) const;

    bool setSymbolHint( const QskAspect, const QskGraphic& );
    bool resetSymbolHint( const QskAspect );
    QskGraphic symbolHint( const QskAspect, QskSkinHintStatus* = nullptr ) const;

    const QskSkinHintTable& hintTable() const;

    bool startHintTransitions( QskAspect::States, QskAspect::States, int index = -1 );
    bool startHintTransitions( const QVector< QskAspect::Subcontrol >&,
        QskAspect::States, QskAspect::States, int index = -1 );

    const QskHintAnimator* runningHintAnimator( QskAspect, int index = -1 ) const;

  protected:
    virtual void updateNode( QSGNode* );
    virtual bool isTransitionAccepted( QskAspect ) const;

    virtual QskAspect::Subcontrol substitutedSubcontrol( QskAspect::Subcontrol ) const;

    QskSkinHintTable& hintTable();

  private:
    Q_DISABLE_COPY( QskSkinnable )

    void startHintTransition( QskAspect, int index,
        QskAnimationHint, const QVariant& from, const QVariant& to );

    QVariant animatedHint( QskAspect, QskSkinHintStatus* ) const;
    QVariant interpolatedHint( QskAspect, QskSkinHintStatus* ) const;
    const QVariant& storedHint( QskAspect, QskSkinHintStatus* = nullptr ) const;

    friend class QskSkinStateChanger;
    void replaceSkinStates( QskAspect::States, int sampleIndex = -1 );

    class PrivateData;
    std::unique_ptr< PrivateData > m_data;
};

template< typename T >
inline T QskSkinnable::flagHint( QskAspect aspect, T defaultValue ) const
{
    const auto& hint = effectiveSkinHint( aspect );
    if ( hint.isValid() && hint.canConvert< int >() )
        return static_cast< T >( hint.value< int >() );

    return defaultValue;
}

inline Qt::Alignment QskSkinnable::alignmentHint(
    QskAspect aspect, Qt::Alignment defaultAlignment ) const
{
    return flagHint< Qt::Alignment >( aspect | QskAspect::Alignment, defaultAlignment );
}

inline bool QskSkinnable::resetColor( QskAspect aspect )
{
    return resetSkinHint( aspect | QskAspect::Color );
}

inline bool QskSkinnable::resetMetric( QskAspect aspect )
{
    return resetSkinHint( aspect | QskAspect::Metric );
}

#endif
