/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "QskAspect.h"

#include <qalgorithms.h>
#include <qdebug.h>
#include <qmetaobject.h>
#include <qvector.h>

#include <bitset>
#include <string>
#include <unordered_map>

static_assert( sizeof( QskAspect ) == sizeof( quint64 ),
    "QskAspect::Aspect has to match quint64" );

static void qskRegisterAspect()
{
    qRegisterMetaType< QskAspect >();

#if QT_VERSION < QT_VERSION_CHECK( 6, 0, 0 )
    QMetaType::registerEqualsComparator< QskAspect >();
#endif
}

Q_CONSTRUCTOR_FUNCTION( qskRegisterAspect )

namespace
{
    using namespace std;

    struct StateInfo
    {
        QskAspect::State state;
        QByteArray name;
    };

    struct AspectRegistry
    {
        QVector< QByteArray > subControlNames;
        unordered_map< const QMetaObject*, QVector< QskAspect::Subcontrol > > subControlTable;
        unordered_map< const QMetaObject*, QVector< StateInfo > > stateTable;
    };
}

static quint8 qskPrimitiveCount = QMetaEnum::fromType< QskAspect::Primitive >().keyCount();

quint8 QskAspect::primitiveCount()
{
    return qskPrimitiveCount;
}

void QskAspect::reservePrimitives( quint8 count )
{
    // applications might need to increase the count to add additional primitives

    constexpr quint8 maxCount = 1 << 5; // we have 5 bits for the primitives

    Q_ASSERT( count <= maxCount );

    if ( count > maxCount )
    {
        qWarning() << "QskAspect: having more than"
            << maxCount << "primitives is not supported.";

        count = maxCount;
    }

    if ( count > qskPrimitiveCount )
        qskPrimitiveCount = count;
}

Q_GLOBAL_STATIC( AspectRegistry, qskAspectRegistry )

QskAspect::State QskAspect::registerState(
    const QMetaObject* metaObject, State state, const char* name )
{
    StateInfo info;
    info.state = state;
    info.name = name;

    auto& stateTable = qskAspectRegistry->stateTable;
    stateTable[ metaObject ] += info;

    return state;
}

QskAspect::Subcontrol QskAspect::nextSubcontrol(
    const QMetaObject* metaObject, const char* name )
{
    auto& names = qskAspectRegistry->subControlNames;
    auto& hashTable = qskAspectRegistry->subControlTable;

    Q_ASSERT_X( names.size() <= LastSubcontrol, "QskAspect",
        "There are no free subcontrol aspects; please modify your"
        " application to declare fewer aspects, or increase the mask size of"
        " QskAspect::Subcontrol in QskAspect.h." );

    names += name;

    // 0 is QskAspect::Control, so we have to start with 1
    const auto subControl = static_cast< Subcontrol >( names.size() );
    hashTable[ metaObject ] += subControl;

    return subControl;
}

QByteArray QskAspect::subControlName( Subcontrol subControl )
{
    const auto& names = qskAspectRegistry->subControlNames;

    const int index = subControl;
    if ( index > 0 && index <= names.size() )
        return names[ index - 1 ];

    return QByteArray();
}

QVector< QByteArray > QskAspect::subControlNames( const QMetaObject* metaObject )
{
    const auto& names = qskAspectRegistry->subControlNames;

    if ( metaObject == nullptr )
        return names;

    const auto subControls = QskAspect::subControls( metaObject );

    QVector< QByteArray > subControlNames;
    subControlNames.reserve( subControls.size() );

    for ( auto subControl : subControls )
        subControlNames += names[ subControl ];

    return subControlNames;
}

QVector< QskAspect::Subcontrol > QskAspect::subControls( const QMetaObject* metaObject )
{
    if ( metaObject )
    {
        const auto& hashTable = qskAspectRegistry->subControlTable;

        auto it = hashTable.find( metaObject );
        if ( it != hashTable.end() )
            return it->second;
    }

    return QVector< Subcontrol >();
}

#ifndef QT_NO_DEBUG_STREAM

static QByteArray qskEnumString( const char* name, int value )
{
    const auto& mo = QskAspect::staticMetaObject;

    const QMetaEnum metaEnum = mo.enumerator( mo.indexOfEnumerator( name ) );
    const char* key = metaEnum.valueToKey( value );

    return key ? QByteArray( key ) : QString::number( value ).toLatin1();
}

static QByteArray qskStateKey( const QMetaObject* metaObject, quint16 state )
{
    const auto& stateTable = qskAspectRegistry->stateTable;

    for ( auto mo = metaObject; mo != nullptr; mo = mo->superClass() )
    {
        const auto it = stateTable.find( mo );
        if ( it != stateTable.end() )
        {
            for ( const auto& info : it->second )
            {
                if ( info.state == state )
                    return info.name;
            }
        }
    }

    return QByteArray();
}

static QByteArray qskStatesToString(
    const QMetaObject* metaObject, QskAspect::States states )
{
    if ( states == 0 )
    {
        return "NoState";
    }

    if ( metaObject == nullptr )
    {
        const std::bitset< 16 > stateBits( states );
        return stateBits.to_string().c_str();
    }

    // not the fastest implementation, but as it is for debugging only

    QByteArray stateString;

    bool first = true;

    for ( int i = 0; i < 16; i++ )
    {
        const uint mask = 1 << i;

        if ( states & mask )
        {
            if ( first )
                first = false;
            else
                stateString += ", ";

            const auto key = qskStateKey( metaObject, mask );
            if ( key.isEmpty() )
            {
                const std::bitset< 16 > stateBits( states );
                stateString += stateBits.to_string().c_str();
            }
            else
            {
                stateString += key;
            }
        }
    }

    return stateString;
}

static inline QDebug qskDebugEnum(
    QDebug debug, const char* name, int value )
{
    qt_QMetaEnum_debugOperator( debug, value,
        &QskAspect::staticMetaObject, name );
    return debug;
}

QDebug operator<<( QDebug debug, QskAspect::Type type )
{
    return qskDebugEnum( debug, "Type", type );
}

QDebug operator<<( QDebug debug, QskAspect::Primitive primitive )
{
    return qskDebugEnum( debug, "Primitive", primitive );
}

QDebug operator<<( QDebug debug, QskAspect::Subcontrol subControl )
{
    QDebugStateSaver saver( debug );

    debug.nospace();
    debug << "QskAspect::Subcontrol" << '(';
    debug << QskAspect::subControlName( subControl );
    debug << ')';

    return debug;
}

QDebug operator<<( QDebug debug, QskAspect::Variation variation )
{
    qskDebugEnum( debug, "Variation", variation );
    return debug;
}

QDebug operator<<( QDebug debug, QskAspect::States states )
{
    qskDebugStates( debug, nullptr, states );
    return debug;
}

QDebug operator<<( QDebug debug, QskAspect aspect )
{
    qskDebugAspect( debug, nullptr, aspect );
    return debug;
}

void qskDebugStates( QDebug debug,
    const QMetaObject* metaObject, QskAspect::States states )
{
    QDebugStateSaver saver( debug );

    debug.resetFormat();
    debug.noquote();
    debug.nospace();

    debug << "QskAspect::States( " << qskStatesToString( metaObject, states ) << " )";
}

void qskDebugAspect( QDebug debug, const QMetaObject* metaObject, QskAspect aspect )
{
    QDebugStateSaver saver( debug );

    debug.resetFormat();
    debug.noquote();
    debug.nospace();

    debug << "QskAspect( ";

    const auto subControlName = QskAspect::subControlName( aspect.subControl() );
    if ( subControlName.isEmpty() )
        debug << QByteArrayLiteral( "NoSubcontrol" );
    else
        debug << subControlName;

    if ( aspect.section() != QskAspect::Body )
        debug << ", " << qskEnumString( "Section", aspect.section() );

    debug << ", " << qskEnumString( "Type", aspect.type() );
    if ( aspect.isAnimator() )
        debug << "(A)";

    if ( aspect.primitive() != 0 )
        debug << ", " << qskEnumString( "Primitive", aspect.primitive() );

    if ( aspect.variation() != QskAspect::NoVariation )
        debug << ", " << qskEnumString( "Variation", aspect.variation() );

    if ( aspect.hasStates() )
        debug << ", " << qskStatesToString( metaObject, aspect.states() );

    debug << " )";
}

#endif

const char* QskAspect::toPrintable() const
{
    QString tmp;

    QDebug debug( &tmp );
    debug << *this;

    // we should find a better implementation
    static QByteArray bytes[ 10 ];
    static int counter = 0;

    counter = ( counter + 1 ) % 10;

    bytes[ counter ] = tmp.toUtf8();
    return bytes[ counter ].constData();
}

QskAspect::State QskAspect::topState() const noexcept
{
    if ( m_bits.states == NoState )
        return NoState;

    const auto n = qCountLeadingZeroBits( static_cast< quint16 >( m_bits.states ) );
    return static_cast< QskAspect::State >( 1 << ( 15 - n ) );
}

#include "moc_QskAspect.cpp"
