/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#include "GridWidgets.h"
#include <QGridLayout>
#include <QDebug>

namespace
{
    class Rectangle : public QWidget
    {
      public:
        Rectangle( const QByteArray& colorName )
            : m_colorName( colorName )
        {
            setObjectName( colorName );

            setSizePolicy( QSizePolicy::Preferred, QSizePolicy::Preferred );

            setPalette( QColor( colorName.constData() ) );
            setAutoFillBackground( true );

            setAttribute( Qt::WA_LayoutUsesWidgetRect, true );
        }

        void setPreferredWidth( qreal width )
        {
            m_preferredSize.setWidth( width );
        }

        void setPreferredHeight( qreal height )
        {
            m_preferredSize.setHeight( height );
        }

        QSize sizeHint() const override
        {
            return m_preferredSize;
        }

      protected:
        void resizeEvent( QResizeEvent* event ) override
        {
            QWidget::resizeEvent( event );
            //qDebug() << m_color << size();
        }

      private:
        const QByteArray m_colorName;
        QSize m_preferredSize = { 50, 50 };
    };
}

GridWidgets::GridWidgets( QWidget* parent )
    : QWidget( parent )
{
    setPalette( Qt::white );
    setAutoFillBackground( true );

    setContentsMargins( QMargins() );

    m_layout = new QGridLayout();
    m_layout->setSpacing( 5 );

    setLayout( m_layout );
}

GridWidgets::~GridWidgets()
{
}

void GridWidgets::insert( const QByteArray& colorName,
    int row, int column, int rowSpan, int columnSpan )
{
    auto rectangle = new Rectangle( colorName );
    m_layout->addWidget( rectangle, row, column, rowSpan, columnSpan );
    rectangle->show();
}

void GridWidgets::setSpacing( Qt::Orientations orientations, int spacing )
{
    if ( orientations & Qt::Horizontal )
        m_layout->setHorizontalSpacing( spacing );

    if ( orientations & Qt::Vertical )
        m_layout->setVerticalSpacing( spacing );
}

void GridWidgets::setSizeHint(
    int pos, Qt::Orientation orientation, Qt::SizeHint which, int hint )
{
    if ( which != Qt::MinimumSize )
    {
        qWarning() << "setSizeHint" << which << "is not supported by QGridLayout.";
        return;
    }

    if ( orientation == Qt::Vertical )
        m_layout->setRowMinimumHeight( pos, hint );
    else
        m_layout->setColumnMinimumWidth( pos, hint );
}

void GridWidgets::setStretchFactor(
    int pos, Qt::Orientation orientation, int stretch )
{
    if ( orientation == Qt::Vertical )
        m_layout->setRowStretch( pos, stretch );
    else
        m_layout->setColumnStretch( pos, stretch );
}

void GridWidgets::setSizeHintAt(
    int index, Qt::Orientation orientation, Qt::SizeHint which, int hint )
{
    Rectangle* rectangle = nullptr;

    if ( auto layoutItem = m_layout->itemAt( index ) )
        rectangle = static_cast< Rectangle* >( layoutItem->widget() );

    if ( rectangle == nullptr )
        return;

    switch( static_cast< int >( which ) )
    {
        case Qt::MinimumSize:
        {
            if ( orientation == Qt::Horizontal )
                rectangle->setMinimumWidth( hint );
            else
                rectangle->setMinimumHeight( hint );

            break;
        }
        case Qt::PreferredSize:
        {
            if ( orientation == Qt::Horizontal )
                rectangle->setPreferredWidth( hint );
            else
                rectangle->setPreferredHeight( hint );

            break;
        }
        case Qt::MaximumSize:
        {
            if ( orientation == Qt::Horizontal )
                rectangle->setMaximumWidth( hint );
            else
                rectangle->setMaximumHeight( hint );

            break;
        }
    }
}

void GridWidgets::setSizePolicyAt( int index, Qt::Orientation orientation, int policy )
{
    QWidget* widget = nullptr;

    if ( auto layoutItem = m_layout->itemAt( index ) )
        widget = layoutItem->widget();

    if ( widget == nullptr )
        return;

    const auto qPolicy = static_cast< QSizePolicy::Policy >( policy & 0xF );
    const bool isConstrained = policy & ( 1 << 4 );

    auto sizePolicy = widget->sizePolicy();

    if ( orientation == Qt::Horizontal )
    {
        sizePolicy.setHorizontalPolicy( qPolicy );
        sizePolicy.setWidthForHeight( isConstrained );
    }
    else
    {
        sizePolicy.setVerticalPolicy( qPolicy );
        sizePolicy.setHeightForWidth( isConstrained );
    }

    widget->setSizePolicy( sizePolicy );
}

void GridWidgets::setAlignmentAt( int index, Qt::Alignment alignment )
{
    if ( auto layoutItem = m_layout->itemAt( index ) )
        layoutItem->setAlignment( alignment );
}

void GridWidgets::setRetainSizeWhenHiddenAt( int index, bool on )
{
    if ( auto layoutItem = m_layout->itemAt( index ) )
    {
        if ( auto widget = layoutItem->widget() )
        {
            auto sizePolicy = widget->sizePolicy();
            sizePolicy.setRetainSizeWhenHidden( on );
            widget->setSizePolicy( sizePolicy );
        }
    }
}

void GridWidgets::setVisibleAt( int index, bool on )
{
    if ( auto layoutItem = m_layout->itemAt( index ) )
    {
        if ( auto widget = layoutItem->widget() )
            widget->setVisible( on );
    }
}

QSize GridWidgets::preferredSize() const
{
    return sizeHint();
}

