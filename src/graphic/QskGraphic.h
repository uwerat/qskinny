/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#ifndef QSK_GRAPHIC_H
#define QSK_GRAPHIC_H

#include "QskGlobal.h"

#include <qmetatype.h>
#include <qflags.h>
#include <qpaintdevice.h>
#include <qshareddata.h>

class QskPainterCommand;
class QskColorFilter;
class QskGraphicPaintEngine;
class QImage;
class QPixmap;
class QPainterPath;
class QPaintEngine;
class QPaintEngineState;
class QTransform;
class QDebug;

class QSK_EXPORT QskGraphic : public QPaintDevice
{
    Q_GADGET

    Q_PROPERTY( qreal aspectRatio READ aspectRatio )
    Q_PROPERTY( QRectF viewBox READ viewBox WRITE setViewBox )
    Q_PROPERTY( QRectF boundingRect READ boundingRect )
    Q_PROPERTY( QRectF controlPointRect READ controlPointRect )
    Q_PROPERTY( QSizeF defaultSize READ defaultSize )
    Q_PROPERTY( quint64 modificationId READ modificationId )

    using Inherited = QPaintDevice;

  public:
    enum RenderHint : quint8
    {
        RenderPensUnscaled = 1 << 0
    };

    typedef QFlags< RenderHint > RenderHints;

    enum CommandType : quint8
    {
        VectorData     = 1 << 0,
        RasterData     = 1 << 1,
        Transformation = 1 << 2
    };

    typedef QFlags< CommandType > CommandTypes;

    QskGraphic();
    QskGraphic( const QskGraphic& );
    QskGraphic( QskGraphic&& );

    ~QskGraphic() override;

    QskGraphic& operator=( const QskGraphic& );
    QskGraphic& operator=( QskGraphic&& );

    bool operator==( const QskGraphic& ) const;
    bool operator!=( const QskGraphic& ) const;

    void reset();

    bool isNull() const;
    bool isEmpty() const;

    CommandTypes commandTypes() const;

    void render( QPainter* ) const;
    void render( QPainter*, const QskColorFilter&,
        QTransform* initialTransform = nullptr ) const;

    void render( QPainter*, const QSizeF&,
        Qt::AspectRatioMode = Qt::IgnoreAspectRatio ) const;

    void render( QPainter*, const QPointF&,
        Qt::Alignment = Qt::AlignTop | Qt::AlignLeft ) const;

    void render( QPainter*, const QRectF&,
        Qt::AspectRatioMode = Qt::IgnoreAspectRatio ) const;

    void render( QPainter*, const QRectF&, const QskColorFilter&,
        Qt::AspectRatioMode = Qt::IgnoreAspectRatio ) const;

    QPixmap toPixmap( qreal devicePixelRatio = 0.0 ) const;

    QPixmap toPixmap( const QSize&,
        Qt::AspectRatioMode = Qt::IgnoreAspectRatio,
        qreal devicePixelRatio = 0.0 ) const;

    QImage toImage( qreal devicePixelRatio = 0.0 ) const;

    QImage toImage( const QSize&,
        Qt::AspectRatioMode = Qt::IgnoreAspectRatio,
        qreal devicePixelRatio = 0.0 ) const;

    QRectF scaledBoundingRect( qreal sx, qreal sy ) const;

    QRectF boundingRect() const;
    QRectF controlPointRect() const;

    const QVector< QskPainterCommand >& commands() const;
    void setCommands( const QVector< QskPainterCommand >& );

    QSizeF defaultSize() const;

    void setViewBox( const QRectF& );
    QRectF viewBox() const;

    qreal aspectRatio() const;

    qreal heightForWidth( qreal width ) const;
    qreal widthForHeight( qreal height ) const;

    void setRenderHint( RenderHint, bool on = true );
    bool testRenderHint( RenderHint ) const;

    RenderHints renderHints() const;

    QPaintEngine* paintEngine() const override;
    int metric( PaintDeviceMetric metric ) const override;

    static QskGraphic fromImage( const QImage& );
    static QskGraphic fromPixmap( const QPixmap& );
    static QskGraphic fromPixmapAsImage( const QPixmap& );
    static QskGraphic fromGraphic( const QskGraphic&, const QskColorFilter& );

    quint64 modificationId() const;
    QskHashValue hash( QskHashValue seed ) const;

  protected:
    virtual QSize sizeMetrics() const;

    virtual void drawPath( const QPainterPath& );

    virtual void drawPixmap( const QRectF&,
        const QPixmap&, const QRectF& );

    virtual void drawImage( const QRectF&,
        const QImage&, const QRectF&, Qt::ImageConversionFlags );

    virtual void updateState( const QPaintEngineState& );

    virtual void addCommand( const QskPainterCommand& );

  private:
    void updateBoundingRect( const QRectF& );
    void updateControlPointRect( const QRectF& );

    class PrivateData;
    QSharedDataPointer< PrivateData > m_data;

    friend class QskGraphicPaintEngine;
    mutable QskGraphicPaintEngine* m_paintEngine;
};

inline bool QskGraphic::operator!=( const QskGraphic& other ) const
{
    return !( *this == other );
}

#ifndef QT_NO_DEBUG_STREAM
QSK_EXPORT QDebug operator<<( QDebug, const QskGraphic& );
#endif

Q_DECLARE_OPERATORS_FOR_FLAGS( QskGraphic::RenderHints )
Q_DECLARE_OPERATORS_FOR_FLAGS( QskGraphic::CommandTypes )
Q_DECLARE_METATYPE( QskGraphic )

#endif
