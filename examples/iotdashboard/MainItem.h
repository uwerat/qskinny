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
        enum Edge {
            LeftEdge = Qsk::LeftToRight,
            RightEdge = Qsk::RightToLeft,
            TopEdge = Qsk::TopToBottom,
            BottomEdge = Qsk::BottomToTop,
            NumEdges
        };
        Q_ENUM( Edge )

        enum Position {
            LeftPos = LeftEdge,
            RightPos = RightEdge,
            TopPos = TopEdge,
            BottomPos = BottomEdge,
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

    protected:
        void keyPressEvent( QKeyEvent* event ) override;

    private:
        Position currentPosition() const;
        Position neighbor( const Position position, const Qsk::Direction direction ) const;
        Qsk::Direction direction( const Position from, const Position to ) const;
        void updateEdge( Qsk::Direction direction, Position position );
        void doSwitch( Qsk::Direction direction, Position position );

        Position m_destination;
        Edge m_currentEdge;
        bool m_isIntermediateHop;

        static QPair< Position, Edge > s_neighbors[ NumPositions ][ NumEdges ];
        static Edge s_edgeTransformations[ NumEdges ][ NumEdges ];
};

class MainItem : public QskControl
{
    Q_OBJECT

    public:
        MainItem( QQuickItem* parent = nullptr );

    protected:
        bool eventFilter(QObject *obj, QEvent *event) override final;
        bool gestureFilter( QQuickItem*, QEvent* ) override final;
        void gestureEvent( QskGestureEvent* ) override final;

    private:
        QskLinearBox* m_mainLayout;
        MenuBar* m_menuBar;
        Cube* m_cube;
        QskPanGestureRecognizer m_panRecognizer;
};
