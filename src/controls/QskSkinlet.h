/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_SKINLET_H
#define QSK_SKINLET_H

#include "QskAspect.h"
#include "QskNamespace.h"

#include <qnamespace.h>
#include <qrect.h>

#include <memory>

class QskArcMetrics;
class QskSkin;
class QskControl;
class QskSkinnable;
class QskGradient;
class QskColorFilter;
class QskGraphic;
class QskTextOptions;
class QskTextColors;
class QskBoxShapeMetrics;
class QskBoxBorderMetrics;
class QskBoxBorderColors;
class QskBoxHints;

class QSGNode;

class QSK_EXPORT QskSkinlet
{
    Q_GADGET

  public:
    Q_INVOKABLE QskSkinlet( QskSkin* = nullptr );
    virtual ~QskSkinlet();

    QskSkin* skin() const;

    virtual void updateNode( QskSkinnable*, QSGNode* parent ) const;

    virtual QSizeF sizeHint( const QskSkinnable*,
        Qt::SizeHint, const QSizeF& ) const;

    virtual QRectF subControlRect( const QskSkinnable*,
        const QRectF&, QskAspect::Subcontrol ) const;

    /*
        When having more than one instance for the
        same QskAspect::Subcontrol it is called a sample
     */
    virtual QRectF sampleRect( const QskSkinnable*,
        const QRectF&, QskAspect::Subcontrol, int index ) const;

    virtual int sampleIndexAt( const QskSkinnable*,
        const QRectF&, QskAspect::Subcontrol, const QPointF& ) const;

    virtual int sampleCount( const QskSkinnable*, QskAspect::Subcontrol ) const;

    virtual QskAspect::States sampleStates( const QskSkinnable*,
        QskAspect::Subcontrol, int index ) const;

    virtual QVariant sampleAt( const QskSkinnable*,
        QskAspect::Subcontrol, int index ) const;

    const QVector< quint8 >& nodeRoles() const;

    void setOwnedBySkinnable( bool on );
    bool isOwnedBySkinnable() const;

    // Helper functions for creating nodes

    static QSGNode* updateBoxNode( const QskSkinnable*, QSGNode*,
        const QRectF&, QskAspect::Subcontrol );

    static QSGNode* updateBoxNode( const QskSkinnable*, QSGNode*,
        const QRectF&, const QskGradient&, QskAspect::Subcontrol );

    static QSGNode* updateBoxNode( const QskSkinnable*, QSGNode*,
        const QRectF&, const QskBoxShapeMetrics&, const QskBoxBorderMetrics&,
        const QskBoxBorderColors&, const QskGradient& );

    static QSGNode* updateBoxNode( const QskSkinnable*, QSGNode*,
        const QRectF&, const QskBoxHints& );

    static QSGNode* updateInterpolatedBoxNode(
        const QskSkinnable*, QSGNode*, const QRectF&,
        QskAspect aspect1, QskAspect aspect2, qreal ratio );

    static QSGNode* updateArcNode( const QskSkinnable*, QSGNode*,
        const QRectF&, QskAspect::Subcontrol );

    static QSGNode* updateArcNode( const QskSkinnable*, QSGNode*,
        const QRectF&, const QskGradient&, QskAspect::Subcontrol );

    static QSGNode* updateArcNode( const QskSkinnable*, QSGNode*,
        const QRectF&, const QskGradient&, const QskArcMetrics& );

    static QSGNode* updateArcNode( const QskSkinnable*, QSGNode*,
        const QRectF&, qreal startAngle, qreal spanAngle, QskAspect::Subcontrol );

    static QSGNode* updateArcNode( const QskSkinnable*, QSGNode*,
        const QRectF&, const QskGradient&, qreal startAngle, qreal spanAngle,
        QskAspect::Subcontrol );

    static QSGNode* updateTextNode( const QskSkinnable*, QSGNode*,
        const QRectF&, Qt::Alignment, const QString&, const QskTextOptions&,
        QskAspect::Subcontrol );

    static QSGNode* updateTextNode( const QskSkinnable*, QSGNode*,
        const QRectF&, Qt::Alignment, const QString&, const QFont&,
        const QskTextOptions&, const QskTextColors&, Qsk::TextStyle );

    // keeping the aspect ratio
    static QSGNode* updateGraphicNode( const QskSkinnable*, QSGNode*,
        const QskGraphic&, const QskColorFilter&, const QRectF&,
        Qt::Alignment, Qt::Orientations mirrored = Qt::Orientations() );

    // stretching to fit
    static QSGNode* updateGraphicNode( const QskSkinnable*, QSGNode*,
        const QskGraphic&, const QskColorFilter&, const QRectF&,
        Qt::Orientations mirrored = Qt::Orientations() );

    static QSGNode* updateBoxClipNode( const QskSkinnable*, QSGNode*,
        const QRectF&, QskAspect::Subcontrol );

  protected:
    void setNodeRoles( const QVector< quint8 >& );
    void appendNodeRoles( const QVector< quint8 >& );

    virtual QSGNode* updateSubNode( const QskSkinnable*,
        quint8 nodeRole, QSGNode* ) const;

    virtual QSGNode* updateBackgroundNode(
        const QskControl*, QSGNode* ) const;

    virtual QSGNode* updateDebugNode(
        const QskControl*, QSGNode* node ) const;

    QSGNode* updateBoxNode( const QskSkinnable*, QSGNode*,
        QskAspect::Subcontrol ) const;

    QSGNode* updateArcNode( const QskSkinnable*, QSGNode*,
        QskAspect::Subcontrol ) const;

    QSGNode* updateArcNode( const QskSkinnable*, QSGNode*,
        qreal startAngle, qreal spanAngle,
        QskAspect::Subcontrol ) const;

    QSGNode* updateBoxClipNode( const QskSkinnable*, QSGNode*,
        QskAspect::Subcontrol ) const;

    QSGNode* updateTextNode( const QskSkinnable*, QSGNode*,
        const QString&, const QskTextOptions&, QskAspect::Subcontrol ) const;

    QSGNode* updateGraphicNode( const QskSkinnable*, QSGNode*,
        const QskGraphic&, QskAspect::Subcontrol,
        Qt::Orientations mirrored = Qt::Orientations() ) const;

    QSGNode* updateSeriesNode( const QskSkinnable*,
        QskAspect::Subcontrol, QSGNode* ) const;

    virtual QSGNode* updateSampleNode( const QskSkinnable*,
        QskAspect::Subcontrol, int index, QSGNode* ) const;

    void replaceChildNode( quint8 nodeRole, QSGNode* parentNode,
        QSGNode* oldNode, QSGNode* newNode ) const;

    QSizeF hintWithoutConstraint(
        const QSizeF& hint, const QSizeF& constraint ) const;

  private:
    Q_DISABLE_COPY( QskSkinlet )

    class PrivateData;
    std::unique_ptr< PrivateData > m_data;
};

inline QRectF QskSkinlet::subControlRect(
    const QskSkinnable*, const QRectF&, QskAspect::Subcontrol ) const
{
    return QRectF();
}

inline QSGNode* QskSkinlet::updateSubNode(
    const QskSkinnable*, quint8, QSGNode*) const
{
    return nullptr;
}

inline QSizeF QskSkinlet::sizeHint(
    const QskSkinnable*, Qt::SizeHint, const QSizeF& ) const
{
    return QSizeF();
}

inline QRectF QskSkinlet::sampleRect( const QskSkinnable*,
    const QRectF&, QskAspect::Subcontrol, int index ) const
{
    Q_UNUSED( index )
    return QRectF();
}

inline int QskSkinlet::sampleCount(
    const QskSkinnable*, QskAspect::Subcontrol ) const
{
    return 0;
}

#endif
