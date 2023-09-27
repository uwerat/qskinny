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
    enum Edge
    {
        LeftEdge = Qsk::LeftToRight,
        RightEdge = Qsk::RightToLeft,
        TopEdge = Qsk::TopToBottom,
        BottomEdge = Qsk::BottomToTop,
        NumEdges
    };
    Q_ENUM( Edge )

    enum Position
    {
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
    void switchPosition( const Qsk::Direction );
    void switchToPosition( const Cube::Position );

  Q_SIGNALS:
    // might be different from indexChanged:
    void cubeIndexChanged( const int index );

  private:
    Position currentPosition() const;
    Position neighbor( const Position, const Qsk::Direction ) const;
    Qsk::Direction direction( const Position from, const Position to ) const;
    void updateEdge( Qsk::Direction, Position );
    void doSwitch( Qsk::Direction, Position );

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
    void keyPressEvent( QKeyEvent* ) override final;

    bool gestureFilter( const QQuickItem*, const QEvent* ) override final;
    void gestureEvent( QskGestureEvent* ) override final;

  private:
    QskLinearBox* m_mainLayout;
    MenuBar* m_menuBar;
    Cube* m_cube;
    QskPanGestureRecognizer m_panRecognizer;
};
