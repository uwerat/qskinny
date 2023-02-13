/******************************************************************************
 * Copyright (C) 2022 Edelhirsch Software GmbH
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#pragma once

#include <QskControl.h>

class StorageBar final : public QskControl
{
    using Inherited = QskControl;

    Q_OBJECT
    Q_PROPERTY( qreal pictures READ pictures WRITE setPictures NOTIFY picturesChanged )
    Q_PROPERTY( qreal music READ music WRITE setMusic NOTIFY musicChanged )
    Q_PROPERTY( qreal videos READ videos WRITE setVideos NOTIFY videosChanged )
    Q_PROPERTY( qreal documents READ documents WRITE setDocuments NOTIFY documentsChanged )
    Q_PROPERTY( qreal others READ others WRITE setOthers NOTIFY othersChanged )

  public:
    QSK_SUBCONTROLS( Pictures, Music, Videos, Documents, Others, Free )
    explicit StorageBar( QskQuickItem* parent = nullptr );

    qreal pictures() const;
    void setPictures( qreal newPictures );
    qreal music() const;
    void setMusic( qreal newMusic );
    qreal videos() const;
    void setVideos( qreal newVideos );
    qreal documents() const;
    void setDocuments( qreal newDocuments );
    qreal others() const;
    void setOthers( qreal newOthers );
    qreal free() const;

  Q_SIGNALS:

    void picturesChanged( qreal value );
    void musicChanged( qreal value );
    void videosChanged( qreal value );
    void documentsChanged( qreal value );
    void othersChanged( qreal value );

  private:
    qreal m_pictures{ 0.0 };
    qreal m_music{ 0.0 };
    qreal m_videos{ 0.0 };
    qreal m_documents{ 0.0 };
    qreal m_others{ 0.0 };
};
