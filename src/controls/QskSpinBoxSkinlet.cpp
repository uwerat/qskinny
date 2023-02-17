/******************************************************************************
 * Copyright (C) 2023 Edelhirsch Software GmbH
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#include "QskSpinBoxSkinlet.h"
#include "QskSpinBox.h"
#include <QFontMetrics>
#include <array>

const auto INCREMENT_TEXT = QStringLiteral("+");
const auto DECREMENT_TEXT = QStringLiteral("-");

enum SampleIndeces { Dec = 0, Txt = 1, Inc = 2, Count };

QskSpinBoxSkinlet::QskSpinBoxSkinlet(QskSkin *)
{
  setNodeRoles({IncPanel, IncText, DecPanel, DecText, TextPanel, TextText});
}

int QskSpinBoxSkinlet::sampleCount(const QskSkinnable *, QskAspect::Subcontrol) const
{
  return Count;
}

QRectF QskSpinBoxSkinlet::sampleRect(const QskSkinnable* const skinnable, const QRectF& rect, QskAspect::Subcontrol subControl, int index) const
{
  if(index == Dec || index == Inc || index == Txt)
  {
    return subControlRect(skinnable, rect, subControl);
  }

  return Inherited::sampleRect( skinnable, rect, subControl, index );
}

QskAspect::States QskSpinBoxSkinlet::sampleStates(const QskSkinnable* const skinnable, QskAspect::Subcontrol subControl, int index) const
{
  using S = QskSpinBox;
  auto states = Inherited::sampleStates( skinnable, subControl, index );  

  if ( subControl == S::DecrementPanel || subControl == S::IncrementPanel || subControl == S::TextPanel)
  {
      const auto* const spinbox = static_cast<const S*>(skinnable);
      const auto cursorPos = spinbox->effectiveSkinHint(S::Layout | QskAspect::Metric | QskAspect::Position).toPointF();
      const QPointF cursorPosAbs{qAbs(cursorPos.x()), qAbs(cursorPos.y())};
      const auto focusIndex = spinbox->focusIndex();

      const auto contain = !cursorPosAbs.isNull() && spinbox->subControlRect(subControl).contains(cursorPosAbs);
      const auto pressed = contain && (cursorPos.x() < 0 || cursorPos.y() < 0);
      const auto hovered = contain && !pressed;
      const auto focused = ( subControl == S::IncrementPanel && focusIndex == S::Increment) ||
                           ( subControl == S::DecrementPanel && focusIndex == S::Decrement) ||
                           ( subControl == S::TextPanel && focusIndex == S::Textbox);

      states.setFlag(QskControl::Hovered, hovered);
      states.setFlag(QskSpinBox::Pressed, pressed);
      states.setFlag(QskControl::Focused, focused);
  }

  return states;
}

QSizeF QskSpinBoxSkinlet::sizeHint(const QskSkinnable* const skinnable, Qt::SizeHint sizeHint, const QSizeF& size) const
{
  using S = QskSpinBox;
  const auto* const spinbox = static_cast<const S*>(skinnable);
  const auto layout = spinbox->alignmentHint(S::Layout);
  const auto spacing = spinbox->spacingHint(S::Layout);

  const auto strutInc = spinbox->strutSizeHint(S::IncrementPanel);
  const auto strutDec = spinbox->strutSizeHint(S::DecrementPanel);
  const auto strutTxt = spinbox->strutSizeHint(S::TextPanel);

  if(sizeHint == Qt::MinimumSize || sizeHint == Qt::MaximumSize || Qt::PreferredSize)
  {
    if(layout == Qt::AlignTop || layout == Qt::AlignBottom || layout == Qt::AlignVCenter)
    {
      const auto w = qMax(strutDec.width(), qMax( strutTxt.width() , strutInc.width()));
      const auto h = strutDec.height() + strutTxt.height() + strutInc.height();
      return {w,h + 2.0 * spacing};
    }
    if(layout == Qt::AlignLeft || layout == Qt::AlignRight || layout == Qt::AlignHCenter)
    {
      const auto w = strutDec.width() + strutTxt.width() + strutInc.width();
      const auto h = qMax(strutDec.height(), qMax( strutTxt.height() , strutInc.height()));
      return {w + 2.0 * spacing,h};
    }
    if(layout == (Qt::AlignLeft | Qt::AlignVCenter) || layout == (Qt::AlignRight | Qt::AlignVCenter))
    {
      const auto w = strutTxt.width() + qMax(strutInc.width(), strutDec.width());
      const auto h = qMax(2.0 * qMax(strutInc.height(), strutDec.height()), strutTxt.height());
      return {w + spacing ,h + spacing};
    }
    if(layout == (Qt::AlignTop | Qt::AlignHCenter) || layout == (Qt::AlignTop | Qt::AlignHCenter))
    {
      const auto w = qMax(strutTxt.width() , strutInc.width() + strutDec.width());
      const auto h = strutTxt.height() + qMax(strutInc.height() , strutDec.height());
      return {w + spacing, h + spacing};
    }
  }
  return Inherited::sizeHint(skinnable, sizeHint, size);
}

QRectF QskSpinBoxSkinlet::subControlRect(const QskSkinnable* const skinnable, const QRectF& rect, QskAspect::Subcontrol subControl) const
{
  using S = QskSpinBox;

  if(subControl == S::DecrementText) return subControlRect(skinnable, rect, S::DecrementPanel);
  if(subControl == S::IncrementText) return subControlRect(skinnable, rect, S::IncrementPanel);
  if(subControl == S::Text) return subControlRect(skinnable, rect, S::TextPanel);

  const auto* const spinbox = static_cast<const S*>(skinnable);
  const auto layout = spinbox->alignmentHint(S::Layout);
  const auto spacing = spinbox->spacingHint(S::Layout);

  std::array<QRectF, Count> rects =
  {
    QRectF{ QPointF{}, spinbox->strutSizeHint(S::DecrementPanel)},
    QRectF{ QPointF{}, spinbox->strutSizeHint(S::TextPanel)},
    QRectF{ QPointF{}, spinbox->strutSizeHint(S::IncrementPanel)},
  };

  const auto center = rect.center();

  // TODO center everything

  if(layout == Qt::AlignLeft)
  {
    rects[Txt].moveTopLeft({0.0 /************/, center.y() - rects[Txt].height() * 0.5});
    rects[Dec].moveTopLeft({rects[Txt].right() + spacing, center.y() - rects[Dec].height() * 0.5});
    rects[Inc].moveTopLeft({rects[Dec].right() + spacing, center.y() - rects[Inc].height() * 0.5});
  }
  else if(layout == Qt::AlignRight)
  {
    rects[Dec].moveTopLeft({0.0 /************/, center.y() - rects[Dec].height() * 0.5});
    rects[Inc].moveTopLeft({rects[Dec].right() + spacing, center.y() - rects[Inc].height() * 0.5});
    rects[Txt].moveTopLeft({rects[Inc].right() + spacing, center.y() - rects[Txt].height() * 0.5});
  }
  else if(layout == Qt::AlignTop)
  {
    rects[Txt].moveTopLeft({center.x() - rects[Txt].width() * 0.5, 0.0 });
    rects[Inc].moveTopLeft({center.x() - rects[Inc].width() * 0.5, rects[Txt].bottom() + spacing});
    rects[Dec].moveTopLeft({center.x() - rects[Dec].width() * 0.5, rects[Inc].bottom() + spacing});
  }
  else if(layout == Qt::AlignBottom)
  {
    rects[Inc].moveTopLeft({center.x() - rects[Inc].width() * 0.5, 0.0});
    rects[Dec].moveTopLeft({center.x() - rects[Dec].width() * 0.5, rects[Inc].bottom() + spacing});
    rects[Txt].moveTopLeft({center.x() - rects[Txt].width() * 0.5, rects[Dec].bottom() + spacing});
  }
  else if(layout == Qt::AlignHCenter)
  {
    rects[Dec].moveTopLeft({0.0 /************/, center.y() - rects[Dec].height() * 0.5});
    rects[Txt].moveTopLeft({rects[Dec].right() + spacing, center.y() - rects[Txt].height() * 0.5});
    rects[Inc].moveTopLeft({rects[Txt].right() + spacing, center.y() - rects[Inc].height() * 0.5});
  }
  else if(layout == Qt::AlignVCenter)
  {
    rects[Inc].moveTopLeft({center.x() - rects[Inc].width() * 0.5, 0.0});
    rects[Txt].moveTopLeft({center.x() - rects[Txt].width() * 0.5, rects[Inc].bottom() + spacing});
    rects[Dec].moveTopLeft({center.x() - rects[Dec].width() * 0.5, rects[Txt].bottom() + spacing});
  }
  else if(layout == (Qt::AlignLeft | Qt::AlignVCenter))
  {
    rects[Txt].moveTopLeft({0.0 /************/, center.y() - rects[Txt].height() * 0.5 });
    rects[Inc].moveTopLeft({rects[Txt].right() + spacing, center.y() - spacing * 0.5 - rects[Inc].height()});
    rects[Dec].moveTopLeft({rects[Txt].right() + spacing, center.y() + spacing * 0.5});
  }
  else if(layout == (Qt::AlignRight | Qt::AlignVCenter))
  {
    const auto dx = qMax(rects[Inc].width(), rects[Dec].width());
    rects[Inc].moveTopLeft({dx - rects[Inc].width(), center.y() - spacing * 0.5 - rects[Inc].height()});
    rects[Dec].moveTopLeft({dx - rects[Dec].width(), center.y() + spacing * 0.5});
    rects[Txt].moveTopLeft({dx + spacing, center.y() - rects[Txt].height() * 0.5 });
  }
  else if(layout == (Qt::AlignTop | Qt::AlignHCenter))
  {
    rects[Txt].moveTopLeft({center.x() - rects[Txt].width() * 0.5, 0.0 });
    rects[Dec].moveTopLeft({rects[Txt].center().x() - spacing * 0.5 - rects[Dec].width(), rects[Txt].bottom() + spacing });
    rects[Inc].moveTopLeft({rects[Txt].center().x() + spacing * 0.5, rects[Txt].bottom() + spacing });
  }
  else if(layout == (Qt::AlignBottom | Qt::AlignHCenter))
  {
    rects[Txt].moveTopLeft({center.x() - rects[Txt].width() * 0.5, center.y() - rects[Txt].height() * 0.5});
    rects[Dec].moveTopLeft({center.x() - spacing * 0.5 - rects[Dec].width() , rects[Txt].top() - spacing - rects[Dec].height() });
    rects[Inc].moveTopLeft({center.x() + spacing * 0.5, rects[Txt].top() - spacing - rects[Inc].height() });
  }

  if(subControl == S::DecrementPanel)
  {
    return rects[Dec];
  }
  if(subControl == S::TextPanel)
  {
    return rects[Txt];
  }
  if(subControl == S::IncrementPanel)
  {
    return rects[Inc];
  }

  return Inherited::subControlRect(skinnable, rect, subControl);
}

QSGNode* QskSpinBoxSkinlet::updateSubNode(const QskSkinnable* const skinnable, const quint8 nodeRole, QSGNode* const node) const
{
  using S = QskSpinBox;
  if(nodeRole == IncPanel) { return updateSeriesNode( skinnable, S::IncrementPanel, node); }
  if(nodeRole == DecPanel) { return updateSeriesNode( skinnable, S::DecrementPanel, node ); }
  if(nodeRole == IncText) { return updateTextNode( skinnable, node, INCREMENT_TEXT, S::IncrementText); }
  if(nodeRole == DecText) { return updateTextNode( skinnable, node, DECREMENT_TEXT, S::DecrementText ); }
  if(nodeRole == TextPanel) { return updateSeriesNode( skinnable, S::TextPanel, node ); }
  if(nodeRole == TextText) { return updateTextNode( skinnable, node, QString::number(static_cast<const S*>(skinnable)->value()), S::Text ); }
  return Inherited::updateSubNode(skinnable, nodeRole, node);
}

QSGNode* QskSpinBoxSkinlet::updateSampleNode(const QskSkinnable* const skinnable, QskAspect::Subcontrol subControl, const int index, QSGNode* const node) const
{
  using S = QskSpinBox;
  const auto* const spinbox = static_cast<const S*>(skinnable);

  if ( subControl == S::DecrementPanel || subControl == S::IncrementPanel || subControl == S::TextPanel )
  {
    const auto rect = sampleRect(spinbox, spinbox->contentsRect(), subControl, index);
    return updateBoxNode( skinnable, node, rect, subControl );
  }

  return Inherited::updateSampleNode( skinnable, subControl, index, node );
}
