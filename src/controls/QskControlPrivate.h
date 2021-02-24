/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_CONTROL_PRIVATE_H
#define QSK_CONTROL_PRIVATE_H

#include "QskGlobal.h"
#include "QskControl.h"
#include "QskQuickItemPrivate.h"

class QskControlPrivate : public QskQuickItemPrivate
{
    using Inherited = QskQuickItemPrivate;

  public:
    static bool inheritLocale( QskControl*, const QLocale& );
    static void resolveLocale( QskControl* );

  protected:
    QskControlPrivate();
    ~QskControlPrivate() override;

  private:
    void setExplicitSizeHint( Qt::SizeHint, const QSizeF& );
    void resetExplicitSizeHint( Qt::SizeHint );
    QSizeF explicitSizeHint( Qt::SizeHint ) const;

    QSizeF implicitSizeHint( Qt::SizeHint, const QSizeF& ) const;
    QSizeF implicitSizeHint() const override final;

    void implicitSizeChanged() override final;
    void layoutConstraintChanged() override final;

    bool maybeGesture( QQuickItem*, QEvent* );

  private:
    Q_DECLARE_PUBLIC( QskControl )

    QSizeF* explicitSizeHints;

    QLocale locale;

    QskSizePolicy sizePolicy;
    int layoutHints : 4;
    unsigned int layoutAlignmentHint : 8;

    bool explicitLocale : 1;

    bool autoFillBackground : 1;
    bool autoLayoutChildren : 1;

    uint focusPolicy : 4;
    bool isWheelEnabled : 1;

    mutable bool blockLayoutRequestEvents : 1;
};

#endif
