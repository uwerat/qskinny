/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "QskInputPredictionBar.h"
#include "QskLinearBox.h"
#include "QskPushButton.h"
#include "QskScrollArea.h"
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

            size = size.expandedTo( strutSizeHint( effectiveSubcontrol( Panel ) ) );

            size = outerBoxSize( Panel, size );

            return size;
        }

        QskAspect::Subcontrol substitutedSubcontrol(
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
    QskScrollArea* scrollArea;
    QskLinearBox* layoutBox;
    QStringList candidates;

    int scrollOffset = 0;
    const int maxButtonCount = 12;
};

QskInputPredictionBar::QskInputPredictionBar( QQuickItem* parent )
    : Inherited( parent )
    , m_data( new PrivateData )
{
    setAutoLayoutChildren( true );
    initSizePolicy( QskSizePolicy::Expanding, QskSizePolicy::Fixed );

    m_data->scrollArea = new QskScrollArea( this );
    m_data->scrollArea->setSizePolicy( QskSizePolicy::Expanding, QskSizePolicy::Fixed );
    m_data->scrollArea->setFlickableOrientations( Qt::Horizontal );

    m_data->layoutBox = new QskLinearBox( Qt::Horizontal, m_data->scrollArea );

    m_data->scrollArea->setScrolledItem( m_data->layoutBox );

    for ( int i = 0; i < m_data->maxButtonCount; i++ )
    {
        auto button = new Button( m_data->layoutBox );
        button->setVisible( false );
        button->setSizePolicy( Qt::Horizontal, QskSizePolicy::Fixed );

        connect( button, &QskPushButton::clicked,
            this, &QskInputPredictionBar::buttonClicked );

        if ( i == 0 )
        {
            // to keep the height
            button->setPlacementPolicy( Qsk::Hidden, QskPlacementPolicy::Reserve );
        }
    }
}

QskInputPredictionBar::~QskInputPredictionBar()
{
}

QskAspect::Subcontrol QskInputPredictionBar::substitutedSubcontrol(
    QskAspect::Subcontrol subControl ) const
{
    if ( subControl == QskBox::Panel )
        return QskInputPredictionBar::Panel;

    return Inherited::substitutedSubcontrol( subControl );
}

void QskInputPredictionBar::setPrediction( const QStringList& candidates )
{
    if ( m_data->candidates != candidates )
    {
        m_data->candidates = candidates;
        m_data->scrollArea->resetImplicitSize();
        m_data->scrollArea->setScrollPos( {0, 0} );

        for( int i = 0; i < m_data->maxButtonCount; ++i )
        {
            auto* button = qobject_cast< QskPushButton* >(
                m_data->layoutBox->itemAtIndex( i ) );

            if( candidates.count() > i )
            {
                button->setText( candidates.at(i) );
                button->setVisible( true );
            }
            else
            {
                button->setVisible( false );
            }
        }
    }
}

QStringList QskInputPredictionBar::candidates() const
{
    return m_data->candidates;
}

void QskInputPredictionBar::buttonClicked()
{
    const int index = m_data->layoutBox->indexOf(
        qobject_cast< QQuickItem* > ( sender() ) );

    Q_EMIT predictiveTextSelected( index );
}

#include "moc_QskInputPredictionBar.cpp"
