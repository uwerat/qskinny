/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_PUSH_BUTTON_H
#define QSK_PUSH_BUTTON_H

#include "QskAbstractButton.h"

class QskBoxShapeMetrics;
class QskGraphic;
class QskTextOptions;

class QSK_EXPORT QskPushButton : public QskAbstractButton
{
    Q_OBJECT

    Q_PROPERTY( QString text READ text WRITE setText NOTIFY textChanged FINAL )

    Q_PROPERTY( QFont font READ font )

    Q_PROPERTY( QskTextOptions textOptions READ textOptions
        WRITE setTextOptions NOTIFY textOptionsChanged )

    Q_PROPERTY( QUrl graphicSource READ graphicSource
        WRITE setGraphicSource NOTIFY graphicSourceChanged FINAL )

    Q_PROPERTY( QskGraphic graphic READ graphic
        WRITE setGraphic NOTIFY graphicChanged FINAL )

    Q_PROPERTY( QSizeF graphicSourceSize READ graphicSourceSize
        WRITE setGraphicSourceSize RESET resetGraphicSourceSize
        NOTIFY graphicSourceSizeChanged FINAL )

    Q_PROPERTY( bool checkable READ isCheckable
        WRITE setCheckable NOTIFY checkableChanged FINAL )

    Q_PROPERTY( QskBoxShapeMetrics shape READ shape
        WRITE setShape RESET resetShape NOTIFY shapeChanged )

    using Inherited = QskAbstractButton;

  public:
    QSK_SUBCONTROLS( Panel, Ripple, Text, Graphic )

    QskPushButton( QQuickItem* parent = nullptr );
    QskPushButton( const QString& text, QQuickItem* parent = nullptr );

    ~QskPushButton() override;

    void setCheckable( bool );
    bool isCheckable() const override final;

    void setShape( const QskBoxShapeMetrics& );
    QskBoxShapeMetrics shape() const;
    void resetShape();

    QString text() const;

    void setTextOptions( const QskTextOptions& );
    QskTextOptions textOptions() const;

    QUrl graphicSource() const;
    QSizeF graphicSourceSize() const;
    QskGraphic graphic() const;
    bool hasGraphic() const;

    void resetGraphicSourceSize();

    QFont font() const;

    QRectF layoutRectForSize( const QSizeF& ) const override;

  public Q_SLOTS:
    void setText( const QString& );
    void setGraphicSource( const QUrl& );
    void setGraphicSource( const QString& );
    void setGraphic( const QskGraphic& );
    void setGraphicSourceSize( const QSizeF& );

  Q_SIGNALS:
    void checkableChanged( bool );
    void shapeChanged();
    void textChanged();
    void textOptionsChanged();
    void graphicChanged();
    void graphicSourceChanged();
    void graphicSourceSizeChanged();

  protected:
    void changeEvent( QEvent* ) override;
    void mousePressEvent( QMouseEvent* ) override;

    void updateResources() override;
    virtual QskGraphic loadGraphic( const QUrl& ) const;

  private:
    class PrivateData;
    std::unique_ptr< PrivateData > m_data;
};

#endif
