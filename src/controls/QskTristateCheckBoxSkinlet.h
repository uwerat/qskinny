#ifndef QSK_TRISTATE_CHECK_BOX_SKINLET_H
#define QSK_TRISTATE_CHECK_BOX_SKINLET_H

#include "QskCheckBoxSkinlet.h"

class QskCheckBox;

class QSK_EXPORT QskTristateCheckBoxSkinlet : public QskCheckBoxSkinlet
{
    Q_GADGET

    using Inherited = QskCheckBoxSkinlet;

  public:
    Q_INVOKABLE QskTristateCheckBoxSkinlet( QskSkin* = nullptr );
    ~QskTristateCheckBoxSkinlet() override;

  protected:
    QSGNode* updateIndicatorNode( const QskCheckBox*, QSGNode* ) const override;
};

#endif
