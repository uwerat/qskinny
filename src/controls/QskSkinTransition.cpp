#include "QskSkinTransition.h"
#include "QskControl.h"
#include "QskSkin.h"
#include "QskSkinHintTable.h"
#include "QskColorFilter.h"
#include "QskHintAnimator.h"

#include <QGuiApplication>
#include <QQuickWindow>
#include <QVector>
#include <QObject>
#include <QPointer>
#include <QGlobalStatic>

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

        inline AnimatorCandidate( QskAspect::Aspect aspect,
                QVariant from, QVariant to ):
            aspect( aspect ),
            from( from ),
            to( to )
        {
        }

        QskAspect::Aspect aspect;
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
            switch( aspect.flagPrimitive() )
            {
                case QskAspect::GraphicRole:
                {
                    if ( !( mask & QskSkinTransition::Color ) )
                        continue;

                    const auto it1 = oldFilters.find( entry.second.toInt() );
                    const auto it2 = newFilters.find( entry.second.toInt() );

                    if ( it1 != oldFilters.end() || it2 != newFilters.end() )
                    {
                        const auto& f1 = ( it1 != oldFilters.end() ) ? it1->second : noFilter;
                        const auto& f2 = ( it2 != newFilters.end() ) ? it2->second : noFilter;

                        if ( f1 != f2 )
                        {
                            candidates += AnimatorCandidate( aspect, 
                                QVariant::fromValue( f1 ),
                                QVariant::fromValue( f2 ) );
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
    class AnimatorGroup final : public QObject
    {           
        Q_OBJECT

    public:         
        AnimatorGroup()
        {
        }
        
        void start()
        {
            m_notifyConnection = QskAnimator::addAdvanceHandler( this,
                SLOT( notify( QQuickWindow* ) ) );

            for ( auto& it : m_map )
                it.second.start();
        }

        bool isRunning() const
        {
            return !m_map.empty();
        }

        void reset()
        {
            disconnect( m_notifyConnection );

            m_map.clear();
            m_updateInfos.clear();
        }

        inline QVariant animatedHint( QskAspect::Aspect aspect ) const
        {
            auto it = m_map.find( aspect );
            if ( it != m_map.cend() )
            {
                const auto& animator = it->second;
                if ( animator.isRunning() )
                    return animator.currentValue();
            }

            return QVariant();
        }

        void addAnimators( QQuickItem* item,
            const QskAnimationHint& animatorHint,
            const QVector< AnimatorCandidate >& candidates, QskSkin* skin )
        {
            if ( !item->isVisible() )
                return;

            if ( auto control = qobject_cast< QskControl* >( item ) )
            {
                if ( control->isInitiallyPainted() && ( skin == control->effectiveSkin() ) )
                    addControlAnimators( control, animatorHint, candidates );
            }

            const auto children = item->childItems();
            for ( auto child : children )
                addAnimators( child, animatorHint, candidates, skin );
        }

    private Q_SLOTS:
        void notify( QQuickWindow* window )
        {
            if ( m_updateInfos.empty() )
                return;

            for ( auto& info : m_updateInfos )
            {
                QskControl* control = info.control;
                if ( control && control->window() == window )
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

            if ( !m_map.empty() )
            {
                if ( !m_map.begin()->second.isRunning() )
                    reset();
            }
        }

    private:
        void addControlAnimators( QskControl* control, const QskAnimationHint& animatorHint,
            const QVector< AnimatorCandidate >& candidates )
        {
            const auto subControls = control->subControls();

            for ( const auto& candidate : candidates )
            {
                using namespace QskAspect;

                if ( candidate.aspect.type() != Metric )
                {
                    if ( !( control->flags() & QQuickItem::ItemHasContents ) )
                    {
                        // while metrics might have an effect on layouts, we
                        // can safely ignore others for controls without content
                        continue;
                    }
                }

                const Subcontrol subControl = candidate.aspect.subControl();
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

                QskAspect::Aspect a = candidate.aspect;
                a.clearStates();
                a.addState( control->skinState() );
 
                const QskSkinHintStatus requestState = control->hintStatus( a );

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

                addAnimator( control, candidate, animatorHint );
                storeUpdateInfo( control, candidate.aspect );
            }
        }

        void addAnimator( QskControl* control,
            const AnimatorCandidate& candidate, QskAnimationHint animationHint )
        {
            auto it = m_map.find( candidate.aspect );
            if ( it != m_map.end() )
                return; // already there

            it = m_map.emplace( candidate.aspect, QskHintAnimator() ).first;
            auto& animator = it->second;

            animator.setAspect( candidate.aspect );
            animator.setStartValue( candidate.from );
            animator.setEndValue( candidate.to );

            animator.setDuration( animationHint.duration );
            animator.setEasingCurve( animationHint.type );

            animator.setControl( nullptr );
            animator.setWindow( control->window() );
        }

        inline void storeUpdateInfo( QskControl* control, QskAspect::Aspect aspect )
        {
            UpdateInfo info;
            info.control = control;

            info.updateModes = UpdateInfo::Update;
            if ( aspect.type() == QskAspect::Metric )
                info.updateModes |= UpdateInfo::Polish;

            auto it = std::lower_bound(
                m_updateInfos.begin(), m_updateInfos.end(), info, UpdateInfo::compare );

            if ( ( it != m_updateInfos.end() ) && ( it->control == info.control ) )
                it->updateModes |= info.updateModes;
            else
                m_updateInfos.insert( it, info );
        }

        std::map< QskAspect::Aspect, QskHintAnimator > m_map;
        std::vector< UpdateInfo > m_updateInfos; // vector: for fast iteration

        QMetaObject::Connection m_notifyConnection;
    };
}

Q_GLOBAL_STATIC( AnimatorGroup, qskSkinAnimator )

QskSkinTransition::QskSkinTransition():
    m_mask( QskSkinTransition::AllTypes )
{
    m_skins[0] = m_skins[1] = nullptr;
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
    m_skins[0] = skin;
}

QskSkin* QskSkinTransition::sourceSkin() const
{
    return m_skins[0];
}

void QskSkinTransition::setTargetSkin( QskSkin* skin )
{
    m_skins[1] = skin;
}

QskSkin* QskSkinTransition::targetSkin() const
{
    return m_skins[1];
}

void QskSkinTransition::setAnimation( QskAnimationHint animation )
{
    m_animation = animation;
}

QskAnimationHint QskSkinTransition::animation() const
{
    return m_animation;
}

void QskSkinTransition::updateSkin( QskSkin*, QskSkin* )
{
    // nop
}

void QskSkinTransition::process()
{
    if ( ( m_skins[0] == nullptr ) || ( m_skins[1] == nullptr ) )
    {
        // do nothing
        return;
    }

    qskSkinAnimator->reset();

    if ( ( m_animation.duration <= 0 ) || ( m_mask == 0 ) )
    {
        // no animations, we can apply the changes
        updateSkin( m_skins[0], m_skins[1] );
        return;
    }

    QVector< AnimatorCandidate > candidates;

    {
        // copy out all hints before updating the skin
        // - would be good to have Copy on Write here

        const auto oldTable = m_skins[0]->hintTable();
        const auto oldFilters = m_skins[0]->graphicFilters();

        // apply the changes
        updateSkin( m_skins[0], m_skins[1] );

        candidates = qskAnimatorCandidates( m_mask, oldTable, oldFilters,
            m_skins[1]->hintTable(), m_skins[1]->graphicFilters() );
    }

    if ( !candidates.isEmpty() )
    {
        /*
          finally we schedule the animators the hard way by running
          over the the item trees. for the moment o.k. but we should
          find a way not to create lots of identical animator objects for
          each object.
        */

        const auto windows = qGuiApp->topLevelWindows();
        for ( const auto window : windows )
        {
            if ( auto quickWindow = qobject_cast< const QQuickWindow* >( window ) )
            {
                qskSkinAnimator->addAnimators( quickWindow->contentItem(),
                    m_animation, candidates, m_skins[1] );
            }
        }

        qskSkinAnimator->start();
    }
}

bool QskSkinTransition::isRunning()
{
    return qskSkinAnimator.exists() && qskSkinAnimator->isRunning();
}

QVariant QskSkinTransition::animatedHint( QskAspect::Aspect aspect )
{
    if ( !qskSkinAnimator.exists() )
        return QVariant();

    return qskSkinAnimator->animatedHint( aspect );
}

#include "QskSkinTransition.moc"
