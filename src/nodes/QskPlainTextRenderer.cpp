/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskPlainTextRenderer.h"
#include "QskTextColors.h"
#include "QskTextOptions.h"

#include <qfontmetrics.h>
#include <qmath.h>
#include <qsgnode.h>

QSK_QT_PRIVATE_BEGIN
#include <private/qsgadaptationlayer_p.h>
#include <private/qsgcontext_p.h>
#include <private/qquickitem_p.h>
QSK_QT_PRIVATE_END

#define GlyphFlag static_cast< QSGNode::Flag >( 0x800 )

QSizeF QskPlainTextRenderer::textSize(
    const QString& text, const QFont& font, const QskTextOptions& options )
{
    // result differs from QQuickText::implicitSizeHint ???
    return textRect( text, font, options, QSizeF( 10e6, 10e6 ) ).size();
}

QRectF QskPlainTextRenderer::textRect(
    const QString& text, const QFont& font, const QskTextOptions& options,
    const QSizeF& size )
{
    const QFontMetricsF fm( font );
    const QRectF r( 0, 0, size.width(), size.height() );

    return fm.boundingRect( r, options.textFlags(), text );
}

static qreal qskLayoutText( QTextLayout* layout,
    qreal lineWidth, const QskTextOptions& options )
{
    if ( layout->text().isEmpty() )
        return 0.0;

    qreal y = 0;

    const auto elideMode = options.effectiveElideMode();

    if ( elideMode == Qt::ElideNone )
    {
        for ( int i = 0; i < options.maximumLineCount(); i++ )
        {
            auto line = layout->createLine();
            if ( !line.isValid() )
                break;

            line.setPosition( QPointF( 0, y ) );
            line.setLineWidth( lineWidth );

            y += line.leading() + line.height();
        }
    }
    else
    {
        const auto engine = layout->engine();

        auto text = engine->elidedText(
            elideMode, QFixed::fromReal( lineWidth ),
            Qt::TextShowMnemonic, 0 );

        // why do we need this padding ???
        text = text.leftJustified( engine->text.length() );
        engine->text = text;

        auto line = layout->createLine();

        if ( line.isValid() )
        {
            /*
                For some reason the position of the text is wrong,
                with QTextOption::NoWrap - even if word wrapping
                for elided text does not make any sense.
                Needs some debugging of QTextLine::layout_helper, TODO ...
             */
            auto option = layout->textOption();
            option.setWrapMode( QTextOption::WrapAnywhere );
            layout->setTextOption( option );

            line.setPosition( QPointF( 0, y ) );
            line.setLineWidth( lineWidth );

            y += line.leading() + line.height();
        }
    }

    return y;
}

static void qskRenderText(
    QQuickItem* item, QSGNode* parentNode, const QTextLayout& layout, qreal baseLine,
    const QColor& color, QQuickText::TextStyle style, const QColor& styleColor )
{
    auto renderContext = QQuickItemPrivate::get(item)->sceneGraphRenderContext();
    auto sgContext = renderContext->sceneGraphContext();

    // Clear out foreign nodes (e.g. from QskRichTextRenderer)
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

    const QPointF position( 0, baseLine );

    for ( int i = 0; i < layout.lineCount(); ++i )
    {
        const auto glyphRuns = layout.lineAt( i ).glyphRuns();

        for ( const auto& glyphRun : glyphRuns )
        {
            if ( glyphNode == nullptr )
            {
                const bool preferNativeGlyphNode = false; // QskTextOptions?

#if QT_VERSION >= QT_VERSION_CHECK( 6, 0, 0 )
                constexpr int renderQuality = -1; // QQuickText::DefaultRenderTypeQuality
                glyphNode = sgContext->createGlyphNode(
                    renderContext, preferNativeGlyphNode, renderQuality );
#else
                glyphNode = sgContext->createGlyphNode(
                    renderContext, preferNativeGlyphNode );
#endif
                glyphNode->setOwnerElement( item );
                glyphNode->setFlags( QSGNode::OwnedByParent | GlyphFlag );
            }

            glyphNode->setStyle( style );
            glyphNode->setColor( color );
            glyphNode->setStyleColor( styleColor );
            glyphNode->setGlyphs( position, glyphRun );
            glyphNode->update();

            if ( glyphNode->parent() != parentNode )
                parentNode->appendChildNode( glyphNode );

            glyphNode = static_cast< QSGGlyphNode* >( glyphNode->nextSibling() );
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

void QskPlainTextRenderer::updateNode( const QString& text,
    const QFont& font, const QskTextOptions& options,
    Qsk::TextStyle style, const QskTextColors& colors,
    Qt::Alignment alignment, const QRectF& rect,
    const QQuickItem* item, QSGTransformNode* node )
{
    QTextOption textOption( alignment );
    textOption.setWrapMode( static_cast< QTextOption::WrapMode >( options.wrapMode() ) );

    QString tmp = text;

#if 0
    const int pos = tmp.indexOf( QLatin1Char( '\x9c' ) );
    if ( pos != -1 )
    {
        // ST: string termination

        tmp = tmp.mid( 0, pos );
        tmp.replace( QLatin1Char( '\n' ), QChar::LineSeparator );
    }
    else
#endif
    if ( tmp.contains( QLatin1Char( '\n' ) ) )
    {
        tmp.replace( QLatin1Char('\n'), QChar::LineSeparator );
    }


    QTextLayout layout;
    layout.setFont( font );
    layout.setTextOption( textOption );
    layout.setText( tmp );

    layout.beginLayout();
    const qreal textHeight = qskLayoutText( &layout, rect.width(), options );
    layout.endLayout();

    const qreal y0 = QFontMetricsF( font ).ascent();

    qreal yBaseline = y0;

    if ( alignment & Qt::AlignVCenter )
    {
        yBaseline += ( rect.height() - textHeight ) * 0.5;
    }
    else if ( alignment & Qt::AlignBottom )
    {
        yBaseline += rect.height() - textHeight;
    }

    if ( yBaseline != y0 )
    {
        /*
            We need to have a stable algo for rounding the text base line,
            so that texts don't start wobbling, when processing transitions
            between margins/paddings.
         */

        const int bh = int( layout.boundingRect().height() );
        yBaseline = ( bh % 2 ) ? qFloor( yBaseline ) : qCeil( yBaseline );
    }

    qskRenderText(
        const_cast< QQuickItem* >( item ), node, layout, yBaseline,
        colors.textColor, static_cast< QQuickText::TextStyle >( style ),
        colors.styleColor );
}

void QskPlainTextRenderer::updateNodeColor(
    QSGNode* parentNode, const QColor& textColor,
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
