/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "QskSkinTransition.h"
#include "QskColorFilter.h"
#include "QskControl.h"
#include "QskWindow.h"
#include "QskAnimationHint.h"
#include "QskHintAnimator.h"
#include "QskSkin.h"
#include "QskSkinHintTable.h"
#include "QskFontRole.h"
#include "QskAspect.h"

#include <qglobalstatic.h>
#include <qguiapplication.h>
#include <qobject.h>
#include <qvector.h>
#include <qhash.h>

#include <vector>

#if 1

/*
    We need to find a way how to retrieve the extra states that are provided
    by QskSkinlet::sampleStates from the control in a generic way. For the moment we
    return a hardcoded list of states that we know because we know. TODO ...
 */
#include "QskMenu.h"
#include "QskRadioBox.h"
#include "QskSegmentedBar.h"
#include "QskListView.h"

static QskAspect::State qskSelectedSampleState( const QskControl* control )
{
    if ( qobject_cast< const QskMenu* >( control ) )
        return QskMenu::Selected;

    if ( qobject_cast< const QskRadioBox* >( control ) )
        return QskRadioBox::Selected;

    if ( qobject_cast< const QskSegmentedBar* >( control ) )
        return QskSegmentedBar::Selected;

    if ( qobject_cast< const QskListView* >( control ) )
        return QskListView::Selected;

    return QskAspect::NoState;
}

#endif

static bool qskHasHintTable( const QskSkin* skin, const QskSkinHintTable& hintTable )
{
    return skin->hintTable().hints().isSharedWith( hintTable.hints() );
}

static void qskSendStyleEventRecursive( QQuickItem* item )
{
    QEvent event( QEvent::StyleChange );
    QCoreApplication::sendEvent( item, &event );

    const auto children = item->childItems();
    for ( auto child : children )
        qskSendStyleEventRecursive( child );
}

static void qskAddCandidates( const QskSkinTransition::Type mask,
    const QHash< QskAspect, QVariant >& hints, QSet< QskAspect >& candidates )
{
    for ( auto it = hints.constBegin(); it != hints.constEnd(); ++it )
    {
        const auto aspect = it.key().trunk();

        if ( aspect.isAnimator() )
            continue;

        bool isCandidate = false;

        switch( aspect.type() )
        {
            case QskAspect::NoType:
            {
                if ( aspect.primitive() == QskAspect::GraphicRole )
                {
                    isCandidate = mask & QskSkinTransition::Color;
                }
                else if ( aspect.primitive() == QskAspect::FontRole )
                {
                    isCandidate = mask & QskSkinTransition::Metric;
                }
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
        enum UpdateMode : quint8
        {
            Polish = 1,
            Update = 2
        };

        static inline bool compare( const UpdateInfo& i1, const UpdateInfo& i2 )
        {
            return i1.control.data() < i2.control.data();
        }

        QPointer< QskControl > control;
        quint8 updateModes = 0;
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
        QVariant animatedFontSize( const QskFontRole& ) const;

        void addGraphicFilterAnimators( const QskAnimationHint&,
            const QHash< int, QskColorFilter >&, const QHash< int, QskColorFilter >& );

        void addFontSizeAnimators( const QskAnimationHint&,
            const QHash< QskFontRole, QFont >&, const QHash< QskFontRole, QFont >& );

        void addItemAspects( QQuickItem*,
            const QskAnimationHint&, const QSet< QskAspect >&,
            const QskSkinHintTable&, const QskSkinHintTable& );

        void update();

      private:

        bool isControlAffected( const QskControl*,
            const QVector< QskAspect::Subcontrol >&, QskAspect ) const;

        void addHint( const QskControl*,
            const QskAnimationHint&, QskAspect,
            const QskSkinHintTable&, const QskSkinHintTable& );

        void storeAnimator( const QskControl*, const QskAspect,
            const QVariant&, const QVariant&, QskAnimationHint );

        void storeUpdateInfo( const QskControl*, QskAspect );

        QQuickWindow* m_window;

        QHash< QskAspect, HintAnimator > m_animatorMap;
        QHash< int, QskVariantAnimator > m_graphicFilterAnimatorMap;
        QHash< QskFontRole, QskVariantAnimator > m_fontSizeAnimatorMap;

        std::vector< UpdateInfo > m_updateInfos; // vector: for fast iteration
    };

    class ApplicationAnimator : public QObject
    {
        Q_OBJECT

      public:
        ~ApplicationAnimator() override;

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
        it.start();

    for ( auto& it : m_graphicFilterAnimatorMap )
        it.start();

    for ( auto& it : m_fontSizeAnimatorMap )
        it.start();
}

bool WindowAnimator::isRunning() const
{
    if ( !m_animatorMap.empty() )
    {
        const auto& animator = m_animatorMap.constBegin().value();
        if ( animator.isRunning() )
            return true;
    }

    if ( !m_graphicFilterAnimatorMap.empty() )
    {
        const auto& animator = m_graphicFilterAnimatorMap.constBegin().value();
        if ( animator.isRunning() )
            return true;
    }

    if ( !m_fontSizeAnimatorMap.empty() )
    {
        const auto& animator = m_fontSizeAnimatorMap.constBegin().value();
        if ( animator.isRunning() )
            return true;
    }

    return false;
}

inline QVariant WindowAnimator::animatedHint( QskAspect aspect ) const
{
    auto it = m_animatorMap.constFind( aspect );
    if ( it != m_animatorMap.constEnd() )
    {
        const auto& animator = it.value();
        if ( animator.isRunning() )
            return animator.currentValue();
    }

    return QVariant();
}

inline QVariant WindowAnimator::animatedGraphicFilter( int graphicRole ) const
{
    auto it = m_graphicFilterAnimatorMap.constFind( graphicRole );
    if ( it != m_graphicFilterAnimatorMap.constEnd() )
    {
        const auto& animator = it.value();
        if ( animator.isRunning() )
            return animator.currentValue();
    }

    return QVariant();
}

inline QVariant WindowAnimator::animatedFontSize( const QskFontRole& fontRole ) const
{
    auto it = m_fontSizeAnimatorMap.constFind( fontRole );
    if ( it != m_fontSizeAnimatorMap.constEnd() )
    {
        const auto& animator = it.value();
        if ( animator.isRunning() )
            return animator.currentValue();
    }

    return QVariant();
}

void WindowAnimator::addGraphicFilterAnimators(
    const QskAnimationHint& animatorHint,
    const QHash< int, QskColorFilter >& filters1,
    const QHash< int, QskColorFilter >& filters2 )
{
    const QskColorFilter noFilter;

    for ( auto it2 = filters2.constBegin(); it2 != filters2.constEnd(); ++it2 )
    {
        auto it1 = filters1.constFind( it2.key() );
        if ( it1 == filters1.constEnd() )
            it1 = filters1.constFind( 0 );

        const auto& f1 = ( it1 != filters1.constEnd() ) ? it1.value() : noFilter;
        const auto& f2 = it2.value();

        if ( f1 != f2 )
        {
            QskVariantAnimator animator;
            animator.setWindow( m_window );
            animator.setDuration( animatorHint.duration );
            animator.setEasingCurve( animatorHint.type );
            animator.setStartValue( QVariant::fromValue( f1 ) );
            animator.setEndValue( QVariant::fromValue( f2 ) );

            m_graphicFilterAnimatorMap.insert( it2.key(), animator );
        }
    }
}

// from QskSkin.cpp
extern QFont qskResolvedFont(
    const QHash< QskFontRole, QFont >&, const QskFontRole& );

void WindowAnimator::addFontSizeAnimators(
    const QskAnimationHint& animatorHint,
    const QHash< QskFontRole, QFont >& fonts1,
    const QHash< QskFontRole, QFont >& fonts2 )
{
    for ( int i = 0; i <= QskFontRole::Display; i++ )
    {
        for ( int j = 0; j <= QskFontRole::VeryHigh; j++ )
        {
            const QskFontRole fontRole(
                static_cast< QskFontRole::Category >( i ),
                static_cast< QskFontRole::Emphasis >( j )
            );

            const auto size1 = qskResolvedFont( fonts1, fontRole ).pixelSize();
            const auto size2 = qskResolvedFont( fonts2, fontRole ).pixelSize();

            if ( ( size1 > 0 ) && ( size2 > 0 ) && ( size1 != size2 ) )
            {
                QskVariantAnimator animator;
                animator.setWindow( m_window );
                animator.setDuration( animatorHint.duration );
                animator.setEasingCurve( animatorHint.type );
                animator.setStartValue( QVariant::fromValue( size1 ) );
                animator.setEndValue( QVariant::fromValue( size2 ) );

                m_fontSizeAnimatorMap.insert( fontRole, animator );
            }
        }
    }
}

void WindowAnimator::addItemAspects( QQuickItem* item,
    const QskAnimationHint& animatorHint, const QSet< QskAspect >& candidates,
    const QskSkinHintTable& table1, const QskSkinHintTable& table2 )
{
    if ( auto control = qskControlCast( ( const QQuickItem* )item ) )
    {
        if ( control->isVisible() && control->isInitiallyPainted() &&
            qskHasHintTable( control->effectiveSkin(), table2 ) )
        {
            const auto subControls = control->subControls();

            const auto& localTable = control->hintTable();

            for ( auto aspect : candidates )
            {
                if ( isControlAffected( control, subControls, aspect ) )
                {
                    aspect.setVariation( control->effectiveVariation() );
                    aspect.setStates( control->skinStates() );
                    aspect.setSection( control->section() );

                    if ( !localTable.resolvedHint( aspect ) )
                        addHint( control, animatorHint, aspect, table1, table2 );

                    if ( auto state = qskSelectedSampleState( control ) )
                    {
                        aspect.addStates( state );
                        if ( !localTable.resolvedHint( aspect ) )
                            addHint( control, animatorHint, aspect, table1, table2 );
                    }
                }
            }

#if 1
            /*
                As it is hard to identify which controls depend on the animated
                graphic filters we schedule an initial update and let the
                controls do the rest: see QskSkinnable::effectiveGraphicFilter
             */
            item->update();
#endif
        }
    }

    const auto children = item->childItems();
    for ( auto child : children )
        addItemAspects( child, animatorHint, candidates, table1, table2 );
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

void WindowAnimator::addHint( const QskControl* control,
    const QskAnimationHint& animatorHint, QskAspect aspect,
    const QskSkinHintTable& table1, const QskSkinHintTable& table2 )
{
    QskAspect r1, r2;

    const auto v1 = table1.resolvedHint( aspect, &r1 );
    const auto v2 = table2.resolvedHint( aspect, &r2 );

    if ( v1 && v2 )
    {
        if ( r1.section() == r2.section() )
            aspect.setSection( r2.section() );

        if ( r1.variation() == r2.variation() )
            aspect.setVariation( r2.variation() );

        if ( r1.states() == r2.states() )
            aspect.setStates( r2.states() );

        bool accecptIdentity = false;
        if ( aspect != aspect.trunk() )
        {
            /*
                We might need an animator even if the values do not differ
                to prevent effectiveSkinHint to find its value from
                another animator that might have been started with
                less extra bits.
             */
            accecptIdentity = true;
        }

        if ( QskVariantAnimator::maybeInterpolate( *v1, *v2, accecptIdentity ) )
        {
            storeAnimator( control, aspect, *v1, *v2, animatorHint );
            storeUpdateInfo( control, aspect );
        }
    }
    else if ( v1 )
    {
        aspect.setVariation( r1.variation() );
        aspect.setStates( r1.states() );

        storeAnimator( control, aspect, *v1, QVariant(), animatorHint );
        storeUpdateInfo( control, aspect );
    }
    else if ( v2 )
    {
        aspect.setVariation( r1.variation() );
        aspect.setStates( r1.states() );

        storeAnimator( control, aspect, QVariant(), *v2, animatorHint );
        storeUpdateInfo( control, aspect );
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

inline void WindowAnimator::storeAnimator(
    const QskControl* control, const QskAspect aspect,
    const QVariant& value1, const QVariant& value2, QskAnimationHint hint )
{
    if ( m_animatorMap.find( aspect ) == m_animatorMap.cend() )
    {
        m_animatorMap.insert( aspect,
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
    struct
    {
        QskSkinHintTable hintTable;
        QHash< int, QskColorFilter > graphicFilters;
        QHash< QskFontRole, QFont > fontTable;
    } tables[ 2 ];

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

void QskSkinTransition::setSourceSkin( const QskSkin* skin )
{
    auto& tables = m_data->tables[ 0 ];

    tables.hintTable = skin->hintTable();
    tables.graphicFilters = skin->graphicFilters();
    tables.fontTable = skin->fontTable();
}

void QskSkinTransition::setTargetSkin( const QskSkin* skin )
{
    auto& tables = m_data->tables[ 1 ];

    tables.hintTable = skin->hintTable();
    tables.graphicFilters = skin->graphicFilters();
    tables.fontTable = skin->fontTable();
}

void QskSkinTransition::run( const QskAnimationHint& animationHint )
{
    qskApplicationAnimator->reset();

    const auto& table1 = m_data->tables[ 0 ].hintTable;
    const auto& table2 = m_data->tables[ 1 ].hintTable;

    const auto& graphicFilters1 = m_data->tables[ 0 ].graphicFilters;
    const auto& graphicFilters2 = m_data->tables[ 1 ].graphicFilters;

    const auto& fontTable1 = m_data->tables[ 0 ].fontTable;
    const auto& fontTable2 = m_data->tables[ 1 ].fontTable;

    QSet< QskAspect > candidates;

    if ( ( animationHint.duration > 0 ) && ( m_data->mask != 0 ) )
    {
        qskAddCandidates( m_data->mask, table1.hints(), candidates );
        qskAddCandidates( m_data->mask, table2.hints(), candidates );
    }

    if ( !candidates.isEmpty() )
    {
        bool doGraphicFilter = m_data->mask & QskSkinTransition::Color;
        bool doFont = m_data->mask & QskSkinTransition::Metric;

        const auto windows = qGuiApp->topLevelWindows();

        for ( const auto window : windows )
        {
            if ( auto w = qobject_cast< QQuickWindow* >( window ) )
            {
                if ( !w->isVisible() )
                    continue;

                if ( !qskHasHintTable( qskEffectiveSkin( w ), table2 ) )
                    continue;

                auto animator = new WindowAnimator( w );

                if ( doGraphicFilter )
                {
                    animator->addGraphicFilterAnimators( animationHint,
                        graphicFilters1, graphicFilters2 );

                    doGraphicFilter = false;
                }

                if ( doFont )
                {
                    animator->addFontSizeAnimators( animationHint,
                        fontTable1, fontTable2 );
                }

                /*
                   finally we schedule the animators the hard way by running
                   over the the item trees.
                 */

                animator->addItemAspects( w->contentItem(),
                    animationHint, candidates, table1, table2 );

                qskApplicationAnimator->add( animator );
            }
        }

        qskApplicationAnimator->start();
    }
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

QVariant QskSkinTransition::animatedFontSize(
    const QQuickWindow* window, const QskFontRole& fontRole )
{
    if ( qskApplicationAnimator.exists() )
    {
        if ( const auto animator = qskApplicationAnimator->windowAnimator( window ) )
            return animator->animatedFontSize( fontRole );
    }

    return QVariant();
}

#include "QskSkinTransition.moc"
