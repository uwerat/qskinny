/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskGraphic.h"
#include "QskGraphicPaintEngine.h"
#include "QskColorFilter.h"
#include "QskPainterCommand.h"

#include <QVector>
#include <QPainter>
#include <QPaintEngine>
#include <QImage>
#include <QPixmap>
#include <QPainterPath>
#include <QGuiApplication>
#include <QtMath>

#include <algorithm>

static inline void qskInsertColor( const QColor& color,
    QVector< QRgb >& colorTable )
{
    const QRgb rgb = color.rgb(); // dropping the alpha value

    const auto it = std::lower_bound( colorTable.begin(), colorTable.end(), rgb );
    if ( ( it == colorTable.end() ) || ( *it != rgb ) )
        colorTable.insert( it, rgb );
}

static inline qreal qskDevicePixelRatio()
{
    return qGuiApp ? qGuiApp->devicePixelRatio() : 1.0;
}

static bool qskHasScalablePen( const QPainter* painter )
{
    const QPen pen = painter->pen();

    bool scalablePen = false;

    if ( pen.style() != Qt::NoPen && pen.brush().style() != Qt::NoBrush )
    {
        scalablePen = !pen.isCosmetic();
        if ( !scalablePen && pen.widthF() == 0.0 )
        {
            const QPainter::RenderHints hints = painter->renderHints();
            if ( hints.testFlag( QPainter::NonCosmeticDefaultPen ) )
                scalablePen = true;
        }
    }

    return scalablePen;
}

static QRectF qskStrokedPathRect(
    const QPainter* painter, const QPainterPath& path )
{
    QPainterPathStroker stroker;
    stroker.setWidth( painter->pen().widthF() );
    stroker.setCapStyle( painter->pen().capStyle() );
    stroker.setJoinStyle( painter->pen().joinStyle() );
    stroker.setMiterLimit( painter->pen().miterLimit() );

    QRectF rect;
    if ( qskHasScalablePen( painter ) )
    {
        QPainterPath stroke = stroker.createStroke( path );
        rect = painter->transform().map( stroke ).boundingRect();
    }
    else
    {
        QPainterPath mappedPath = painter->transform().map( path );
        mappedPath = stroker.createStroke( mappedPath );

        rect = mappedPath.boundingRect();
    }

    return rect;
}

static inline void qskExecCommand(
    QPainter* painter, const QskPainterCommand& cmd,
    const QskColorFilter& colorFilter,
    QskGraphic::RenderHints renderHints,
    const QTransform& transform,
    const QTransform* initialTransform )
{
    switch( cmd.type() )
    {
        case QskPainterCommand::Path:
        {
            bool doMap = false;

            if ( painter->transform().isScaling() )
            {
                bool isCosmetic = painter->pen().isCosmetic();
                if ( isCosmetic && painter->pen().widthF() == 0.0 )
                {
                    QPainter::RenderHints hints = painter->renderHints();
                    if ( hints.testFlag( QPainter::NonCosmeticDefaultPen ) )
                        isCosmetic = false;
                }

                if ( isCosmetic )
                {
                    // OpenGL2 seems to be buggy for cosmetic pens.
                    // It interpolates curves in too rough steps then

                    doMap = painter->paintEngine()->type() == QPaintEngine::OpenGL2;
                }
                else
                {
                    doMap = renderHints.testFlag( QskGraphic::RenderPensUnscaled );
                }
            }

            if ( doMap )
            {
                const QTransform tr = painter->transform();

                painter->resetTransform();

                QPainterPath path = tr.map( *cmd.path() );
                if ( initialTransform )
                {
                    painter->setTransform( *initialTransform );
                    path = initialTransform->inverted().map( path );
                }

                painter->drawPath( path );

                painter->setTransform( tr );
            }
            else
            {
                painter->drawPath( *cmd.path() );
            }
            break;
        }
        case QskPainterCommand::Pixmap:
        {
            const QskPainterCommand::PixmapData* data = cmd.pixmapData();
            painter->drawPixmap( data->rect, data->pixmap, data->subRect );
            break;
        }
        case QskPainterCommand::Image:
        {
            const QskPainterCommand::ImageData* data = cmd.imageData();
            painter->drawImage( data->rect, data->image,
                data->subRect, data->flags );
            break;
        }
        case QskPainterCommand::State:
        {
            const QskPainterCommand::StateData* data = cmd.stateData();

            if ( data->flags & QPaintEngine::DirtyPen )
                painter->setPen( colorFilter.substituted( data->pen ) );

            if ( data->flags & QPaintEngine::DirtyBrush )
                painter->setBrush( colorFilter.substituted( data->brush ) );

            if ( data->flags & QPaintEngine::DirtyBrushOrigin )
                painter->setBrushOrigin( data->brushOrigin );

            if ( data->flags & QPaintEngine::DirtyFont )
                painter->setFont( data->font );

            if ( data->flags & QPaintEngine::DirtyBackground )
            {
                painter->setBackgroundMode( data->backgroundMode );
                painter->setBackground( colorFilter.substituted( data->backgroundBrush ) );
            }

            if ( data->flags & QPaintEngine::DirtyTransform )
            {
                painter->setTransform( data->transform * transform );
            }

            if ( data->flags & QPaintEngine::DirtyClipEnabled )
                painter->setClipping( data->isClipEnabled );

            if ( data->flags & QPaintEngine::DirtyClipRegion )
            {
                painter->setClipRegion( data->clipRegion,
                    data->clipOperation );
            }

            if ( data->flags & QPaintEngine::DirtyClipPath )
            {
                painter->setClipPath( data->clipPath, data->clipOperation );
            }

            if ( data->flags & QPaintEngine::DirtyHints )
            {
                const QPainter::RenderHints hints = data->renderHints;

                painter->setRenderHint( QPainter::Antialiasing,
                    hints.testFlag( QPainter::Antialiasing ) );

                painter->setRenderHint( QPainter::TextAntialiasing,
                    hints.testFlag( QPainter::TextAntialiasing ) );

                painter->setRenderHint( QPainter::SmoothPixmapTransform,
                    hints.testFlag( QPainter::SmoothPixmapTransform ) );

                painter->setRenderHint( QPainter::HighQualityAntialiasing,
                    hints.testFlag( QPainter::HighQualityAntialiasing ) );

                painter->setRenderHint( QPainter::NonCosmeticDefaultPen,
                    hints.testFlag( QPainter::NonCosmeticDefaultPen ) );
            }

            if ( data->flags & QPaintEngine::DirtyCompositionMode )
                painter->setCompositionMode( data->compositionMode );

            if ( data->flags & QPaintEngine::DirtyOpacity )
                painter->setOpacity( data->opacity );

            break;
        }
        default:
            break;
    }
}

namespace
{
    class PathInfo
    {
    public:
        PathInfo():
            m_scalablePen( false )
        {
            // QVector needs a default constructor
        }

        PathInfo( const QRectF& pointRect,
            const QRectF& boundingRect, bool scalablePen ):
            m_pointRect( pointRect ),
            m_boundingRect( boundingRect ),
            m_scalablePen( scalablePen )
        {
        }

        inline QRectF scaledBoundingRect( qreal sx, qreal sy, bool scalePens ) const
        {
            if ( sx == 1.0 && sy == 1.0 )
                return m_boundingRect;

            QTransform transform;
            transform.scale( sx, sy );

            QRectF rect;
            if ( scalePens && m_scalablePen )
            {
                rect = transform.mapRect( m_boundingRect );
            }
            else
            {
                rect = transform.mapRect( m_pointRect );

                const qreal l = qAbs( m_pointRect.left() - m_boundingRect.left() );
                const qreal r = qAbs( m_pointRect.right() - m_boundingRect.right() );
                const qreal t = qAbs( m_pointRect.top() - m_boundingRect.top() );
                const qreal b = qAbs( m_pointRect.bottom() - m_boundingRect.bottom() );

                rect.adjust( -l, -t, r, b );
            }

            return rect;
        }

        inline double scaleFactorX( const QRectF& pathRect,
            const QRectF& targetRect, bool scalePens ) const
        {
            if ( pathRect.width() <= 0.0 )
                return 0.0;

            const QPointF p0 = m_pointRect.center();

            const qreal l = qAbs( pathRect.left() - p0.x() );
            const qreal r = qAbs( pathRect.right() - p0.x() );

            const double w = 2.0 * qMin( l, r )
                * targetRect.width() / pathRect.width();

            double sx;
            if ( scalePens && m_scalablePen )
            {
                sx = w / m_boundingRect.width();
            }
            else
            {
                const qreal pw = qMax(
                    qAbs( m_boundingRect.left() - m_pointRect.left() ),
                    qAbs( m_boundingRect.right() - m_pointRect.right() ) );

                sx = ( w - 2 * pw ) / m_pointRect.width();
            }

            return sx;
        }

        inline double scaleFactorY( const QRectF& pathRect,
            const QRectF& targetRect, bool scalePens ) const
        {
            if ( pathRect.height() <= 0.0 )
                return 0.0;

            const QPointF p0 = m_pointRect.center();

            const qreal t = qAbs( pathRect.top() - p0.y() );
            const qreal b = qAbs( pathRect.bottom() - p0.y() );

            const double h = 2.0 * qMin( t, b )
                * targetRect.height() / pathRect.height();

            double sy;
            if ( scalePens && m_scalablePen )
            {
                sy = h / m_boundingRect.height();
            }
            else
            {
                const double pw =
                    qMax( qAbs( m_boundingRect.top() - m_pointRect.top() ),
                        qAbs( m_boundingRect.bottom() - m_pointRect.bottom() ) );

                sy = ( h - 2 * pw ) / m_pointRect.height();
            }

            return sy;
        }

    private:
        QRectF m_pointRect;
        QRectF m_boundingRect;
        bool m_scalablePen;
    };
}

class QskGraphic::PrivateData : public QSharedData
{
public:
    PrivateData():
        boundingRect( 0.0, 0.0, -1.0, -1.0 ),
        pointRect( 0.0, 0.0, -1.0, -1.0 ),
        hasRasterData( false ),
        renderHints( 0 )
    {
    }

    PrivateData( const PrivateData& other ):
        QSharedData( other ),
        defaultSize( other.defaultSize ),
        commands( other.commands ),
        pathInfos( other.pathInfos ),
        colorTable( other.colorTable ),
        boundingRect( other.boundingRect ),
        pointRect( other.pointRect ),
        hasRasterData( other.hasRasterData ),
        renderHints( other.renderHints )
    {
    }

    ~PrivateData()
    {
    }

    inline bool operator==( const PrivateData& other ) const
    {
        return ( renderHints == other.renderHints )
               && ( commands == other.commands );
    }

    QSizeF defaultSize;
    QVector< QskPainterCommand > commands;
    QVector< PathInfo > pathInfos;
    QVector< QRgb > colorTable;

    QRectF boundingRect;
    QRectF pointRect;

    bool hasRasterData : 1;
    uint renderHints : 4;
};

QskGraphic::QskGraphic():
    m_data( new PrivateData() ),
    m_paintEngine( nullptr )
{
}

QskGraphic::QskGraphic( const QskGraphic& other ):
    QPaintDevice(),
    m_data( other.m_data ),
    m_paintEngine( nullptr )
{
}

QskGraphic::QskGraphic( QskGraphic&& other ):
    m_data( std::move( other.m_data ) ),
    m_paintEngine( nullptr )
{
}

QskGraphic::~QskGraphic()
{
    delete m_paintEngine;
}

QskGraphic& QskGraphic::operator=( const QskGraphic& other )
{
    delete m_paintEngine;
    m_data = other.m_data;
    m_paintEngine = nullptr;

    return *this;
}

QskGraphic& QskGraphic::operator=( QskGraphic&& other )
{
    m_data = std::move( other.m_data );
    m_paintEngine = nullptr;
    return *this;
}

bool QskGraphic::operator==( const QskGraphic& other ) const
{
    return *m_data == *other.m_data;
}

QPaintEngine* QskGraphic::paintEngine() const
{
    if ( m_paintEngine == nullptr )
        m_paintEngine = new QskGraphicPaintEngine();

    return m_paintEngine;
}

int QskGraphic::metric( PaintDeviceMetric deviceMetric ) const
{
    int value;

    switch ( deviceMetric )
    {
        case PdmWidth:
        {
            value = qCeil( defaultSize().width() );
            break;
        }
        case PdmHeight:
        {
            value = qCeil( defaultSize().height() );
            break;
        }
        case PdmNumColors:
        {
            value = 0xffffffff;
            break;
        }
        case PdmDepth:
        {
            value = 32;
            break;
        }
        case PdmPhysicalDpiX:
        case PdmPhysicalDpiY:
        case PdmDpiY:
        case PdmDpiX:
        {
            value = 72;
            break;
        }
        case PdmWidthMM:
        {
            value = qRound( metric( PdmWidth ) * 25.4 / metric( PdmDpiX ) );
            break;
        }
        case PdmHeightMM:
        {
            value = qRound( metric( PdmHeight ) * 25.4 / metric( PdmDpiY ) );
            break;
        }
        default:
            value = 0;
    }
    return value;
}

void QskGraphic::reset()
{
    m_data->commands.clear();
    m_data->pathInfos.clear();

    m_data->boundingRect = QRectF( 0.0, 0.0, -1.0, -1.0 );
    m_data->pointRect = QRectF( 0.0, 0.0, -1.0, -1.0 );
    m_data->defaultSize = QSizeF();

    delete m_paintEngine;
    m_paintEngine = nullptr;
}

bool QskGraphic::isNull() const
{
    return m_data->commands.isEmpty();
}

bool QskGraphic::isEmpty() const
{
    return m_data->boundingRect.isEmpty();
}

bool QskGraphic::isScalable() const
{
    return !m_data->hasRasterData;
}

void QskGraphic::setRenderHint( RenderHint hint, bool on )
{
    if ( on )
        m_data->renderHints |= hint;
    else
        m_data->renderHints &= ~hint;
}

bool QskGraphic::testRenderHint( RenderHint hint ) const
{
    return m_data->renderHints & hint;
}

QskGraphic::RenderHints QskGraphic::renderHints() const
{
    return static_cast< QskGraphic::RenderHints >( m_data->renderHints );
}

QRectF QskGraphic::boundingRect() const
{
    if ( m_data->boundingRect.width() < 0 )
        return QRectF();

    return m_data->boundingRect;
}

QRectF QskGraphic::controlPointRect() const
{
    if ( m_data->pointRect.width() < 0 )
        return QRectF();

    return m_data->pointRect;
}

QVector<QRgb> QskGraphic::colorTable() const
{
    return m_data->colorTable;
}

QRectF QskGraphic::scaledBoundingRect( qreal sx, qreal sy ) const
{
    if ( sx == 1.0 && sy == 1.0 )
        return m_data->boundingRect;

    const bool scalePens = !( m_data->renderHints & RenderPensUnscaled );

    QTransform transform;
    transform.scale( sx, sy );

    QRectF rect = transform.mapRect( m_data->pointRect );

    for ( int i = 0; i < m_data->pathInfos.size(); i++ )
        rect |= m_data->pathInfos[i].scaledBoundingRect( sx, sy, scalePens );

    return rect;
}

QSize QskGraphic::sizeMetrics() const
{
    const QSizeF sz = defaultSize();
    return QSize( qCeil( sz.width() ), qCeil( sz.height() ) );
}

void QskGraphic::setDefaultSize( const QSizeF& size )
{
    const double w = qMax( qreal( 0.0 ), size.width() );
    const double h = qMax( qreal( 0.0 ), size.height() );

    m_data->defaultSize = QSizeF( w, h );
}

QSizeF QskGraphic::defaultSize() const
{
    if ( !m_data->defaultSize.isEmpty() )
        return m_data->defaultSize;

    return boundingRect().size();
}

void QskGraphic::render( QPainter* painter ) const
{
    render( painter, QskColorFilter() );
}

void QskGraphic::render( QPainter* painter,
    const QskColorFilter& colorFilter, QTransform *initialTransform ) const
{
    if ( isNull() )
        return;

    const int numCommands = m_data->commands.size();
    const QskPainterCommand* commands = m_data->commands.constData();

    const QTransform transform = painter->transform();
    const QskGraphic::RenderHints renderHints( m_data->renderHints );

    painter->save();

    for ( int i = 0; i < numCommands; i++ )
    {
        qskExecCommand( painter, commands[i], colorFilter,
            renderHints, transform, initialTransform );
    }

    painter->restore();
}

void QskGraphic::render( QPainter* painter, const QSizeF& size,
    Qt::AspectRatioMode aspectRatioMode ) const
{
    const QRectF r( 0.0, 0.0, size.width(), size.height() );
    render( painter, r, aspectRatioMode );
}

void QskGraphic::render( QPainter* painter, const QRectF& rect,
    Qt::AspectRatioMode aspectRatioMode ) const
{
    render( painter, rect, QskColorFilter(), aspectRatioMode );
}

void QskGraphic::render( QPainter* painter, const QRectF& rect,
    const QskColorFilter& colorFilter, Qt::AspectRatioMode aspectRatioMode ) const
{
    if ( isEmpty() || rect.isEmpty() )
        return;

    double sx = 1.0;
    double sy = 1.0;

    if ( m_data->pointRect.width() > 0.0 )
        sx = rect.width() / m_data->pointRect.width();

    if ( m_data->pointRect.height() > 0.0 )
        sy = rect.height() / m_data->pointRect.height();

    const bool scalePens = !( m_data->renderHints & RenderPensUnscaled );

    for ( int i = 0; i < m_data->pathInfos.size(); i++ )
    {
        const PathInfo info = m_data->pathInfos[i];

        const double ssx = info.scaleFactorX(
            m_data->pointRect, rect, scalePens );

        if ( ssx > 0.0 )
            sx = qMin( sx, ssx );

        const double ssy = info.scaleFactorY(
            m_data->pointRect, rect, scalePens );

        if ( ssy > 0.0 )
            sy = qMin( sy, ssy );
    }

    if ( aspectRatioMode == Qt::KeepAspectRatio )
    {
        const double s = qMin( sx, sy );
        sx = s;
        sy = s;
    }
    else if ( aspectRatioMode == Qt::KeepAspectRatioByExpanding )
    {
        const double s = qMax( sx, sy );
        sx = s;
        sy = s;
    }

    QTransform tr;
    tr.translate( rect.center().x() - 0.5 * sx * m_data->pointRect.width(),
        rect.center().y() - 0.5 * sy * m_data->pointRect.height() );
    tr.scale( sx, sy );
    tr.translate( -m_data->pointRect.x(), -m_data->pointRect.y() );

    const QTransform transform = painter->transform();
    QTransform* initialTransform = nullptr;
    if ( !scalePens && transform.isScaling() )
    {
        // we don't want to scale pens according to sx/sy,
        // but we want to apply the scaling from the
        // painter transformation later

        initialTransform = new QTransform();
        initialTransform->scale( transform.m11(), transform.m22() );
    }

    painter->setTransform( tr, true );
    render( painter, colorFilter, initialTransform );

    painter->setTransform( transform );

    delete initialTransform;
}

void QskGraphic::render( QPainter* painter,
    const QPointF& pos, Qt::Alignment alignment ) const
{
    QRectF r( pos, defaultSize() );

    if ( alignment & Qt::AlignLeft )
    {
        r.moveLeft( pos.x() );
    }
    else if ( alignment & Qt::AlignHCenter )
    {
        r.moveCenter( QPointF( pos.x(), r.center().y() ) );
    }
    else if ( alignment & Qt::AlignRight )
    {
        r.moveRight( pos.x() );
    }

    if ( alignment & Qt::AlignTop )
    {
        r.moveTop( pos.y() );
    }
    else if ( alignment & Qt::AlignVCenter )
    {
        r.moveCenter( QPointF( r.center().x(), pos.y() ) );
    }
    else if ( alignment & Qt::AlignBottom )
    {
        r.moveBottom( pos.y() );
    }

    render( painter, r );
}

QPixmap QskGraphic::toPixmap( qreal devicePixelRatio ) const
{
    if ( isNull() )
        return QPixmap();

    if ( devicePixelRatio <= 0.0 )
        devicePixelRatio = qskDevicePixelRatio();

    const QSizeF sz = defaultSize();

    const int w = qCeil( sz.width() );
    const int h = qCeil( sz.height() );

    QPixmap pixmap( w, h );
    pixmap.setDevicePixelRatio( devicePixelRatio );
    pixmap.fill( Qt::transparent );

    const QRectF r( 0.0, 0.0, sz.width(), sz.height() );

    QPainter painter( &pixmap );
    render( &painter, r, Qt::KeepAspectRatio );
    painter.end();

    return pixmap;
}

QPixmap QskGraphic::toPixmap( const QSize& size,
    Qt::AspectRatioMode aspectRatioMode, qreal devicePixelRatio ) const
{
    if ( devicePixelRatio <= 0.0 )
        devicePixelRatio = qskDevicePixelRatio();

    QPixmap pixmap( size );
    pixmap.setDevicePixelRatio( devicePixelRatio );
    pixmap.fill( Qt::transparent );

    const QRect r( 0, 0, size.width(), size.height() );

    QPainter painter( &pixmap );
    render( &painter, r, aspectRatioMode );
    painter.end();

    return pixmap;
}

QImage QskGraphic::toImage( const QSize& size,
    Qt::AspectRatioMode aspectRatioMode, qreal devicePixelRatio  ) const
{
    if ( devicePixelRatio <= 0.0 )
        devicePixelRatio = qskDevicePixelRatio();

    QImage image( size * devicePixelRatio, QImage::Format_ARGB32_Premultiplied );
    image.setDevicePixelRatio( devicePixelRatio );
    image.fill( 0 );

    const QRect r( 0, 0, size.width(), size.height() );

    QPainter painter( &image );
    render( &painter, r, aspectRatioMode );
    painter.end();

    return image;
}

QImage QskGraphic::toImage( qreal devicePixelRatio ) const
{
    if ( isNull() )
        return QImage();

    if ( devicePixelRatio <= 0.0 )
        devicePixelRatio = qskDevicePixelRatio();

    const QSizeF sz = defaultSize();

    const int w = qCeil( sz.width() ) * devicePixelRatio;
    const int h = qCeil( sz.height() ) * devicePixelRatio;

    QImage image( w, h, QImage::Format_ARGB32 );
    image.setDevicePixelRatio( devicePixelRatio );
    image.fill( 0 );

    const QRect r( 0, 0, sz.width(), sz.height() );

    QPainter painter( &image );
    render( &painter, r, Qt::KeepAspectRatio );
    painter.end();

    return image;
}

void QskGraphic::drawPath( const QPainterPath& path )
{
    const QPainter* painter = paintEngine()->painter();
    if ( painter == NULL )
        return;

    m_data->commands += QskPainterCommand( path );

    if ( !path.isEmpty() )
    {
        const QPainterPath scaledPath = painter->transform().map( path );

        QRectF pointRect = scaledPath.boundingRect();
        QRectF boundingRect = pointRect;

        if ( painter->pen().style() != Qt::NoPen
             && painter->pen().brush().style() != Qt::NoBrush )
        {
            boundingRect = qskStrokedPathRect( painter, path );
        }

        updateControlPointRect( pointRect );
        updateBoundingRect( boundingRect );

        m_data->pathInfos += PathInfo( pointRect,
            boundingRect, qskHasScalablePen( painter ) );
    }
}

void QskGraphic::drawPixmap( const QRectF& rect,
    const QPixmap& pixmap, const QRectF& subRect )
{
    const QPainter* painter = paintEngine()->painter();
    if ( painter == NULL )
        return;

    m_data->commands += QskPainterCommand( rect, pixmap, subRect );
    m_data->hasRasterData = true;

    const QRectF r = painter->transform().mapRect( rect );
    updateControlPointRect( r );
    updateBoundingRect( r );
}

void QskGraphic::drawImage( const QRectF& rect, const QImage& image,
    const QRectF& subRect, Qt::ImageConversionFlags flags )
{
    const QPainter* painter = paintEngine()->painter();
    if ( painter == NULL )
        return;

    m_data->commands += QskPainterCommand( rect, image, subRect, flags );
    m_data->hasRasterData = true;

    const QRectF r = painter->transform().mapRect( rect );

    updateControlPointRect( r );
    updateBoundingRect( r );
}

void QskGraphic::updateState( const QPaintEngineState& state )
{
    m_data->commands += QskPainterCommand( state );

    if ( state.state() & QPaintEngine::DirtyPen )
    {
        const QPen pen = state.pen();

        if ( pen.isSolid() )
        {
            qskInsertColor( pen.color(), m_data->colorTable );
        }
        else if ( auto gradient = pen.brush().gradient() )
        {
            for ( const auto& stop : gradient->stops() )
                qskInsertColor( stop.second, m_data->colorTable );
        }
    }

    if ( state.state() & QPaintEngine::DirtyBrush )
    {
        const QBrush brush = state.brush();

        if ( brush.style() == Qt::SolidPattern )
        {
            qskInsertColor( brush.color(), m_data->colorTable );
        }
        else if ( auto gradient = brush.gradient() )
        {
            for ( const auto& stop : gradient->stops() )
                qskInsertColor( stop.second, m_data->colorTable );
        }
    }
}

void QskGraphic::updateBoundingRect( const QRectF& rect )
{
    QRectF br = rect;

    const QPainter* painter = paintEngine()->painter();
    if ( painter && painter->hasClipping() )
    {
        QRectF cr = painter->clipRegion().boundingRect();
        cr = painter->transform().mapRect( cr );

        br &= cr;
    }

    if ( m_data->boundingRect.width() < 0 )
        m_data->boundingRect = br;
    else
        m_data->boundingRect |= br;
}

void QskGraphic::updateControlPointRect( const QRectF& rect )
{
    if ( m_data->pointRect.width() < 0.0 )
        m_data->pointRect = rect;
    else
        m_data->pointRect |= rect;
}

const QVector< QskPainterCommand >& QskGraphic::commands() const
{
    return m_data->commands;
}

void QskGraphic::setCommands( const QVector< QskPainterCommand >& commands )
{
    reset();

    const int numCommands = commands.size();
    if ( numCommands <= 0 )
        return;

    // to calculate a proper bounding rectangle we don't simply copy
    // the commands.

    const QskPainterCommand* cmds = commands.constData();

    QPainter painter( this );
    for ( int i = 0; i < numCommands; i++ )
    {
        qskExecCommand( &painter, cmds[i],
            QskColorFilter(), RenderHints(), QTransform(), NULL );
    }

    painter.end();
}

QskGraphic QskGraphic::fromImage( const QImage& image )
{
    QskGraphic graphic;

    if ( !image.isNull() )
    {
        QPainter painter( &graphic );
        painter.drawImage( 0, 0, image );
        painter.end();
    }

    return graphic;
}

QskGraphic QskGraphic::fromPixmap( const QPixmap& pixmap )
{
    QskGraphic graphic;

    if ( !pixmap.isNull() )
    {
        QPainter painter( &graphic );
        painter.drawPixmap( 0, 0, pixmap );
        painter.end();
    }

    return graphic;
}
