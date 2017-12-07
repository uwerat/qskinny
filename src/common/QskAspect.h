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

    enum FlagPrimitive : quint8
    {
        NoFlagPrimitive,

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
        NoMetricPrimitive,

        Size,
        Position,
        MinimumWidth,
        MinimumHeight,
        MaximumWidth,
        MaximumHeight,

        Margin,
        Padding,
        Shadow,

        Spacing,

        Shape,
        Border
    };
    QSK_ENUM( MetricPrimitive )

    enum { LastType = Color };   // max. value for all types
    enum { LastPrimitive = 16 }; // max. value for all sort of primitives

    enum ColorPrimitive : quint8
    {
        NoColorPrimitive,

        TextColor,
        StyleColor,
        LinkColor
    };
    QSK_ENUM( ColorPrimitive )

    enum Placement : quint8
    {
        Preserved = 0,
        Transposed = 1
    };
    QSK_ENUM ( Placement )

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
        constexpr Aspect( Placement );

        constexpr Aspect( const Aspect& ) = default;
        constexpr Aspect( Aspect&& ) = default;

        Aspect& operator=( const QskAspect::Aspect& ) = default;

        bool operator==( const Aspect& ) const;
        bool operator!=( const Aspect& ) const;

        bool operator<( const Aspect& ) const;

        constexpr Aspect operator|( Subcontrol ) const;
        constexpr Aspect operator|( Type ) const;
        constexpr Aspect operator|( FlagPrimitive ) const;
        constexpr Aspect operator|( MetricPrimitive ) const;
        constexpr Aspect operator|( ColorPrimitive ) const;
        constexpr Aspect operator|( Placement ) const;
        constexpr Aspect operator|( State ) const;

        constexpr quint64 value() const;

        bool isAnimator() const;
        void setAnimator( bool on );

        Subcontrol subControl() const;
        void setSubControl( Subcontrol );

        Type type() const;
        void setType( Type );

        Placement placement() const;
        void setPlacement( Placement );

        State state() const;
        State topState() const;

        void addState( State state );
        void clearState( State state );
        void clearStates();

        FlagPrimitive flagPrimitive() const;
        ColorPrimitive colorPrimitive() const;
        MetricPrimitive metricPrimitive() const;

        void setPrimitive( Type, uint primitive );
        uint primitive() const;
        void clearPrimitive();

        const char* toPrintable() const;

    private:
        constexpr Aspect( Subcontrol, Type, Placement );
        constexpr Aspect( uint subControl, uint type, bool isAnimator,
            uint primitive, uint placement, uint states );

        struct Bits
        {
            uint subControl : 12;

            uint type : 3;
            bool isAnimator : 1;

            uint primitive : 7;
            uint placement : 1;
            uint reserved1 : 8;

            uint states : 16;
            uint reserved2 : 16;
        };

        union
        {
            Bits m_bits;
            quint64 m_value;
        };
    };

    inline constexpr Aspect::Aspect():
        Aspect( Control, Flag, Preserved )
    {
    }

    inline constexpr Aspect::Aspect( Subcontrol subControl ):
        Aspect( subControl, Flag, Preserved )
    {
    }

    inline constexpr Aspect::Aspect( Type type ):
        Aspect( Control, type, Preserved )
    {
    }

    inline constexpr Aspect::Aspect( Placement placement ):
        Aspect( Control, Flag, placement )
    {
    }

    inline constexpr Aspect::Aspect( Subcontrol subControl, Type type, Placement placement ):
        Aspect( subControl, type, false, 0, placement, NoState )
    {
    }

    inline constexpr Aspect::Aspect( uint subControl, uint type, bool isAnimator,
            uint primitive, uint placement, uint states ):
        m_bits { subControl, type, isAnimator, primitive, placement, 0, states, 0 }
    {
    }

    inline bool Aspect::operator==( const Aspect& other ) const
    {
        return m_value == other.m_value;
    }

    inline bool Aspect::operator!=( const Aspect& other ) const
    {
        return m_value != other.m_value;
    }

    inline bool Aspect::operator<( const Aspect& other ) const
    {
        return m_value < other.m_value;
    }

    inline constexpr Aspect Aspect::operator|( Subcontrol subControl ) const
    {
        return Aspect( subControl, m_bits.type, m_bits.isAnimator,
            m_bits.primitive, m_bits.placement, m_bits.states );
    }

    inline constexpr Aspect Aspect::operator|( Type type ) const
    {
        return Aspect( m_bits.subControl, type, m_bits.isAnimator,
            m_bits.primitive, m_bits.placement,  m_bits.states );
    }

    inline constexpr Aspect Aspect::operator|( FlagPrimitive primitive ) const
    {
        return Aspect( m_bits.subControl, m_bits.type, m_bits.isAnimator,
            primitive, m_bits.placement, m_bits.states );
    }

    inline constexpr Aspect Aspect::operator|( MetricPrimitive primitive ) const
    {
        return operator|( static_cast< FlagPrimitive >( primitive ) );
    }

    inline constexpr Aspect Aspect::operator|( ColorPrimitive primitive ) const
    {
        return operator|( static_cast< FlagPrimitive >( primitive ) );
    }

    inline constexpr Aspect Aspect::operator|( Placement placement ) const
    {
        return Aspect( m_bits.subControl, m_bits.type, m_bits.isAnimator,
            m_bits.primitive, placement, m_bits.states );
    }

    inline constexpr Aspect Aspect::operator|( State state ) const
    {
        return Aspect( m_bits.subControl, m_bits.type, m_bits.isAnimator,
            m_bits.primitive, m_bits.placement, m_bits.states | state );
    }

    inline constexpr quint64 Aspect::value() const
    {
        return m_value;
    }

    inline bool Aspect::isAnimator() const
    {
        return m_bits.isAnimator;
    }

    inline void Aspect::setAnimator( bool on )
    {
        m_bits.isAnimator = on;
    }

    inline Subcontrol Aspect::subControl() const
    {
        return static_cast< Subcontrol >( m_bits.subControl );
    }

    inline void Aspect::setSubControl( Subcontrol subControl )
    {
        m_bits.subControl = subControl;
    }

    inline Type Aspect::type() const
    {
        return static_cast< Type >( m_bits.type );
    }

    inline void Aspect::setType( Type type )
    {
        m_bits.type = type;
    }

    inline State Aspect::state() const
    {
        return static_cast< State >( m_bits.states );
    }

    inline void Aspect::addState( State state )
    {
        m_bits.states |= state;
    }

    inline void Aspect::clearState( State state )
    {
        m_bits.states &= ~state;
    }

    inline void Aspect::clearStates()
    {
        m_bits.states = 0;
    }

    inline FlagPrimitive Aspect::flagPrimitive() const
    {
        if ( m_bits.type != Flag )
            return NoFlagPrimitive;

        return static_cast< FlagPrimitive >( m_bits.primitive );
    }

    inline ColorPrimitive Aspect::colorPrimitive() const
    {
        if ( m_bits.type != Color )
            return NoColorPrimitive;

        return static_cast< ColorPrimitive >( m_bits.primitive );
    }

    inline MetricPrimitive Aspect::metricPrimitive() const
    {
        if ( m_bits.type != Metric )
            return NoMetricPrimitive;

        return static_cast< MetricPrimitive >( m_bits.primitive );
    }

    inline uint Aspect::primitive() const
    {
        return m_bits.primitive;
    }

    inline void Aspect::setPrimitive( Type type, uint primitive )
    {
        m_bits.type = type;
        m_bits.primitive = primitive;
    }

    inline void Aspect::clearPrimitive()
    {
        m_bits.primitive = 0;
    }

    inline Placement Aspect::placement() const
    {
        return static_cast< Placement >( m_bits.placement );
    }

    inline void Aspect::setPlacement( Placement placement )
    {
        m_bits.placement = placement;
    }

    inline constexpr Aspect operator|( State state, const Aspect& aspect )
    {
        return aspect | state;
    }

    inline constexpr Aspect operator|( Subcontrol subControl, const Aspect& aspect )
    {
        return aspect | subControl;
    }

    inline constexpr Aspect operator|( Type type, const Aspect& aspect )
    {
        return aspect | type;
    }

    inline constexpr Aspect operator|( Placement placement, const Aspect& aspect )
    {
        return aspect | placement;
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

    inline constexpr Aspect operator|( Type type, Placement placement )
    {
        return Aspect( type ) | placement;
    }

    inline constexpr Aspect operator|( Placement placement, Type type )
    {
        return type | placement;
    }

    inline constexpr Aspect operator|( State state, Subcontrol subControl )
    {
        return subControl | state;
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

    inline constexpr Aspect operator|( Subcontrol subControl, Placement placement )
    {
        return Aspect( subControl ) | placement;
    }

    inline constexpr Aspect operator|( Placement placement, Subcontrol subControl )
    {
        return subControl | placement;
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
QSK_EXPORT QDebug operator<<( QDebug, const QskAspect::Subcontrol& );

QSK_EXPORT QDebug operator<<( QDebug, const QskAspect::FlagPrimitive& );
QSK_EXPORT QDebug operator<<( QDebug, const QskAspect::ColorPrimitive& );
QSK_EXPORT QDebug operator<<( QDebug, const QskAspect::MetricPrimitive& );

QSK_EXPORT QDebug operator<<( QDebug, const QskAspect::Placement& );
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
