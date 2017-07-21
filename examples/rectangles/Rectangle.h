/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef _RECTANGLE_H
#define _RECTANGLE_H

#include "QskGlobal.h"
#include "QskControl.h"

class Border;
class QskCorner;
class QskGradient;

class Rectangle : public QskControl
{
    Q_OBJECT

    Q_PROPERTY( QskCorner corner READ corner
        WRITE setCorner NOTIFY cornerChanged )

    Q_PROPERTY( Border border READ border
        WRITE setBorder NOTIFY borderChanged FINAL )

    Q_PROPERTY( QColor color READ color WRITE setColor NOTIFY colorChanged FINAL )

    Q_PROPERTY( QskGradient gradient READ gradient
        WRITE setGradient RESET resetGradient NOTIFY colorChanged FINAL )

    using Inherited = QskControl;

public:
    Rectangle( QQuickItem* parent = nullptr );
    virtual ~Rectangle();

    void setCorner( const QskCorner& );
    const QskCorner& corner() const;

    void setBorder( const Border& );
    const Border& border() const;

    void setBorderWidth( int );
    int borderWidth() const;

    void setBorderColor( const QColor& );
    QColor borderColor() const;

    void setColor( const QColor& );
    QColor color() const;

    QskGradient gradient() const;
    void setGradient( const QskGradient& gradient );
    void resetGradient();

Q_SIGNALS:
    void cornerChanged();
    void borderChanged();

    void colorChanged();
    void gradientChanged();

protected:
    virtual void updateNode( QSGNode* ) override;

private:
    class PrivateData;
    std::unique_ptr< PrivateData > m_data;
};

#endif
