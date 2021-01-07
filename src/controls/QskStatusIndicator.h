/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_STATUS_INDICATOR_H
#define QSK_STATUS_INDICATOR_H

#include "QskControl.h"

class QskGraphic;
class QskColorFilter;
class QUrl;

class QSK_EXPORT QskStatusIndicator : public QskControl
{
    Q_OBJECT

    Q_PROPERTY( int status READ status() WRITE setStatus NOTIFY statusChanged )
    Q_PROPERTY( int graphicRole READ graphicRole
        WRITE setGraphicRole RESET resetGraphicRole NOTIFY graphicRoleChanged )

    using Inherited = QskControl;

  public:
    QSK_SUBCONTROLS( Graphic )

    QskStatusIndicator( QQuickItem* parent = nullptr );
    ~QskStatusIndicator() override;

    Q_INVOKABLE QUrl source( int status ) const;
    Q_INVOKABLE void setSource( int status, const QUrl& url );

    QskGraphic graphic( int status ) const;
    void setGraphic( int status, const QskGraphic& );

    void setGraphicRole( int role );
    void resetGraphicRole();
    int graphicRole() const;

    virtual QskColorFilter graphicFilter( int status ) const;
    virtual QskGraphic loadSource( const QUrl& ) const;

    int status() const;
    bool hasStatus( int status ) const;

    QList<int> statusList() const;

  public Q_SLOTS:
    void setStatus( int status );

  Q_SIGNALS:
    void statusChanged( int status );
    void graphicRoleChanged( int );

  protected:
    void changeEvent( QEvent* ) override;
    void updateLayout() override;

  private:
    class PrivateData;
    std::unique_ptr< PrivateData > m_data;
};

#endif
