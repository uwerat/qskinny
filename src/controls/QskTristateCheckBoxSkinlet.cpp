#include "QskTristateCheckBoxSkinlet.h"
#include "QskTristateCheckBox.h"
#include "QskSGNode.h"

#include <QSGFlatColorMaterial>
#include <qsgnode.h>

namespace
{
    class IndicatorNode : public QSGGeometryNode
    {
      public:
        IndicatorNode()
            : m_geometry( QSGGeometry::defaultAttributes_Point2D(), 3 )
        {
            m_geometry.setDrawingMode( QSGGeometry::DrawLineStrip );
            m_geometry.setLineWidth( 2 );
            setGeometry( &m_geometry );

            setMaterial( &m_material );
        }

        void update( bool isPartially, const QRectF& rect, const QColor& color )
        {
            if ( color != m_material.color() )
            {
                m_material.setColor( color );
                markDirty( QSGNode::DirtyMaterial );
            }

            if ( rect != m_rect || isPartially != m_isPartially )
            {
                m_rect = rect;
                m_isPartially = isPartially;

                const auto x = rect.x();
                const auto y = rect.y();
                const auto w = rect.width();
                const auto h = rect.height();

                auto points = m_geometry.vertexDataAsPoint2D();

                if ( isPartially )
                {
                    points[0].set( x, y + h / 2 );
                    points[1] = points[0];
                    points[2].set( x + w, y + h / 2 );
                }
                else
                {
                    points[0].set( x, y + h / 2 );
                    points[1].set( x + w / 3, y + h );
                    points[2].set( x + w, y );
                }

                markDirty( QSGNode::DirtyGeometry );
            }
        }

      private:

        QSGFlatColorMaterial m_material;
        QSGGeometry m_geometry;

        QRectF m_rect;
        bool m_isPartially;
    };
}

QskTristateCheckBoxSkinlet::QskTristateCheckBoxSkinlet( QskSkin* skin )
    : Inherited( skin )
{
}

QskTristateCheckBoxSkinlet::~QskTristateCheckBoxSkinlet()
{
}

QSGNode* QskTristateCheckBoxSkinlet::updateIndicatorNode(
    const QskCheckBox* checkBox, QSGNode* node ) const
{
    using Q = QskCheckBox;

    const auto tristate = qobject_cast< const QskTristateCheckBox* >( checkBox );

    if ( checkBox->isChecked() == false )
        return nullptr;

    auto rect = checkBox->subControlRect(Q::IndicatorTic)
        .marginsRemoved(checkBox->marginHint(Q::IndicatorTic) );

    if ( rect.isEmpty() )
        return nullptr;

    auto indicatorNode = QskSGNode::ensureNode< IndicatorNode >( node );
    indicatorNode->update(
        tristate->checkState() == Qt::CheckState::PartiallyChecked, rect,
        checkBox->color( Q::IndicatorTic ) );

    return indicatorNode;
}

#include "moc_QskTristateCheckBoxSkinlet.cpp"
