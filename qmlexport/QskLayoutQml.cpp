/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskLayoutQml.h"

void QskGridBoxQml::setHorizontalSpacing( qreal spacing )
{
    const auto oldSpacing = QskGridBox::spacing( Qt::Horizontal );
    QskGridBox::setSpacing( Qt::Horizontal, spacing );

    if ( oldSpacing != QskGridBox::spacing( Qt::Horizontal ) )
        Q_EMIT verticalSpacingChanged();
}

qreal QskGridBoxQml::horizontalSpacing() const
{
    return QskGridBox::spacing( Qt::Horizontal );
}

void QskGridBoxQml::resetHorizontalSpacing()
{
    const auto orientation = Qt::Horizontal;

    const auto oldSpacing = QskGridBox::spacing( orientation );
    QskGridBox::resetSpacing( orientation );

    if ( oldSpacing != QskGridBox::spacing( orientation ) )
        Q_EMIT horizontalSpacingChanged();
}

void QskGridBoxQml::setVerticalSpacing( qreal spacing )
{
    const auto oldSpacing = QskGridBox::spacing( Qt::Vertical );
    QskGridBox::setSpacing( Qt::Vertical, spacing );

    if ( oldSpacing != QskGridBox::spacing( Qt::Vertical ) )
        Q_EMIT verticalSpacingChanged();
}

qreal QskGridBoxQml::verticalSpacing() const
{
    return QskGridBox::spacing( Qt::Vertical );
}

void QskGridBoxQml::resetVerticalSpacing()
{
    const auto orientation = Qt::Vertical;

    const auto oldSpacing = QskGridBox::spacing( orientation );
    QskGridBox::resetSpacing( orientation );

    if ( oldSpacing != QskGridBox::spacing( orientation ) )
        Q_EMIT verticalSpacingChanged();
}

#include "moc_QskLayoutQml.cpp"
