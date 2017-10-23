/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_SKINLET_H
#define QSK_SKINLET_H 1

#include "QskGlobal.h"
#include "QskAspect.h"

#include <Qt>
#include <QRectF>
#include <memory>

class QskSkin;
class QskControl;
class QskSkinnable;
class QskGradient;
class QskColorFilter;
class QskGraphic;
class QskTextOptions;

class QSGNode;

class QRectF;
class QRect;
class QSize;

class QSK_EXPORT QskSkinlet
{
    Q_GADGET

public:
    Q_INVOKABLE QskSkinlet( QskSkin* = nullptr );
    virtual ~QskSkinlet();

    QskSkin* skin() const;

    void updateNode( QskSkinnable*, QSGNode* parent ) const;
    virtual QRectF subControlRect( const QskSkinnable*, QskAspect::Subcontrol ) const;

    const QVector< quint8 >& nodeRoles() const;

    void setOwnedBySkinnable( bool on );
    bool isOwnedBySkinnable() const;

    static void setNodeRole( QSGNode* node, quint8 nodeRole );
    static quint8 nodeRole( const QSGNode* node);

    static QSGNode* findNodeByRole( QSGNode* parent, quint8 nodeRole );

    static QSGNode* updateBoxNode( const QskSkinnable*, QSGNode*,
        const QRectF&, QskAspect::Subcontrol );

    static QSGNode* updateTextNode( const QskSkinnable*, QSGNode*,
        const QRectF&, Qt::Alignment, const QString&, const QskTextOptions&,
        QskAspect::Subcontrol );

    // keeping the aspect ratio
    static QSGNode* updateGraphicNode( const QskSkinnable*, QSGNode*,
        const QskGraphic&, const QskColorFilter&,
        const QRectF&, Qt::Alignment );

    // stretching to fit
    static QSGNode* updateGraphicNode( const QskSkinnable*, QSGNode*,
        const QskGraphic&, const QskColorFilter&, const QRectF& );

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
        const QskGraphic&, QskAspect::Subcontrol ) const;

    void insertRemoveNodes( QSGNode* parentNode,
        QSGNode* oldNode, QSGNode* newNode, int nodeRole ) const;

private:
    void insertNodeSorted( QSGNode* node, QSGNode* parentNode ) const;

    class PrivateData;
    std::unique_ptr< PrivateData > m_data;
};

inline QRectF QskSkinlet::subControlRect(
    const QskSkinnable*, QskAspect::Subcontrol ) const
{
    return QRectF();
}

inline QSGNode* QskSkinlet::updateSubNode(
    const QskSkinnable*, quint8, QSGNode*) const
{
    return nullptr;
}

#endif
