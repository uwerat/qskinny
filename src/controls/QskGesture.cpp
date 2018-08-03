/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskGesture.h"

QskGesture::QskGesture( Type type )
    : m_type( type )
    , m_state( NoGesture )
{
}

QskGesture::~QskGesture()
{
}

void QskGesture::setState( State state )
{
    m_state = state;
}

QskTapGesture::QskTapGesture()
    : QskGesture( Tap )
{
}

QskTapGesture::~QskTapGesture()
{
}

void QskTapGesture::setPosition( const QPointF& position )
{
    m_position = position;
}

QskTapAndHoldGesture::QskTapAndHoldGesture()
    : QskGesture( TapAndHold )
    , m_timeout( 0 )
{
}

QskTapAndHoldGesture::~QskTapAndHoldGesture()
{
}

void QskTapAndHoldGesture::setPosition( const QPointF& position )
{
    m_position = position;
}

void QskTapAndHoldGesture::setTimeout( int timeout )
{
    m_timeout = timeout;
}

QskPanGesture::QskPanGesture()
    : QskGesture( Pan )
    , m_angle( 0.0 )
    , m_velocity( 0.0 )
{
}

QskPanGesture::~QskPanGesture()
{
}

void QskPanGesture::setVelocity( qreal velocity )
{
    m_velocity = velocity;
}

void QskPanGesture::setAngle( qreal degrees )
{
    m_angle = degrees;
}

void QskPanGesture::setOrigin( const QPointF& pos )
{
    m_origin = pos;
}

void QskPanGesture::setLastPosition( const QPointF& pos )
{
    m_lastPosition = pos;
}

void QskPanGesture::setPosition( const QPointF& pos )
{
    m_position = pos;
}

QskSwipeGesture::QskSwipeGesture()
    : QskGesture( Swipe )
    , m_velocity( 0.0 )
    , m_angle( 0.0 )
{
}

QskSwipeGesture::~QskSwipeGesture()
{
}

void QskSwipeGesture::setVelocity( qreal velocity )
{
    m_velocity = velocity;
}

void QskSwipeGesture::setAngle( qreal angle )
{
    m_angle = angle;
}

#include "moc_QskGesture.cpp"
