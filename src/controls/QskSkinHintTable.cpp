/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskSkinHintTable.h"

QskSkinHintTable::QskSkinHintTable( bool extraAnimatorLookup ):
    m_hints( nullptr ),
    m_animatorAspects( nullptr )
{
    if ( extraAnimatorLookup )
        m_animatorAspects = new std::unordered_map< int, std::set< QskAspect::Aspect > >();
}

QskSkinHintTable::~QskSkinHintTable()
{
    delete m_hints;
    delete m_animatorAspects;
}

const std::set< QskAspect::Aspect >& QskSkinHintTable::animatorAspects(
    QskAspect::Subcontrol subControl ) const
{
    if ( m_animatorAspects )
    {
        auto it = m_animatorAspects->find( subControl );
        if ( it != m_animatorAspects->cend() )
            return it->second;
    }

    static std::set< QskAspect::Aspect > dummyAspects;
    return dummyAspects;
}

const std::unordered_map< QskAspect::Aspect, QVariant >& QskSkinHintTable::hints() const
{
    if ( m_hints )
        return *m_hints;

    static std::unordered_map< QskAspect::Aspect, QVariant > dummyHints;
    return dummyHints;
}

void QskSkinHintTable::setSkinHint( QskAspect::Aspect aspect, const QVariant& skinHint )
{
    // For edges/corners, add all the implied assignments
    if ( aspect.isBoxPrimitive() )
    {
        if ( aspect.boxPrimitive() == QskAspect::Radius )
        {
            setSkinHint( aspect | QskAspect::RadiusX, skinHint );
            setSkinHint( aspect | QskAspect::RadiusY, skinHint );
            return;
        }

        const auto edges = aspect.edge();

        const auto bitcount = qPopulationCount( static_cast< quint8 >( edges ) );
        if ( !bitcount || bitcount > 1 )
        {
            using namespace QskAspect;

            auto aspectEdges = aspect;
            aspectEdges.clearEdge();

            if ( !bitcount || edges & TopEdge )
                setSkinHint( aspectEdges | TopEdge, skinHint );

            if ( !bitcount || ( edges & LeftEdge ) )
                setSkinHint( aspectEdges | LeftEdge, skinHint );

            if ( !bitcount || ( edges & RightEdge ) )
                setSkinHint( aspectEdges | RightEdge, skinHint );

            if ( !bitcount || ( edges & BottomEdge ) )
                setSkinHint( aspectEdges | BottomEdge, skinHint );
        }

        if ( bitcount > 1 ) // Allows 0 to imply AllEdges
            return;
    }

    if ( m_hints == nullptr )
        m_hints = new std::remove_pointer< decltype( m_hints ) >::type;

    auto it = m_hints->find( aspect );
    if ( it == m_hints->end() )
    {
        m_hints->emplace( aspect, skinHint );
    }
    else if ( it->second != skinHint )
    {
        it->second = skinHint;
    }

    if ( m_animatorAspects && aspect.isAnimator() )
        ( *m_animatorAspects )[ aspect.subControl() ].insert( aspect );
}

void QskSkinHintTable::removeSkinHint( QskAspect::Aspect aspect )
{
    if ( m_hints == nullptr )
        return;

    m_hints->erase( aspect );
    if ( m_hints->empty() )
    {
        delete m_hints;
        m_hints = nullptr;
    }

    if ( m_animatorAspects && aspect.isAnimator() )
    {
        auto it = m_animatorAspects->find( aspect.subControl() );
        if ( it != m_animatorAspects->end() )
        {
            auto& aspects = it->second;
            aspects.erase( aspect );
            if ( aspects.empty() )
                m_animatorAspects->erase( it );
        }
    }
}

const QVariant& QskSkinHintTable::skinHint( QskAspect::Aspect aspect ) const
{
    if ( m_hints )
    {
        auto it = m_hints->find( aspect );
        if ( it != m_hints->cend() )
            return it->second;
    }

    static QVariant invalidHint;
    return invalidHint;
}
