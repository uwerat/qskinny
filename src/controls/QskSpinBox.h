/******************************************************************************
 * Copyright (C) 2023 Edelhirsch Software GmbH
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#pragma once

#include <QskControl.h>
#include <QskPushButton.h>
#include <QskBoundedValueInput.h>

class QSK_EXPORT QskSpinBox : public QskBoundedValueInput
{
  Q_OBJECT
  using Inherited = QskBoundedValueInput;
public:
  enum FocusIndeces : int { Decrement = 0, Textbox = 1, Increment = 2, None = 3 };
  Q_ENUM(FocusIndeces)

  Q_PROPERTY(FocusIndeces focusIndex READ focusIndex NOTIFY focusIndexChanged)
  QSK_SUBCONTROLS(IncrementPanel, DecrementPanel, IncrementText, DecrementText, TextPanel, Text, Layout)
  QSK_STATES( Pressed )

  explicit QskSpinBox( QQuickItem* parent = nullptr );
  ~QskSpinBox() override;
  FocusIndeces focusIndex() const;

Q_SIGNALS:
  void focusIndexChanged(int index);

private:
  void hoverEnterEvent( QHoverEvent* event) override;
  void hoverLeaveEvent( QHoverEvent* event) override;
  void hoverMoveEvent( QHoverEvent* event) override;

  void mouseReleaseEvent(QMouseEvent* event) override;
  void mousePressEvent(QMouseEvent* event) override;

  void keyPressEvent( QKeyEvent* event ) override;
  void keyReleaseEvent( QKeyEvent* event ) override;

  void focusInEvent(QFocusEvent* event) override;
  QRectF focusIndicatorRect() const override;

  class PrivateData;
  std::unique_ptr<PrivateData> m_data;
};
