/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#ifndef QSK_SKIN_H
#define QSK_SKIN_H

#include "QskAspect.h"

#include <qcolor.h>
#include <qobject.h>

#include <memory>
#include <type_traits>
#include <unordered_map>

class QskSkinnable;
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

#if 1
    // Use Qt::ColorScheme once minimum version is Qt 6.5
    enum ColorScheme
    {
        UnknownScheme,

        LightScheme,
        DarkScheme
    };
    Q_ENUM( ColorScheme )
#endif

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

    void addGraphicProvider( const QString& providerId, QskGraphicProvider* );
    QskGraphicProvider* graphicProvider( const QString& providerId ) const;
    bool hasGraphicProvider() const;

    virtual const int* dialogButtonLayout( Qt::Orientation ) const;
    virtual QString dialogButtonText( int button ) const;

    QskSkinlet* skinlet( const QMetaObject* );
    const QMetaObject* skinletMetaObject( const QMetaObject* ) const;

    const QskSkinHintTable& hintTable() const;
    QskSkinHintTable& hintTable();

    const std::unordered_map< int, QFont >& fonts() const;
    const std::unordered_map< int, QskColorFilter >& graphicFilters() const;

  private:
    void declareSkinlet( const QMetaObject* metaObject,
        const QMetaObject* skinletMetaObject );

    class PrivateData;
    std::unique_ptr< PrivateData > m_data;
};

template< typename Skinnable, typename Skinlet >
inline void QskSkin::declareSkinlet()
{
    Q_STATIC_ASSERT( ( std::is_base_of< QskSkinnable, Skinnable >::value ) );
    Q_STATIC_ASSERT( ( std::is_base_of< QskSkinlet, Skinlet >::value ) );
    declareSkinlet( &Skinnable::staticMetaObject, &Skinlet::staticMetaObject );
}

#endif
