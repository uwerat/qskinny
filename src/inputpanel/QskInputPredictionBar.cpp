/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskInputPredictionBar.h"
#include "QskLinearBox.h"
#include "QskPushButton.h"
#include "QskTextOptions.h"

#include <qfontmetrics.h>
#include <qstringlist.h>

QSK_SUBCONTROL( QskInputPredictionBar, Panel )
QSK_SUBCONTROL( QskInputPredictionBar, ButtonPanel )
QSK_SUBCONTROL( QskInputPredictionBar, ButtonText )

namespace
{
    class Button final : public QskPushButton
    {
      public:
        Button( QQuickItem* parent )
            : QskPushButton( parent )
        {
            QskTextOptions options;
            options.setElideMode( Qt::ElideRight );

            setTextOptions( options );
        }

        QSizeF contentsSizeHint(
            Qt::SizeHint which, const QSizeF& ) const override
        {
            if ( which != Qt::PreferredSize )
                return QSizeF();

            auto size = QFontMetricsF( font() ).size( Qt::TextSingleLine, text() );

            size = size.expandedTo( strutSizeHint( Panel ) );
            size = outerBoxSize( Panel, size );

            return size;
        }

        QskAspect::Subcontrol effectiveSubcontrol(
            QskAspect::Subcontrol subControl ) const override
        {
            if ( subControl == QskPushButton::Panel )
                return QskInputPredictionBar::ButtonPanel;

            if ( subControl == QskPushButton::Text )
                return QskInputPredictionBar::ButtonText;

            return subControl;
        }
    };
}

class QskInputPredictionBar::PrivateData
{
  public:
    QskLinearBox* layoutBox;
    QStringList candidates;

    int scrollOffset = 0;
    const int buttonCount = 12;
};

QskInputPredictionBar::QskInputPredictionBar( QQuickItem* parent )
    : Inherited( parent )
    , m_data( new PrivateData )
{
    setAutoLayoutChildren( true );
    initSizePolicy( QskSizePolicy::Expanding, QskSizePolicy::Fixed );

    m_data->layoutBox = new QskLinearBox( Qt::Horizontal, this );

    for ( int i = 0; i < m_data->buttonCount; i++ )
    {
        auto button = new Button( m_data->layoutBox );
        button->setVisible( false );
        button->setSizePolicy( Qt::Horizontal, QskSizePolicy::Maximum );

        connect( button, &QskPushButton::clicked,
            this, &QskInputPredictionBar::buttonClicked );

        if ( i == 0 )
        {
            // to keep the height
            button->setLayoutHint( QskControl::RetainSizeWhenHidden, true );
        }
    }
}

QskInputPredictionBar::~QskInputPredictionBar()
{
}

QskAspect::Subcontrol QskInputPredictionBar::effectiveSubcontrol(
    QskAspect::Subcontrol subControl ) const
{
    if ( subControl == QskBox::Panel )
        return QskInputPredictionBar::Panel;

    return subControl;
}

void QskInputPredictionBar::setPrediction( const QStringList& candidates )
{
    if ( m_data->candidates != candidates )
    {
        m_data->candidates = candidates;
        setScrollOffset( 0 );
    }
}

QStringList QskInputPredictionBar::candidates() const
{
    return m_data->candidates;
}

void QskInputPredictionBar::setScrollOffset( int offset )
{
    m_data->scrollOffset = offset;

    const int candidateCount = m_data->candidates.length();
    const int count = std::min( candidateCount, m_data->buttonCount );
    const bool continueLeft = m_data->scrollOffset > 0;
    const bool continueRight = ( candidateCount - m_data->scrollOffset ) > count;

    for ( int i = 0; i < count; i++ )
    {
        auto button = qobject_cast< QskPushButton* >(
            m_data->layoutBox->itemAtIndex( i ) );

        if ( continueLeft && i == 0 )
        {
            button->setText( QChar( 0x2B05 ) );
        }
        else if ( continueRight && ( i == m_data->buttonCount - 1 ) )
        {
            button->setText( QChar( 0x27A1 ) );
        }
        else
        {
            const int index = i + m_data->scrollOffset;
            button->setText( m_data->candidates[ index ] );
        }

        button->setVisible( true );
    }

    for ( int i = count; i < m_data->buttonCount; ++i )
        m_data->layoutBox->itemAtIndex( i )->setVisible( false );
}

void QskInputPredictionBar::buttonClicked()
{
    const int index = m_data->layoutBox->indexOf(
        qobject_cast< QQuickItem* > ( sender() ) );

    const int offset = m_data->scrollOffset;

    if ( index == 0 )
    {
        if ( offset > 0 )
        {
            setScrollOffset( offset - 1 );
            return;
        }
    }
    else if ( index == m_data->buttonCount - 1 )
    {
        if ( m_data->candidates.count() - offset > m_data->buttonCount )
        {
            setScrollOffset( offset + 1 );
            return;
        }
    }

    Q_EMIT predictiveTextSelected( offset + index );
}

#include "moc_QskInputPredictionBar.cpp"
