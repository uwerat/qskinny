#include "QskSkinTransition.h"
#include "QskColorFilter.h"
#include "QskControl.h"
#include "QskHintAnimator.h"
#include "QskSkin.h"
#include "QskSkinHintTable.h"

#include <qglobalstatic.h>
#include <qguiapplication.h>
#include <qobject.h>
#include <qquickwindow.h>
#include <qvector.h>

#include <unordered_map>
#include <vector>

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

    class AnimatorCandidate
    {
      public:
        AnimatorCandidate() = default;

        inline AnimatorCandidate( QskAspect aspect, QVariant from, QVariant to )
            : aspect( aspect )
            , from( from )
            , to( to )
        {
        }

        QskAspect aspect;
        QVariant from;
        QVariant to;
    };
}

static QVector< AnimatorCandidate > qskAnimatorCandidates(
    QskSkinTransition::Type mask,
    const QskSkinHintTable& oldTable,
    const std::unordered_map< int, QskColorFilter >& oldFilters,
    const QskSkinHintTable& newTable,
    const std::unordered_map< int, QskColorFilter >& newFilters )
{
    // building a list of candidates for animations by comparing
    // the old/new set of skin hints

    const QskColorFilter noFilter;
    QVector< AnimatorCandidate > candidates;

    if ( !oldTable.hasHints() )
        return candidates;

    for ( const auto& entry : newTable.hints() )
    {
        const auto aspect = entry.first;

        if ( aspect.isAnimator() )
            continue;

        const auto type = aspect.type();

        if ( type == QskAspect::Flag )
        {
            switch ( aspect.flagPrimitive() )
            {
                case QskAspect::GraphicRole:
                {
                    if ( !( mask & QskSkinTransition::Color ) )
                        continue;

                    int role1 = 0;

                    const auto value = oldTable.resolvedHint( aspect );
                    if ( value )
                        role1 = value->toInt();

                    const int role2 = entry.second.toInt();

                    /*
                        When the role is the same we already have the animators
                        for the graphic filter table running
                     */
                    if ( role1 != role2 )
                    {
                        const auto it1 = oldFilters.find( role1 );
                        const auto it2 = newFilters.find( role2 );

                        if ( it1 != oldFilters.end() || it2 != newFilters.end() )
                        {
                            const auto& f1 = ( it1 != oldFilters.end() ) ? it1->second : noFilter;
                            const auto& f2 = ( it2 != newFilters.end() ) ? it2->second : noFilter;

                            if ( f1 != f2 )
                            {
                                candidates += AnimatorCandidate( aspect,
                                    QVariant::fromValue( f1 ), QVariant::fromValue( f2 ) );
                            }
                        }
                    }
                    break;
                }
                case QskAspect::FontRole:
                {
                    if ( !( mask & QskSkinTransition::Metric ) )
                        continue;

                    break;
                }
                default:
                    ;
            }
        }
        else
        {
            if ( ( ( type == QskAspect::Color ) && ( mask & QskSkinTransition::Color ) ) ||
                ( ( type == QskAspect::Metric ) && ( mask & QskSkinTransition::Metric ) ) )
            {
                auto value = oldTable.resolvedHint( aspect );
                if ( value == nullptr && aspect.subControl() != QskAspect::Control )
                {
                    auto a = aspect;
                    a.setSubControl( QskAspect::Control );
                    a.clearStates();
                    value = oldTable.resolvedHint( a );
                }

                /*
                    We are missing transitions, when a hint in newTable
                    gets resolved from QskControl. TODO ...
                 */
                if ( value && *value != entry.second )
                    candidates += AnimatorCandidate( aspect, *value, entry.second );
            }
        }
    }

    return candidates;
}

namespace
{
    class AnimatorGroup
    {
      public:
        AnimatorGroup( QQuickWindow* window = nullptr )
            : m_window( window )
        {
        }

        inline const QQuickWindow* window() const
        {
            return m_window;
        }

        void start()
        {
            for ( auto& it : m_hintAnimatorMap )
                it.second.start();

            for ( auto& it : m_graphicFilterAnimatorMap )
                it.second.start();
        }

        bool isRunning() const
        {
            if ( !m_hintAnimatorMap.empty() )
            {
                const auto& animator = m_hintAnimatorMap.begin()->second;
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

        inline QVariant animatedHint( QskAspect aspect ) const
        {
            auto it = m_hintAnimatorMap.find( aspect );
            if ( it != m_hintAnimatorMap.cend() )
            {
                const auto& animator = it->second;
                if ( animator.isRunning() )
                    return animator.currentValue();
            }

            return QVariant();
        }

        inline QVariant animatedGraphicFilter( int graphicRole ) const
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

        void addGraphicFilterAnimators(
            const QskAnimationHint& animatorHint,
            const std::unordered_map< int, QskColorFilter >& oldFilters,
            const std::unordered_map< int, QskColorFilter >& newFilters )
        {
            const QskColorFilter noFilter;

            for ( auto it2 = newFilters.begin(); it2 != newFilters.end(); ++it2 )
            {
                auto it1 = oldFilters.find( it2->first );
                if ( it1 == oldFilters.cend() )
                    it1 = oldFilters.find( 0 );

                const auto& f1 = ( it1 != oldFilters.cend() ) ? it1->second : noFilter;
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

        void addAnimators( QQuickItem* item, const QskAnimationHint& animatorHint,
            const QVector< AnimatorCandidate >& candidates, QskSkin* skin )
        {
            if ( !item->isVisible() )
                return;

            if ( auto control = qskControlCast( item ) )
            {
                if ( control->isInitiallyPainted() && ( skin == control->effectiveSkin() ) )
                {
                    addControlAnimators( control, animatorHint, candidates );
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
                addAnimators( child, animatorHint, candidates, skin );
        }

        void update()
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

      private:

        void addControlAnimators( QskControl* control, const QskAnimationHint& animatorHint,
            const QVector< AnimatorCandidate >& candidates )
        {
            const auto subControls = control->subControls();

            for ( const auto& candidate : candidates )
            {
                if ( !candidate.aspect.isMetric() )
                {
                    if ( !( control->flags() & QQuickItem::ItemHasContents ) )
                    {
                        // while metrics might have an effect on layouts, we
                        // can safely ignore others for controls without content
                        continue;
                    }
                }

                const auto subControl = candidate.aspect.subControl();
                if ( subControl != control->effectiveSubcontrol( subControl ) )
                {
                    // The control uses subcontrol redirection, so we can assume it
                    // is not interested in this subcontrol.
                    continue;
                }

                if ( subControl != QskAspect::Control )
                {
                    if ( !subControls.contains( subControl ) )
                    {
                        // the control is not interested in the aspect
                        continue;
                    }
                }
                else
                {
                    if ( !control->autoFillBackground() )
                    {
                        // no need to animate the background unless we show it
                        continue;
                    }
                }

                auto a = candidate.aspect;
                a.setState( control->skinState() );

                const auto requestState = control->hintStatus( a );

                if ( requestState.source != QskSkinHintStatus::Skin )
                {
                    // The control does not resolve the aspect from the skin.
                    continue;
                }

                if ( candidate.aspect != requestState.aspect )
                {
                    // the aspect was resolved to something else
                    continue;
                }

                addAnimator( control->window(), candidate, animatorHint );
                storeUpdateInfo( control, candidate.aspect );
            }
        }

        void addAnimator( QQuickWindow* window,
            const AnimatorCandidate& candidate, QskAnimationHint animationHint )
        {
            auto it = m_hintAnimatorMap.find( candidate.aspect );
            if ( it != m_hintAnimatorMap.end() )
                return; // already there

            it = m_hintAnimatorMap.emplace( candidate.aspect, QskHintAnimator() ).first;
            auto& animator = it->second;

            animator.setAspect( candidate.aspect );
            animator.setStartValue( candidate.from );
            animator.setEndValue( candidate.to );

            animator.setDuration( animationHint.duration );
            animator.setEasingCurve( animationHint.type );
            animator.setUpdateFlags( animationHint.updateFlags );

            animator.setControl( nullptr );
            animator.setWindow( window );
        }

        inline void storeUpdateInfo( QskControl* control, QskAspect aspect )
        {
            UpdateInfo info;
            info.control = control;

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

        QQuickWindow* m_window;
        std::unordered_map< QskAspect, QskHintAnimator > m_hintAnimatorMap;
        std::unordered_map< int, QskVariantAnimator > m_graphicFilterAnimatorMap;
        std::vector< UpdateInfo > m_updateInfos; // vector: for fast iteration
    };

    class AnimatorGroups : public QObject
    {
        Q_OBJECT

      public:
        ~AnimatorGroups()
        {
            reset();
        }

        inline AnimatorGroup* animatorGroup( const QQuickWindow* window )
        {
            if ( !m_animatorGroups.empty() && window )
            {
                for ( auto group : m_animatorGroups )
                {
                    if ( group->window() == window )
                        return group;
                }
            }

            return nullptr;
        }

        void add( AnimatorGroup* group )
        {
            m_animatorGroups.push_back( group );
        }

        void start()
        {
            m_connections[0] = QskAnimator::addAdvanceHandler(
                this, SLOT(notify(QQuickWindow*)), Qt::UniqueConnection );

            m_connections[1] = QskAnimator::addCleanupHandler(
                this, SLOT(cleanup(QQuickWindow*)), Qt::UniqueConnection );

            for ( auto& group : m_animatorGroups )
                group->start();
        }

        void reset()
        {
            qDeleteAll( m_animatorGroups );
            m_animatorGroups.clear();

            disconnect( m_connections[0] );
            disconnect( m_connections[1] );
        }

        inline bool isRunning() const
        {
            return !m_animatorGroups.empty();
        }

      private Q_SLOTS:
        void notify( QQuickWindow* window )
        {
            for ( auto& group : m_animatorGroups )
            {
                if ( group->window() == window )
                {
                    group->update();
                    return;
                }
            }
        }

        void cleanup( QQuickWindow* window )
        {
            for ( auto it = m_animatorGroups.begin();
                it != m_animatorGroups.end(); ++it )
            {
                auto group = *it;
                if ( group->window() == window )
                {
                    if ( !group->isRunning() )
                    {
                        // The notification might be for other animators

                        m_animatorGroups.erase( it );
                        delete group;
                    }

                    break;
                }
            }

            if ( m_animatorGroups.empty() )
                reset();
        }

      private:
        /*
            It should be possible to find an implementation, that interpolates
            a skin hint only once for all windows. But as our animtors are driven by
            QQuickWindow::afterAnimating the code will have to be somehow tricky.
            But as skin transitions are no operations, that happen often, we can accept
            the overhaed of the current implementation and do the finetuning later.
         */
        std::vector< AnimatorGroup* > m_animatorGroups;
        QMetaObject::Connection m_connections[2];
    };
}

Q_GLOBAL_STATIC( AnimatorGroups, qskSkinAnimator )

QskSkinTransition::QskSkinTransition()
    : m_mask( QskSkinTransition::AllTypes )
{
    m_skins[ 0 ] = m_skins[ 1 ] = nullptr;
}

QskSkinTransition::~QskSkinTransition()
{
}

void QskSkinTransition::setMask( Type type )
{
    m_mask = type;
}

QskSkinTransition::Type QskSkinTransition::mask() const
{
    return m_mask;
}

void QskSkinTransition::setSourceSkin( QskSkin* skin )
{
    m_skins[ 0 ] = skin;
}

QskSkin* QskSkinTransition::sourceSkin() const
{
    return m_skins[ 0 ];
}

void QskSkinTransition::setTargetSkin( QskSkin* skin )
{
    m_skins[ 1 ] = skin;
}

QskSkin* QskSkinTransition::targetSkin() const
{
    return m_skins[ 1 ];
}

void QskSkinTransition::setAnimation( QskAnimationHint animationHint )
{
    m_animationHint = animationHint;
}

QskAnimationHint QskSkinTransition::animation() const
{
    return m_animationHint;
}

void QskSkinTransition::updateSkin( QskSkin*, QskSkin* )
{
    // nop
}

void QskSkinTransition::process()
{
    if ( ( m_skins[ 0 ] == nullptr ) || ( m_skins[ 1 ] == nullptr ) )
    {
        // do nothing
        return;
    }

    qskSkinAnimator->reset();

    if ( ( m_animationHint.duration <= 0 ) || ( m_mask == 0 ) )
    {
        // no animations, we can apply the changes
        updateSkin( m_skins[ 0 ], m_skins[ 1 ] );
        return;
    }

    QVector< AnimatorCandidate > candidates;
    const auto oldFilters = m_skins[ 0 ]->graphicFilters();

    {
        // copy out all hints before updating the skin
        // - would be good to have Copy on Write here

        const auto oldTable = m_skins[ 0 ]->hintTable();

        // apply the changes
        updateSkin( m_skins[ 0 ], m_skins[ 1 ] );

        candidates = qskAnimatorCandidates( m_mask, oldTable, oldFilters,
            m_skins[ 1 ]->hintTable(), m_skins[ 1 ]->graphicFilters() );
    }

    if ( !candidates.isEmpty() )
    {
        bool doGraphicFilter = m_mask & QskSkinTransition::Color;

        const auto windows = qGuiApp->topLevelWindows();

        for ( const auto window : windows )
        {
            if ( auto quickWindow = qobject_cast< QQuickWindow* >( window ) )
            {
                if ( !quickWindow->isVisible() )
                    continue;

                auto* group = new AnimatorGroup( quickWindow );

                if ( doGraphicFilter )
                {
                    group->addGraphicFilterAnimators(
                        m_animationHint, oldFilters,
                        m_skins[ 1 ]->graphicFilters() );

                    doGraphicFilter = false;
                }

                /*
                   finally we schedule the animators the hard way by running
                   over the the item trees.
                 */

                group->addAnimators( quickWindow->contentItem(),
                    m_animationHint, candidates, m_skins[ 1 ] );

                qskSkinAnimator->add( group );
            }
        }

        qskSkinAnimator->start();
    }
}

bool QskSkinTransition::isRunning()
{
    if ( qskSkinAnimator.exists() )
        return qskSkinAnimator->isRunning();

    return false;
}

QVariant QskSkinTransition::animatedHint(
    const QQuickWindow* window, QskAspect aspect )
{
    if ( qskSkinAnimator.exists() )
    {
        if ( const auto group = qskSkinAnimator->animatorGroup( window ) )
            return group->animatedHint( aspect );
    }

    return QVariant();
}

QVariant QskSkinTransition::animatedGraphicFilter(
    const QQuickWindow* window, int graphicRole )
{
    if ( qskSkinAnimator.exists() )
    {
        if ( const auto group = qskSkinAnimator->animatorGroup( window ) )
            return group->animatedGraphicFilter( graphicRole );
    }

    return QVariant();
}

#include "QskSkinTransition.moc"
