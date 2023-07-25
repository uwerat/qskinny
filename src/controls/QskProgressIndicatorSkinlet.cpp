/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "QskProgressIndicatorSkinlet.h"
#include "QskProgressIndicator.h"

QskProgressIndicatorSkinlet::QskProgressIndicatorSkinlet( QskSkin* skin )
    : Inherited( skin )
{
    setNodeRoles( { GrooveRole, FillRole } );
}

QskProgressIndicatorSkinlet::~QskProgressIndicatorSkinlet()
{
}

QSGNode* QskProgressIndicatorSkinlet::updateSubNode(
    const QskSkinnable* skinnable, quint8 nodeRole, QSGNode* node ) const
{
    const auto indicator = static_cast< const QskProgressIndicator* >( skinnable );

    switch( nodeRole )
    {
        case GrooveRole:
        {
            return updateGrooveNode( indicator, node );
        }

        case FillRole:
        {
            return updateFillNode( indicator, node );
        }
    }

    return Inherited::updateSubNode( skinnable, nodeRole, node );
}

#include "moc_QskProgressIndicatorSkinlet.cpp"
