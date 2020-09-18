#ifndef QSKSHADOWEDRECTANGLE_H
#define QSKSHADOWEDRECTANGLE_H

#include "src/shadowedrectangle.h"

#include <QskControl.h>

class QskShadowedRectangle : public QskControl
{
        Q_OBJECT

    public:
        QSK_SUBCONTROLS( Shadow )

        QskShadowedRectangle( QQuickItem* parent = nullptr );
//        ~QskShadowedRectangle() override;

//        BorderGroup* border() const;
//        ShadowGroup* shadow() const;
//        CornersGroup* corners() const;

//        qreal radius() const;
//        void setRadius( qreal newRadius );
//        Q_SIGNAL void radiusChanged();

//        QColor color() const;
//        void setColor( const QColor& newColor );
//        Q_SIGNAL void colorChanged();

//        void componentComplete() override;

//        bool isSoftwareRendering() const;

//    Q_SIGNALS:
//        void softwareRenderingChanged();

//    protected:
//        PaintedRectangleItem* softwareItem() const;
//        void itemChange( QQuickItem::ItemChange change, const QQuickItem::ItemChangeData& value ) override;

//    private:
//        void checkSoftwareItem();
//        const std::unique_ptr<BorderGroup> m_border;
//        const std::unique_ptr<ShadowGroup> m_shadow;
//        const std::unique_ptr<CornersGroup> m_corners;
//        qreal m_radius = 0.0;
//        QColor m_color = Qt::white;
//        PaintedRectangleItem* m_softwareItem = nullptr;
};

#endif // QSKSHADOWEDRECTANGLE_H
