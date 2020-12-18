/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_ASPECT_H
#define QSK_ASPECT_H

#include "QskGlobal.h"
#include "QskFlags.h"

#include <qmetaobject.h>
#include <qnamespace.h>
#include <functional>

/*
    hack to run moc over a namespace, what is not
    yet supported with Qt 5.6
 */

#ifdef Q_MOC_RUN

#define QSK_NAMESPACE( name ) struct name
#define QSK_ENUM( name ) Q_GADGET Q_ENUM( name )

#else

#define QSK_NAMESPACE( name ) namespace name
#define QSK_ENUM( name ) \
    inline constexpr const QMetaObject* qt_getEnumMetaObject(name) noexcept { return &staticMetaObject; } \
    inline constexpr const char* qt_getEnumName(name) noexcept { return #name; }

#endif

QSK_NAMESPACE( QskAspect )
{
    extern QSK_EXPORT const QMetaObject staticMetaObject;

    enum Subcontrol : quint16
    {
        Control        = 0,
        LastSubcontrol = ( 1 << 12 ) - 1
    };
    QSK_ENUM( Subcontrol )

    enum Type : quint8
    {
        Flag   = 0,
        Metric = 1,
        Color  = 2,
    };
    QSK_ENUM( Type )

    constexpr uint typeCount = 3;

    enum FlagPrimitive : quint8
    {
        NoFlagPrimitive,

        Alignment,
        Direction,
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

        StrutSize,

        Size,
        Position,

        Margin,
        Padding,
        Shadow,

        Spacing,

        Shape,
        Border
    };
    QSK_ENUM( MetricPrimitive )

    enum ColorPrimitive : quint8
    {
        NoColorPrimitive,

        TextColor,
        StyleColor,
        LinkColor
    };
    QSK_ENUM( ColorPrimitive )

    enum { LastPrimitive = 15 }; // max. value for all sort of primitives

    enum Placement : quint8
    {
        NoPlacement = 0,

        Vertical = Qt::Vertical,
        Horizontal = Qt::Horizontal,

        Top    = 1,
        Left   = 2,
        Right  = 3,
        Bottom = 4
    };
    QSK_ENUM( Placement )

    enum State : quint16
    {
        NoState          = 0,

        FirstSystemState = 1 << 0,
        FirstUserState   = 1 << 4,
        LastUserState    = 1 << 11,
        LastSystemState  = 1 << 15,

        AllUserStates    =   0x0FF0,
        AllStates        =   0xFFFF
    };
    QSK_ENUM( State )
}

QSK_DECLARE_OPERATORS_FOR_FLAGS( QskAspect::State )

#undef QSK_NAMESPACE
#undef QSK_ENUM

namespace QskAspect
{
    class QSK_EXPORT Aspect
    {
      public:
        constexpr Aspect() noexcept;
        constexpr Aspect( Subcontrol ) noexcept;
        constexpr Aspect( Type ) noexcept;
        constexpr Aspect( Placement ) noexcept;

        constexpr Aspect( const Aspect& ) noexcept = default;
        constexpr Aspect( Aspect&& ) noexcept = default;

        Aspect& operator=( const QskAspect::Aspect& ) noexcept = default;

        bool operator==( const Aspect& ) const noexcept;
        bool operator!=( const Aspect& ) const noexcept;

        bool operator<( const Aspect& ) const noexcept;

        constexpr Aspect operator|( Subcontrol ) const noexcept;
        constexpr Aspect operator|( Type ) const noexcept;
        constexpr Aspect operator|( FlagPrimitive ) const noexcept;
        constexpr Aspect operator|( MetricPrimitive ) const noexcept;
        constexpr Aspect operator|( ColorPrimitive ) const noexcept;
        constexpr Aspect operator|( Placement ) const noexcept;
        constexpr Aspect operator|( State ) const noexcept;

        constexpr quint64 value() const noexcept;

        constexpr bool isAnimator() const noexcept;
        void setAnimator( bool on ) noexcept;

        constexpr bool isMetric() const noexcept;
        constexpr bool isColor() const noexcept;
        constexpr bool isFlag() const noexcept;

        constexpr Subcontrol subControl() const noexcept;
        void setSubControl( Subcontrol ) noexcept;

        constexpr Type type() const noexcept;
        void setType( Type ) noexcept;

        constexpr Placement placement() const noexcept;
        void setPlacement( Placement ) noexcept;

        constexpr State state() const noexcept;
        State topState() const noexcept;

        void addState( State ) noexcept;
        void clearState( State ) noexcept;
        void clearStates() noexcept;

        constexpr FlagPrimitive flagPrimitive() const noexcept;
        constexpr ColorPrimitive colorPrimitive() const noexcept;
        constexpr MetricPrimitive metricPrimitive() const noexcept;

        void setPrimitive( Type, uint primitive ) noexcept;
        constexpr uint primitive() const noexcept;
        void clearPrimitive() noexcept;

        const char* toPrintable() const;

      private:
        constexpr Aspect( Subcontrol, Type, Placement ) noexcept;

        constexpr Aspect( uint subControl, uint type, bool isAnimator,
            uint primitive, uint placement, uint states ) noexcept;

        struct Bits
        {
            uint subControl : 12;

            uint type : 3;
            uint isAnimator : 1;

            uint primitive : 5;
            uint placement : 3;
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

    inline constexpr Aspect::Aspect() noexcept
        : Aspect( Control, Flag, NoPlacement )
    {
    }

    inline constexpr Aspect::Aspect( Subcontrol subControl ) noexcept
        : Aspect( subControl, Flag, NoPlacement )
    {
    }

    inline constexpr Aspect::Aspect( Type type ) noexcept
        : Aspect( Control, type, NoPlacement )
    {
    }

    inline constexpr Aspect::Aspect( Placement placement ) noexcept
        : Aspect( Control, Flag, placement )
    {
    }

    inline constexpr Aspect::Aspect(
            Subcontrol subControl, Type type, Placement placement ) noexcept
        : Aspect( subControl, type, false, 0, placement, NoState )
    {
    }

    inline constexpr Aspect::Aspect( uint subControl, uint type, bool isAnimator,
            uint primitive, uint placement, uint states ) noexcept
        : m_bits { subControl, type, isAnimator, primitive, placement, 0, states, 0 }
    {
    }

    inline bool Aspect::operator==( const Aspect& other ) const noexcept
    {
        return m_value == other.m_value;
    }

    inline bool Aspect::operator!=( const Aspect& other ) const noexcept
    {
        return m_value != other.m_value;
    }

    inline bool Aspect::operator<( const Aspect& other ) const noexcept
    {
        return m_value < other.m_value;
    }

    inline constexpr Aspect Aspect::operator|( Subcontrol subControl ) const noexcept
    {
        return Aspect( subControl, m_bits.type, m_bits.isAnimator,
            m_bits.primitive, m_bits.placement, m_bits.states );
    }

    inline constexpr Aspect Aspect::operator|( Type type ) const noexcept
    {
        return Aspect( m_bits.subControl, type, m_bits.isAnimator,
            m_bits.primitive, m_bits.placement, m_bits.states );
    }

    inline constexpr Aspect Aspect::operator|( FlagPrimitive primitive ) const noexcept
    {
        return Aspect( m_bits.subControl, m_bits.type, m_bits.isAnimator,
            primitive, m_bits.placement, m_bits.states );
    }

    inline constexpr Aspect Aspect::operator|( MetricPrimitive primitive ) const noexcept
    {
        return operator|( static_cast< FlagPrimitive >( primitive ) );
    }

    inline constexpr Aspect Aspect::operator|( ColorPrimitive primitive ) const noexcept
    {
        return operator|( static_cast< FlagPrimitive >( primitive ) );
    }

    inline constexpr Aspect Aspect::operator|( Placement placement ) const noexcept
    {
        return Aspect( m_bits.subControl, m_bits.type, m_bits.isAnimator,
            m_bits.primitive, placement, m_bits.states );
    }

    inline constexpr Aspect Aspect::operator|( State state ) const noexcept
    {
        return Aspect( m_bits.subControl, m_bits.type, m_bits.isAnimator,
            m_bits.primitive, m_bits.placement, m_bits.states | state );
    }

    inline constexpr quint64 Aspect::value() const noexcept
    {
        return m_value;
    }

    inline constexpr bool Aspect::isAnimator() const noexcept
    {
        return m_bits.isAnimator;
    }

    inline void Aspect::setAnimator( bool on ) noexcept
    {
        m_bits.isAnimator = on;
    }

    inline constexpr Subcontrol Aspect::subControl() const noexcept
    {
        return static_cast< Subcontrol >( m_bits.subControl );
    }

    inline void Aspect::setSubControl( Subcontrol subControl ) noexcept
    {
        m_bits.subControl = subControl;
    }

    inline constexpr Type Aspect::type() const noexcept
    {
        return static_cast< Type >( m_bits.type );
    }

    inline void Aspect::setType( Type type ) noexcept
    {
        m_bits.type = type;
    }

    inline constexpr bool Aspect::isMetric() const noexcept
    {
        return type() == Metric;
    }

    inline constexpr bool Aspect::isColor() const noexcept
    {
        return type() == Color;
    }

    inline constexpr bool Aspect::isFlag() const noexcept
    {
        return type() == Flag;
    }

    inline constexpr State Aspect::state() const noexcept
    {
        return static_cast< State >( m_bits.states );
    }

    inline void Aspect::addState( State state ) noexcept
    {
        m_bits.states |= state;
    }

    inline void Aspect::clearState( State state ) noexcept
    {
        m_bits.states &= ~state;
    }

    inline void Aspect::clearStates() noexcept
    {
        m_bits.states = 0;
    }

    inline constexpr FlagPrimitive Aspect::flagPrimitive() const noexcept
    {
        return ( m_bits.type == Flag )
            ?  static_cast< FlagPrimitive >( m_bits.primitive )
            : NoFlagPrimitive;
    }

    inline constexpr ColorPrimitive Aspect::colorPrimitive() const noexcept
    {
        return ( m_bits.type == Color )
            ?  static_cast< ColorPrimitive >( m_bits.primitive )
            : NoColorPrimitive;
    }

    inline constexpr MetricPrimitive Aspect::metricPrimitive() const noexcept
    {
        return ( m_bits.type == Metric )
            ? static_cast< MetricPrimitive >( m_bits.primitive )
            : NoMetricPrimitive;
    }

    inline constexpr uint Aspect::primitive() const noexcept
    {
        return m_bits.primitive;
    }

    inline void Aspect::setPrimitive( Type type, uint primitive ) noexcept
    {
        m_bits.type = type;
        m_bits.primitive = primitive;
    }

    inline void Aspect::clearPrimitive() noexcept
    {
        m_bits.primitive = 0;
    }

    inline constexpr Placement Aspect::placement() const noexcept
    {
        return static_cast< Placement >( m_bits.placement );
    }

    inline void Aspect::setPlacement( Placement placement ) noexcept
    {
        m_bits.placement = placement;
    }

    inline constexpr Aspect operator|(
        State state, const Aspect& aspect ) noexcept
    {
        return aspect | state;
    }

    inline constexpr Aspect operator|(
        Subcontrol subControl, const Aspect& aspect ) noexcept
    {
        return aspect | subControl;
    }

    inline constexpr Aspect operator|(
        Type type, const Aspect& aspect ) noexcept
    {
        return aspect | type;
    }

    inline constexpr Aspect operator|(
        Placement placement, const Aspect& aspect ) noexcept
    {
        return aspect | placement;
    }

    inline constexpr Aspect operator|(
        Subcontrol subControl, Type type ) noexcept
    {
        return Aspect( subControl ) | type;
    }

    inline constexpr Aspect operator|( Type type, Subcontrol subControl ) noexcept
    {
        return subControl | type;
    }

    inline constexpr Aspect operator|( Subcontrol subControl, State state ) noexcept
    {
        return Aspect( subControl ) | state;
    }

    inline constexpr Aspect operator|( Type type, Placement placement ) noexcept
    {
        return Aspect( type ) | placement;
    }

    inline constexpr Aspect operator|( Placement placement, Type type ) noexcept
    {
        return type | placement;
    }

    inline constexpr Aspect operator|( State state, Subcontrol subControl ) noexcept
    {
        return subControl | state;
    }

    inline constexpr Aspect operator|(
        Subcontrol subControl, FlagPrimitive primitive ) noexcept
    {
        return Aspect( subControl ) | primitive;
    }

    inline constexpr Aspect operator|(
        FlagPrimitive primitive, Subcontrol subControl ) noexcept
    {
        return subControl | primitive;
    }

    inline constexpr Aspect operator|(
        Subcontrol subControl, ColorPrimitive primitive ) noexcept
    {
        return Aspect( subControl ) | primitive;
    }

    inline constexpr Aspect operator|(
        ColorPrimitive primitive, Subcontrol subControl ) noexcept
    {
        return subControl | primitive;
    }

    inline constexpr Aspect operator|(
        Subcontrol subControl, MetricPrimitive primitive ) noexcept
    {
        return Aspect( subControl ) | primitive;
    }

    inline constexpr Aspect operator|(
        MetricPrimitive primitive, Subcontrol subControl ) noexcept
    {
        return subControl | primitive;
    }

    inline constexpr Aspect operator|(
        Subcontrol subControl, Placement placement ) noexcept
    {
        return Aspect( subControl ) | placement;
    }

    inline constexpr Aspect operator|(
        Placement placement, Subcontrol subControl ) noexcept
    {
        return subControl | placement;
    }

    QSK_EXPORT State registerState( const QMetaObject*, State, const char* );
    QSK_EXPORT Subcontrol nextSubcontrol( const QMetaObject*, const char* );

    QSK_EXPORT QByteArray subControlName( Subcontrol );
    QSK_EXPORT QVector< QByteArray > subControlNames( const QMetaObject* = nullptr );
    QSK_EXPORT QVector< Subcontrol > subControls( const QMetaObject* );

    QSK_EXPORT quint8 primitiveCount( Type );
    QSK_EXPORT void reservePrimitives( Type, quint8 count );

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

QSK_EXPORT QDebug operator<<( QDebug, QskAspect::Aspect );
QSK_EXPORT QDebug operator<<( QDebug, QskAspect::Type );
QSK_EXPORT QDebug operator<<( QDebug, QskAspect::Subcontrol );

QSK_EXPORT QDebug operator<<( QDebug, QskAspect::FlagPrimitive );
QSK_EXPORT QDebug operator<<( QDebug, QskAspect::ColorPrimitive );
QSK_EXPORT QDebug operator<<( QDebug, QskAspect::MetricPrimitive );

QSK_EXPORT QDebug operator<<( QDebug, QskAspect::Placement );
QSK_EXPORT QDebug operator<<( QDebug, QskAspect::State );

QSK_EXPORT void qskDebugState( QDebug, const QMetaObject*, QskAspect::State );
QSK_EXPORT void qskDebugAspect( QDebug, const QMetaObject*, QskAspect::Aspect );

#endif

#define QSK_SUBCONTROL( type, name ) \
    const QskAspect::Subcontrol type::name = \
        QskAspect::nextSubcontrol( &type::staticMetaObject, #type "::" #name );

#define QSK_STATE( type, name, value ) \
    static_assert( \
        ( value >= QskAspect::FirstUserState ) && ( value <= QskAspect::LastUserState ), \
        "Invalid state" \
    ); \
    const QskAspect::State type::name = \
        QskAspect::registerState( &type::staticMetaObject, value, #type "::" #name );

#define QSK_SYSTEM_STATE( type, name, value ) \
    const QskAspect::State type::name = \
        QskAspect::registerState( &type::staticMetaObject, value, #type "::" #name );

#if !defined( _MSC_VER )

#define QSK_SUBCONTROLS( ... ) static const QskAspect::Subcontrol __VA_ARGS__;
#define QSK_STATES( ... ) static const QskAspect::State __VA_ARGS__;

#else

/*
    Working around a MSVC bug: when static member are defined in one statement
    only the first one is exported. Unfortuately the code below is also not
    compliant with ISO C++11 and gcc -pedantic f.e. would fail.
 */

#define _QSK_EXPAND(x) x

#define _QSK_EVAL_0( m, x, ... ) m(x)
#define _QSK_EVAL_1( m, x, ... ) m(x) _QSK_EXPAND( _QSK_EVAL_0( m, __VA_ARGS__ ) )
#define _QSK_EVAL_2( m, x, ... ) m(x) _QSK_EXPAND( _QSK_EVAL_1( m, __VA_ARGS__ ) )
#define _QSK_EVAL_3( m, x, ... ) m(x) _QSK_EXPAND( _QSK_EVAL_2( m, __VA_ARGS__ ) )
#define _QSK_EVAL_4( m, x, ... ) m(x) _QSK_EXPAND( _QSK_EVAL_3( m, __VA_ARGS__ ) )
#define _QSK_EVAL_5( m, x, ... ) m(x) _QSK_EXPAND( _QSK_EVAL_4( m, __VA_ARGS__ ) )
#define _QSK_EVAL_6( m, x, ... ) m(x) _QSK_EXPAND( _QSK_EVAL_5( m, __VA_ARGS__ ) )
#define _QSK_EVAL_7( m, x, ... ) m(x) _QSK_EXPAND( _QSK_EVAL_6( m, __VA_ARGS__ ) )
#define _QSK_EVAL_8( m, x, ... ) m(x) _QSK_EXPAND( _QSK_EVAL_7( m, __VA_ARGS__ ) )
#define _QSK_EVAL_9( m, x, ... ) m(x) _QSK_EXPAND( _QSK_EVAL_8( m, __VA_ARGS__ ) )
#define _QSK_EVAL_A( m, x, ... ) m(x) _QSK_EXPAND( _QSK_EVAL_9( m, __VA_ARGS__ ) )
#define _QSK_EVAL_B( m, x, ... ) m(x) _QSK_EXPAND( _QSK_EVAL_A( m, __VA_ARGS__ ) )
#define _QSK_EVAL_C( m, x, ... ) m(x) _QSK_EXPAND( _QSK_EVAL_B( m, __VA_ARGS__ ) )
#define _QSK_EVAL_D( m, x, ... ) m(x) _QSK_EXPAND( _QSK_EVAL_C( m, __VA_ARGS__ ) )
#define _QSK_EVAL_E( m, x, ... ) m(x) _QSK_EXPAND( _QSK_EVAL_D( m, __VA_ARGS__ ) )
#define _QSK_EVAL_F( m, x, ... ) m(x) _QSK_EXPAND( _QSK_EVAL_E( m, __VA_ARGS__ ) )

#define _QSK_EVAL_NARG( ... ) _QSK_EVAL_NARG_( __VA_ARGS__, _QSK_EVAL_RSEQ_N() )
#define _QSK_EVAL_NARG_( ... ) _QSK_EXPAND( _QSK_EVAL_ARG_N(  __VA_ARGS__ ) )

#define _QSK_EVAL_ARG_N( _0, _1, _2, _3, _4, _5, _6, _7, _8, _9, A, B, C, D, E, F, N, ... ) N
#define _QSK_EVAL_RSEQ_N() F, E, D, C, B, A, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0

#define _QSK_CAT(x,y) x ## y
#define _QSK_EVAL_( N, m, ... ) _QSK_EXPAND( _QSK_CAT( _QSK_EVAL_, N )( m, __VA_ARGS__ ) )
#define _QSK_EVAL( m, ... ) _QSK_EVAL_( _QSK_EVAL_NARG( __VA_ARGS__ ), m, __VA_ARGS__ )

#define _QSK_SUBCONTROLS_HELPER( name ) static const QskAspect::Subcontrol name;
#define QSK_SUBCONTROLS( ... ) _QSK_EVAL( _QSK_SUBCONTROLS_HELPER, __VA_ARGS__ )

#define _QSK_STATES_HELPER( name ) static const QskAspect::State name;
#define QSK_STATES( ... ) _QSK_EVAL( _QSK_STATES_HELPER, __VA_ARGS__ )

#endif

#endif
