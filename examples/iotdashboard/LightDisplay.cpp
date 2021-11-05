/******************************************************************************
 * Copyright (C) 2021 Edelhirsch Software GmbH
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#include "LightDisplay.h"
#include "Skin.h"

#include <QskAnimator.h>
#include <QskRgbValue.h>
#include <QskSetup.h>
#include <QskTextLabel.h>
#include <QskQuick.h>

#include <QGuiApplication>
#include <QQuickWindow>
#include <QQuickPaintedItem>
#include <QPainter>
#include <QRadialGradient>

QSK_SUBCONTROL( LightDisplay, Groove )
QSK_SUBCONTROL( LightDisplay, ColdPart )
QSK_SUBCONTROL( LightDisplay, WarmPart )
QSK_SUBCONTROL( LightDisplay, ValueText )
QSK_SUBCONTROL( LightDisplay, LeftLabel )
QSK_SUBCONTROL( LightDisplay, RightLabel )

LightDisplay::LightDisplay( QQuickItem* parent )
    : QskBoundedControl( parent )
{
    setAlignmentHint( LeftLabel, Qt::AlignRight );
}

#include "moc_LightDisplay.cpp"
