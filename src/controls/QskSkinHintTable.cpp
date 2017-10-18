/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskSkinHintTable.h"

QVariant QskSkinHintTable::invalidHint;

inline const QVariant* qskResolvedHint( QskAspect::Aspect aspect,
    const std::unordered_map< QskAspect::Aspect, QVariant >& hints,
    QskAspect::Aspect* resolvedAspect )
{
    const auto a = aspect;

    Q_FOREVER
    {
        auto it = hints.find( aspect );
        if ( it != hints.cend() )
        {
            if ( resolvedAspect )
                *resolvedAspect = aspect;

            return &it->second;
        }

        if ( const auto topState = aspect.topState() )
        {
            aspect.clearState( topState );
            continue;
        }

        if ( aspect.placement() )
        {
            // clear the placement bit and restart
            aspect = a;
            aspect.setPlacement( static_cast< QskAspect::Placement >( 0 ) );

            continue;
        }

        return nullptr;
    }
}

QskSkinHintTable::QskSkinHintTable():
    m_hints( nullptr ),
    m_animatorCount( 0 ),
    m_hasStates( false )
{
}

QskSkinHintTable::QskSkinHintTable( const QskSkinHintTable& other ):
    m_hints( nullptr ),
    m_animatorCount( other.m_animatorCount ),
    m_hasStates( other.m_hasStates )
{
    if ( other.m_hints )
        m_hints = new HintMap( *(other.m_hints) );
}

QskSkinHintTable::~QskSkinHintTable()
{
    delete m_hints;
}

QskSkinHintTable& QskSkinHintTable::operator=( const QskSkinHintTable& other )
{
    m_animatorCount = other.m_animatorCount;
    m_hasStates = other.m_hasStates;

    if ( other.m_hints )
    {
        if ( m_hints == nullptr )
            m_hints = new HintMap();

        *m_hints = *other.m_hints;
    }
    else
    {
        delete m_hints;
        m_hints = nullptr;
    }

    return *this;
}

const std::unordered_map< QskAspect::Aspect, QVariant >& QskSkinHintTable::hints() const
{
    if ( m_hints )
        return *m_hints;

    static std::unordered_map< QskAspect::Aspect, QVariant > dummyHints;
    return dummyHints;
}

void QskSkinHintTable::setAnimation(
    QskAspect::Aspect aspect, QskAnimationHint animation )
{
    aspect.setAnimator( true );

    const QVariant v = QVariant::fromValue( animation );

    if ( aspect.type() == 0 )
    {
        using namespace QskAspect;

        setHint( aspect | Color, v );
        setHint( aspect | Metric, v );
    }
    else
    {
        setHint( aspect, v );
    }
}

void QskSkinHintTable::setHint( QskAspect::Aspect aspect, const QVariant& skinHint )
{
    if ( m_hints == nullptr )
        m_hints = new HintMap();

    auto it = m_hints->find( aspect );
    if ( it == m_hints->end() )
    {
        m_hints->emplace( aspect, skinHint );
        if ( aspect.isAnimator() )
            m_animatorCount++;
    }
    else if ( it->second != skinHint )
    {
        it->second = skinHint;
    }

    if ( aspect.state() )
        m_hasStates = true;
}

void QskSkinHintTable::removeHint( QskAspect::Aspect aspect )
{
    if ( m_hints == nullptr )
        return;

    if ( m_hints->erase( aspect ) )
    {
        if ( aspect.isAnimator() )
            m_animatorCount--;
        
        if ( m_hints->empty() )
        {
            delete m_hints;
            m_hints = nullptr;
        }
    }
}

void QskSkinHintTable::clear()
{
    delete m_hints;
    m_hints = nullptr;

    m_animatorCount = 0;
}

const QVariant* QskSkinHintTable::resolvedHint(
    QskAspect::Aspect aspect, QskAspect::Aspect* resolvedAspect ) const
{
    if ( m_hints != nullptr )
        return qskResolvedHint( aspect, *m_hints, resolvedAspect );

    return nullptr;
}

QskAspect::Aspect QskSkinHintTable::resolvedAspect( QskAspect::Aspect aspect ) const
{
    QskAspect::Aspect a;

    if ( m_hints != nullptr )
        qskResolvedHint( aspect, *m_hints, &a );

    return a;
}

QskAspect::Aspect QskSkinHintTable::resolvedAnimator(
    QskAspect::Aspect aspect, QskAnimationHint& hint ) const
{
    if ( m_hints && m_animatorCount > 0 )
    {
        Q_FOREVER
        {
            auto it = m_hints->find( aspect );
            if ( it != m_hints->cend() )
            {
                hint = it->second.value< QskAnimationHint >();
                return aspect;
            }

            if ( const auto topState = aspect.topState() )
                aspect.clearState( topState );
            else
                break;
        }
    }

    return QskAspect::Aspect();
}
