/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "QskColorSelectionWindow.h"

#include "QskBoxBorderColors.h"
#include "QskBoxBorderMetrics.h"
#include "QskBoxShapeMetrics.h"
#include "QskColorPicker.h"
#include "QskComboBox.h"
#include "QskGridBox.h"
#include "QskLinearBox.h"
#include "QskSlider.h"
#include "QskTextField.h"
#include "QskTextLabel.h"

template< typename W >
class QskColorSelectionWindow< W >::PrivateData
{
  public:
    QskColorPicker* colorPicker;
};

template< typename W >
QskColorSelectionWindow< W >::QskColorSelectionWindow( QObject* parent, const QString& title,
        QskDialog::Actions actions, QskDialog::Action defaultAction )
        : Inherited( parent, title, actions, defaultAction )
        , m_data( new PrivateData )
{
    auto* outerBox = new QskLinearBox( Qt::Vertical );
    outerBox->setMargins( 20 );
    outerBox->setSpacing( 20 );
#if 1
    outerBox->setFixedSize( 350, 500 );
#endif
    auto* upperBox = new QskLinearBox( Qt::Horizontal, outerBox );
    upperBox->setSizePolicy( Qt::Vertical, QskSizePolicy::Expanding );
    upperBox->setSpacing( 12 );

    m_data->colorPicker = new QskColorPicker( upperBox );
    m_data->colorPicker->setStrutSizeHint( QskColorPicker::Selector, { 18, 18 } );
    m_data->colorPicker->setBoxShapeHint( QskColorPicker::Selector, { 100, Qt::RelativeSize } );
    m_data->colorPicker->setBoxBorderMetricsHint( QskColorPicker::Selector, 2 );
    m_data->colorPicker->setBoxBorderColorsHint( QskColorPicker::Selector, Qt::black );
    m_data->colorPicker->setGradientHint( QskColorPicker::Selector, Qt::transparent );

    auto* outputBox = new QskBox( upperBox );
    outputBox->setPanel( true );

    QObject::connect( m_data->colorPicker, &QskColorPicker::selectedColorChanged,
        this, [this, outputBox]()
    {
        const auto c = m_data->colorPicker->selectedColor();
        outputBox->setGradientHint( QskBox::Panel, c );
    } );

    upperBox->setStretchFactor( m_data->colorPicker, 9 );
    upperBox->setStretchFactor( outputBox, 1 );


    auto* valueSlider = new QskSlider( outerBox );
    valueSlider->setBoundaries( 0, 1 );
    valueSlider->setValue( m_data->colorPicker->value() );

    QskGradient g( Qt::black, Qt::white );
    g.setLinearDirection( Qt::Horizontal );
    valueSlider->setGradientHint( QskSlider::Groove, g );
    valueSlider->setGradientHint( QskSlider::Fill, Qt::transparent );
    valueSlider->setGradientHint( QskSlider::Handle, Qt::black );

    QObject::connect( valueSlider, &QskSlider::valueChanged,
        m_data->colorPicker, &QskColorPicker::setValueAsRatio );


    auto* gridBox = new QskGridBox( outerBox );
    gridBox->setSizePolicy( Qt::Vertical, QskSizePolicy::Preferred );

    auto* menu = new QskComboBox( gridBox );
    menu->addOption( QUrl(), "RGB" );
    menu->setCurrentIndex( 0 );
    gridBox->addItem( menu, 0, 0 );

    auto* rgbValue = new QskTextField( gridBox );
    rgbValue->setReadOnly( true );
    gridBox->addItem( rgbValue, 0, 2 );

    auto* redValue = new QskTextField( gridBox );
    redValue->setReadOnly( true );
    gridBox->addItem( redValue, 1, 0 );
    gridBox->addItem( new QskTextLabel( "Red", gridBox ), 1, 1 );

    auto* greenValue = new QskTextField( gridBox );
    greenValue->setReadOnly( true );
    gridBox->addItem( greenValue, 2, 0 );
    gridBox->addItem( new QskTextLabel( "Green", gridBox ), 2, 1 );

    auto* blueValue = new QskTextField( gridBox );
    blueValue->setReadOnly( true );
    gridBox->addItem( blueValue, 3, 0 );
    gridBox->addItem( new QskTextLabel( "Blue", gridBox ), 3, 1 );

    QObject::connect( m_data->colorPicker, &QskColorPicker::selectedColorChanged,
        this, [this, rgbValue, redValue, greenValue, blueValue]()
    {
            const auto c = m_data->colorPicker->selectedColor();
            rgbValue->setText( c.name() );

            redValue->setText( QString::number( c.red() ) );
            greenValue->setText( QString::number( c.green() ) );
            blueValue->setText( QString::number( c.blue() ) );
    } );

    Inherited::setContentItem( outerBox );
}

template< typename W >
QskColorSelectionWindow< W >::~QskColorSelectionWindow() = default;

template< typename W >
QColor QskColorSelectionWindow< W >::selectedColor() const
{
    return m_data->colorPicker->selectedColor();
}

template class QskColorSelectionWindow< QskDialogWindow >;
template class QskColorSelectionWindow< QskDialogSubWindow >;
