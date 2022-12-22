#pragma once

#include <QskControl.h>
#include <QskPanGestureRecognizer.h>
#include <QskStackBox.h>

#include <QQuickWindow>

class MenuBar;
class QskBox;
class QskLinearBox;

class Cube : public QskStackBox
{
    Q_OBJECT

    public:
        enum {
            Left,
            Front,
            Right,
            Back,
            Top,
            Bottom
        } Position;

        explicit Cube( QQuickItem* parent = nullptr );
        void startAnimation( Qsk::Direction direction );
};

class MainItem : public QskControl
{
    Q_OBJECT

    public:
        MainItem( QQuickItem* parent = nullptr );

    protected:
        bool gestureFilter( QQuickItem*, QEvent* ) override final;
        void gestureEvent( QskGestureEvent* ) override final;

    private:
        void switchToPage( const int index );

        MenuBar* m_mainMenuBar;
        MenuBar* m_otherMenuBar;
        Cube* m_cube;
        QskLinearBox* m_mainLayout;
        QskLinearBox* m_otherLayout;
        QskPanGestureRecognizer m_panRecognizer;
        int m_currentIndex;
};
