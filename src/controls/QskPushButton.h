/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
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

    Q_PROPERTY( QUrl iconSource READ iconSource
        WRITE setIconSource NOTIFY iconSourceChanged FINAL )

    Q_PROPERTY( QskGraphic icon READ icon
        WRITE setIcon NOTIFY iconChanged FINAL )

    Q_PROPERTY( QSizeF iconStrutSize READ iconStrutSize
        WRITE setIconStrutSize RESET resetIconStrutSize
        NOTIFY iconStrutSizeChanged FINAL )

    Q_PROPERTY( bool checkable READ isCheckable
        WRITE setCheckable NOTIFY checkableChanged FINAL )

    Q_PROPERTY( Emphasis emphasis READ emphasis
        WRITE setEmphasis NOTIFY emphasisChanged )

    Q_PROPERTY( QskBoxShapeMetrics shape READ shape
        WRITE setShape RESET resetShape NOTIFY shapeChanged )

    using Inherited = QskAbstractButton;

  public:
    QSK_SUBCONTROLS( Panel, Splash, Text, Icon )

    enum Emphasis : qint8
    {
        VeryLowEmphasis  = -2,
        LowEmphasis      = -1,
        NoEmphasis       = 0,
        HighEmphasis     = 1,
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

    void setIconStrutSize( const QSizeF& );
    QSizeF iconStrutSize() const;
    void resetIconStrutSize();

    QUrl iconSource() const;
    QskGraphic icon() const;
    bool hasIcon() const;

    QFont font() const;

    QRectF layoutRectForSize( const QSizeF& ) const override;
    QskAspect::Variation effectiveVariation() const override;

  public Q_SLOTS:
    void setText( const QString& );
    void setIconSource( const QUrl& );
    void setIconSource( const QString& );
    void setIcon( const QskGraphic& );

  Q_SIGNALS:
    void checkableChanged( bool );
    void emphasisChanged( Emphasis ) const;
    void shapeChanged();

    void textChanged();
    void textOptionsChanged();

    void iconChanged();
    void iconSourceChanged();
    void iconStrutSizeChanged();

  protected:
    void changeEvent( QEvent* ) override;
    void mousePressEvent( QMouseEvent* ) override;

    void updateResources() override;
    virtual QskGraphic loadIcon( const QUrl& ) const;

  private:
    class PrivateData;
    std::unique_ptr< PrivateData > m_data;
};

#endif
