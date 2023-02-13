/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#pragma once

#include <QskControl.h>

class Stroke;
class QskGradient;
class QPainterPath;

class ShapeItem : public QskControl
{
    Q_OBJECT

    Q_PROPERTY( Stroke stroke READ stroke WRITE setStroke
        RESET resetStroke NOTIFY strokeChanged )

    Q_PROPERTY( QskGradient gradient READ gradient WRITE setGradient
        RESET resetGradient NOTIFY gradientChanged )

    Q_PROPERTY( QPainterPath path READ path WRITE setPath
        RESET resetPath NOTIFY pathChanged )

  public:
    ShapeItem( QQuickItem* parent = nullptr );
    ~ShapeItem() override;

    Stroke stroke() const;
    void setStroke( const Stroke& );
    void setStroke( const QColor&, qreal width = 1.0 );
    void resetStroke();

    QskGradient gradient() const;
    void setGradient( const QskGradient& );
    void resetGradient();

    QPainterPath path() const;
    void setPath( const QPainterPath& );
    void resetPath();

  Q_SIGNALS:
    void strokeChanged( const Stroke& );
    void gradientChanged( const QskGradient& );
    void pathChanged( const QPainterPath& );

  protected:
    void updateNode( QSGNode* ) override;

  private:
    class PrivateData;
    std::unique_ptr< PrivateData > m_data;
};
