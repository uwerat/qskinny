/******************************************************************************
 * Copyright (C) 2022 Edelhirsch Software GmbH
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#include "StoragePage.h"
#include "Box.h"
#include "CircularProgressBar.h"
#include "Diagram.h"
#include "EnergyMeter.h"
#include "StorageBar.h"
#include "StorageMeter.h"
#include <QTimer>
#include <QskAnimator.h>
#include <QskBox.h>
#include <QskBoxShapeMetrics.h>
#include <QskGradient.h>
#include <QskGradientStop.h>
#include <QskGraphicLabel.h>
#include <QskProgressBar.h>
#include <QskPushButton.h>
#include <QskSkin.h>
#include <QskStackBox.h>
#include <QskTextLabel.h>

QSK_SUBCONTROL( StoragePage, Panel )

struct StorageRowAnimator final : public QObject, public QskAnimator
{
    explicit StorageRowAnimator( QQuickWindow* const window, QObject* parent )
        : QObject( parent )
    {
        setEasingCurve( QEasingCurve::InQuad );
        setWindow( window );
        setDuration( 400 );
    }

    void advance( qreal value ) override
    {
        callback( value );
    }

    std::function< void( qreal ) > callback = []( qreal ) {};
};

StoragePage::StoragePage( QQuickItem* const parent )
    : QskLinearBox( Qt::Vertical, parent )
{
    setPanel( true );
    setSizePolicy( QskSizePolicy::Expanding, QskSizePolicy::Expanding );
    setDefaultAlignment( Qt::AlignTop );
    setSpacing( 24 );
    setMargins( 30 );
    setSizePolicy( Qt::Horizontal, QskSizePolicy::Minimum );

    setSubcontrolProxy( QskBox::Panel, StoragePage::Panel );

    addRow( "Backup (B:)", "Used for daily backups", 0.1, 0.1, 0.1, 0.02, 0.01 );
    addRow( "Share (S:)", "Used for sharing files publicly", 0.05, 0.05, 0.2, 0.2, 0.01 );
    addRow( "Exchange (X:)", "Used for exchanging large files", 0.1, 0.1, 0.1, 0.1, 0.5 );

    addSpacer( 1, 99 );
}

void StoragePage::addRow( const QString& title, const QString& description,
    qreal pictures, qreal music, qreal videos, qreal documents, qreal others )
{
    Storage storage;

    storage.title = title;
    storage.description = description;
    storage.distribution.pictures = pictures;
    storage.distribution.music = music;
    storage.distribution.videos = videos;
    storage.distribution.documents = documents;
    storage.distribution.others = others;

    auto* const box = new Box( "Network Storage", this );
    auto* const layout = new QskLinearBox( Qt::Horizontal, box );
    auto* const left = new QskLinearBox( Qt::Vertical, layout );
    left->setDefaultAlignment( Qt::AlignCenter );
    auto* const center = new QskLinearBox( Qt::Vertical, layout );
    left->setDefaultAlignment( Qt::AlignLeft );
    auto* const right = new QskLinearBox( Qt::Vertical, layout );
    layout->setStretchFactor( left, 1 );
    layout->setStretchFactor( center, 2 );
    layout->setStretchFactor( right, 5 );

    const auto percent = 100.0 * ( 1.0 - storage.distribution.free() );
    auto* const meter = new StorageMeter( left );
    meter->setValue( percent );
    meter->setMinimumSize( 64, 64 );
    meter->setMaximumSize( 64, 64 );

    auto* const maintitle = new QskTextLabel( storage.title, center );
    maintitle->setFontRole( QskSkin::LargeFont );
    auto* const subtitle = new QskTextLabel( storage.description, center );
    subtitle->setFontRole( QskSkin::MediumFont );

    const auto& media = storage.distribution;

    auto* const bar = new StorageBar( right );
    bar->setPictures( media.pictures );
    bar->setMusic( media.music );
    bar->setVideos( media.videos );
    bar->setDocuments( media.documents );
    bar->setOthers( media.others );

    auto* const legend = new QskLinearBox( Qt::Horizontal, right );
    legend->setSpacing( 12 );
    legend->addSpacer( 1, 999 );
    auto* const sync = new QskPushButton( "Update", legend );
    sync->setFontRoleHint( QskPushButton::Text, QskSkin::SmallFont );

    using S = StorageBar;
    for ( const auto& pair : QVector< QPair< QString, QskGradient > >{
              { QStringLiteral( "Picture" ), bar->gradientHint( S::Pictures ) },
              { QStringLiteral( "Music" ), bar->gradientHint( S::Music ) },
              { QStringLiteral( "Videos" ), bar->gradientHint( S::Videos ) },
              { QStringLiteral( "Documents" ), bar->gradientHint( S::Documents ) },
              { QStringLiteral( "Others" ), bar->gradientHint( S::Others ) },
              { QStringLiteral( "Free" ), bar->gradientHint( S::Free ) } } )
    {
        constexpr int size = 8;
        auto* const dot = new QskBox( legend );
        dot->setBoxShapeHint( QskBox::Panel, { size / 2 } );
        dot->setMinimumSize( size, size );
        dot->setMaximumSize( size, size );
        dot->setGradientHint( QskBox::Panel, pair.second );
        auto* const label = new QskTextLabel( pair.first, legend );
        label->setFontRole( QskSkin::SmallFont );
    }

    auto* const animator = new StorageRowAnimator( window(), sync );
    animator->callback = [ meter, bar, media ]( qreal v ) {
        meter->setValue( 100 * ( 1.0 - media.free() ) * v );
        bar->setPictures( media.pictures * v );
        bar->setMusic( media.music * v );
        bar->setVideos( media.videos * v );
        bar->setDocuments( media.documents * v );
        bar->setOthers( media.others * v );
    };
    connect( sync, &QskPushButton::clicked, animator, [ animator ]() { animator->start(); } );
}
