/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_QUICK_ITEM_PRIVATE_H
#define QSK_QUICK_ITEM_PRIVATE_H

#include "QskGlobal.h"
#include "QskQuickItem.h"

QSK_QT_PRIVATE_BEGIN
#include <private/qquickitem_p.h>
QSK_QT_PRIVATE_END

class QskQuickItemPrivate : public QQuickItemPrivate
{
    using Inherited = QQuickItemPrivate;

  protected:
    QskQuickItemPrivate();
    ~QskQuickItemPrivate() override;

  public:
    void applyUpdateFlags( QskQuickItem::UpdateFlags );

  protected:
    virtual void layoutConstraintChanged();
    virtual void implicitSizeChanged();

  private:
    QSGTransformNode* createTransformNode() override;
    void transformChanged() override;

    void cleanupNodes();
    void mirrorChange() override;

    qreal getImplicitWidth() const override final;
    qreal getImplicitHeight() const override final;

    void implicitWidthChanged() override final;
    void implicitHeightChanged() override final;

    void updateImplicitSize( bool doNotify );

    void setImplicitSize( qreal width, qreal height, bool doNotify );
    virtual QSizeF implicitSizeHint() const = 0;

  private:
    Q_DECLARE_PUBLIC( QskQuickItem )

    quint8 updateFlags;
    quint8 updateFlagsMask;

    bool polishOnResize : 1;

    bool blockedPolish : 1;
    bool blockedImplicitSize : 1;
    bool clearPreviousNodes : 1;

    bool initiallyPainted : 1;
};

#endif
