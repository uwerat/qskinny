/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#ifndef QSK_ITEM_PRIVATE_H
#define QSK_ITEM_PRIVATE_H

#include "QskGlobal.h"
#include "QskItem.h"

QSK_QT_PRIVATE_BEGIN
#include <private/qquickitem_p.h>
QSK_QT_PRIVATE_END

class QskItemPrivate : public QQuickItemPrivate
{
    using Inherited = QQuickItemPrivate;

  protected:
    QskItemPrivate();
    ~QskItemPrivate() override;

  public:
    void applyUpdateFlags( QskItem::UpdateFlags );
    QSGTransformNode* createTransformNode() override;

  protected:
    virtual void layoutConstraintChanged();
    virtual void implicitSizeChanged();

  private:
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
    Q_DECLARE_PUBLIC( QskItem )

    quint8 updateFlags;
    quint8 updateFlagsMask;

    bool polishOnResize : 1;
    bool polishOnParentResize : 1;

    bool blockedPolish : 1;
    bool blockedImplicitSize : 1;
    bool clearPreviousNodes : 1;

    bool initiallyPainted : 1;
    bool wheelEnabled : 1;

    uint focusPolicy : 4;
};

#endif
