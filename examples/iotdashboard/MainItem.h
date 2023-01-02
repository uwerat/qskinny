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
        enum Position {
            Left,
            Right,
            Top,
            Bottom,
            Front,
            Back,
            NumPositions
        };

        explicit Cube( QQuickItem* parent = nullptr );

    public Q_SLOTS:
        void switchPosition( const Qsk::Direction direction );
        void switchToPosition( const Cube::Position position );

    Q_SIGNALS:
        // might be different from indexChanged:
        void cubeIndexChanged( const int index );

    private:
        Qsk::Direction direction( const Position from, const Position to );
        void startAnimation( Qsk::Direction direction, int position );

        Cube::Position m_currentPosition;

        static Position s_neighbors[ NumPositions ][ 4 ];
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
        QskLinearBox* m_mainLayout;
        MenuBar* m_menuBar;
        Cube* m_cube;
        QskPanGestureRecognizer m_panRecognizer;
};
