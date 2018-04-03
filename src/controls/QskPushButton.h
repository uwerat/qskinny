/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_PUSH_BUTTON_H
#define QSK_PUSH_BUTTON_H

#include "QskAbstractButton.h"

class QskCorner;
class QskGraphic;
class QskTextOptions;

class QSK_EXPORT QskPushButton : public QskAbstractButton
{
    Q_OBJECT

    Q_PROPERTY( QString text READ text WRITE setText NOTIFY textChanged FINAL )

    Q_PROPERTY( QskTextOptions textOptions READ textOptions
        WRITE setTextOptions NOTIFY textOptionsChanged )

    Q_PROPERTY( QUrl graphicSource READ graphicSource
        WRITE setGraphicSource NOTIFY graphicSourceChanged FINAL )

    Q_PROPERTY( QskGraphic graphic READ graphic
        WRITE setGraphic NOTIFY graphicChanged FINAL )

    Q_PROPERTY( bool flat READ isFlat WRITE setFlat NOTIFY flatChanged FINAL )
    Q_PROPERTY( QskCorner corner READ corner WRITE setCorner NOTIFY cornerChanged )

    using Inherited = QskAbstractButton;

public:
    QSK_SUBCONTROLS( Panel, Text, Graphic )

    QskPushButton( QQuickItem* parent = nullptr );
    QskPushButton( const QString& text, QQuickItem* parent = nullptr );

    virtual ~QskPushButton();

    void setCorner( const QskCorner& );
    QskCorner corner() const;

    QString text() const;

    void setTextOptions( const QskTextOptions& );
    QskTextOptions textOptions() const;

    QUrl graphicSource() const;
    QskGraphic graphic() const;
    bool hasGraphic() const;

    void setFlat( bool );
    bool isFlat() const;

    QFont font() const;

    virtual QSizeF contentsSizeHint() const override;
    virtual QRectF layoutRect() const override;

public Q_SLOTS:
    void setText( const QString& text );
    void setGraphicSource( const QUrl& url );
    void setGraphic( const QskGraphic& );

Q_SIGNALS:
    void cornerChanged();
    void borderWidthChanged();
    void textChanged();
    void textOptionsChanged();
    void flatChanged();
    void graphicChanged();
    void graphicSourceChanged();

    void hovered( bool );

protected:
    virtual void hoverEnterEvent( QHoverEvent* ) override;
    virtual void hoverLeaveEvent( QHoverEvent* ) override;
    virtual void changeEvent( QEvent* ) override;

    virtual void updateLayout() override;
    virtual QskGraphic loadGraphic( const QUrl& ) const;

private:
    class PrivateData;
    std::unique_ptr< PrivateData > m_data;
};

#endif
