/******************************************************************************
 * QSkinny - Copyright (C) 2021 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef DIAGRAM_SKINLET_H
#define DIAGRAM_SKINLET_H

#include <QskSkinlet.h>

class Diagram;

class DiagramSkinlet : public QskSkinlet
{
        Q_GADGET

        using Inherited = QskSkinlet;

    public:
        enum NodeRole
        {
            ChartRole,
            SeparatorRole,
        };

        Q_INVOKABLE DiagramSkinlet( QskSkin* = nullptr );
        ~DiagramSkinlet() override;

        QRectF subControlRect( const QskSkinnable*,
                               const QRectF&, QskAspect::Subcontrol ) const override;

    protected:

        QSGNode* updateSubNode( const QskSkinnable*,
                                quint8 nodeRole, QSGNode* ) const override;

    private:
        QSGNode* updateChartNode( const Diagram*, QSGNode* ) const;
        QSGNode* updateSeparatorNode( const Diagram*, QSGNode* ) const;
};

#endif // DIAGRAM_SKINLET_H
