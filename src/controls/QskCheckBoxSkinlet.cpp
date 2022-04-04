#include "QskCheckBoxSkinlet.h"
#include "QskCheckBox.h"

#include <QSGFlatColorMaterial>
#include <qsgnode.h>

class Tic : public QSGGeometryNode {
    QSGFlatColorMaterial material;
    QSGGeometry geometry = QSGGeometry(
        QSGGeometry::defaultAttributes_Point2D(), 3 );
    const QRectF& target;
  public:
    Tic( const QRectF& rect, const QColor& color ): target( rect ) {
        geometry.setDrawingMode( QSGGeometry::DrawLineStrip );
        geometry.setLineWidth( 2 );
        setGeometry( &geometry );

        material.setColor( color );
        setMaterial( &material );

        markDirty( QSGNode::DirtyGeometry );
    }

    void setColor( const QColor& color ) {
        material.setColor( color );
        markDirty( QSGNode::DirtyMaterial );
    }

    void makeTic() {
        const auto& size = target.size();
        const auto x = target.x();
        const auto y = target.y();

        geometry.vertexDataAsPoint2D()[0].set( x, y + size.height() / 2 );
        geometry.vertexDataAsPoint2D()[1].set( x + size.width() / 3,
            y + size.height() );
        geometry.vertexDataAsPoint2D()[2].set( x + size.width(), y );
        markDirty( QSGNode::DirtyGeometry );
    }

    void makePartially() {
        const auto& size = target.size();
        const auto x = target.x();
        const auto y = target.y();

        geometry.vertexDataAsPoint2D()[0].set( x, y + size.height() / 2 );
        geometry.vertexDataAsPoint2D()[1].set( x, y + size.height() / 2 );
        geometry.vertexDataAsPoint2D()[2].set( x + size.width(),
            y + size.height() / 2 );

        markDirty( QSGNode::DirtyGeometry );
    }

    void makeEmpty() {
        const auto x = target.x();
        const auto y = target.y();

        geometry.vertexDataAsPoint2D()[0].set( x, y );
        geometry.vertexDataAsPoint2D()[1].set( x, y );
        geometry.vertexDataAsPoint2D()[2].set( x, y );

        markDirty( QSGNode::DirtyGeometry );
    }
};

QskCheckBoxSkinlet::QskCheckBoxSkinlet( QskSkin* skin )
    : QskSkinlet( skin )
{
    setNodeRoles( { BoxRole, TickRole } );
}

QskCheckBoxSkinlet::~QskCheckBoxSkinlet()
{
}

QRectF QskCheckBoxSkinlet::subControlRect(
    const QskSkinnable*,
    const QRectF& contentsRect,
    QskAspect::Subcontrol ) const
{
    return contentsRect;
}

QSGNode* QskCheckBoxSkinlet::updateSubNode(
    const QskSkinnable* skinnable, quint8 nodeRole, QSGNode* node ) const
{
    switch( nodeRole ) {
        case BoxRole:
            return updateBoxNode( skinnable, node,
                QskCheckBox::Box );
        case TickRole:
            auto control = dynamic_cast< const QskCheckBox* >( skinnable );
            auto rect = control->subControlRect( QskCheckBox::Tick );
            rect = rect.marginsRemoved(
                skinnable->marginHint( QskCheckBox::Tick ) );

            Tic* tic;
            if ( static_cast< Tic* >( node ) == nullptr )
            {
                tic = new Tic( rect, skinnable->color( QskCheckBox::Tick ) );
            }
            else
            {
                tic = static_cast< Tic* >( node );
            }

            switch ( control->checkState() ) {
                case Qt::CheckState::Unchecked:
                    tic->setColor( skinnable->color( QskCheckBox::Tick ) );
                    tic->makeEmpty();
                    break;
                case Qt::CheckState::PartiallyChecked:
                    tic->setColor( skinnable->color(
                        QskCheckBox::Tick | QskCheckBox::PartiallyChecked ) );
                    tic->makePartially();
                    break;
                case Qt::CheckState::Checked:
                    tic->setColor( skinnable->color(
                        QskCheckBox::Tick | QskCheckBox::Checked ) );
                    tic->makeTic();
                    break;
            }

            return tic;
    }

    return Inherited::updateSubNode( skinnable, nodeRole, node );
}

QSizeF QskCheckBoxSkinlet::sizeHint( const QskSkinnable* skinnable,
    Qt::SizeHint, const QSizeF& ) const
{
    return skinnable->strutSizeHint( QskCheckBox::Box );
}
