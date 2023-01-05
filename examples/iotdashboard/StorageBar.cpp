/******************************************************************************
 * Copyright (C) 2022 Edelhirsch Software GmbH
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#include "StorageBar.h"
#include <QskAnimator.h>
#include <QskBox.h>
#include <QskBoxShapeMetrics.h>
#include <QskSkinlet.h>

QSK_SUBCONTROL( StorageBar, Pictures )
QSK_SUBCONTROL( StorageBar, Music )
QSK_SUBCONTROL( StorageBar, Videos )
QSK_SUBCONTROL( StorageBar, Documents )
QSK_SUBCONTROL( StorageBar, Others )
QSK_SUBCONTROL( StorageBar, Free )

using S = StorageBar;

StorageBar::StorageBar( QskQuickItem* const parent )
    : Inherited( parent )
{
    static constexpr qreal size = 16.0;
    static constexpr qreal radius = size / 2.0;

    setMinimumSize( -1, size );
    setMaximumSize( -1, size );

    setBoxShapeHint( S::Pictures, { radius, 0.0, radius, 0.0 } );
    setBoxShapeHint( S::Free, { 0.0, radius, 0.0, radius } );
}

qreal StorageBar::pictures() const
{
    return m_pictures;
}

void StorageBar::setPictures( qreal newPictures )
{
    if ( qFuzzyCompare( m_pictures, newPictures ) )
    {
        return;
    }

    m_pictures = newPictures;
    Q_EMIT picturesChanged( m_pictures );
    update();
}

qreal StorageBar::music() const
{
    return m_music;
}

void StorageBar::setMusic( qreal newMusic )
{
    if ( qFuzzyCompare( m_music, newMusic ) )
    {
        return;
    }
    m_music = newMusic;
    Q_EMIT musicChanged( m_music );
    update();
}

qreal StorageBar::videos() const
{
    return m_videos;
}

void StorageBar::setVideos( qreal newVideos )
{
    if ( qFuzzyCompare( m_videos, newVideos ) )
    {
        return;
    }
    m_videos = newVideos;
    Q_EMIT videosChanged( m_videos );
    update();
}

qreal StorageBar::documents() const
{
    return m_documents;
}

void StorageBar::setDocuments( qreal newDocuments )
{
    if ( qFuzzyCompare( m_documents, newDocuments ) )
    {
        return;
    }
    m_documents = newDocuments;
    Q_EMIT documentsChanged( m_documents );
    update();
}

qreal StorageBar::others() const
{
    return m_others;
}

void StorageBar::setOthers( qreal newOthers )
{
    if ( qFuzzyCompare( m_others, newOthers ) )
    {
        return;
    }
    m_others = newOthers;
    Q_EMIT othersChanged( m_others );
    update();
}

qreal StorageBar::free() const
{
    return 1.0 - m_pictures - m_music - m_videos - m_documents - m_others;
}
