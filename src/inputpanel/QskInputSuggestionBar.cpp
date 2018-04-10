/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskInputSuggestionBar.h"
#include "QskPushButton.h"
#include "QskLinearBox.h"
#include "QskTextOptions.h"

#include <QFontMetricsF>
#include <QVector>

QSK_SUBCONTROL( QskInputSuggestionBar, Panel )
QSK_SUBCONTROL( QskInputSuggestionBar, ButtonPanel )
QSK_SUBCONTROL( QskInputSuggestionBar, ButtonText )

namespace
{
    class Button final : public QskPushButton
    {
    public:
        Button( QQuickItem* parent ):
            QskPushButton( parent )
        {
            QskTextOptions options;
            options.setElideMode( Qt::ElideRight );

            setTextOptions( options );
        }

        virtual QSizeF contentsSizeHint() const override
        {
            auto size = QFontMetricsF( font() ).size( Qt::TextSingleLine, text() );

            const QSizeF minSize( metric( Panel | QskAspect::MinimumWidth ),
                metric( Panel | QskAspect::MinimumHeight ) );

            size = size.expandedTo( minSize );
            size = outerBoxSize( Panel, size );

            return size;
        }

        virtual QskAspect::Subcontrol effectiveSubcontrol(
            QskAspect::Subcontrol subControl ) const override final
        {
            if( subControl == QskPushButton::Panel )
                return QskInputSuggestionBar::ButtonPanel;

            if( subControl == QskPushButton::Text )
                return QskInputSuggestionBar::ButtonText;

            return subControl;
        }
    };
}

class QskInputSuggestionBar::PrivateData
{
public:
    QskLinearBox* layoutBox;
    QVector< QString > candidates;

    int candidateOffset = 0;
    const int buttonCount = 12;
};

QskInputSuggestionBar::QskInputSuggestionBar( QQuickItem* parent ):
    Inherited( parent ),
    m_data( new PrivateData )
{
    setAutoLayoutChildren( true );
    initSizePolicy( QskSizePolicy::Expanding, QskSizePolicy::Fixed );

    m_data->layoutBox = new QskLinearBox( Qt::Horizontal, this );

    for( int i = 0; i < m_data->buttonCount; i++ )
    {
        auto button = new Button( m_data->layoutBox );
        button->setVisible( false );
        button->setSizePolicy( Qt::Horizontal, QskSizePolicy::Maximum );

        connect( button, &QskPushButton::clicked,
            this, &QskInputSuggestionBar::candidateClicked );

        if ( i == 0 )
        {
            // to keep the height
            m_data->layoutBox->setRetainSizeWhenHidden( button, true );
        }
    }

}

QskInputSuggestionBar::~QskInputSuggestionBar()
{
}

QskAspect::Subcontrol QskInputSuggestionBar::effectiveSubcontrol(
    QskAspect::Subcontrol subControl ) const
{
    if( subControl == QskBox::Panel )
        return QskInputSuggestionBar::Panel;

    return subControl;
}

void QskInputSuggestionBar::setCandidates( const QVector< QString >& candidates )
{
    if( m_data->candidates != candidates )
    {
        m_data->candidates = candidates;
        setCandidateOffset( 0 );
    }
}

QVector< QString > QskInputSuggestionBar::candidates() const
{
    return m_data->candidates;
}

void QskInputSuggestionBar::setCandidateOffset( int offset )
{
    m_data->candidateOffset = offset;

    const auto candidateCount = m_data->candidates.length();
    const auto count = std::min( candidateCount, m_data->buttonCount );
    const bool continueLeft = m_data->candidateOffset > 0;
    const bool continueRight = ( candidateCount - m_data->candidateOffset ) > count;

    for( int i = 0; i < count; i++ )
    {
        auto button = qobject_cast< QskPushButton* >(
            m_data->layoutBox->itemAtIndex( i ) );

        if( continueLeft && i == 0 )
        {
            button->setText( QChar( 0x2B05 ) );
        }
        else if( continueRight && ( i == m_data->buttonCount - 1 ) )
        {
            button->setText( QChar( 0x27A1 ) );
        }
        else
        {
            const int index = i + m_data->candidateOffset;
            button->setText( m_data->candidates[index] );
        }

        button->setVisible( true );
    }

    for( int i = count; i < m_data->buttonCount; ++i )
        m_data->layoutBox->itemAtIndex( i )->setVisible( false );
}

void QskInputSuggestionBar::candidateClicked()
{
    const int index = m_data->layoutBox->indexOf(
        qobject_cast< QQuickItem* > ( sender() ) );

    const int offset = m_data->candidateOffset;

    if ( index == 0 )
    {
        if ( offset > 0 )
        {
            setCandidateOffset( offset - 1 );
            return;
        }
    }
    else if ( index == m_data->buttonCount - 1 )
    {
        if ( m_data->candidates.count() - offset > m_data->buttonCount )
        {
            setCandidateOffset( offset + 1 );
            return;
        }
    }

    Q_EMIT suggested( offset + index );
}

#include "moc_QskInputSuggestionBar.cpp"
