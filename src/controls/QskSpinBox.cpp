/******************************************************************************
 * Copyright (C) 2023 Edelhirsch Software GmbH
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#include "QskSpinBox.h"
#include <QskLinearBox.h>
#include <QskGridBox.h>
#include <QskTextInput.h>
#include <QRegExpValidator>
#include <QskBoxShapeMetrics.h>
#include <QskBoxBorderColors.h>
#include <QskBoxBorderMetrics.h>
#include <QskSkinlet.h>
#include <QskEvent.h>
#include <QtMath>
#include <QGuiApplication>
#include <QStyleHints>
#include <QskIntervalF.h>
#include <array>

QSK_SUBCONTROL(QskSpinBox, IncrementPanel)
QSK_SUBCONTROL(QskSpinBox, DecrementPanel)
QSK_SUBCONTROL(QskSpinBox, IncrementText)
QSK_SUBCONTROL(QskSpinBox, DecrementText)
QSK_SUBCONTROL(QskSpinBox, Text)
QSK_SUBCONTROL(QskSpinBox, TextPanel)
QSK_SUBCONTROL(QskSpinBox, Layout)

QSK_SYSTEM_STATE(QskSpinBox, Pressed, ( QskAspect::QskAspect::FirstSystemState << 0))

namespace aliased_enum
{
  constexpr auto D = QskSpinBox::Decrement;
  constexpr auto T = QskSpinBox::Textbox;
  constexpr auto I = QskSpinBox::Increment;
  constexpr auto N = QskSpinBox::None;
}

class QskSpinBox::PrivateData
{
public:

  explicit PrivateData(QskSpinBox* const parent) : q(parent)
  {
  }

  void setValue(qreal value)
  {
    value = qBound(q->minimum(), value, q->maximum());
    if(!qFuzzyCompare(m_value, value))
    {
      m_value = value;
      Q_EMIT q->valueChanged(m_value);
      q->polish();
      q->update();
    }
  }

  qreal value() const { return m_value; }

  FocusIndeces defaultFocusIndex() const
  {
    const auto layout = q->alignmentHint(QskSpinBox::Layout);

    if(layout == Qt::AlignLeft) return QskSpinBox::Textbox;
    if(layout == Qt::AlignRight) return QskSpinBox::Decrement;
    if(layout == Qt::AlignHCenter) return QskSpinBox::Decrement;
    if(layout == Qt::AlignTop) return QskSpinBox::Textbox;
    if(layout == Qt::AlignBottom) return QskSpinBox::Increment;
    if(layout == Qt::AlignVCenter) return QskSpinBox::Increment;
    if(layout == (Qt::AlignLeft | Qt::AlignVCenter)) return QskSpinBox::Textbox;
    if(layout == (Qt::AlignRight | Qt::AlignVCenter)) return QskSpinBox::Increment;
    if(layout == (Qt::AlignTop | Qt::AlignHCenter)) return QskSpinBox::Textbox;
    if(layout == (Qt::AlignBottom | Qt::AlignHCenter)) return QskSpinBox::Decrement;

    return None;
  }

  FocusIndeces nextFocusIndex() const
  {
    const auto layout = q->alignmentHint(QskSpinBox::Layout);
    using namespace aliased_enum;

    // [0][1][2][3] := index
    // [D][T][I][N] := control
    using LUT = std::array<QskSpinBox::FocusIndeces,4>;
    if(layout == Qt::AlignLeft) return LUT{I,D,N,T}[m_focusIndex];
    if(layout == Qt::AlignRight) return LUT{I,N,T,D}[m_focusIndex];
    if(layout == Qt::AlignHCenter) return LUT{T,I,N,D}[m_focusIndex];
    if(layout == Qt::AlignTop) return LUT{I,D,N,T}[m_focusIndex];
    if(layout == Qt::AlignBottom) return LUT{I,N,T,D}[m_focusIndex];
    if(layout == Qt::AlignVCenter) return LUT{N,D,T,I}[m_focusIndex];
    if(layout == (Qt::AlignLeft | Qt::AlignVCenter)) return LUT{N,I,D,T}[m_focusIndex];
    if(layout == (Qt::AlignRight | Qt::AlignVCenter)) return LUT{T,N,D,I}[m_focusIndex];
    if(layout == (Qt::AlignTop | Qt::AlignHCenter)) return LUT{I,D,N,T}[m_focusIndex];
    if(layout == (Qt::AlignBottom | Qt::AlignHCenter)) return LUT{I,N,T,D}[m_focusIndex];

    return None;
  }

  FocusIndeces previousFocusIndex() const
  {
    const auto layout = q->alignmentHint(QskSpinBox::Layout);
    using namespace aliased_enum;

     // [0][1][2][3] := index
     // [D][T][I][N] := control
    using LUT = std::array<FocusIndeces,4>;
    if(layout == Qt::AlignLeft) return LUT{N,D,T,I}[m_focusIndex];
    if(layout == Qt::AlignRight) return LUT{N,I,D,T}[m_focusIndex];
    if(layout == Qt::AlignHCenter) return LUT{N,D,T,I}[m_focusIndex];
    if(layout == Qt::AlignTop) return LUT{T,N,D,I}[m_focusIndex];
    if(layout == Qt::AlignBottom) return LUT{N,I,D,T}[m_focusIndex];
    if(layout == Qt::AlignVCenter) return LUT{T,I,N,D}[m_focusIndex];
    if(layout == (Qt::AlignLeft | Qt::AlignVCenter)) return LUT{I,N,T,D}[m_focusIndex];
    if(layout == (Qt::AlignRight | Qt::AlignVCenter)) return LUT{I,D,N,T}[m_focusIndex];
    if(layout == (Qt::AlignTop | Qt::AlignHCenter)) return LUT{T,N,D,I}[m_focusIndex];
    if(layout == (Qt::AlignBottom | Qt::AlignHCenter)) return LUT{N,I,D,T}[m_focusIndex];

    return None;
  }

  FocusIndeces focusIndex() const
  {
    return m_focusIndex;
  }

  void focusNext()
  {
    const auto index = nextFocusIndex();
    setFocus(index);
  }

  void focusPrevious()
  {
    const auto index = previousFocusIndex();
    setFocus(index);
  }

  void focusDefault()
  {
    setFocus(defaultFocusIndex());
  }

  void setFocus(const FocusIndeces index)
  {
    using namespace aliased_enum;
    Q_ASSERT(index == D || index == T || index == I || index == N);
    if(index == D || index == T || index == I || index == N)
    {
      m_focusIndex = index;
      Q_EMIT q->focusIndexChanged(m_focusIndex);
    }
  }

  QRectF focusIndicatorRect() const
  {
    using namespace aliased_enum;
    if(m_focusIndex == D) return q->subControlRect(QskSpinBox::DecrementPanel);
    if(m_focusIndex == I) return q->subControlRect(QskSpinBox::IncrementPanel);
    if(m_focusIndex == T) return q->subControlRect(QskSpinBox::TextPanel);
    return {};
  }

  void saveMousePosition(const QPointF& pos)
  {
    q->setSkinHint(QskSpinBox::Layout | QskAspect::Metric | QskAspect::Position, pos );
  }

private:
  qreal m_value{0.0f};
  QskSpinBox* const q;
  FocusIndeces m_focusIndex = FocusIndeces::None;
};

using S = QskSpinBox;

QskSpinBox::QskSpinBox(QQuickItem* const parent)
    : Inherited(parent)
    , m_data(std::make_unique<PrivateData>(this))
{
  setBoundaries(0.0,1.0);
  setAcceptHoverEvents(true);
  setAcceptedMouseButtons(Qt::LeftButton);
  setFocusPolicy( Qt::StrongFocus );

  connect( this, &S::focusIndexChanged, this, &S::focusIndicatorRectChanged );
  connect( this, &S::boundariesChanged, this, [this](const QskIntervalF& interval){ if(!interval.contains(value())) m_data->setValue(minimum()); });
  connect( this, &S::valueChanged, this, [this](){ polish(); });
}

QskSpinBox::~QskSpinBox() = default;

void QskSpinBox::hoverEnterEvent(QHoverEvent* event)
{
  m_data->saveMousePosition( qskHoverPosition( event ) );
}

void QskSpinBox::hoverLeaveEvent(QHoverEvent* event)
{
  m_data->saveMousePosition( {} );
}

void QskSpinBox::hoverMoveEvent(QHoverEvent *event)
{
  m_data->saveMousePosition( qskHoverPosition( event ) );
}

void QskSpinBox::mouseReleaseEvent(QMouseEvent *event)
{
  m_data->saveMousePosition( qskMousePosition( event ) );

  const auto focus = ( focusPolicy() & Qt::ClickFocus ) == Qt::ClickFocus && !QGuiApplication::styleHints()->setFocusOnTouchRelease();

  if(subControlRect(QskSpinBox::IncrementPanel).contains( event->pos() ))
  {
    increment(+stepSize());

    if( focus )
    {
      m_data->setFocus(Increment);
    }

    return;
  }

  if(subControlRect(QskSpinBox::DecrementPanel).contains( event->pos() ))
  {
    increment(-stepSize());

    if( focus )
    {
      m_data->setFocus(Decrement);
    }

    return;
  }

  if(subControlRect(QskSpinBox::TextPanel).contains( event->pos() ))
  {
    if( focus )
    {
      m_data->setFocus(Textbox);
    }

    return;
  }

  event->ignore();
}

void QskSpinBox::mousePressEvent(QMouseEvent *event)
{
  m_data->saveMousePosition( -1 * qskMousePosition( event ) );

  const auto focus = ( focusPolicy() & Qt::ClickFocus ) == Qt::ClickFocus && !QGuiApplication::styleHints()->setFocusOnTouchRelease();

  if(subControlRect(QskSpinBox::IncrementPanel).contains( event->pos() ))
  {
    if( focus )
    {
      m_data->setFocus(QskSpinBox::Increment);
    }
    return;
  }

  if(subControlRect(QskSpinBox::DecrementPanel).contains( event->pos() ))
  {
    if( focus )
    {
      m_data->setFocus(QskSpinBox::Decrement);
    }
    return;
  }

  event->ignore();
}

void QskSpinBox::keyPressEvent(QKeyEvent *event)
{
  switch( event->key() )
  {
    case Qt::Key_Plus:
    case Qt::Key_Up:
    case Qt::Key_Right:
      increment(+stepSize());
      break;
    case Qt::Key_Minus:
    case Qt::Key_Down:
    case Qt::Key_Left:
      increment(-stepSize());
      break;
    case Qt::Key_Select:
    case Qt::Key_Space:
      // TODO click currently focused -/+
      break;
    default:
    {
      const int steps = qskFocusChainIncrement( event );
      if(steps != 0)
      {
        for(int i = 0; i < steps; ++i)
        {
          m_data->focusNext();
        }
        for(int i = steps; i < 0; ++i)
        {
          m_data->focusPrevious();
        }
      }
    }
  }
  Inherited::keyPressEvent( event );
}

void QskSpinBox::keyReleaseEvent( QKeyEvent* event )
{
  if( event->key() == Qt::Key_Select || event->key() == Qt::Key_Space )
  {
    return;
  }

  Inherited::keyReleaseEvent( event );
}

void QskSpinBox::focusInEvent(QFocusEvent *event)
{
  switch( event->reason() )
  {
    case Qt::TabFocusReason:
      m_data->focusNext();
      break;

    case Qt::BacktabFocusReason:
      m_data->focusPrevious();
      break;

    default:
      if(m_data->focusIndex() == QskSpinBox::None)
      {
        m_data->focusDefault();
        return;
      }
  }
  Inherited::focusInEvent( event );
}

QRectF QskSpinBox::focusIndicatorRect() const
{
  auto rect = m_data->focusIndicatorRect();
  return rect;
}

void QskSpinBox::increment(qreal offset)
{
  m_data->setValue(m_data->value() + offset);
}

qreal QskSpinBox::value() const
{
  return m_data->value();
}

QskSpinBox::FocusIndeces QskSpinBox::focusIndex() const
{
  return m_data->focusIndex();
}
