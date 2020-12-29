/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_GRAPHIC_LABEL_H
#define QSK_GRAPHIC_LABEL_H

#include "QskControl.h"

class QskGraphic;
class QskColorFilter;

class QSK_EXPORT QskGraphicLabel : public QskControl
{
    Q_OBJECT

    Q_PROPERTY( QUrl source READ source WRITE setSource NOTIFY sourceChanged )

    Q_PROPERTY( bool mirror READ mirror WRITE setMirror NOTIFY mirrorChanged )

    Q_PROPERTY( QSize sourceSize READ sourceSize
        WRITE setSourceSize RESET resetSourceSize NOTIFY sourceSizeChanged )

    Q_PROPERTY( int graphicRole READ graphicRole
        WRITE setGraphicRole RESET resetGraphicRole NOTIFY graphicRoleChanged )

    Q_PROPERTY( Qt::Alignment alignment READ alignment
        WRITE setAlignment RESET resetAlignment NOTIFY alignmentChanged )

    Q_PROPERTY( FillMode fillMode READ fillMode
        WRITE setFillMode NOTIFY fillModeChanged )

    using Inherited = QskControl;

  public:
    QSK_SUBCONTROLS( Graphic )

    enum FillMode
    {
        Stretch,
        PreserveAspectFit,
        PreserveAspectCrop,
        Pad
    };

    Q_ENUM( FillMode )

    QskGraphicLabel( QQuickItem* parent = nullptr );

    QskGraphicLabel( const QUrl& url, QQuickItem* parent = nullptr );
    QskGraphicLabel( const QString& url, QQuickItem* parent = nullptr );

    QskGraphicLabel( const QskGraphic&, QQuickItem* parent = nullptr );

    ~QskGraphicLabel() override;

    QskGraphic graphic() const;

    virtual QskColorFilter graphicFilter() const;

    QUrl source() const;
    void setSource( const QString& source );
    void setSource( const QUrl& url );

    void setSourceSize( const QSize& size );
    void resetSourceSize();
    QSize sourceSize() const;
    QSizeF effectiveSourceSize() const;

    void setMirror( bool on );
    bool mirror() const;

    void setAlignment( Qt::Alignment );
    void resetAlignment();
    Qt::Alignment alignment() const;

    void setFillMode( FillMode );
    FillMode fillMode() const;

    bool isEmpty() const;

    void setGraphicRole( int role );
    void resetGraphicRole();
    int graphicRole() const;

  Q_SIGNALS:
    void sourceChanged();
    void mirrorChanged();
    void sourceSizeChanged();
    void graphicRoleChanged( int );
    void alignmentChanged( Qt::Alignment );
    void fillModeChanged( FillMode );

  public Q_SLOTS:
    void setGraphic( const QskGraphic& );

  protected:
    void changeEvent( QEvent* ) override;
    void updateResources() override;
    virtual QskGraphic loadSource( const QUrl& ) const;

  private:
    class PrivateData;
    std::unique_ptr< PrivateData > m_data;
};

#endif
