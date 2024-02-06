/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "QskMessageSubWindow.h"
#include "QskTextLabel.h"

namespace
{
    class TextLabel final : public QskTextLabel
    {
      public:
        TextLabel()
        {
            setObjectName( QStringLiteral( "QskMessageSubWindowTextLabel" ) );

            initSizePolicy( QskSizePolicy::Ignored, QskSizePolicy::ConstrainedExpanding );
            setLayoutAlignmentHint( Qt::AlignLeft | Qt::AlignTop );

            setAlignment( Qt::AlignLeft | Qt::AlignTop );
            setWrapMode( QskTextOptions::WordWrap );
            setElideMode( Qt::ElideRight );
        }
    };
}

QskMessageSubWindow::QskMessageSubWindow( QQuickItem* parent )
    : Inherited( parent )
{
    auto label = new TextLabel();

    connect( label, &QskTextLabel::textChanged,
        this, &QskMessageSubWindow::textChanged );

    connect( label, &QskTextLabel::textOptionsChanged,
        this, &QskMessageSubWindow::textOptionsChanged );

    setContentItem( label );
}

QskMessageSubWindow::~QskMessageSubWindow()
{
}

void QskMessageSubWindow::setText( const QString& text )
{
    if ( auto label = qobject_cast< QskTextLabel* >( contentItem() ) )
        label->setText( text );
}

QString QskMessageSubWindow::text() const
{
    if ( auto label = qobject_cast< const QskTextLabel* >( contentItem() ) )
        return label->text();

    return QString();
}

void QskMessageSubWindow::setTextOptions( const QskTextOptions& options )
{
    if ( auto label = qobject_cast< QskTextLabel* >( contentItem() ) )
        label->setTextOptions( options );
}

QskTextOptions QskMessageSubWindow::textOptions() const
{
    if ( auto label = qobject_cast< const QskTextLabel* >( contentItem() ) )
        return label->textOptions();

    return QskTextOptions();
}

#include "moc_QskMessageSubWindow.cpp"
