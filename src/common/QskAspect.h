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

class QSK_EXPORT QskAspect
{
    Q_GADGET

  public:

    enum Type : quint8
    {
        Flag   = 0,
        Metric = 1,
        Color  = 2,
    };
    Q_ENUM( Type )

    static constexpr uint typeCount = 3;

    enum Primitive : quint8
    {
        NoPrimitive = 0,

        Alignment,
        Style,
        GraphicRole,
        FontRole,

        TextColor,
        StyleColor,
        LinkColor,

        StrutSize,
        Size,
        Position,

        Margin,
        Padding,
        Spacing,

        Shadow,
        Shape,
        Border
    };
    Q_ENUM( Primitive )

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
    Q_ENUM( Placement )

    enum Subcontrol : quint16
    {
        Control        = 0,
        LastSubcontrol = ( 1 << 12 ) - 1
    };

    Q_ENUM( Subcontrol )

    enum State : quint16
    {
        NoState          = 0,

        FirstSystemState = 1 << 0,
        FirstUserState   = 1 << 4,
        LastUserState    = 1 << 11,
        LastSystemState  = 1 << 15,

        AllStates        =   0xFFFF
    };
    Q_ENUM( State )

    constexpr QskAspect() noexcept;
    constexpr QskAspect( Subcontrol ) noexcept;
    constexpr QskAspect( Type ) noexcept;
    constexpr QskAspect( Placement ) noexcept;

    constexpr QskAspect( const QskAspect& ) noexcept = default;
    constexpr QskAspect( QskAspect&& ) noexcept = default;

    QskAspect& operator=( const QskAspect& ) noexcept = default;

    bool operator==( const QskAspect& ) const noexcept;
    bool operator!=( const QskAspect& ) const noexcept;

    bool operator<( const QskAspect& ) const noexcept;

    constexpr QskAspect operator|( Subcontrol ) const noexcept;
    constexpr QskAspect operator|( Type ) const noexcept;
    constexpr QskAspect operator|( Primitive ) const noexcept;
    constexpr QskAspect operator|( Placement ) const noexcept;
    constexpr QskAspect operator|( State ) const noexcept;

    constexpr QskAspect stateless() const noexcept;
    constexpr QskAspect trunk() const noexcept;

    constexpr quint64 value() const noexcept;

    constexpr bool isAnimator() const noexcept;
    void setAnimator( bool on ) noexcept;

    constexpr Subcontrol subControl() const noexcept;
    void setSubControl( Subcontrol ) noexcept;

    constexpr Type type() const noexcept;
    void setType( Type ) noexcept;

    constexpr bool isMetric() const noexcept;
    constexpr bool isColor() const noexcept;
    constexpr bool isFlag() const noexcept;

    constexpr Placement placement() const noexcept;
    void setPlacement( Placement ) noexcept;

    constexpr State state() const noexcept;
    State topState() const noexcept;

    void setState( State ) noexcept;
    void addState( State ) noexcept;
    constexpr bool hasState() const noexcept;
    void clearState( State ) noexcept;
    void clearStates() noexcept;

    constexpr Primitive primitive() const noexcept;
    void setPrimitive( Type, Primitive primitive ) noexcept;
    void clearPrimitive() noexcept;

    constexpr Primitive flagPrimitive() const noexcept;
    constexpr Primitive colorPrimitive() const noexcept;
    constexpr Primitive metricPrimitive() const noexcept;

    const char* toPrintable() const;

    static State registerState( const QMetaObject*, State, const char* );
    static Subcontrol nextSubcontrol( const QMetaObject*, const char* );

    static QByteArray subControlName( Subcontrol );
    static QVector< QByteArray > subControlNames( const QMetaObject* = nullptr );
    static QVector< Subcontrol > subControls( const QMetaObject* );

    static quint8 primitiveCount();
    static void reservePrimitives( quint8 count );

  private:
    constexpr QskAspect( Subcontrol, Type, Placement ) noexcept;

    constexpr QskAspect( uint subControl, uint type, bool isAnimator,
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

QSK_DECLARE_OPERATORS_FOR_FLAGS( QskAspect::State )

inline constexpr QskAspect::QskAspect() noexcept
    : QskAspect( Control, Flag, NoPlacement )
{
}

inline constexpr QskAspect::QskAspect( Subcontrol subControl ) noexcept
    : QskAspect( subControl, Flag, NoPlacement )
{
}

inline constexpr QskAspect::QskAspect( Type type ) noexcept
    : QskAspect( Control, type, NoPlacement )
{
}

inline constexpr QskAspect::QskAspect( Placement placement ) noexcept
    : QskAspect( Control, Flag, placement )
{
}

inline constexpr QskAspect::QskAspect(
        Subcontrol subControl, Type type, Placement placement ) noexcept
    : QskAspect( subControl, type, false, 0, placement, NoState )
{
}

inline constexpr QskAspect::QskAspect( uint subControl, uint type, bool isAnimator,
        uint primitive, uint placement, uint states ) noexcept
    : m_bits { subControl, type, isAnimator, primitive, placement, 0, states, 0 }
{
}

inline bool QskAspect::operator==( const QskAspect& other ) const noexcept
{
    return m_value == other.m_value;
}

inline bool QskAspect::operator!=( const QskAspect& other ) const noexcept
{
    return m_value != other.m_value;
}

inline bool QskAspect::operator<( const QskAspect& other ) const noexcept
{
    return m_value < other.m_value;
}

inline constexpr QskAspect QskAspect::operator|( Subcontrol subControl ) const noexcept
{
    return QskAspect( subControl, m_bits.type, m_bits.isAnimator,
        m_bits.primitive, m_bits.placement, m_bits.states );
}

inline constexpr QskAspect QskAspect::operator|( Type type ) const noexcept
{
    return QskAspect( m_bits.subControl, type, m_bits.isAnimator,
        m_bits.primitive, m_bits.placement, m_bits.states );
}

inline constexpr QskAspect QskAspect::operator|( Primitive primitive ) const noexcept
{
    return QskAspect( m_bits.subControl, m_bits.type, m_bits.isAnimator,
        primitive, m_bits.placement, m_bits.states );
}

inline constexpr QskAspect QskAspect::operator|( Placement placement ) const noexcept
{
    return QskAspect( m_bits.subControl, m_bits.type, m_bits.isAnimator,
        m_bits.primitive, placement, m_bits.states );
}

inline constexpr QskAspect QskAspect::operator|( State state ) const noexcept
{
    return QskAspect( m_bits.subControl, m_bits.type, m_bits.isAnimator,
        m_bits.primitive, m_bits.placement, m_bits.states | state );
}

inline constexpr QskAspect QskAspect::stateless() const noexcept
{
    return QskAspect( m_bits.subControl, m_bits.type, m_bits.isAnimator,
        m_bits.primitive, m_bits.placement, 0 );
}

inline constexpr QskAspect QskAspect::trunk() const noexcept
{
    return QskAspect( m_bits.subControl, m_bits.type, m_bits.isAnimator,
        m_bits.primitive, 0, 0 );
}

inline constexpr quint64 QskAspect::value() const noexcept
{
    return m_value;
}

inline constexpr bool QskAspect::isAnimator() const noexcept
{
    return m_bits.isAnimator;
}

inline void QskAspect::setAnimator( bool on ) noexcept
{
    m_bits.isAnimator = on;
}

inline constexpr QskAspect::Subcontrol QskAspect::subControl() const noexcept
{
    return static_cast< Subcontrol >( m_bits.subControl );
}

inline void QskAspect::setSubControl( Subcontrol subControl ) noexcept
{
    m_bits.subControl = subControl;
}

inline constexpr QskAspect::Type QskAspect::type() const noexcept
{
    return static_cast< Type >( m_bits.type );
}

inline void QskAspect::setType( Type type ) noexcept
{
    m_bits.type = type;
}

inline constexpr bool QskAspect::isMetric() const noexcept
{
    return type() == Metric;
}

inline constexpr bool QskAspect::isColor() const noexcept
{
    return type() == Color;
}

inline constexpr bool QskAspect::isFlag() const noexcept
{
    return type() == Flag;
}

inline constexpr QskAspect::State QskAspect::state() const noexcept
{
    return static_cast< State >( m_bits.states );
}

inline void QskAspect::setState( State state ) noexcept
{
    m_bits.states = state;
}

inline void QskAspect::addState( State state ) noexcept
{
    m_bits.states |= state;
}

inline constexpr bool QskAspect::hasState() const noexcept
{
    return m_bits.states;
}

inline void QskAspect::clearState( State state ) noexcept
{
    m_bits.states &= ~state;
}

inline void QskAspect::clearStates() noexcept
{
    m_bits.states = 0;
}

inline constexpr QskAspect::Primitive QskAspect::primitive() const noexcept
{
    return static_cast< QskAspect::Primitive >( m_bits.primitive );
}

inline void QskAspect::setPrimitive( Type type, QskAspect::Primitive primitive ) noexcept
{
    m_bits.type = type;
    m_bits.primitive = primitive;
}

inline constexpr QskAspect::Primitive QskAspect::flagPrimitive() const noexcept
{
    return ( m_bits.type == Flag )
        ?  static_cast< Primitive >( m_bits.primitive ) : NoPrimitive;
}

inline constexpr QskAspect::Primitive QskAspect::colorPrimitive() const noexcept
{
    return ( m_bits.type == Color )
        ?  static_cast< Primitive >( m_bits.primitive ) : NoPrimitive;
}

inline constexpr QskAspect::Primitive QskAspect::metricPrimitive() const noexcept
{
    return ( m_bits.type == Metric )
        ? static_cast< Primitive >( m_bits.primitive ) : NoPrimitive;
}

inline void QskAspect::clearPrimitive() noexcept
{
    m_bits.primitive = NoPrimitive;
}

inline constexpr QskAspect::Placement QskAspect::placement() const noexcept
{
    return static_cast< Placement >( m_bits.placement );
}

inline void QskAspect::setPlacement( Placement placement ) noexcept
{
    m_bits.placement = placement;
}

inline constexpr QskAspect operator|(
    QskAspect::State state, const QskAspect& aspect ) noexcept
{
    return aspect | state;
}

inline constexpr QskAspect operator|(
    QskAspect::Subcontrol subControl, const QskAspect& aspect ) noexcept
{
    return aspect | subControl;
}

inline constexpr QskAspect operator|(
    QskAspect::Type type, const QskAspect& aspect ) noexcept
{
    return aspect | type;
}

inline constexpr QskAspect operator|(
    QskAspect::Placement placement, const QskAspect& aspect ) noexcept
{
    return aspect | placement;
}

inline constexpr QskAspect operator|(
    QskAspect::Subcontrol subControl, QskAspect::Type type ) noexcept
{
    return QskAspect( subControl ) | type;
}

inline constexpr QskAspect operator|(
    QskAspect::Type type, QskAspect::Subcontrol subControl ) noexcept
{
    return subControl | type;
}

inline constexpr QskAspect operator|(
    QskAspect::Subcontrol subControl, QskAspect::State state ) noexcept
{
    return QskAspect( subControl ) | state;
}

inline constexpr QskAspect operator|(
    QskAspect::Type type, QskAspect::Placement placement ) noexcept
{
    return QskAspect( type ) | placement;
}

inline constexpr QskAspect operator|(
    QskAspect::Placement placement, QskAspect::Type type ) noexcept
{
    return type | placement;
}

inline constexpr QskAspect operator|(
    QskAspect::State state, QskAspect::Subcontrol subControl ) noexcept
{
    return subControl | state;
}

inline constexpr QskAspect operator|(
    QskAspect::Subcontrol subControl, QskAspect::Primitive primitive ) noexcept
{
    return QskAspect( subControl ) | primitive;
}

inline constexpr QskAspect operator|(
    QskAspect::Primitive primitive, QskAspect::Subcontrol subControl ) noexcept
{
    return subControl | primitive;
}

inline constexpr QskAspect operator|(
    QskAspect::Subcontrol subControl, QskAspect::Placement placement ) noexcept
{
    return QskAspect( subControl ) | placement;
}

inline constexpr QskAspect operator|(
    QskAspect::Placement placement, QskAspect::Subcontrol subControl ) noexcept
{
    return subControl | placement;
}

namespace std
{
    template< > struct hash< QskAspect >
    {
        constexpr size_t operator()( const QskAspect& aspect ) const noexcept
        {
            return aspect.value();
        }
    };
}

Q_DECLARE_TYPEINFO( QskAspect, Q_MOVABLE_TYPE );

#ifndef QT_NO_DEBUG_STREAM

class QDebug;

QSK_EXPORT QDebug operator<<( QDebug, QskAspect );
QSK_EXPORT QDebug operator<<( QDebug, QskAspect::Type );
QSK_EXPORT QDebug operator<<( QDebug, QskAspect::Subcontrol );
QSK_EXPORT QDebug operator<<( QDebug, QskAspect::Primitive );
QSK_EXPORT QDebug operator<<( QDebug, QskAspect::Placement );
QSK_EXPORT QDebug operator<<( QDebug, QskAspect::State );

QSK_EXPORT void qskDebugState( QDebug, const QMetaObject*, QskAspect::State );
QSK_EXPORT void qskDebugAspect( QDebug, const QMetaObject*, QskAspect );

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
    static_assert( \
        ( value >= QskAspect::FirstSystemState && value < QskAspect::FirstUserState ) || \
        ( value > QskAspect::LastUserState && value <= QskAspect::LastSystemState ), \
        "Invalid system state" \
    ); \
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
