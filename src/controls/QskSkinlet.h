/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_SKINLET_H
#define QSK_SKINLET_H

#include "QskAspect.h"

#include <qnamespace.h>
#include <qrect.h>

#include <memory>

class QskSkin;
class QskControl;
class QskSkinnable;
class QskGradient;
class QskColorFilter;
class QskGraphic;
class QskTextOptions;

class QSGNode;

class QSK_EXPORT QskSkinlet
{
    Q_GADGET

  public:
    Q_INVOKABLE QskSkinlet( QskSkin* = nullptr );
    virtual ~QskSkinlet();

    QskSkin* skin() const;

    virtual void updateNode( QskSkinnable*, QSGNode* parent ) const;

    virtual QRectF subControlRect( const QskSkinnable*,
        const QRectF&, QskAspect::Subcontrol ) const;

    virtual QSizeF sizeHint( const QskSkinnable*,
        Qt::SizeHint, const QSizeF& ) const;

    const QVector< quint8 >& nodeRoles() const;

    void setOwnedBySkinnable( bool on );
    bool isOwnedBySkinnable() const;

    static QSGNode* updateBoxNode( const QskSkinnable*, QSGNode*,
        const QRectF&, QskAspect::Subcontrol );

    static QSGNode* updateBoxNode( const QskSkinnable*, QSGNode*,
        const QRectF&, const QskGradient&, QskAspect::Subcontrol );

    static QSGNode* updateTextNode( const QskSkinnable*, QSGNode*,
        const QRectF&, Qt::Alignment, const QString&, const QskTextOptions&,
        QskAspect::Subcontrol );

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

    QSGNode* updateBoxClipNode( const QskSkinnable*, QSGNode*,
        QskAspect::Subcontrol ) const;

    QSGNode* updateTextNode( const QskSkinnable*, QSGNode*,
        const QString&, const QskTextOptions&, QskAspect::Subcontrol ) const;

    QSGNode* updateGraphicNode( const QskSkinnable*, QSGNode*,
        const QskGraphic&, QskAspect::Subcontrol,
        Qt::Orientations mirrored = Qt::Orientations() ) const;

    void replaceChildNode( quint8 nodeRole, QSGNode* parentNode,
        QSGNode* oldNode, QSGNode* newNode ) const;

  private:
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

#endif
