/*
 *  SPDX-FileCopyrightText: 2020 Arjen Hiemstra <ahiemstra@heimr.nl>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "shadowedrectangle.h"

#include <QQuickWindow>

#include "scenegraph/shadowedrectanglenode.h"
#include "scenegraph/paintedrectangleitem.h"

BorderGroup::BorderGroup( QObject* parent )
    : QObject( parent )
{
}

qreal BorderGroup::width() const
{
    return m_width;
}

void BorderGroup::setWidth( qreal newWidth )
{
    if( newWidth == m_width )
    {
        return;
    }

    m_width = newWidth;
    Q_EMIT changed();
}

QColor BorderGroup::color() const
{
    return m_color;
}

void BorderGroup::setColor( const QColor& newColor )
{
    if( newColor == m_color )
    {
        return;
    }

    m_color = newColor;
    Q_EMIT changed();
}

ShadowGroup::ShadowGroup( QObject* parent )
    : QObject( parent )
{
}

qreal ShadowGroup::size() const
{
    return m_size;
}

void ShadowGroup::setSize( qreal newSize )
{
    if( newSize == m_size )
    {
        return;
    }

    m_size = newSize;
    Q_EMIT changed();
}

qreal ShadowGroup::xOffset() const
{
    return m_xOffset;
}

void ShadowGroup::setXOffset( qreal newXOffset )
{
    if( newXOffset == m_xOffset )
    {
        return;
    }

    m_xOffset = newXOffset;
    Q_EMIT changed();
}

qreal ShadowGroup::yOffset() const
{
    return m_yOffset;
}

void ShadowGroup::setYOffset( qreal newYOffset )
{
    if( newYOffset == m_yOffset )
    {
        return;
    }

    m_yOffset = newYOffset;
    Q_EMIT changed();
}

QColor ShadowGroup::color() const
{
    return m_color;
}

void ShadowGroup::setColor( const QColor& newColor )
{
    if( newColor == m_color )
    {
        return;
    }

    m_color = newColor;
    Q_EMIT changed();
}

CornersGroup::CornersGroup( QObject* parent )
    : QObject( parent )
{
}

qreal CornersGroup::topLeft() const
{
    return m_topLeft;
}

void CornersGroup::setTopLeft( qreal newTopLeft )
{
    if( newTopLeft == m_topLeft )
    {
        return;
    }

    m_topLeft = newTopLeft;
    Q_EMIT changed();
}

qreal CornersGroup::topRight() const
{
    return m_topRight;
}

void CornersGroup::setTopRight( qreal newTopRight )
{
    if( newTopRight == m_topRight )
    {
        return;
    }

    m_topRight = newTopRight;
    Q_EMIT changed();
}

qreal CornersGroup::bottomLeft() const
{
    return m_bottomLeft;
}

void CornersGroup::setBottomLeft( qreal newBottomLeft )
{
    if( newBottomLeft == m_bottomLeft )
    {
        return;
    }

    m_bottomLeft = newBottomLeft;
    Q_EMIT changed();
}

qreal CornersGroup::bottomRight() const
{
    return m_bottomRight;
}

void CornersGroup::setBottomRight( qreal newBottomRight )
{
    if( newBottomRight == m_bottomRight )
    {
        return;
    }

    m_bottomRight = newBottomRight;
    Q_EMIT changed();
}

QVector4D CornersGroup::toVector4D( float all ) const
{
    return QVector4D
    {
        m_bottomRight < 0.0 ? all : m_bottomRight,
        m_topRight < 0.0 ? all : m_topRight,
        m_bottomLeft < 0.0 ? all : m_bottomLeft,
        m_topLeft < 0.0 ? all : m_topLeft
    };
}

ShadowedRectangle::ShadowedRectangle( QQuickItem* parentItem )
    : QQuickItem( parentItem )
    , m_border( new BorderGroup )
    , m_shadow( new ShadowGroup )
    , m_corners( new CornersGroup )
{
    setFlag( QQuickItem::ItemHasContents, true );

    connect( m_border.get(), &BorderGroup::changed, this, &ShadowedRectangle::update );
    connect( m_shadow.get(), &ShadowGroup::changed, this, &ShadowedRectangle::update );
    connect( m_corners.get(), &CornersGroup::changed, this, &ShadowedRectangle::update );
}

ShadowedRectangle::~ShadowedRectangle()
{
}

BorderGroup* ShadowedRectangle::border() const
{
    return m_border.get();
}

ShadowGroup* ShadowedRectangle::shadow() const
{
    return m_shadow.get();
}

CornersGroup* ShadowedRectangle::corners() const
{
    return m_corners.get();
}

qreal ShadowedRectangle::radius() const
{
    return m_radius;
}

void ShadowedRectangle::setRadius( qreal newRadius )
{
    if( newRadius == m_radius )
    {
        return;
    }

    m_radius = newRadius;

    if( !isSoftwareRendering() )
    {
        update();
    }

    Q_EMIT radiusChanged();
}

QColor ShadowedRectangle::color() const
{
    return m_color;
}

void ShadowedRectangle::setColor( const QColor& newColor )
{
    if( newColor == m_color )
    {
        return;
    }

    m_color = newColor;

    if( !isSoftwareRendering() )
    {
        update();
    }

    Q_EMIT colorChanged();
}

void ShadowedRectangle::componentComplete()
{
    QQuickItem::componentComplete();

    checkSoftwareItem();
}

bool ShadowedRectangle::isSoftwareRendering() const
{
    return false;
}

PaintedRectangleItem* ShadowedRectangle::softwareItem() const
{
    return m_softwareItem;
}

void ShadowedRectangle::itemChange( QQuickItem::ItemChange change, const QQuickItem::ItemChangeData& value )
{
    if( change == QQuickItem::ItemSceneChange && value.window )
    {
        checkSoftwareItem();
        //TODO: only conditionally emit?
        Q_EMIT softwareRenderingChanged();
    }
}

QSGNode* ShadowedRectangle::updatePaintNode( QSGNode* node, QQuickItem::UpdatePaintNodeData* data )
{
    Q_UNUSED( data );

    auto shadowNode = static_cast<ShadowedRectangleNode*>( node );

    if( !shadowNode )
    {
        shadowNode = new ShadowedRectangleNode{};

        // Cache lowPower state so we only execute the full check once.
        static bool lowPower = QByteArrayList{"1", "true"}.contains( qgetenv( "KIRIGAMI_LOWPOWER_HARDWARE" ).toLower() );

        if( lowPower )
        {
            shadowNode->setShaderType( ShadowedRectangleMaterial::ShaderType::LowPower );
        }
    }

    shadowNode->setBorderEnabled( m_border->isEnabled() );
    shadowNode->setRect( boundingRect() );
    shadowNode->setSize( m_shadow->size() );
    shadowNode->setRadius( m_corners->toVector4D( m_radius ) );
    shadowNode->setOffset( QVector2D{float( m_shadow->xOffset() ), float( m_shadow->yOffset() )} );
    shadowNode->setColor( m_color );
    shadowNode->setShadowColor( m_shadow->color() );
    shadowNode->setBorderWidth( m_border->width() );
    shadowNode->setBorderColor( m_border->color() );
    shadowNode->updateGeometry();
    return shadowNode;
}

void ShadowedRectangle::checkSoftwareItem()
{
    if( !m_softwareItem && isSoftwareRendering() )
    {
        m_softwareItem = new PaintedRectangleItem{this};
        // The software item is added as a "normal" child item, this means it
        // will be part of the normal item sort order. Since there is no way to
        // control the ordering of children, just make sure to have a very low Z
        // value for the child, to force it to be the lowest item.
        m_softwareItem->setZ( -99.0 );

        auto updateItem = [this]()
        {
            auto borderWidth = m_border->width();
            auto rect = boundingRect().adjusted( -borderWidth / 2, -borderWidth / 2, borderWidth / 2, borderWidth / 2 );
            m_softwareItem->setX( -borderWidth / 2 );
            m_softwareItem->setY( -borderWidth / 2 );
            m_softwareItem->setSize( rect.size() );
            m_softwareItem->setColor( m_color );
            m_softwareItem->setRadius( m_radius );
            m_softwareItem->setBorderWidth( borderWidth );
            m_softwareItem->setBorderColor( m_border->color() );
        };

        updateItem();

        connect( this, &ShadowedRectangle::widthChanged, m_softwareItem, updateItem );
        connect( this, &ShadowedRectangle::heightChanged, m_softwareItem, updateItem );
        connect( this, &ShadowedRectangle::colorChanged, m_softwareItem, updateItem );
        connect( this, &ShadowedRectangle::radiusChanged, m_softwareItem, updateItem );
        connect( m_border.get(), &BorderGroup::changed, m_softwareItem, updateItem );
        setFlag( QQuickItem::ItemHasContents, false );
    }
}
