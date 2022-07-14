#pragma once

#include <QskControl.h>
#include <QskPanGestureRecognizer.h>

#include <QQuickWindow>

class QskBox;
class QskLinearBox;
class QskStackBox;

class MainItem : public QskControl
{
    Q_OBJECT

    public:
        MainItem( QQuickItem* parent = nullptr );

    protected:
        bool gestureFilter( QQuickItem*, QEvent* ) override final;
        void gestureEvent( QskGestureEvent* ) override final;

    private:
        QskStackBox* m_cube;
        QskLinearBox* m_mainLayout;
        QskLinearBox* m_otherLayout;
        QskPanGestureRecognizer m_panRecognizer;
};
