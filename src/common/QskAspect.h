/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_ASPECT_H
#define QSK_ASPECT_H

#include "QskFlags.h"
#include <QMetaObject>
#include <functional>

#ifdef Q_MOC_RUN

#define QSK_NAMESPACE( name ) struct name
#define QSK_ENUM( name ) Q_GADGET Q_ENUM( name )

#else

#define QSK_NAMESPACE( name ) namespace name
#define QSK_ENUM( name )

#endif

QSK_NAMESPACE( QskAspect )
{
    enum Subcontrol : quint16
    {
        Control =           0,
        LastSubcontrol =    ( 1 << 12 ) - 1
    };
    QSK_ENUM( Subcontrol )

    enum Type : quint8
    {
        Flag   = 0,
        Metric = 1,
        Color  = 2,
    };
    QSK_ENUM( Type )

    enum Edge : quint8
    {
        LeftEdge        = 1 << 0,
        TopEdge         = 1 << 1,
        RightEdge       = 1 << 2,
        BottomEdge      = 1 << 3,

        HorizontalEdges = TopEdge | BottomEdge,
        VerticalEdges   = LeftEdge | RightEdge,

        AllEdges        = HorizontalEdges | VerticalEdges
    };
    QSK_ENUM( Edge )

    enum Corner : quint8
    {
        TopLeftCorner     = 1 << 0,
        TopRightCorner    = 1 << 1,
        BottomRightCorner = 1 << 2,
        BottomLeftCorner  = 1 << 3,

        LeftCorners       = TopLeftCorner | BottomLeftCorner,
        RightCorners      = TopRightCorner | BottomRightCorner,
        TopCorners        = TopLeftCorner | TopRightCorner,
        BottomCorners     = BottomLeftCorner | BottomRightCorner,

        AllCorners        = LeftCorners | RightCorners
    };
    QSK_ENUM( Corner )

    enum BoxPrimitive : quint8
    {
        Background =     0,

        RadiusX =        3,
        RadiusY =        4,
        Border =         5,

        Radius  =        7, // RadiusX | RadiusY
    };
    QSK_ENUM( BoxPrimitive )

    enum FlagPrimitive : quint8
    {
        NoFlag,

        Alignment,
        Style,
        SizeMode,
        Decoration,
        GraphicRole,
        FontRole
    };
    QSK_ENUM( FlagPrimitive )

    enum MetricPrimitive : quint8
    {
        NoMetric,

        Size,
        Position,
        MinimumWidth,
        MinimumHeight,
        MaximumWidth,
        MaximumHeight,

        Margin,
        Padding,
        Shadow,

        Spacing
    };
    QSK_ENUM( MetricPrimitive )

    enum ColorPrimitive : quint8
    {
        NoColor,

        TextColor,
        StyleColor,
        LinkColor
    };
    QSK_ENUM( ColorPrimitive )

    enum State : quint16
    {
        NoState =         0,

        FirstSystemState =  1 << 0,
        FirstUserState =    1 << 4,
        LastUserState =     1 << 11,
        LastSystemState =   1 << 15,

        AllStates =   0xFFFF
    };
    QSK_ENUM ( State )

    extern const QMetaObject staticMetaObject;
}

QSK_DECLARE_OPERATORS_FOR_FLAGS( QskAspect::Edge )
QSK_DECLARE_OPERATORS_FOR_FLAGS( QskAspect::Corner )
QSK_DECLARE_OPERATORS_FOR_FLAGS( QskAspect::State )

#undef QSK_NAMESPACE
#undef QSK_ENUM

namespace QskAspect
{
    class QSK_EXPORT Aspect
    {
    public:
        constexpr Aspect();
        constexpr Aspect( Subcontrol );
        constexpr Aspect( Type );
        constexpr Aspect( BoxPrimitive );

        constexpr Aspect( const Aspect& ) = default;
        constexpr Aspect( Aspect&& ) = default;

        Aspect& operator=( const QskAspect::Aspect& ) = default;

        bool operator==( const Aspect& ) const;
        bool operator!=( const Aspect& ) const;

        bool operator<( const Aspect& ) const;

        constexpr Aspect operator|( Subcontrol ) const;
        constexpr Aspect operator|( Type ) const;
        constexpr Aspect operator|( BoxPrimitive ) const;
        constexpr Aspect operator|( Edge ) const;
        constexpr Aspect operator|( Corner ) const;
        constexpr Aspect operator|( FlagPrimitive ) const;
        constexpr Aspect operator|( MetricPrimitive ) const;
        constexpr Aspect operator|( ColorPrimitive ) const;
        constexpr Aspect operator|( State ) const;

        constexpr quint64 value() const;

        bool isAnimator() const;
        void setAnimator( bool on );

        Subcontrol subControl() const;
        void setSubControl( Subcontrol );

        Type type() const;
        void setType( Type );

        State state() const;
        State topState() const;

        void addState( State state );
        void clearState( State state );
        void clearStates();

        bool isBoxPrimitive() const;
        BoxPrimitive boxPrimitive() const;
        void setBoxPrimitive( BoxPrimitive primitive );

        Edge edge() const;
        void setEdge( Edge edge );
        void clearEdge();

        Corner corner() const;
        void setCorner( Corner corner );

        FlagPrimitive flagPrimitive() const;
        ColorPrimitive colorPrimitive() const;
        MetricPrimitive metricPrimitive() const;

        const char* toPrintable() const;

    private:
        constexpr Aspect( Subcontrol, Type, BoxPrimitive );
        constexpr Aspect( uint subControl, uint type, bool isAnimator,
            uint primitive, bool isBoxPrimitive, uint states );

        uint m_subControl : 12;

        uint m_type : 3;
        bool m_isAnimator : 1;

        uint m_reserved1 : 8;

        uint m_primitive : 7;
        bool m_isBoxPrimitive : 1;

        uint m_states : 16;

        uint m_reserved2 : 16;

    } Q_PACKED;

    inline constexpr Aspect::Aspect():
        Aspect( Control, Flag, Background )
    {
    }

    inline constexpr Aspect::Aspect( Subcontrol subControl ):
        Aspect( subControl, Flag, Background )
    {
    }

    inline constexpr Aspect::Aspect( Type type ):
        Aspect( Control, type, Background )
    {
    }

    inline constexpr Aspect::Aspect( BoxPrimitive primitive ):
        Aspect( Control, Flag, primitive )
    {
    }

    inline constexpr Aspect::Aspect( Subcontrol subControl, Type type, BoxPrimitive primitive ):
        Aspect( subControl, type, false, primitive, true, NoState )
    {
    }

    inline constexpr Aspect::Aspect( uint subControl, uint type, bool isAnimator,
            uint primitive, bool isBoxPrimitive, uint states ):
        m_subControl( subControl ),
        m_type( type ),
        m_isAnimator( isAnimator ),
        m_reserved1( 0 ),
        m_primitive( primitive ),
        m_isBoxPrimitive( isBoxPrimitive ),
        m_states( states ),
        m_reserved2( 0 )
    {
    }

    inline bool Aspect::operator==( const Aspect& other ) const
    {
        return value() == other.value();
    }

    inline bool Aspect::operator!=( const Aspect& other ) const
    {
        return value() != other.value();
    }

    inline bool Aspect::operator<( const Aspect& other ) const
    {
        return value() < other.value();
    }

    inline constexpr Aspect Aspect::operator|( Subcontrol subControl ) const
    {
        return Aspect( subControl, m_type, m_isAnimator,
            m_primitive, m_isBoxPrimitive, m_states );
    }

    inline constexpr Aspect Aspect::operator|( Type type ) const
    {
        return Aspect( m_subControl, type, m_isAnimator,
            m_primitive, m_isBoxPrimitive, m_states );
    }

    inline constexpr Aspect Aspect::operator|( BoxPrimitive primitive ) const
    {
        return Aspect( m_subControl, m_type, m_isAnimator,
            ( m_primitive & ~0x7 ) | primitive, true, m_states );
    }

    inline constexpr Aspect Aspect::operator|( Edge edge ) const
    {
        return Aspect( m_subControl, m_type, m_isAnimator,
            m_primitive | ( edge << 3 ), true, m_states );
    }

    inline constexpr Aspect Aspect::operator|( Corner corner ) const
    {
        return operator|( static_cast< Edge >( corner ) );
    }

    inline constexpr Aspect Aspect::operator|( FlagPrimitive primitive ) const
    {
        return Aspect( m_subControl, m_type, m_isAnimator,
            primitive, false, m_states );
    }

    inline constexpr Aspect Aspect::operator|( MetricPrimitive primitive ) const
    {
        return operator|( static_cast< FlagPrimitive >( primitive ) );
    }

    inline constexpr Aspect Aspect::operator|( ColorPrimitive primitive ) const
    {
        return operator|( static_cast< FlagPrimitive >( primitive ) );
    }

    inline constexpr Aspect Aspect::operator|( State state ) const
    {
        return Aspect( m_subControl, m_type, m_isAnimator,
            m_primitive, m_isBoxPrimitive, m_states | state );
    }

    inline constexpr quint64 Aspect::value() const
    {
        return *reinterpret_cast< const quint64* >( this );
    }

    inline bool Aspect::isAnimator() const
    {
        return m_isAnimator;
    }

    inline void Aspect::setAnimator( bool on )
    {
        m_isAnimator = on;
    }

    inline Subcontrol Aspect::subControl() const
    {
        return static_cast< Subcontrol >( m_subControl );
    }

    inline void Aspect::setSubControl( Subcontrol subControl )
    {
        m_subControl = subControl;
    }

    inline Type Aspect::type() const
    {
        return static_cast< Type >( m_type );
    }

    inline void Aspect::setType( Type type )
    {
        m_type = type;
    }

    inline State Aspect::state() const
    {
        return static_cast< State >( m_states );
    }

    inline void Aspect::addState( State state )
    {
        m_states |= state;
    }

    inline void Aspect::clearState( State state )
    {
        m_states &= ~state;
    }

    inline void Aspect::clearStates()
    {
        m_states = 0;
    }

    inline bool Aspect::isBoxPrimitive() const
    {
        return m_isBoxPrimitive;
    }

    inline BoxPrimitive Aspect::boxPrimitive() const
    {
        return static_cast< BoxPrimitive >( m_primitive & 0x7 );
    }

    inline void Aspect::setBoxPrimitive( BoxPrimitive primitive )
    {
        m_isBoxPrimitive = true;
        m_primitive = primitive;
    }

    inline Edge Aspect::edge() const
    {
        if ( m_isBoxPrimitive )
            return static_cast< Edge >( m_primitive >> 3 );

        return static_cast< Edge >( 0 );
    }

    inline void Aspect::setEdge( Edge edge )
    {
        m_isBoxPrimitive = true;
        m_primitive |= ( edge << 3 );
    }

    inline void Aspect::clearEdge()
    {
        if ( m_isBoxPrimitive )
            m_primitive &= ~( AllEdges << 3 );
    }

    inline Corner Aspect::corner() const
    {
        return static_cast< Corner >( edge() );
    }

    inline void Aspect::setCorner( Corner corner )
    {
        setEdge( static_cast< Edge >( corner ) );
    }

    inline FlagPrimitive Aspect::flagPrimitive() const
    {
        if ( m_isBoxPrimitive || ( m_type != Flag ) )
            return NoFlag;

        return static_cast< FlagPrimitive >( m_primitive );
    }

    inline ColorPrimitive Aspect::colorPrimitive() const
    {
        if ( m_isBoxPrimitive || ( m_type != Color ) )
            return NoColor;

        return static_cast< ColorPrimitive >( m_primitive );
    }

    inline MetricPrimitive Aspect::metricPrimitive() const
    {
        if ( m_isBoxPrimitive || ( m_type != Metric ) )
            return NoMetric;

        return static_cast< MetricPrimitive >( m_primitive );
    }

    inline constexpr Aspect operator|( State state, const Aspect& aspect )
    {
        return aspect | state;
    }

    inline constexpr Aspect operator|( Edge edge, const Aspect& aspect )
    {
        return aspect | edge;
    }

    inline constexpr Aspect operator|( Corner corner, const Aspect& aspect )
    {
        return aspect | corner;
    }

    inline constexpr Aspect operator|( Subcontrol subControl, const Aspect& aspect )
    {
        return subControl | aspect;
    }

    inline constexpr Aspect operator|( Type type, const Aspect& aspect )
    {
        return aspect | type;
    }

    inline constexpr Aspect operator|( BoxPrimitive primitive, const Aspect& aspect )
    {
        return aspect | primitive;
    }

    inline constexpr Aspect operator|( Subcontrol subControl, Type type )
    {
        return Aspect( subControl ) | type;
    }

    inline constexpr Aspect operator|( Type type, Subcontrol subControl )
    {
        return subControl | type;
    }

    inline constexpr Aspect operator|( Subcontrol subControl, State state )
    {
        return Aspect( subControl ) | state;
    }

    inline constexpr Aspect operator|( State state, Subcontrol subControl )
    {
        return subControl | state;
    }

    inline constexpr Aspect operator|( Type type, BoxPrimitive primitive )
    {
        return Aspect( type ) | primitive;
    }

    inline constexpr Aspect operator|( BoxPrimitive primitive, Type type )
    {
        return type | primitive;
    }

    inline constexpr Aspect operator|( BoxPrimitive primitive, Edge edge )
    {
        return Aspect( primitive ) | edge;
    }

    inline constexpr Aspect operator|( Edge edge, BoxPrimitive primitive )
    {
        return primitive | edge;
    }

    inline constexpr Aspect operator|( BoxPrimitive primitive, Corner corner )
    {
        return primitive | static_cast< Edge >( corner );
    }

    inline constexpr Aspect operator|( Corner corner, BoxPrimitive primitive )
    {
        return primitive | corner;
    }

    inline constexpr Aspect operator|( Subcontrol subControl, FlagPrimitive primitive )
    {
        return Aspect( subControl ) | primitive;
    }

    inline constexpr Aspect operator|( FlagPrimitive primitive, Subcontrol subControl )
    {
        return subControl | primitive;
    }

    inline constexpr Aspect operator|( Subcontrol subControl, ColorPrimitive primitive )
    {
        return Aspect( subControl ) | primitive;
    }

    inline constexpr Aspect operator|( ColorPrimitive primitive, Subcontrol subControl )
    {
        return subControl | primitive;
    }

    inline constexpr Aspect operator|( Subcontrol subControl, MetricPrimitive primitive )
    {
        return Aspect( subControl ) | primitive;
    }

    inline constexpr Aspect operator|( MetricPrimitive primitive, Subcontrol subControl )
    {
        return subControl | primitive;
    }

    inline constexpr Aspect operator|( Subcontrol subControl, BoxPrimitive primitive )
    {
        return Aspect( subControl ) | primitive;
    }

    inline constexpr Aspect operator|( BoxPrimitive primitive, Subcontrol subControl )
    {
        return subControl | primitive;
    }

    inline constexpr Aspect operator|( Subcontrol subControl, Edge edge )
    {
        return Aspect( subControl ) | edge;
    }

    inline constexpr Aspect operator|( Edge edge, Subcontrol subControl )
    {
        return subControl | edge;
    }

    inline constexpr Aspect operator|( Subcontrol subControl, Corner corner )
    {
        return Aspect( subControl ) | corner;
    }

    inline constexpr Aspect operator|( Corner corner, Subcontrol subControl )
    {
        return subControl | corner;
    }

    QSK_EXPORT State registerState( const QMetaObject*, State, const char* );
    QSK_EXPORT Subcontrol nextSubcontrol( const QMetaObject*, const char* );

    QSK_EXPORT QByteArray subControlName( Subcontrol );
    QSK_EXPORT QVector< QByteArray > subControlNames( const QMetaObject* = nullptr );
    QSK_EXPORT QVector< Subcontrol > subControls( const QMetaObject* );
}

namespace std
{
    template< > struct hash< QskAspect::Aspect >
    {
        constexpr size_t operator()( const QskAspect::Aspect& aspect ) const noexcept
        {
            return aspect.value();
        }
    };
}

Q_DECLARE_TYPEINFO( QskAspect::Aspect, Q_MOVABLE_TYPE );

#ifndef QT_NO_DEBUG_STREAM

class QDebug;

QSK_EXPORT QDebug operator<<( QDebug, const QskAspect::Aspect& );
QSK_EXPORT QDebug operator<<( QDebug, const QskAspect::Type& );
QSK_EXPORT QDebug operator<<( QDebug, const QskAspect::Edge& );
QSK_EXPORT QDebug operator<<( QDebug, const QskAspect::Corner& );
QSK_EXPORT QDebug operator<<( QDebug, const QskAspect::BoxPrimitive& );
QSK_EXPORT QDebug operator<<( QDebug, const QskAspect::FlagPrimitive& );
QSK_EXPORT QDebug operator<<( QDebug, const QskAspect::ColorPrimitive& );
QSK_EXPORT QDebug operator<<( QDebug, const QskAspect::MetricPrimitive& );
QSK_EXPORT QDebug operator<<( QDebug, const QskAspect::Subcontrol& );
QSK_EXPORT QDebug operator<<( QDebug, const QskAspect::State& );

QSK_EXPORT void qskDebugState( QDebug, const QMetaObject*, QskAspect::State );
QSK_EXPORT void qskDebugAspect( QDebug, const QMetaObject*, QskAspect::Aspect );

#endif

#define QSK_SUBCONTROLS( ... ) static const QskAspect::Subcontrol __VA_ARGS__;
#define QSK_STATES( ... ) static const QskAspect::State __VA_ARGS__;

#define QSK_SUBCONTROL( type, name ) \
    const QskAspect::Subcontrol type::name = \
        QskAspect::nextSubcontrol( &type::staticMetaObject, #type "::" #name );

#define QSK_STATE( type, name, value ) \
    const QskAspect::State type::name = \
        QskAspect::registerState( &type::staticMetaObject, value, #type "::" #name );
#endif
