/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#ifndef QSK_SKIN_H
#define QSK_SKIN_H

#include "QskAspect.h"

#include <qcolor.h>
#include <qobject.h>

#include <memory>
#include <type_traits>

class QskSkinnable;
class QskSkinlet;

class QskColorFilter;
class QskGraphic;
class QskGraphicProvider;
class QskFontRole;

class QskSkinHintTable;

class QVariant;
template< typename Key, typename T > class QHash;

class QSK_EXPORT QskSkin : public QObject
{
    Q_OBJECT

    using Inherited = QObject;

  public:
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

    void setSkinHint( QskAspect, const QVariant& hint );
    const QVariant& skinHint( QskAspect ) const;

    void setGraphicFilter( int graphicRole, const QskColorFilter& );
    void resetGraphicFilter( int graphicRole );
    QskColorFilter graphicFilter( int graphicRole ) const;

    void setFont( const QskFontRole&, const QFont& );
    void resetFont( const QskFontRole& );
    QFont font( const QskFontRole& ) const;

    void addGraphicProvider( const QString& providerId, QskGraphicProvider* );
    QskGraphicProvider* graphicProvider( const QString& providerId ) const;
    bool hasGraphicProvider() const;

    virtual const int* dialogButtonLayout( Qt::Orientation ) const;
    virtual QString dialogButtonText( int button ) const;

    QskSkinlet* skinlet( const QMetaObject* );
    const QMetaObject* skinletMetaObject( const QMetaObject* ) const;

    const QskSkinHintTable& hintTable() const;
    QskSkinHintTable& hintTable();

    const QHash< QskFontRole, QFont >& fontTable() const;
    const QHash< int, QskColorFilter >& graphicFilters() const;

    ColorScheme colorScheme() const;

  public Q_SLOTS:
    void setColorScheme( ColorScheme );

  Q_SIGNALS:
    void colorSchemeChanged( ColorScheme );

  protected:
    void clearHints();
    virtual void initHints() = 0;

    void setupFontTable( const QString& family, bool italic = false );
    void completeFontTable();

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
