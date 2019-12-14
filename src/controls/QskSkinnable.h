/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
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

class QskControl;
class QskAnimationHint;
class QskColorFilter;
class QskBoxShapeMetrics;
class QskBoxBorderMetrics;
class QskBoxBorderColors;
class QskGradient;

class QskSkin;
class QskSkinlet;
class QskSkinHintTable;

class QSK_EXPORT QskSkinHintStatus
{
  public:
    enum Source
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
    QskAspect::Aspect aspect;
};

class QSK_EXPORT QskSkinnable
{
  public:
    QskSkinnable();
    virtual ~QskSkinnable();

    void setSkinlet( const QskSkinlet* skinlet );
    const QskSkinlet* skinlet() const;

    void setColor( QskAspect::Aspect, Qt::GlobalColor );
    void setColor( QskAspect::Aspect, QRgb );
    void setColor( QskAspect::Aspect, const QColor& );

    QColor color( QskAspect::Aspect, QskSkinHintStatus* = nullptr ) const;

    void setMetric( QskAspect::Aspect, qreal metric );
    qreal metric( QskAspect::Aspect, QskSkinHintStatus* = nullptr ) const;

    void setMarginsHint( QskAspect::Aspect, qreal );
    void setMarginsHint( QskAspect::Aspect, const QMarginsF& );
    QMarginsF marginsHint( QskAspect::Aspect, QskSkinHintStatus* = nullptr ) const;

    void setGradientHint( QskAspect::Aspect, const QskGradient& );
    QskGradient gradientHint( QskAspect::Aspect, QskSkinHintStatus* = nullptr ) const;

    void setBoxShapeHint( QskAspect::Aspect, const QskBoxShapeMetrics& );
    QskBoxShapeMetrics boxShapeHint( QskAspect::Aspect, QskSkinHintStatus* = nullptr ) const;

    void setBoxBorderMetricsHint( QskAspect::Aspect, const QskBoxBorderMetrics& );
    QskBoxBorderMetrics boxBorderMetricsHint(
        QskAspect::Aspect, QskSkinHintStatus* = nullptr ) const;

    void setBoxBorderColorsHint( QskAspect::Aspect, const QskBoxBorderColors& );
    QskBoxBorderColors boxBorderColorsHint( QskAspect::Aspect, QskSkinHintStatus* = nullptr ) const;

    void setFlagHint( QskAspect::Aspect, int flag );
    int flagHint( QskAspect::Aspect ) const;

    template< typename T > T flagHint( QskAspect::Aspect, T = T() ) const;

    void setFontRole( QskAspect::Aspect, int role );
    int fontRole( QskAspect::Aspect ) const;

    QFont effectiveFont( QskAspect::Aspect ) const;

    void setGraphicRole( QskAspect::Aspect, int role );
    int graphicRole( QskAspect::Aspect ) const;

    QskColorFilter effectiveGraphicFilter( QskAspect::Aspect ) const;

    void setAnimation( QskAspect::Aspect, QskAnimationHint );
    QskAnimationHint animation( QskAspect::Aspect, QskSkinHintStatus* = nullptr ) const;

    bool resetHint( QskAspect::Aspect );

    QskAnimationHint effectiveAnimation( QskAspect::Type, QskAspect::Subcontrol,
        QskAspect::State, QskSkinHintStatus* status = nullptr ) const;

    QVariant effectiveHint( QskAspect::Aspect, QskSkinHintStatus* = nullptr ) const;
    virtual QskAspect::Placement effectivePlacement() const;

    QskSkinHintStatus hintStatus( QskAspect::Aspect ) const;

    QskAspect::State skinState() const;
    const char* skinStateAsPrintable() const;
    const char* skinStateAsPrintable( QskAspect::State ) const;

    QSizeF outerBoxSize( QskAspect::Aspect, const QSizeF& innerBoxSize ) const;
    QSizeF innerBoxSize( QskAspect::Aspect, const QSizeF& outerBoxSize ) const;

    QRectF innerBox( QskAspect::Aspect, const QRectF& outerBox ) const;
    QRectF outerBox( QskAspect::Aspect, const QRectF& innerBox ) const;

    QMarginsF innerPadding( QskAspect::Aspect, const QSizeF& ) const;

    virtual const QskSkinlet* effectiveSkinlet() const;
    QskSkin* effectiveSkin() const;

    void startTransition( QskAspect::Aspect,
        QskAnimationHint, QVariant from, QVariant to );

    virtual QskAspect::Subcontrol effectiveSubcontrol( QskAspect::Subcontrol ) const;

    QskControl* controlCast();
    const QskControl* controlCast() const;

    virtual QskControl* owningControl() const = 0;
    virtual const QMetaObject* metaObject() const = 0;

    void debug( QskAspect::Aspect ) const;
    void debug( QskAspect::State ) const;
    void debug( QDebug, QskAspect::Aspect ) const;
    void debug( QDebug, QskAspect::State ) const;

  protected:
    void setSkinState( QskAspect::State );
    void setSkinStateFlag( QskAspect::State, bool = true );

    virtual void updateNode( QSGNode* );
    virtual bool isTransitionAccepted( QskAspect::Aspect ) const;

    QskSkinHintTable& hintTable();
    const QskSkinHintTable& hintTable() const;

  private:
    QVariant animatedValue( QskAspect::Aspect, QskSkinHintStatus* ) const;
    const QVariant& storedHint( QskAspect::Aspect, QskSkinHintStatus* = nullptr ) const;

    class PrivateData;
    std::unique_ptr< PrivateData > m_data;
};

template< typename T >
inline T QskSkinnable::flagHint( QskAspect::Aspect aspect, T defaultValue ) const
{
    const auto& hint = effectiveHint( aspect );
    if ( hint.isValid() && hint.canConvert< int >() )
        return static_cast< T >( hint.value< int >() );

    return defaultValue;
}

inline QskAspect::Placement QskSkinnable::effectivePlacement() const
{
    return QskAspect::NoPlacement;
}

#endif
