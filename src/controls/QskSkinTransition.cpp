/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskSkinTransition.h"
#include "QskColorFilter.h"
#include "QskControl.h"
#include "QskWindow.h"
#include "QskAnimationHint.h"
#include "QskHintAnimator.h"
#include "QskSkin.h"
#include "QskSkinHintTable.h"

#include <qglobalstatic.h>
#include <qguiapplication.h>
#include <qobject.h>
#include <qvector.h>

#include <unordered_map>
#include <vector>

static void qskSendStyleEventRecursive( QQuickItem* item )
{
    QEvent event( QEvent::StyleChange );
    QCoreApplication::sendEvent( item, &event );

    const auto children = item->childItems();
    for ( auto child : children )
        qskSendStyleEventRecursive( child );
}

static void qskAddCandidates( const QskSkinTransition::Type mask,
    const QskSkin* skin, QSet< QskAspect >& candidates )
{
    for ( const auto& entry : skin->hintTable().hints() )
    {
        const auto aspect = entry.first.trunk();

        if ( aspect.isAnimator() )
            continue;

        bool isCandidate = false;

        switch( aspect.type() )
        {
            case QskAspect::Flag:
            {
                if ( aspect.flagPrimitive() == QskAspect::GraphicRole )
                {
                    isCandidate = mask & QskSkinTransition::Color;
                }
#if 0
                else if ( aspect.flagPrimitive() == QskAspect::FontRole )
                {
                    isCandidate = mask & QskSkinTransition::Metric;
                }
#endif
                break;
            }
            case QskAspect::Color:
            {
                isCandidate = mask & QskSkinTransition::Color;
                break;
            }
            case QskAspect::Metric:
            {
                isCandidate = mask & QskSkinTransition::Metric;
                break;
            }
        }

        if ( isCandidate )
            candidates += aspect;
    }
}

namespace
{
    class UpdateInfo
    {
      public:
        enum UpdateMode
        {
            Polish = 1,
            Update = 2
        };

        static inline bool compare( const UpdateInfo& i1, const UpdateInfo& i2 )
        {
            return i1.control.data() < i2.control.data();
        }

        QPointer< QskControl > control;
        int updateModes;
    };

    class HintAnimator : public QskHintAnimator
    {
      public:
        inline HintAnimator( const QskControl* control, const QskAspect aspect,
            const QVariant& value1, const QVariant& value2, QskAnimationHint hint )
        {
            setAspect( aspect );
            setStartValue( value1 );
            setEndValue( value2 );

            setDuration( hint.duration );
            setEasingCurve( hint.type );
            setUpdateFlags( hint.updateFlags );

            setWindow( control->window() );
        }
    };

    class WindowAnimator
    {
      public:
        WindowAnimator( QQuickWindow* = nullptr );

        const QQuickWindow* window() const;

        void start();
        bool isRunning() const;

        QVariant animatedHint( QskAspect ) const;
        QVariant animatedGraphicFilter( int graphicRole ) const;

        void addGraphicFilterAnimators( const QskAnimationHint&,
            const QskSkin*, const QskSkin* );

        void addItemAspects( QQuickItem*,
            const QskAnimationHint&, const QSet< QskAspect >&,
            const QskSkin*, const QskSkin* );

        void update();

      private:

        bool isControlAffected( const QskControl*,
            const QVector< QskAspect::Subcontrol >&, QskAspect ) const;

        void addHints( const QskControl*,
            const QskAnimationHint&, const QSet< QskAspect >& candidates,
            const QskSkin* skin1, const QskSkin* skin2 );

        void storeAnimator( const QskControl*, const QskAspect,
            const QVariant&, const QVariant&, QskAnimationHint );

        void storeUpdateInfo( const QskControl*, QskAspect );

        QQuickWindow* m_window;
        std::unordered_map< QskAspect, HintAnimator > m_animatorMap;
        std::unordered_map< int, QskVariantAnimator > m_graphicFilterAnimatorMap;
        std::vector< UpdateInfo > m_updateInfos; // vector: for fast iteration
    };

    class ApplicationAnimator : public QObject
    {
        Q_OBJECT

      public:
        ~ApplicationAnimator();

        WindowAnimator* windowAnimator( const QQuickWindow* );

        void add( WindowAnimator* );

        void start();
        void reset();
        bool isRunning() const;

      private Q_SLOTS:
        // using functor slots ?
        void notify( QQuickWindow* );
        void cleanup( QQuickWindow* );

      private:
        /*
            It should be possible to find an implementation, that interpolates
            a skin hint only once for all windows. But as our animtors are driven by
            QQuickWindow::afterAnimating the code will have to be somehow tricky.
            But as skin transitions are no operations, that happen often, we can accept
            the overhaed of the current implementation and do the finetuning later.
         */
        std::vector< WindowAnimator* > m_windowAnimators;
        QMetaObject::Connection m_connections[2];
    };
}

Q_GLOBAL_STATIC( ApplicationAnimator, qskApplicationAnimator )

WindowAnimator::WindowAnimator( QQuickWindow* window )
    : m_window( window )
{
}

inline const QQuickWindow* WindowAnimator::window() const
{
    return m_window;
}

void WindowAnimator::start()
{
    for ( auto& it : m_animatorMap )
        it.second.start();

    for ( auto& it : m_graphicFilterAnimatorMap )
        it.second.start();
}

bool WindowAnimator::isRunning() const
{
    if ( !m_animatorMap.empty() )
    {
        const auto& animator = m_animatorMap.begin()->second;
        if ( animator.isRunning() )
            return true;
    }

    if ( !m_graphicFilterAnimatorMap.empty() )
    {
        const auto& animator = m_graphicFilterAnimatorMap.begin()->second;
        if ( animator.isRunning() )
            return true;
    }

    return false;
}

inline QVariant WindowAnimator::animatedHint( QskAspect aspect ) const
{
    auto it = m_animatorMap.find( aspect );
    if ( it != m_animatorMap.cend() )
    {
        const auto& animator = it->second;
        if ( animator.isRunning() )
            return animator.currentValue();
    }

    return QVariant();
}

inline QVariant WindowAnimator::animatedGraphicFilter( int graphicRole ) const
{
    auto it = m_graphicFilterAnimatorMap.find( graphicRole );
    if ( it != m_graphicFilterAnimatorMap.cend() )
    {
        const auto& animator = it->second;
        if ( animator.isRunning() )
            return animator.currentValue();
    }

    return QVariant();
}

void WindowAnimator::addGraphicFilterAnimators( const QskAnimationHint& animatorHint,
    const QskSkin* skin1, const QskSkin* skin2 )
{
    const QskColorFilter noFilter;

    const auto& filter1 = skin1->graphicFilters();
    const auto& filter2 = skin2->graphicFilters();

    for ( auto it2 = filter2.begin(); it2 != filter2.end(); ++it2 )
    {
        auto it1 = filter1.find( it2->first );
        if ( it1 == filter1.cend() )
            it1 = filter1.find( 0 );

        const auto& f1 = ( it1 != filter1.cend() ) ? it1->second : noFilter;
        const auto& f2 = it2->second;

        if ( f1 != f2 )
        {
            QskVariantAnimator animator;
            animator.setWindow( m_window );
            animator.setDuration( animatorHint.duration );
            animator.setEasingCurve( animatorHint.type );
            animator.setStartValue( QVariant::fromValue( f1 ) );
            animator.setEndValue( QVariant::fromValue( f2 ) );

            m_graphicFilterAnimatorMap.emplace( it2->first, animator );
        }
    }
}

void WindowAnimator::addItemAspects( QQuickItem* item,
    const QskAnimationHint& animatorHint, const QSet< QskAspect >& candidates,
    const QskSkin* skin1, const QskSkin* skin2  )
{
    if ( !item->isVisible() )
        return;

    if ( auto control = qskControlCast( item ) )
    {
        if ( control->isInitiallyPainted() && ( control->effectiveSkin() == skin2 ) )
        {
            addHints( control, animatorHint, candidates, skin1, skin2 );
#if 1
            /*
                As it is hard to identify which controls depend on the animated
                graphic filters we schedule an initial update and let the
                controls do the rest: see QskSkinnable::effectiveGraphicFilter
             */
            control->update();
#endif
        }
    }

    const auto children = item->childItems();
    for ( auto child : children )
        addItemAspects( child, animatorHint, candidates, skin1, skin2 );
}

void WindowAnimator::update()
{
    for ( auto& info : m_updateInfos )
    {
        if ( auto control = info.control )
        {
            if ( info.updateModes & UpdateInfo::Polish )
            {
                control->resetImplicitSize();
                control->polish();
            }

            if ( info.updateModes & UpdateInfo::Update )
                control->update();
        }
    }
}

void WindowAnimator::addHints( const QskControl* control,
    const QskAnimationHint& animatorHint, const QSet< QskAspect >& candidates,
    const QskSkin* skin1, const QskSkin* skin2 )
{
    const auto subControls = control->subControls();

    const auto& localTable = control->hintTable();

    const auto& table1 = skin1->hintTable();
    const auto& table2 = skin2->hintTable();

    for ( auto aspect : candidates )
    {
        if ( !isControlAffected( control, subControls, aspect ) )
            continue;

        aspect.setPlacement( control->effectivePlacement() );
        aspect.setStates( control->skinStates() );

        if ( localTable.resolvedHint( aspect ) )
        {
            // value is not from the skin - ignored
            continue;
        }

        QskAspect r1, r2;

        const auto v1 = table1.resolvedHint( aspect, &r1 );
        const auto v2 = table2.resolvedHint( aspect, &r2 );

        if ( v1 && v2 )
        {
            if ( QskVariantAnimator::maybeInterpolate( *v1, *v2 ) )
            {
                if ( r1.placement() == r2.placement() )
                    aspect.setPlacement( r2.placement() );

                if ( r1.states() == r2.states() )
                    aspect.setStates( r2.states() );

                storeAnimator( control, aspect, *v1, *v2, animatorHint );
                storeUpdateInfo( control, aspect );
            }
        }
        else if ( v1 )
        {
            aspect.setPlacement( r1.placement() );
            aspect.setStates( r1.states() );

            storeAnimator( control, aspect, *v1, QVariant(), animatorHint );
            storeUpdateInfo( control, aspect );
        }
        else if ( v2 )
        {
            aspect.setPlacement( r1.placement() );
            aspect.setStates( r1.states() );

            storeAnimator( control, aspect, QVariant(), *v2, animatorHint );
            storeUpdateInfo( control, aspect );
        }
    }
}

inline bool WindowAnimator::isControlAffected( const QskControl* control,
    const QVector< QskAspect::Subcontrol >& subControls, const QskAspect aspect ) const
{
    if ( !aspect.isMetric() )
    {
        if ( !( control->flags() & QQuickItem::ItemHasContents ) )
        {
            // while metrics might have an effect on layouts, we
            // ignore all others for controls without content
            return false;
        }
    }

    const auto subControl = aspect.subControl();
    if ( subControl != control->effectiveSubcontrol( subControl ) )
    {
        // The control uses subcontrol redirection, so we can assume it
        // is not interested in this subcontrol.

        return false;
    }

    if ( subControl != QskAspect::NoSubcontrol )
    {
        if ( !subControls.contains( subControl ) )
        {
            // the control is not interested in the aspect
            return false;
        }
    }

    return true;
}

inline void WindowAnimator::storeAnimator( const QskControl* control, const QskAspect aspect,
    const QVariant& value1, const QVariant& value2, QskAnimationHint hint )
{
    if ( m_animatorMap.find( aspect ) == m_animatorMap.cend() )
    {
        m_animatorMap.emplace( aspect,
            HintAnimator( control, aspect, value1, value2, hint ) );
    }
}

inline void WindowAnimator::storeUpdateInfo( const QskControl* control, QskAspect aspect )
{
    UpdateInfo info;
    info.control = const_cast< QskControl* >( control );

    info.updateModes = UpdateInfo::Update;
    if ( aspect.isMetric() )
        info.updateModes |= UpdateInfo::Polish;

    auto it = std::lower_bound(
        m_updateInfos.begin(), m_updateInfos.end(), info, UpdateInfo::compare );

    if ( ( it != m_updateInfos.end() ) && ( it->control == info.control ) )
        it->updateModes |= info.updateModes;
    else
        m_updateInfos.insert( it, info );
}

ApplicationAnimator::~ApplicationAnimator()
{
    reset();
}

inline WindowAnimator* ApplicationAnimator::windowAnimator( const QQuickWindow* window )
{
    if ( window )
    {
        for ( auto animator : m_windowAnimators )
        {
            if ( animator->window() == window )
                return animator;
        }
    }

    return nullptr;
}

void ApplicationAnimator::add( WindowAnimator* animator )
{
    m_windowAnimators.push_back( animator );
}

void ApplicationAnimator::start()
{
    m_connections[0] = QskAnimator::addAdvanceHandler(
        this, SLOT(notify(QQuickWindow*)), Qt::UniqueConnection );

    m_connections[1] = QskAnimator::addCleanupHandler(
        this, SLOT(cleanup(QQuickWindow*)), Qt::UniqueConnection );

    for ( auto& animator : m_windowAnimators )
        animator->start();
}

void ApplicationAnimator::reset()
{
    qDeleteAll( m_windowAnimators );
    m_windowAnimators.clear();

    disconnect( m_connections[0] );
    disconnect( m_connections[1] );
}

inline bool ApplicationAnimator::isRunning() const
{
    return !m_windowAnimators.empty();
}

void ApplicationAnimator::notify( QQuickWindow* window )
{
    for ( auto& animator : m_windowAnimators )
    {
        if ( animator->window() == window )
        {
            animator->update();
            return;
        }
    }
}

void ApplicationAnimator::cleanup( QQuickWindow* window )
{
    for ( auto it = m_windowAnimators.begin();
        it != m_windowAnimators.end(); ++it )
    {
        auto animator = *it;
        if ( animator->window() == window )
        {
            if ( !animator->isRunning() )
            {
                // The notification might be for other animators

                m_windowAnimators.erase( it );
                delete animator;
            }

            // let the items know, that we are done
            qskSendStyleEventRecursive( window->contentItem() );

            break;
        }
    }

    if ( m_windowAnimators.empty() )
        reset();
}

class QskSkinTransition::PrivateData
{
  public:
    QskSkin* skins[ 2 ] = {};
    QskAnimationHint animationHint;
    Type mask = QskSkinTransition::AllTypes;
};

QskSkinTransition::QskSkinTransition()
    : m_data( new PrivateData() )
{
}

QskSkinTransition::~QskSkinTransition()
{
}

void QskSkinTransition::setMask( Type type )
{
    m_data->mask = type;
}

QskSkinTransition::Type QskSkinTransition::mask() const
{
    return m_data->mask;
}

void QskSkinTransition::setSourceSkin( QskSkin* skin )
{
    m_data->skins[ 0 ] = skin;
}

QskSkin* QskSkinTransition::sourceSkin() const
{
    return m_data->skins[ 0 ];
}

void QskSkinTransition::setTargetSkin( QskSkin* skin )
{
    m_data->skins[ 1 ] = skin;
}

QskSkin* QskSkinTransition::targetSkin() const
{
    return m_data->skins[ 1 ];
}

void QskSkinTransition::setAnimation( QskAnimationHint animationHint )
{
    m_data->animationHint = animationHint;
}

QskAnimationHint QskSkinTransition::animation() const
{
    return m_data->animationHint;
}

void QskSkinTransition::updateSkin( QskSkin*, QskSkin* )
{
    // nop
}

void QskSkinTransition::process()
{
    qskApplicationAnimator->reset();

    auto skin1 = m_data->skins[ 0 ];
    auto skin2 = m_data->skins[ 1 ];

    QSet< QskAspect > candidates;

    if ( skin1 && skin2 )
    {
        if ( ( m_data->animationHint.duration > 0 ) && ( m_data->mask != 0 ) )
        {
            qskAddCandidates( m_data->mask, skin1, candidates );
            qskAddCandidates( m_data->mask, skin2, candidates );
        }
    }

    if ( !candidates.isEmpty() )
    {
        bool doGraphicFilter = m_data->mask & QskSkinTransition::Color;

        const auto windows = qGuiApp->topLevelWindows();

        for ( const auto window : windows )
        {
            if ( auto w = qobject_cast< QQuickWindow* >( window ) )
            {
                if ( !w->isVisible() || ( qskEffectiveSkin( w ) != skin2 ) )
                {
                    continue;
                }

                auto animator = new WindowAnimator( w );

                if ( doGraphicFilter )
                {
                    animator->addGraphicFilterAnimators(
                        m_data->animationHint, skin1, skin2 );

                    doGraphicFilter = false;
                }

                /*
                   finally we schedule the animators the hard way by running
                   over the the item trees.
                 */

                animator->addItemAspects( w->contentItem(),
                    m_data->animationHint, candidates, skin1, skin2 );

                qskApplicationAnimator->add( animator );
            }
        }

        qskApplicationAnimator->start();
    }

    // apply the changes
    updateSkin( skin1, skin2 );
}

bool QskSkinTransition::isRunning()
{
    if ( qskApplicationAnimator.exists() )
        return qskApplicationAnimator->isRunning();

    return false;
}

QVariant QskSkinTransition::animatedHint(
    const QQuickWindow* window, QskAspect aspect )
{
    if ( qskApplicationAnimator.exists() )
    {
        if ( const auto animator = qskApplicationAnimator->windowAnimator( window ) )
            return animator->animatedHint( aspect );
    }

    return QVariant();
}

QVariant QskSkinTransition::animatedGraphicFilter(
    const QQuickWindow* window, int graphicRole )
{
    if ( qskApplicationAnimator.exists() )
    {
        if ( const auto animator = qskApplicationAnimator->windowAnimator( window ) )
            return animator->animatedGraphicFilter( graphicRole );
    }

    return QVariant();
}

#include "QskSkinTransition.moc"
