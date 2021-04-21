#ifndef PIECHART_SKINLET_H
#define PIECHART_SKINLET_H

#include <QskSkinlet.h>

class PieChart;

class PieChartSkinlet : public QskSkinlet
{
        Q_GADGET

    public:
        enum NodeRole
        {
            PanelRole,
            LabelsRole
        };

        Q_INVOKABLE PieChartSkinlet( QskSkin* skin = nullptr );

        QRectF subControlRect( const QskSkinnable*, const QRectF&, QskAspect::Subcontrol ) const override;

    protected:
        virtual QSGNode* updateSubNode( const QskSkinnable*, quint8 nodeRole, QSGNode* node ) const override;

    private:
        QSGNode* updatePanelNode( const PieChart*, QSGNode* ) const;
        QSGNode* updateLabelsNode( const PieChart*, QSGNode* ) const;
};

#endif // PIECHART_SKINLET_H
