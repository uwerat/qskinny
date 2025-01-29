/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "QskFontSelectionWindow.h"

#include "QskFontRole.h"
#include "QskGridBox.h"
#include "QskLinearBox.h"
#include "QskSimpleListBox.h"
#include "QskTextLabel.h"

#include <QFontDatabase>

template< typename W >
class QskFontSelectionWindow< W >::PrivateData
{
  public:
    QFont selectedFont;

    QskSimpleListBox* familyView;
    QskSimpleListBox* styleView;
    QskSimpleListBox* sizeView;

    QskTextLabel* outputLabel;
};

template< typename W >
QskFontSelectionWindow< W >::QskFontSelectionWindow( QObject* parent, const QString& title,
        QskDialog::Actions actions, QskDialog::Action defaultAction )
        : Inherited( parent, title, actions, defaultAction )
        , m_data( new PrivateData )
{
    auto* outerBox = new QskLinearBox( Qt::Vertical );
    outerBox->setMargins( 20 );
    outerBox->setSpacing( 20 );
#if 1
    outerBox->setFixedSize( 700, 500 );
#endif

    setupControls( outerBox );
    loadFontInfo();

    Inherited::setContentItem( outerBox );
}

template< typename W >
QskFontSelectionWindow< W >::~QskFontSelectionWindow() = default;

template< typename W >
QFont QskFontSelectionWindow< W >::selectedFont() const
{
    return m_data->selectedFont;
}

template< typename W >
void QskFontSelectionWindow< W >::setupControls( QQuickItem* parentItem )
{
    auto* gridBox = new QskGridBox( parentItem );
    gridBox->setSpacing( 10 );

    const QskFontRole role( QskFontRole::Subtitle, QskFontRole::Normal );

    auto* familyLabel = new QskTextLabel( "Family", gridBox );
    familyLabel->setFontRole( role );
    gridBox->addItem( familyLabel, 0, 0 );

    auto* styleLabel = new QskTextLabel( "Style", gridBox );
    styleLabel->setFontRole( role );
    gridBox->addItem( styleLabel, 0, 1 );

    auto* sizeLabel = new QskTextLabel( "Size", gridBox );
    sizeLabel->setFontRole( role );
    gridBox->addItem( sizeLabel, 0, 2 );

    m_data->familyView = new QskSimpleListBox( gridBox );
    m_data->familyView->setSizePolicy( Qt::Vertical, QskSizePolicy::Expanding );
    gridBox->addItem( m_data->familyView, 1, 0 );

    m_data->styleView = new QskSimpleListBox( gridBox );
    m_data->styleView->setSizePolicy( Qt::Vertical, QskSizePolicy::Expanding );
    gridBox->addItem( m_data->styleView, 1, 1 );

    m_data->sizeView = new QskSimpleListBox( gridBox );
    m_data->sizeView->setSizePolicy( Qt::Vertical, QskSizePolicy::Expanding );
    gridBox->addItem( m_data->sizeView, 1, 2 );

    auto* sampleLabel = new QskTextLabel( "Sample", gridBox );
    sampleLabel->setFontRole( role );
    gridBox->addItem( sampleLabel, 2, 0 );

    m_data->outputLabel = new QskTextLabel( gridBox );
    m_data->outputLabel->setSizePolicy( QskSizePolicy::Preferred, QskSizePolicy::Minimum );
    m_data->outputLabel->setElideMode( Qt::ElideRight );
    gridBox->addItem( m_data->outputLabel, 3, 0, 1, 3 );

    gridBox->setColumnStretchFactor( 0, 5 );
    gridBox->setColumnStretchFactor( 1, 3 );
    gridBox->setColumnStretchFactor( 2, 2 );
}

template< typename W >
void QskFontSelectionWindow< W >::loadFontInfo()
{
    const auto families = QFontDatabase::families();
    m_data->familyView->setEntries( families );

    QObject::connect( m_data->familyView, &QskSimpleListBox::selectedEntryChanged,
        this, [this]( const QString& family )
    {
            const auto styles = QFontDatabase::styles( family );
            m_data->styleView->setEntries( styles );
    } );

    QObject::connect( m_data->familyView, &QskSimpleListBox::selectedEntryChanged,
        this, [this]( const QString& family )
    {
            const auto sizes = QFontDatabase::pointSizes( family );
            QStringList sizesString;
            sizesString.reserve( sizes.count() );

            for( const auto size : sizes )
            {
                sizesString.append( QString::number( size ) );
            }

            m_data->sizeView->setEntries( sizesString );
    } );

    auto displaySample = [this]()
    {
        const auto family = m_data->familyView->selectedEntry();
        const auto style = m_data->styleView->selectedEntry();
        const auto size = m_data->sizeView->selectedEntry();

        if( !family.isNull() && !style.isNull() && !size.isNull() )
        {
            auto& f = m_data->selectedFont;

            f = QFont( family, size.toInt() );
            f.setStyleName( style );

            m_data->outputLabel->setSkinHint( QskTextLabel::Text | QskAspect::FontRole, f );
            m_data->outputLabel->resetImplicitSize();
            m_data->outputLabel->setText( "The quick brown fox jumps over the lazy dog" );
        }
        else
        {
            m_data->outputLabel->setText( {} );
        }
    };

    QObject::connect( m_data->familyView, &QskSimpleListBox::selectedEntryChanged, this, displaySample );
    QObject::connect( m_data->styleView, &QskSimpleListBox::selectedEntryChanged, this, displaySample );
    QObject::connect( m_data->sizeView, &QskSimpleListBox::selectedEntryChanged, this, displaySample );
}

template class QskFontSelectionWindow< QskDialogWindow >;
template class QskFontSelectionWindow< QskDialogSubWindow >;
