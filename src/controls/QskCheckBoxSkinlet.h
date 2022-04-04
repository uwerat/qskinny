#ifndef QSK_CHECK_BOX_SKINLET_H
#define QSK_CHECK_BOX_SKINLET_H

#include "QskSkinlet.h"

class QSK_EXPORT QskCheckBoxSkinlet : public QskSkinlet
{
    Q_GADGET

    using Inherited = QskSkinlet;
  public:
    enum NodeRole
    {
        BoxRole,
        TickRole,
    };

    Q_INVOKABLE QskCheckBoxSkinlet( QskSkin* = nullptr );
    ~QskCheckBoxSkinlet() override;

    QRectF subControlRect( const QskSkinnable*,
        const QRectF&, QskAspect::Subcontrol ) const override;

    QSizeF sizeHint( const QskSkinnable*,
        Qt::SizeHint, const QSizeF& ) const override;

  protected:
    QSGNode* updateSubNode( const QskSkinnable*,
        quint8 nodeRole, QSGNode* ) const override;
};

#endif // QSK_CHECK_BOX_SKINLET_H
