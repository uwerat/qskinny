#ifndef QSK_SWITCH_BUTTON_SKINLET_H
#define QSK_SWITCH_BUTTON_SKINLET_H


#include "QskSkinlet.h"

class QSK_EXPORT QskSwitchButtonSkinlet : public QskSkinlet
{
    Q_GADGET

    using Inherited = QskSkinlet;

  public:
    enum NodeRole
    {
        KnopRole,
        GrooveRole
    };

    Q_INVOKABLE QskSwitchButtonSkinlet( QskSkin* parent = nullptr );
    ~QskSwitchButtonSkinlet() override;


    QRectF subControlRect( const QskSkinnable*,
        const QRectF& rect, QskAspect::Subcontrol ) const override;

    QSizeF sizeHint( const QskSkinnable*,
        Qt::SizeHint, const QSizeF& ) const override;

  protected:
    QSGNode* updateSubNode( const QskSkinnable*,
        quint8 nodeRole, QSGNode* ) const override;
};
#endif
