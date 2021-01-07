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

class QskSkinHintTable;

class QVariant;

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

    void setSkinHint( QskAspect, const QVariant& hint );
    const QVariant& skinHint( QskAspect ) const;

    void setGraphicFilter( int graphicRole, const QskColorFilter& );
    void resetGraphicFilter( int graphicRole );
    QskColorFilter graphicFilter( int graphicRole ) const;

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
    QskSkinHintTable& hintTable();

    const std::unordered_map< int, QFont >& fonts() const;
    const std::unordered_map< int, QskColorFilter >& graphicFilters() const;

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
