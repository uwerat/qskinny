/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskLayoutElement.h"
#include "QskLayoutMetrics.h"
#include "QskSizePolicy.h"

static inline bool qskCanGrow(
    const QskLayoutElement* element, Qt::Orientation orientation )
{
    const quint32 growFlags = QskSizePolicy::GrowFlag | QskSizePolicy::ExpandFlag;
    return element->sizePolicy().policy( orientation ) & growFlags;
}

QskLayoutElement::QskLayoutElement()
{
}

QskLayoutElement::~QskLayoutElement()
{
}

QSizeF QskLayoutElement::sizeConstraint(
    Qt::SizeHint which, const QSizeF& constraint ) const
{
    if ( constraint.isValid() )
        return constraint;

    auto policy = sizePolicy();

    bool ignoreWidth = false;
    bool ignoreHeight = false;

    if ( which == Qt::PreferredSize )
    {
        /*
            First we are checking the IgnoreFlag, to avoid doing
            pointless calculations.
         */
        ignoreWidth = policy.policy( Qt::Horizontal ) & QskSizePolicy::IgnoreFlag;
        ignoreHeight = policy.policy( Qt::Vertical ) & QskSizePolicy::IgnoreFlag;

        if ( ( ignoreWidth && ignoreHeight )
            || ( ignoreWidth && constraint.height() >= 0.0 )
            || ( ignoreHeight && constraint.width() >= 0.0 ) )
        {
            return QSizeF();
        }
    }

    const auto whichH = policy.effectiveSizeHintType( which, Qt::Horizontal );
    const auto whichV = policy.effectiveSizeHintType( which, Qt::Vertical );

    QSizeF size;

    int constraintType = QskSizePolicy::Unconstrained;

    /*
        We apply a constraint - even, when the policy is unconstrained.
        Do we really want to do this ???
     */
    if ( constraint.height() >= 0.0 )
    {
        const auto h = boundedSize( Qt::Vertical, constraint.height() );
        size = sizeHint( whichV, QSizeF( -1.0, h ) );

        if ( ( whichH != whichV ) || ( size.height() != h ) )
            constraintType = QskSizePolicy::WidthForHeight;
    }
    else if ( constraint.width() >= 0.0 )
    {
        const auto w = boundedSize( Qt::Horizontal, constraint.width() );
        size = sizeHint( whichH, QSizeF( w, -1.0 ) );

        if ( ( whichV != whichH ) || ( size.width() != w ) )
            constraintType = QskSizePolicy::HeightForWidth;
    }
    else
    {
        constraintType = policy.constraintType();

        switch( constraintType )
        {
            case QskSizePolicy::WidthForHeight:
            {
                size = sizeHint( whichV );
                break;
            }
            case QskSizePolicy::HeightForWidth:
            {
                size = sizeHint( whichH );
                break;
            }
            default:
            {
                if ( whichV != whichH )
                {
                    if ( !ignoreWidth )
                        size.rwidth() = sizeHint( whichH ).width();

                    if ( !ignoreHeight )
                        size.rheight() = sizeHint( whichV ).height();
                }
                else
                {
                    size = sizeHint( whichH );
                }
            }
        }
    }

    switch( constraintType )
    {
        case QskSizePolicy::HeightForWidth:
        {
            const QSizeF c( size.width(), -1.0 );
            size.rheight() = sizeHint( whichV, c ).height();
            break;
        }
        case QskSizePolicy::WidthForHeight:
        {
            const QSizeF c( -1.0, size.height() );
            size.rwidth() = sizeHint( whichH, c ).width();
            break;
        }
    }

    if ( ignoreWidth || constraint.width() >= 0.0 )
        size.rwidth() = -1.0;

    if ( ignoreHeight || constraint.height() >= 0.0 )
        size.rheight() = -1.0;

    return size;
}

QSizeF QskLayoutElement::constrainedSize( const QSizeF& size ) const
{
    if ( size.width() <= 0.0 && size.height() <= 0.0 )
        return QSizeF( 0.0, 0.0 );

    QSizeF min, max;

    const auto policy = sizePolicy();

    switch( policy.constraintType() )
    {
        case QskSizePolicy::WidthForHeight:
        {
            const qreal h = boundedSize( Qt::Vertical, size.height() );

            min = sizeConstraint( Qt::MinimumSize, QSizeF( -1.0, h ) );
            max = sizeConstraint( Qt::MaximumSize, QSizeF( -1.0, h ) );

            min.rheight() = max.rheight() = h;
            break;
        }
        case QskSizePolicy::HeightForWidth:
        {
            const qreal w = boundedSize( Qt::Horizontal, size.width() );

            min = sizeConstraint( Qt::MinimumSize, QSizeF( w, -1.0 ) );
            max = sizeConstraint( Qt::MaximumSize, QSizeF( w, -1.0 ) );

            min.rwidth() = max.rwidth() = w;
            break;
        }
        default:
        {
            min = sizeConstraint( Qt::MinimumSize, QSizeF() );
            max = sizeConstraint( Qt::MaximumSize, QSizeF() );
        }
    }

    qreal width = size.width();
    qreal height = size.height();

    if ( max.width() >= 0.0 )
        width = qMin( width, max.width() );

    if ( max.height() >= 0.0 )
        height = qMin( height, max.height() );

    width = qMax( width, min.width() );
    height = qMax( height, min.height() );

    return QSizeF( width, height );
}

QskLayoutMetrics QskLayoutElement::metrics(
    Qt::Orientation orientation, qreal constraint ) const
{
    const auto policy = sizePolicy().policy( orientation );

    qreal minimum, preferred, maximum;

    const auto expandFlags = QskSizePolicy::GrowFlag | QskSizePolicy::ExpandFlag;

    if ( ( policy & QskSizePolicy::ShrinkFlag ) &&
        ( policy & expandFlags ) && ( policy & QskSizePolicy::IgnoreFlag ) )
    {
        // we don't need to calculate the preferred size

        minimum = metric( orientation, Qt::MinimumSize, -1.0 );
        maximum = metric( orientation, Qt::MaximumSize, -1.0 );
        preferred = minimum;
    }
    else
    {
        if ( constraint >= 0.0 )
        {
            if ( !( policy & QskSizePolicy::ConstrainedFlag ) )
                constraint = -1.0;
        }

        preferred = metric( orientation, Qt::PreferredSize, constraint );

        if ( policy & QskSizePolicy::ShrinkFlag )
            minimum = metric( orientation, Qt::MinimumSize, -1.0 );
        else
            minimum = preferred;

        if ( policy & expandFlags )
            maximum = metric( orientation, Qt::MaximumSize, -1.0 );
        else
            maximum = preferred;

        if ( policy & QskSizePolicy::IgnoreFlag )
            preferred = minimum;
    }

    return QskLayoutMetrics( minimum, preferred, maximum );
}

qreal QskLayoutElement::metric( Qt::Orientation orientation,
    Qt::SizeHint which, qreal constraint ) const
{
    qreal metric = 0.0;

    if ( which == Qt::MinimumSize )
    {
        const auto hint = sizeHint( Qt::MinimumSize );

        if ( orientation == Qt::Horizontal )
            metric = hint.width();
        else
            metric = hint.height();

        if ( metric < 0.0 )
            metric = 0.0;
    }
    else if ( which == Qt::MaximumSize )
    {
        const auto hint = sizeHint( Qt::MaximumSize );

        if ( orientation == Qt::Horizontal )
            metric = hint.width();
        else
            metric = hint.height();

        if ( metric < 0.0 )
            metric = QskLayoutMetrics::unlimited;
    }
    else if ( which == Qt::PreferredSize )
    {
        const auto constraintType = sizePolicy().constraintType();

        if ( constraintType == QskSizePolicy::Unconstrained )
        {
            const auto hint = sizeHint( Qt::PreferredSize );

            if ( orientation == Qt::Horizontal )
                metric = hint.width();
            else
                metric = hint.height();
        }
        else
        {
            if ( constraint < 0.0 )
            {
                auto hint = sizeHint( Qt::PreferredSize );

                if ( orientation == Qt::Horizontal )
                {
                    if ( constraintType == QskSizePolicy::WidthForHeight )
                        hint.setWidth( widthForHeight( hint.height() ) );

                    metric = hint.width();
                }
                else
                {
                    if ( constraintType == QskSizePolicy::HeightForWidth )
                        hint.setHeight( heightForWidth( hint.width() ) );

                    metric = hint.height();
                }
            }
            else
            {
                if ( orientation == Qt::Horizontal )
                {
                    if ( !qskCanGrow( this, Qt::Vertical ) )
                    {
                        const auto maxH = sizeHint( Qt::PreferredSize ).height();

                        if ( maxH >= 0.0 )
                            constraint = qMin( constraint, maxH );
                    }

                    metric = widthForHeight( constraint );
                }
                else
                {
                    if ( !qskCanGrow( this, Qt::Horizontal ) )
                    {
                        const auto maxW = sizeHint( Qt::PreferredSize ).width();

                        if ( maxW >= 0.0 )
                            constraint = qMin( constraint, maxW );
                    }

                    metric = heightForWidth( constraint );
                }
            }
        }

        if ( metric < 0.0 )
            metric = 0.0;
    }

    return metric;
}

qreal QskLayoutElement::boundedSize( Qt::Orientation orientation, qreal size ) const
{
    const auto policy = sizePolicy();

    const auto whichMin = policy.effectiveSizeHintType( Qt::MinimumSize, orientation );
    const auto whichMax = policy.effectiveSizeHintType( Qt::MaximumSize, orientation );

    const auto hintMin = sizeHint( whichMin );
    const auto hintMax = ( whichMax == whichMin ) ? hintMin : sizeHint( whichMax );

    if ( orientation == Qt::Horizontal )
    {
        if ( hintMax.width() >= 0.0 )
            size = qMin( size, hintMax.width() );

        size = qMax( size, hintMin.width() );
    }
    else
    {
        if ( hintMax.height() >= 0.0 )
            size = qMin( size, hintMax.height() );

        size = qMax( size, hintMin.height() );
    }

    return size;
}

#include "QskQuick.h"

QskItemLayoutElement::QskItemLayoutElement( const QQuickItem* item )
    : m_item( item )
{
}

QskSizePolicy QskItemLayoutElement::sizePolicy() const
{
    return qskSizePolicy( m_item );
}

QSizeF QskItemLayoutElement::sizeHint( Qt::SizeHint which,
    const QSizeF& constraint ) const
{
    return qskEffectiveSizeHint( m_item, which, constraint );
}

Qt::Alignment QskItemLayoutElement::alignment() const
{
    return qskLayoutAlignmentHint( m_item );
}
