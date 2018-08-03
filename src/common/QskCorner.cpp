/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskCorner.h"

QskCorner::QskCorner()
{
}

void QskCorner::setRadius( qreal radius )
{
    m_metrics.setRadius( radius );
}

qreal QskCorner::radius() const
{
    return m_metrics.radius( Qt::TopLeftCorner ).width();
}

void QskCorner::setSizeMode( Qt::SizeMode mode )
{
    m_metrics.setSizeMode( mode );
}

Qt::SizeMode QskCorner::sizeMode() const
{
    return m_metrics.sizeMode();
}

void QskCorner::setAspectRatioMode( Qt::AspectRatioMode mode )
{
    m_metrics.setAspectRatioMode( mode );
}

Qt::AspectRatioMode QskCorner::aspectRatioMode() const
{
    return m_metrics.aspectRatioMode();
}

#include "moc_QskCorner.cpp"
