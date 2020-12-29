/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskTextLabel.h"
#include "QskAspect.h"
#include "QskTextOptions.h"

QSK_SUBCONTROL( QskTextLabel, Panel )
QSK_SUBCONTROL( QskTextLabel, Text )

class QskTextLabel::PrivateData
{
  public:
    PrivateData( const QString& txt )
        : text( txt )
        , hasPanel( false )
    {
        effectiveTextFormat = textOptions.format();
    }

    inline QskTextOptions::TextFormat effectiveFormat() const
    {
        if ( textOptions.format() != QskTextOptions::AutoText )
            return textOptions.format();

        if ( effectiveTextFormat == QskTextOptions::AutoText )
            effectiveTextFormat = textOptions.effectiveFormat( text );

        return effectiveTextFormat;
    }

    QString text;

    QskTextOptions textOptions;
    mutable QskTextOptions::TextFormat effectiveTextFormat;

    bool hasPanel : 1;
};

QskTextLabel::QskTextLabel( QQuickItem* parent )
    : QskTextLabel( QString(), parent )
{
}

QskTextLabel::QskTextLabel( const QString& text, QQuickItem* parent )
    : Inherited( parent )
    , m_data( new PrivateData( text ) )
{
    initSizePolicy( QskSizePolicy::Minimum, QskSizePolicy::Fixed );
}

QskTextLabel::~QskTextLabel()
{
}

void QskTextLabel::setPanel( bool on )
{
    if ( on == m_data->hasPanel )
        return;

    m_data->hasPanel = on;

    resetImplicitSize();
    update();

    Q_EMIT panelChanged( on );
}

bool QskTextLabel::hasPanel() const
{
    return m_data->hasPanel;
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

    Q_EMIT textOptionsChanged( options );
}

QskTextOptions QskTextLabel::textOptions() const
{
    return m_data->textOptions;
}

void QskTextLabel::setTextFormat( QskTextOptions::TextFormat format )
{
    auto options = m_data->textOptions;
    options.setFormat( format );

    setTextOptions( options );
}

QskTextOptions::TextFormat QskTextLabel::textFormat() const
{
    return m_data->textOptions.format();
}

QskTextOptions::TextFormat QskTextLabel::effectiveTextFormat() const
{
    return m_data->effectiveFormat();
}

void QskTextLabel::setWrapMode( QskTextOptions::WrapMode wrapMode )
{
    auto options = m_data->textOptions;
    options.setWrapMode( wrapMode );

    setTextOptions( options );

}

QskTextOptions::WrapMode QskTextLabel::wrapMode() const
{
    return m_data->textOptions.wrapMode();
}

void QskTextLabel::setElideMode( Qt::TextElideMode elideMode )
{
    auto options = m_data->textOptions;
    options.setElideMode( elideMode );

    setTextOptions( options );
}

Qt::TextElideMode QskTextLabel::elideMode() const
{
    return m_data->textOptions.elideMode();
}

void QskTextLabel::setFontRole( int role )
{
    if ( setFontRoleHint( Text, role ) )
        Q_EMIT fontRoleChanged( role );
}

void QskTextLabel::resetFontRole()
{
    if ( resetFontRoleHint( Text ) )
        Q_EMIT fontRoleChanged( fontRoleHint( Text ) );
}

int QskTextLabel::fontRole() const
{
    return fontRoleHint( Text );
}

void QskTextLabel::setTextColor( const QColor& color )
{
    if ( setColor( Text, color ) )
        Q_EMIT textColorChanged( color );
}

void QskTextLabel::resetTextColor()
{
    if ( resetColor( Text ) )
        Q_EMIT textColorChanged( color( Text ) );
}

QColor QskTextLabel::textColor() const
{
    return color( Text );
}

void QskTextLabel::setAlignment( Qt::Alignment alignment )
{
    if ( setAlignmentHint( Text, alignment ) )
        Q_EMIT alignmentChanged( alignment );
}

void QskTextLabel::resetAlignment()
{
    if ( resetAlignmentHint( Text ) )
        Q_EMIT alignmentChanged( alignment() );
}

Qt::Alignment QskTextLabel::alignment() const
{
    return alignmentHint( Text, Qt::AlignLeft | Qt::AlignTop );
}

QFont QskTextLabel::font() const
{
    return effectiveFont( QskTextLabel::Text );
}

void QskTextLabel::changeEvent( QEvent* event )
{
    switch ( event->type() )
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
