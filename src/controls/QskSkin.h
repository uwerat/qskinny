/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_SKIN_H
#define QSK_SKIN_H

#include "QskAspect.h"

#include <qcolor.h>
#include <qobject.h>

#include <memory>
#include <type_traits>
#include <unordered_map>

class QskControl;
class QskSkinlet;

class QskColorFilter;
class QskGraphic;
class QskGraphicProvider;

class QskAnimationHint;
class QskSkinHintTable;

class QVariant;
class QskMargins;
class QskGradient;
class QskBoxShapeMetrics;
class QskBoxBorderMetrics;
class QskBoxBorderColors;

class QSK_EXPORT QskSkin : public QObject
{
    Q_OBJECT

    using Inherited = QObject;

  public:
    enum SkinFontRole
    {
        DefaultFont = 0,

        TinyFont,
        SmallFont,
        MediumFont,
        LargeFont,
        HugeFont
    };

    Q_ENUM( SkinFontRole )

    QskSkin( QObject* parent = nullptr );
    ~QskSkin() override;

    template< typename Control, typename Skinlet >
    void declareSkinlet();

    virtual void resetColors( const QColor& accent );

    void setColor( QskAspect, Qt::GlobalColor );
    void setColor( QskAspect, QRgb );
    void setColor( QskAspect, const QColor& );

    QColor color( QskAspect ) const;

    void setMetric( QskAspect, qreal metric );
    qreal metric( QskAspect ) const;

    void setStrutSize( QskAspect, const QSizeF& );
    void setStrutSize( QskAspect, qreal width, qreal height );
    QSizeF strutSize( QskAspect ) const;

    void setMargin( QskAspect, const QskMargins& );
    void setMargin( QskAspect,
        qreal left, qreal top, qreal right, qreal bottom );
    QskMargins margin( QskAspect ) const;

    void setPadding( QskAspect, const QskMargins& );
    void setPadding( QskAspect,
        qreal left, qreal top, qreal right, qreal bottom );
    QskMargins padding( QskAspect ) const;

    void setSpacing( QskAspect, qreal );
    qreal spacing( QskAspect ) const;

    void setGradient( QskAspect, const QskGradient& );
    QskGradient gradient( QskAspect ) const;

    void setBoxShape( QskAspect, qreal radius, Qt::SizeMode = Qt::AbsoluteSize );
    void setBoxShape( QskAspect, qreal topLeft, qreal topRight,
        qreal bottomLeft, qreal bottomRight, Qt::SizeMode = Qt::AbsoluteSize );

    void setBoxShape( QskAspect, const QskBoxShapeMetrics& );
    QskBoxShapeMetrics boxShape( QskAspect ) const;

    void setBoxBorderMetrics( QskAspect,
        qreal borderWidth, Qt::SizeMode = Qt::AbsoluteSize );

    void setBoxBorderMetrics( QskAspect, qreal left, qreal top,
        qreal right, qreal bottom, Qt::SizeMode = Qt::AbsoluteSize );

    void setBoxBorderMetrics( QskAspect, const QskBoxBorderMetrics& );
    QskBoxBorderMetrics boxBorderMetrics( QskAspect ) const;

    void setBoxBorderColors( QskAspect, const QskBoxBorderColors& );
    QskBoxBorderColors boxBorderColors( QskAspect ) const;

    void setAnimation( QskAspect, QskAnimationHint );
    QskAnimationHint animation( QskAspect ) const;

    void setAlignment( QskAspect, Qt::Alignment );
    Qt::Alignment alignment( QskAspect ) const;

    void setSkinHint( QskAspect, const QVariant& hint );
    const QVariant& skinHint( QskAspect ) const;

    void setGraphicFilter( int graphicRole, const QskColorFilter& );
    void resetGraphicFilter( int graphicRole );
    QskColorFilter graphicFilter( int graphicRole ) const;

    void setFontRole( QskAspect, int fontRole );
    void setGraphicRole( QskAspect, int graphicRole );

    void setFont( int fontRole, const QFont& );
    void resetFont( int fontRole );
    QFont font( int fontRole ) const;

    void setupFonts( const QString& family,
        int weight = -1, bool italic = false );

    virtual QskGraphic symbol( int symbolType ) const;

    void addGraphicProvider( const QString& providerId, QskGraphicProvider* );
    QskGraphicProvider* graphicProvider( const QString& providerId ) const;
    bool hasGraphicProvider() const;

    virtual const int* dialogButtonLayout( Qt::Orientation ) const;

    void setStateMask( QskAspect::State );
    QskAspect::State stateMask() const;

    QskSkinlet* skinlet( const QMetaObject* );

    const QskSkinHintTable& hintTable() const;

    const std::unordered_map< int, QFont >& fonts() const;
    const std::unordered_map< int, QskColorFilter >& graphicFilters() const;

  protected:
    QskSkinHintTable& skinHintTable();

  private:
    void declareSkinlet( const QMetaObject* controlMetaObject,
        const QMetaObject* skinMetaObject );

    class PrivateData;
    std::unique_ptr< PrivateData > m_data;
};

template< typename Control, typename Skinlet >
inline void QskSkin::declareSkinlet()
{
    Q_STATIC_ASSERT( ( std::is_base_of< QskControl, Control >::value ) );
    Q_STATIC_ASSERT( ( std::is_base_of< QskSkinlet, Skinlet >::value ) );
    declareSkinlet( &Control::staticMetaObject, &Skinlet::staticMetaObject );
}

#endif
