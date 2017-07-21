/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskPlainTextRenderer.h"
#include "QskSkinlet.h"

#include <QFontMetrics>
#include <QGuiApplication>
#include <QSGTransformNode>
#include <QQuickWindow>

QSK_QT_PRIVATE_BEGIN
#include <private/qsgadaptationlayer_p.h>

#if QT_VERSION < QT_VERSION_CHECK( 5, 8, 0 )
#include <private/qsgcontext_p.h>
typedef QSGRenderContext RenderContext;
#else
#include <private/qsgdefaultrendercontext_p.h>
typedef QSGDefaultRenderContext RenderContext;
#endif

QSK_QT_PRIVATE_END

#define GlyphFlag static_cast< QSGNode::Flag >( 0x800 )

QskPlainTextRenderer::QskPlainTextRenderer():
    m_fontMetrics( m_font )
{
}

QskPlainTextRenderer::~QskPlainTextRenderer()
{
}

void QskPlainTextRenderer::setFont( const QFont& font )
{
    m_font = font;
    m_fontMetrics = QFontMetricsF( m_font );
}

void QskPlainTextRenderer::setOptions( const QskTextOptions& options )
{
    m_options = options;
}

void QskPlainTextRenderer::setAlignment( Qt::Alignment alignment )
{
    m_alignment = alignment;
}

QSizeF QskPlainTextRenderer::implicitSizeHint( const QString& text ) const
{
    // result differs from QskTextRenderer::implicitSizeHint ???
    return textRect( QSizeF( 10e6, 10e6 ), text ).size();
}

QRectF QskPlainTextRenderer::textRect( const QSizeF& size, const QString& text ) const
{
    return m_fontMetrics.boundingRect( { QPointF(), size }, flags(), text );
}

int QskPlainTextRenderer::flags() const
{
    return m_options.textFlags() | m_alignment;
}

static qreal qskLayoutText( QTextLayout* layout, const QPointF& position, qreal lineWidth,
    const QskTextOptions& options )
{
    const auto maxLineCount = options.wrapMode() == QskTextOptions::NoWrap
        ? 1 : options.maximumLineCount();
    int lineNumber = 0;
    int characterPosition = 0;
    qreal y = 0;
    Q_FOREVER
    {
        if ( ++lineNumber > maxLineCount )
            break;

        if ( lineNumber == maxLineCount )
        {
            const auto elideMode = options.elideMode();
            auto engine = layout->engine();
            const auto textLength = engine->text.length();
            if ( elideMode != Qt::ElideNone && textLength > characterPosition )
            {
                if ( lineNumber == 1 || elideMode == Qt::ElideRight )
                {
                    auto option = layout->textOption();
                    option.setWrapMode( QTextOption::WrapAnywhere );
                    layout->setTextOption( option );

                    auto elidedText = engine->elidedText(
                        elideMode, QFixed::fromReal( lineWidth ),
                        Qt::TextShowMnemonic, characterPosition )
                        .leftJustified( textLength - characterPosition );

                    engine->text.replace( characterPosition, elidedText.length(), elidedText );
                    Q_ASSERT( engine->text.length() == textLength );
                }
            }
        }

        auto line = layout->createLine();
        if ( !line.isValid() )
            break;

        line.setPosition( position + QPointF( 0, y ) );
        line.setLineWidth( lineWidth );
        characterPosition = line.textStart() + line.textLength();
        y += line.leading() + line.height();
    }
    return y;
}

static void qskRenderText(
    QQuickItem* item, QSGNode* parentNode, QTextLayout* layout, const QPointF& position,
    const QColor& color, QQuickText::TextStyle style, const QColor& styleColor )
{
    auto rc = RenderContext::from( QOpenGLContext::currentContext() );
    auto sgContext = rc->sceneGraphContext();

    // Clear out foreign nodes (e.g. from QskTextRenderer)
    QSGNode* node = parentNode->firstChild();
    while ( node )
    {
        auto sibling = node->nextSibling();
        if ( !( node->flags() & GlyphFlag ) )
        {
            parentNode->removeChildNode( node );
            delete node;
        }
        node = sibling;
    }

    auto glyphNode = static_cast< QSGGlyphNode* >( parentNode->firstChild() );
    for ( int i = 0; i < layout->lineCount(); ++i )
    {
        auto line = layout->lineAt( i );
        for ( auto glyphRun : line.glyphRuns() )
        {
            const bool doCreate = !glyphNode;
            if ( doCreate )
            {
                glyphNode = sgContext->createGlyphNode( rc, false ); // ### add native rendering to QskTextOptions?
                glyphNode->setOwnerElement( item );
                glyphNode->setFlags( QSGNode::OwnedByParent | GlyphFlag );
            }
            glyphNode->setStyle( style );
            glyphNode->setColor( color );
            glyphNode->setStyleColor( styleColor );
            glyphNode->setGlyphs( position, glyphRun );
            glyphNode->update();
            if ( doCreate )
            {
                parentNode->appendChildNode( glyphNode );
            }
            glyphNode = static_cast< decltype( glyphNode ) >( glyphNode->nextSibling() );
        }
    }

    // Remove leftover glyphs
    while ( glyphNode )
    {
        auto sibling = glyphNode->nextSibling();
        if ( glyphNode->flags() & GlyphFlag )
        {
            parentNode->removeChildNode( glyphNode );
            delete glyphNode;
        }
        glyphNode = static_cast< QSGGlyphNode* >( sibling );
    }
}

void QskPlainTextRenderer::updateNode(
    const QQuickItem* item, const QRectF& rect, const QString& text, QSGNode* parentNode,
    const QColor& textColor, Qsk::TextStyle style, const QColor& styleColor )
{
    QTextOption textOption( m_alignment );
    textOption.setWrapMode( static_cast< QTextOption::WrapMode >( m_options.wrapMode() ) );

    QTextLayout layout;
    layout.setFont( m_font );
    layout.setTextOption( textOption );
    layout.setText( text );

    layout.beginLayout();
    QPointF position;
    position.ry() += qskLayoutText( &layout, position, rect.width(), m_options );
    layout.endLayout();

    position.setX( 0 );
    position.setY( m_fontMetrics.ascent()
        + ( m_alignment & Qt::AlignVCenter
            ? ( rect.height() - position.y() ) * 0.5 : 0 ) );

    qskRenderText( const_cast< QQuickItem* >( item ), parentNode, &layout, position,
        textColor, static_cast< QQuickText::TextStyle >( style ), styleColor );
}

void QskPlainTextRenderer::updateNodeColor( QSGNode* parentNode, const QColor& textColor,
    Qsk::TextStyle style, const QColor& styleColor )
{
    auto glyphNode = static_cast< QSGGlyphNode* >( parentNode->firstChild() );
    while ( glyphNode )
    {
        if ( glyphNode->flags() & GlyphFlag )
        {
            glyphNode->setColor( textColor );
            glyphNode->setStyle( static_cast< QQuickText::TextStyle >( style ) );
            glyphNode->setStyleColor( styleColor );
            glyphNode->update();
        }
        glyphNode = static_cast< QSGGlyphNode* >( glyphNode->nextSibling() );
    }
}
