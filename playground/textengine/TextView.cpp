/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "TextView.h"

#include <QskTextLabel.h>
#include <QskComboBox.h>
#include <QskScrollArea.h>

#include <QFont>

class OptionsPanel : public QskLinearBox
{
    Q_OBJECT

  public:
    OptionsPanel( QQuickItem* parent )
        : QskLinearBox( Qt::Vertical, parent )
    {
        {
            m_orientationBox = new QskComboBox( this );
            m_orientationBox->addOption( QUrl(), "No constraint" );
            m_orientationBox->addOption( QUrl(), "Height for width" );
            m_orientationBox->addOption( QUrl(), "Width for height" );
            m_orientationBox->setCurrentIndex( 0 );

            connect( m_orientationBox, &QskComboBox::currentIndexChanged,
                this, &OptionsPanel::changed );
        }

    }

    QskTextOptions textOptions() const
    {
        QskTextOptions options;

        options.setFormat( QskTextOptions::AutoText );
        options.setElideMode( Qt::ElideNone );
        options.setWrapMode( QskTextOptions::WordWrap );
        options.setFontSizeMode( QskTextOptions::FixedSize );
        options.setMaximumLineCount(
            QskTextOptions().maximumLineCount() );

        return options;
    }

    QskSizePolicy textSizePolicy() const
    {
        switch( m_orientationBox->currentIndex() )
        {
            case 1:
                return QskSizePolicy( QskSizePolicy::Ignored, QskSizePolicy::Constrained );

            case 2:
                return QskSizePolicy( QskSizePolicy::Constrained, QskSizePolicy::Ignored );

            default:
                return QskSizePolicy( QskSizePolicy::Ignored, QskSizePolicy::Ignored );
        }
    }

  Q_SIGNALS:
    void changed();

  private:
    QskComboBox* m_orientationBox;
};

class TextLabel : public QskTextLabel
{
  public:
    TextLabel( QQuickItem* parent = nullptr )
        : QskTextLabel( parent )
    {
        setSkinHint( Text | QskAspect::FontRole, QFont( "Roboto", 22 ) );

        setText(
            "Gschupfte Nudla, saure Kuddla, Häbresmuas a Flädlessuppe, "
            "Grundbiara mit Mill und Käs, a Moscht vom Kealar gar it räs!"
        );
    }
};

TextView::TextView( QQuickItem* parent )
    : QskLinearBox( Qt::Vertical, parent )
{
    setMargins( 10 );
    setBackgroundColor( Qt::lightGray );

    setExtraSpacingAt( Qt::RightEdge | Qt::BottomEdge );

    m_panel = new OptionsPanel( this );
    m_panel->setSizePolicy( Qt::Vertical, QskSizePolicy::Fixed );

    m_label = new TextLabel();
    m_label->setBackgroundColor( Qt::white );

    auto scrollArea = new QskScrollArea( this );
    scrollArea->setItemResizable( true );
    scrollArea->setScrolledItem( m_label );

    updateLabel();

    connect( m_panel, &OptionsPanel::changed,
        this, &TextView::updateLabel );
}

void TextView::updateLabel()
{
    m_label->setTextOptions( m_panel->textOptions() );
    m_label->setSizePolicy( m_panel->textSizePolicy() );
}

#include "TextView.moc"
