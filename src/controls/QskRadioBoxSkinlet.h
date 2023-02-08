#ifndef QSK_RADIO_BOX_SKINLET_H
#define QSK_RADIO_BOX_SKINLET_H

#include "QskSkinlet.h"

class QskRadioBox;

class QSK_EXPORT QskRadioBoxSkinlet : public QskSkinlet
{
  Q_GADGET

    using Inherited = QskSkinlet;

 public:
  enum NodeRole
    {
      PanelRole,
      RadioRole,
      SymbolRole,
      TextRole,
      RippleRole,

      RoleCount
    };

    Q_INVOKABLE QskRadioBoxSkinlet( QskSkin* = nullptr );
    ~QskRadioBoxSkinlet() override;

    QRectF subControlRect( const QskSkinnable*,
        const QRectF&, QskAspect::Subcontrol ) const override;

    QSizeF sizeHint( const QskSkinnable*,
        Qt::SizeHint, const QSizeF& ) const override;

    int sampleCount( const QskSkinnable*, QskAspect::Subcontrol ) const override;

    QRectF sampleRect( const QskSkinnable*,
        const QRectF&, QskAspect::Subcontrol, int index ) const override;

    QRectF textRect( const QskRadioBox*, const QRectF&, int ) const;
    QRectF radioRect( const QskRadioBox*, const QskAspect::Subcontrol target, const QRectF&, int ) const;

    QskAspect::States sampleStates( const QskSkinnable*,
        QskAspect::Subcontrol, int index ) const override;
    
  protected:
    QSGNode* updateSubNode( const QskSkinnable*,
        quint8 nodeRole, QSGNode* ) const override;

    QSGNode* updateSampleNode( const QskSkinnable*,
        QskAspect::Subcontrol, int index, QSGNode* ) const override;

  private:
    qreal lineHeight( const QskRadioBox* target ) const;
};

#endif
