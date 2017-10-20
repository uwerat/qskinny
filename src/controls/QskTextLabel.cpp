/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskTextLabel.h"
#include "QskAspect.h"
#include "QskTextOptions.h"
#include "QskTextNode.h"

#include <QFontMetricsF>
#include <QtMath>

QSK_SUBCONTROL( QskTextLabel, Text )

class QskTextLabel::PrivateData
{
public:
    PrivateData( const QString& txt ):
        text( txt )
    {
        effectiveTextFormat = textOptions.format();
    }

    QskTextOptions effectiveOptions() const
    {
        if ( textOptions.format() != QskTextOptions::AutoText )
            return textOptions;

        if ( effectiveTextFormat == QskTextOptions::AutoText )
            effectiveTextFormat = textOptions.effectiveFormat( text );

        QskTextOptions options = textOptions;
        options.setFormat( effectiveTextFormat );

        return options;
    }

    QString text;

    QskTextOptions textOptions;
    mutable QskTextOptions::TextFormat effectiveTextFormat;
};

QskTextLabel::QskTextLabel( QQuickItem* parent ):
    QskTextLabel( QString(), parent )
{
}

QskTextLabel::QskTextLabel( const QString& text, QQuickItem* parent ):
    Inherited( parent ),
    m_data( new PrivateData( text ) )
{
    initSizePolicy( QskSizePolicy::Minimum, QskSizePolicy::Fixed );
}

QskTextLabel::~QskTextLabel()
{
}

void QskTextLabel::setText( const QString& text )
{
    if ( text == m_data->text )
        return;

    m_data->text = text;
    m_data->effectiveTextFormat = m_data->textOptions.format();

    resetImplicitSize();
    update();

    Q_EMIT textChanged( text );
}

QString QskTextLabel::text() const
{
    return m_data->text;
}

void QskTextLabel::setTextOptions( const QskTextOptions& options )
{
    if ( options == m_data->textOptions )
        return;

#if 0
    // we are killing user settings of the policy this way ??

    const QskSizePolicy::Policy policy = ( options.wrapMode() == QTextOption::NoWrap )
        ? QskSizePolicy::Minimum : QskSizePolicy::Preferred;

    setSizePolicy( policy, sizePolicy().verticalPolicy() );
#endif

    m_data->effectiveTextFormat = options.format();
    m_data->textOptions = options;

    resetImplicitSize();
    update();

    Q_EMIT textOptionsChanged();
}

QskTextOptions QskTextLabel::textOptions() const
{
    return m_data->textOptions;
}

void QskTextLabel::setFontRole( int role )
{
    const int oldRole = fontRole();

    QskSkinnable::setFontRole( effectiveSubcontrol( Text ), role );

    if ( oldRole != role )
    {
        resetImplicitSize();
        update();

        Q_EMIT fontRoleChanged();
    }
}

int QskTextLabel::fontRole() const
{
    return QskSkinnable::fontRole( Text );
}

void QskTextLabel::setTextColor( const QColor& color )
{
    const QColor oldColor = textColor();

    QskSkinnable::setColor( effectiveSubcontrol( Text ), color );

    if ( oldColor != color )
    {
        update();
        Q_EMIT textColorChanged();
    }
}

QColor QskTextLabel::textColor() const
{
    return QskSkinnable::color( Text );
}

Qt::Alignment QskTextLabel::alignment() const
{
    return flagHint< Qt::Alignment >(
        Text | QskAspect::Alignment, Qt::AlignLeft | Qt::AlignTop );
}

void QskTextLabel::setAlignment( Qt::Alignment alignment )
{
    if ( alignment == this->alignment() )
        return;

    const auto subControl = effectiveSubcontrol( Text );
    setFlagHint( subControl | QskAspect::Alignment, alignment );

    if ( m_data->text.isEmpty() )
        update();

    Q_EMIT alignmentChanged();
}

QFont QskTextLabel::font() const 
{
    return effectiveFont( QskTextLabel::Text );
}

QSizeF QskTextLabel::contentsSizeHint() const
{
    if ( !m_data->text.isEmpty() )
    {
        const auto font = effectiveFont( Text );
        return QskTextNode::textSize(
            m_data->text, font, m_data->effectiveOptions() );
    }

    return QSizeF( 0, 0 );
}

qreal QskTextLabel::heightForWidth( qreal width ) const
{
    const auto font = effectiveFont( Text );
    const qreal lineHeight = QFontMetricsF( font ).height();

    if ( m_data->text.isEmpty() ||
        ( m_data->textOptions.wrapMode() == QskTextOptions::NoWrap ) )
    {
        return lineHeight;
    }

    qreal maxHeight = std::numeric_limits< qreal >::max();
    if ( maxHeight / lineHeight > m_data->textOptions.maximumLineCount() )
    {
        // be careful with overflows
        maxHeight = m_data->textOptions.maximumLineCount() * lineHeight;
    }

    QSizeF size( width, maxHeight );
    size = QskTextNode::textSize( m_data->text, font, size, m_data->effectiveOptions() );

    return qCeil( size.height() );
}

qreal QskTextLabel::widthForHeight( qreal height ) const
{
    if ( m_data->text.isEmpty() ||
        ( m_data->textOptions.wrapMode() == QskTextOptions::NoWrap ) )
    {
        return Inherited::widthForHeight( height );
    }

    const auto font = effectiveFont( Text );
    const qreal maxWidth = std::numeric_limits< qreal >::max();

    QSizeF size( maxWidth, height );
    size = QskTextNode::textSize( m_data->text, font, size, m_data->effectiveOptions() );

    return qCeil( size.width() );
}

void QskTextLabel::changeEvent( QEvent* event )
{
    switch( event->type() )
    {
        case QEvent::LocaleChange:
        {
            if ( !m_data->text.isEmpty() )
            {
                // maybe Qt::LayoutDirection has changed
                update();
            }

            break;
        }
        default:
            break;
    }

    Inherited::changeEvent( event );
}

#include "moc_QskTextLabel.cpp"
