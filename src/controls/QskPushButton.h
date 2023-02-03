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
        WRITE setTextOptions RESET resetTextOptions NOTIFY textOptionsChanged )

    Q_PROPERTY( QUrl graphicSource READ graphicSource
        WRITE setGraphicSource NOTIFY graphicSourceChanged FINAL )

    Q_PROPERTY( QskGraphic graphic READ graphic
        WRITE setGraphic NOTIFY graphicChanged FINAL )

    Q_PROPERTY( QSizeF graphicStrutSize READ graphicStrutSize
        WRITE setGraphicStrutSize RESET resetGraphicStrutSize
        NOTIFY graphicStrutSizeChanged FINAL )

    Q_PROPERTY( bool checkable READ isCheckable
        WRITE setCheckable NOTIFY checkableChanged FINAL )

    Q_PROPERTY( Emphasis emphasis READ emphasis
        WRITE setEmphasis NOTIFY emphasisChanged )

    Q_PROPERTY( QskBoxShapeMetrics shape READ shape
        WRITE setShape RESET resetShape NOTIFY shapeChanged )

    using Inherited = QskAbstractButton;

  public:
    QSK_SUBCONTROLS( Panel, Ripple, Text, Graphic )

    enum Emphasis
    {   
        VeryLowEmphasis = -2,
        LowEmphasis = -1,
        NoEmphasis = 0,
        HighEmphasis = 1,
        VeryHighEmphasis = 2
    };
    Q_ENUM( Emphasis )

    QskPushButton( QQuickItem* parent = nullptr );
    QskPushButton( const QString& text, QQuickItem* parent = nullptr );

    ~QskPushButton() override;

    void setEmphasis( Emphasis );
    Emphasis emphasis() const;

    void setCheckable( bool );
    bool isCheckable() const override final;

    void setShape( const QskBoxShapeMetrics& );
    QskBoxShapeMetrics shape() const;
    void resetShape();

    QString text() const;

    void setTextOptions( const QskTextOptions& );
    QskTextOptions textOptions() const;
    void resetTextOptions();

    void setGraphicStrutSize( const QSizeF& );
    QSizeF graphicStrutSize() const;
    void resetGraphicStrutSize();

    QUrl graphicSource() const;
    QskGraphic graphic() const;
    bool hasGraphic() const;

    QFont font() const;

    QRectF layoutRectForSize( const QSizeF& ) const override;
    QskAspect::Variation effectiveVariation() const override;

  public Q_SLOTS:
    void setText( const QString& );
    void setGraphicSource( const QUrl& );
    void setGraphicSource( const QString& );
    void setGraphic( const QskGraphic& );

  Q_SIGNALS:
    void checkableChanged( bool );
    void emphasisChanged( Emphasis ) const;
    void shapeChanged();

    void textChanged();
    void textOptionsChanged();

    void graphicChanged();
    void graphicSourceChanged();
    void graphicStrutSizeChanged();

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
