/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_PAN_GESTURE_RECOGNIZER_H
#define QSK_PAN_GESTURE_RECOGNIZER_H

#include "QskGestureRecognizer.h"
#include <memory>

class QSK_EXPORT QskPanGestureRecognizer : public QskGestureRecognizer
{
    using Inherited = QskGestureRecognizer;

  public:
    QskPanGestureRecognizer();
    ~QskPanGestureRecognizer() override;

    void setMinDistance( int pixels );
    int minDistance() const;

    void setOrientations( Qt::Orientations );
    Qt::Orientations orientations() const;

  private:
    void pressEvent( const QMouseEvent* ) override;
    void moveEvent( const QMouseEvent* ) override;
    void releaseEvent( const QMouseEvent* ) override;

    class PrivateData;
    std::unique_ptr< PrivateData > m_data;
};

#endif
