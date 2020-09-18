#ifndef QSKSHADOWEDRECTANGLESKINLET_H
#define QSKSHADOWEDRECTANGLESKINLET_H

#include "src/shadowedrectangle.h"

#include <QskSkinlet.h>

class QskShadowedRectangle;

class QskShadowedRectangleSkinlet : public QskSkinlet
{
        Q_GADGET

    public:
        enum NodeRole
        {
            ShadowRole,
        };

        Q_INVOKABLE QskShadowedRectangleSkinlet( QskSkin* skin = nullptr );

        QRectF subControlRect( const QskSkinnable*, const QRectF&, QskAspect::Subcontrol ) const override;

    protected:
        virtual QSGNode* updateSubNode( const QskSkinnable*, quint8 nodeRole, QSGNode* node ) const override;

    private:
        QSGNode* updateShadowNode( const QskShadowedRectangle*, QSGNode* ) const;

        // ### move this to the value class:
        const std::unique_ptr<BorderGroup> m_border;
        const std::unique_ptr<ShadowGroup> m_shadow;
        const std::unique_ptr<CornersGroup> m_corners;
        qreal m_radius = 0.0;
        QColor m_color = Qt::white;
        PaintedRectangleItem* m_softwareItem = nullptr;
};

#endif // QSKSHADOWEDRECTANGLESKINLET_H
