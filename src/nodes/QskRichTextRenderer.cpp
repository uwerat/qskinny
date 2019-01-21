/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskRichTextRenderer.h"
#include "QskTextColors.h"
#include "QskTextOptions.h"

#include <qglobalstatic.h>
#include <qmutex.h>
#include <qthread.h>

class QQuickWindow;

QSK_QT_PRIVATE_BEGIN
#include <private/qquicktext_p.h>
#include <private/qquicktext_p_p.h>
QSK_QT_PRIVATE_END

// Since Qt 5.7 QQuickTextNode is public and could be used TODO ...

namespace
{
    class TextItem final : public QQuickText
    {
      public:
        TextItem()
        {
#if 1
            /*
               QQuickTextPrivate::ExtraData::ExtraData is not exported with MSVC, so we
               preallocate it by setting/unsetting the bottom padding
             */
            setBottomPadding( 1 );
            setBottomPadding( 0 );
#endif

            // fonts are supposed to be defined in the application skin and we
            // probably don't want to have them scaled
            setFontSizeMode( QQuickText::FixedSize );

#if 0
            setAntialiasing( true );
            setRenderType( QQuickText::QtRendering );
            setPadding( 0 );

            setMinimumPixelSize();
            setMinimumPointSize();

            // also something, that should be defined in an application skin
            setLineHeightMode( ... );
            setLineHeight();
#endif
        }

        inline void setGeometry( const QRectF& rect )
        {
            auto d = QQuickTextPrivate::get( this );

            d->heightValid = true;
            d->widthValid = true;

            if ( ( d->x != rect.x() ) || ( d->y != rect.y() ) )
            {
                d->x = rect.x();
                d->y = rect.y();
                d->dirty( QQuickItemPrivate::Position );
            }

            if ( ( d->width != rect.width() ) || ( d->height != rect.height() ) )
            {
                d->height = rect.height();
                d->width = rect.width();
                d->dirty( QQuickItemPrivate::Size );
            }
        }

        inline void setAlignment( Qt::Alignment alignment )
        {
            setHAlign( ( QQuickText::HAlignment )( int( alignment ) & 0x0f ) );
            setVAlign( ( QQuickText::VAlignment )( int( alignment ) & 0xf0 ) );
        }

        inline void setOptions( const QskTextOptions& options )
        {
            // what about Qt::TextShowMnemonic ???
            setTextFormat( ( QQuickText::TextFormat ) options.format() );
            setElideMode( ( QQuickText::TextElideMode ) options.elideMode() );
            setMaximumLineCount( options.maximumLineCount() );
            setWrapMode( static_cast< QQuickText::WrapMode >( options.wrapMode() ) );
        }

        inline void begin()
        {
            classBegin();
            QQuickTextPrivate::get( this )->updateOnComponentComplete = true;
        }

        inline void end()
        {
            componentComplete();
        }

        inline void reset()
        {
            setText( QString() );
        }

        inline QRectF layedOutTextRect() const
        {
            auto that = const_cast< TextItem* >( this );
            return QQuickTextPrivate::get( that )->layedOutTextRect;
        }

        void updateTextNode( QQuickWindow* window, QSGNode* parentNode )
        {
            QQuickItemPrivate::get( this )->refWindow( window );

            while ( parentNode->firstChild() )
                delete parentNode->firstChild();

            auto node = QQuickText::updatePaintNode( nullptr, nullptr );
            node->reparentChildNodesTo( parentNode );
            delete node;

            QQuickItemPrivate::get( this )->derefWindow();
        }

      protected:
        QSGNode* updatePaintNode( QSGNode*, UpdatePaintNodeData* ) override
        {
            Q_ASSERT( false );
            return nullptr;
        }
    };

    class TextItemMap
    {
      public:
        ~TextItemMap()
        {
            qDeleteAll( m_hash );
        }

        inline TextItem* item()
        {
            const auto thread = QThread::currentThread();

            QMutexLocker locker( &m_mutex );

            auto it = m_hash.constFind( thread );
            if ( it == m_hash.constEnd() )
            {
                auto textItem = new TextItem();
                QObject::connect( thread, &QThread::finished,
                    textItem, [ this, thread ] { removeItem( thread ); } );

                m_hash.insert( thread, textItem );
                return textItem;
            }

            return it.value();
        }

      private:
        void removeItem( const QThread* thread )
        {
            auto textItem = m_hash.take( thread );
            if ( textItem )
                textItem->deleteLater();
        }

        QMutex m_mutex;
        QHash< const QThread*, TextItem* > m_hash;
    };
}

/*
    size requests and rendering might be from different threads and we
    better use different items as we might end up in events internally
    being sent, that leads to crashes because of it
 */
Q_GLOBAL_STATIC( TextItemMap, qskTextItemMap )

QSizeF QskRichTextRenderer::textSize(
    const QString& text, const QFont& font, const QskTextOptions& options )
{
    auto& textItem = *qskTextItemMap->item();

    textItem.begin();

    textItem.setFont( font );
    textItem.setOptions( options );

    textItem.setWidth( -1 );
    textItem.setText( text );

    textItem.end();

    const QSizeF sz( textItem.implicitWidth(), textItem.implicitHeight() );

    textItem.reset();

    return sz;
}

QRectF QskRichTextRenderer::textRect(
    const QString& text, const QFont& font,
    const QskTextOptions& options, const QSizeF& size )
{
    auto& textItem = *qskTextItemMap->item();

    textItem.begin();

    textItem.setFont( font );
    textItem.setOptions( options );
    textItem.setAlignment( Qt::Alignment() );

    textItem.setWidth( size.width() );
    textItem.setHeight( size.height() );

    textItem.setText( text );

    textItem.end();

    const auto rect = textItem.layedOutTextRect();

    textItem.reset();

    return rect;
}

void QskRichTextRenderer::updateNode(
    const QString& text, const QFont& font,
    const QskTextOptions& options, Qsk::TextStyle style,
    const QskTextColors& colors, Qt::Alignment alignment,
    const QRectF& rect, const QQuickItem* item, QSGTransformNode* node )
{
    // are we killing internal caches of QQuickText, when always using
    // the same item for the creation the text nodes. TODO ...

    auto& textItem = *qskTextItemMap->item();

    textItem.begin();

    textItem.setGeometry( rect );

    textItem.setBottomPadding( 0 );
    textItem.setTopPadding( 0 );
    textItem.setFont( font );
    textItem.setOptions( options );
    textItem.setAlignment( alignment );

    textItem.setColor( colors.textColor );
    textItem.setStyle( static_cast< QQuickText::TextStyle >( style ) );
    textItem.setStyleColor( colors.styleColor );
    textItem.setLinkColor( colors.linkColor );

    textItem.setText( text );

    textItem.end();

    if ( alignment & Qt::AlignVCenter )
    {
        /*
            We need to have a stable algo for rounding the text base line,
            so that texts don't start wobbling, when processing transitions
            between margins/paddings. We manipulate the layout code
            by adding some padding, so that the position of base line
            gets always floored.
         */
        auto d = QQuickTextPrivate::get( &textItem );

        const qreal h = d->layedOutTextRect.height() + d->lineHeightOffset();

        if ( static_cast< int >( rect.height() - h ) % 2 )
        {
            if ( static_cast< int >( h ) % 2 )
                d->extra->bottomPadding = 1;
            else
                d->extra->topPadding = 1;
        }
    }

    textItem.updateTextNode( item->window(), node );
    textItem.reset();
}
