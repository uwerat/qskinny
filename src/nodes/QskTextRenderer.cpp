/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskTextRenderer.h"
#include <QQuickWindow>

QSK_QT_PRIVATE_BEGIN
#include <private/qquicktext_p.h>
#include <private/qquicktext_p_p.h>
QSK_QT_PRIVATE_END

#include <limits>

class QskTextHelperItem final : public QQuickText
{
public:
    QskTextHelperItem();

    void refWindow( QQuickWindow* window );
    void derefWindow();

    void begin();
    void end();

    QRectF layedOutTextRect() const;
    void updateTextNode( QQuickWindow*, QSGNode* );

protected:
    virtual QSGNode* updatePaintNode(
        QSGNode*, UpdatePaintNodeData* ) override final;
};

QskTextHelperItem::QskTextHelperItem():
    QQuickText( nullptr )
{
}

QRectF QskTextHelperItem::layedOutTextRect() const
{
    auto that = const_cast< QskTextHelperItem* >( this );
    return QQuickTextPrivate::get( that )->layedOutTextRect;
}

void QskTextHelperItem::updateTextNode( QQuickWindow* window, QSGNode* parentNode )
{
    QQuickItemPrivate::get( this )->refWindow( window );

    while ( parentNode->firstChild() )
        delete parentNode->firstChild(); // This is done in QQuickText::updatePaintNode anyway

    auto node = QQuickText::updatePaintNode( nullptr, nullptr );
    node->reparentChildNodesTo( parentNode );
    delete node;

    QQuickItemPrivate::get( this )->derefWindow();
}

void QskTextHelperItem::begin()
{
    classBegin();
}

void QskTextHelperItem::end()
{
    componentComplete();
}

QSGNode* QskTextHelperItem::updatePaintNode( QSGNode*, UpdatePaintNodeData* )
{
    // should never be called
    return nullptr;
}

/*
    size requests and rendering might be from different threads and we
    better use different items as we might end up in events internally
    being sent, that leads to crashes because of it
 */
static QskTextHelperItem* qskRenderHelper = nullptr;
static QskTextHelperItem* qskLayoutHelper = nullptr;

void QskTextRenderer::setFont( const QFont& font )
{
    m_font = font;
}

void QskTextRenderer::setOptions( const QskTextOptions& options )
{
    m_options = options;
}

void QskTextRenderer::setAlignment( Qt::Alignment alignment )
{
    m_alignment = alignment;
}

QSizeF QskTextRenderer::implicitSizeHint( const QString& text ) const
{
    if ( qskLayoutHelper == NULL )
        qskLayoutHelper = new QskTextHelperItem();

    QskTextHelperItem& textItem = *qskLayoutHelper;

    textItem.begin();

    setupItem( &textItem );

    textItem.setWidth( -1 );
    textItem.setText( text );

    textItem.end();

    return QSizeF( textItem.implicitWidth(), textItem.implicitHeight() );
}

QRectF QskTextRenderer::textRect( const QSizeF& size, const QString& text ) const
{
    if ( qskLayoutHelper == NULL )
        qskLayoutHelper = new QskTextHelperItem();

    QskTextHelperItem& textItem = *qskLayoutHelper;

    textItem.begin();

    setupItem( &textItem );

    textItem.setWidth( size.width() );
    textItem.setHeight( size.height() );

    textItem.setText( text );

    textItem.end();

    return textItem.layedOutTextRect();
}

void QskTextRenderer::setupItem( QskTextHelperItem* textItem ) const
{
#if 0
    textItem->setAntialiasing( true );
    textItem->setRenderType( QQuickText::QtRendering );
    textItem->setPadding( 0 );
#endif

    textItem->setHAlign( ( QQuickText::HAlignment ) ( int( m_alignment ) & 0x0f ) );
    textItem->setVAlign( ( QQuickText::VAlignment ) ( int( m_alignment ) & 0xf0 ) );

    // fonts are supposed to be defined in the application skin and we
    // probably don't want to have them scaled
    textItem->setFont( m_font );
    textItem->setFontSizeMode( QQuickText::FixedSize );

#if 0
    textItem->setMinimumPixelSize();
    textItem->setMinimumPointSize();
#endif

    textItem->setTextFormat( ( QQuickText::TextFormat ) m_options.format() );
    textItem->setElideMode( ( QQuickText::TextElideMode ) m_options.elideMode() );
    textItem->setMaximumLineCount( m_options.maximumLineCount() );
    textItem->setWrapMode( static_cast< QQuickText::WrapMode >( m_options.wrapMode() ) );

    // what about Qt::TextShowMnemonic ???

#if 0
    // also something, that should be defined in an application skin
    textItem->setLineHeightMode( ... );
    textItem->setLineHeight();
#endif
}

void QskTextRenderer::updateNode( const QQuickItem* item,
    const QRectF& rect, const QString& text, QSGTransformNode* parentNode,
    const QColor& textColor, Qsk::TextStyle style, const QColor& styleColor,
    const QColor& linkColor )
{
    // are we killing internal caches of QQuickText, when always using
    // the same item for the creation the text nodes ???

    if ( qskRenderHelper == NULL )
        qskRenderHelper = new QskTextHelperItem();

    QskTextHelperItem& textItem = *qskRenderHelper;

    textItem.begin();

    setupItem( &textItem );

#if 0
    // the position of textItem seems to have no effect
    // on the position of the node. We do it by translation later.

    textItem.setX( rect.x() );
    textItem.setY( rect.y() );
#endif

    if ( rect.width() != textItem.width() ||
        rect.height() != textItem.height() )
    {
        textItem.setWidth( rect.width() );
        textItem.setHeight( rect.height() );
        textItem.doLayout();
    }

    textItem.setColor( textColor );
    textItem.setStyle( static_cast< QQuickText::TextStyle >( style ) );
    textItem.setStyleColor( styleColor );
    textItem.setLinkColor( linkColor );

    textItem.setText( text );

    textItem.end();

    textItem.updateTextNode( item->window(), parentNode );

    textItem.setText( QString::null );
}
