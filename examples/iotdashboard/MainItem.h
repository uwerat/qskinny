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
            LeftPos,
            RightPos,
            TopPos,
            BottomPos,
            FrontPos,
            BackPos,
            NumPositions
        };
        Q_ENUM( Position )

        explicit Cube( QQuickItem* parent = nullptr );

    public Q_SLOTS:
        void switchPosition( const Qsk::Direction direction );
        void switchToPosition( const Cube::Position position );

    Q_SIGNALS:
        // might be different from indexChanged:
        void cubeIndexChanged( const int index );

    private:
        Position neighbor( const Position position, const Qsk::Direction direction ) const;
        Qsk::Direction direction( const Position from, const Position to ) const;
        void doSwitch( Qsk::Direction direction, Position position );

        Position m_currentPosition;
        Position m_previousPosition;

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
