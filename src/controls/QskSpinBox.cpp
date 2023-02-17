/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskSpinBox.h"
#include "QskLinearBox.h"
#include "QskGridBox.h"
#include "QskTextInput.h"
#include "QskBoxShapeMetrics.h"
#include "QskBoxBorderColors.h"
#include "QskBoxBorderMetrics.h"
#include "QskSkinlet.h"
#include "QskIntervalF.h"
#include "QskEvent.h"

#include <QtMath>
#include <QGuiApplication>
#include <QStyleHints>

#include <array>

QSK_SUBCONTROL(QskSpinBox, Inc)
QSK_SUBCONTROL(QskSpinBox, Dec)
QSK_SUBCONTROL(QskSpinBox, IncText)
QSK_SUBCONTROL(QskSpinBox, DecText)
QSK_SUBCONTROL(QskSpinBox, Text)
QSK_SUBCONTROL(QskSpinBox, TextPanel)
QSK_SUBCONTROL(QskSpinBox, Layout)

QSK_SYSTEM_STATE(QskSpinBox, Pressed, ( QskAspect::QskAspect::FirstSystemState << 0))

class QskSpinBox::PrivateData
{
public:

  enum FocusIndeces : int { Dec = 0, Text = 1, Inc = 2, None = 3 };

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

    if(layout == Qt::AlignLeft) return Text;
    if(layout == Qt::AlignRight) return Dec;
    if(layout == Qt::AlignHCenter) return Dec;
    if(layout == Qt::AlignTop) return Text;
    if(layout == Qt::AlignBottom) return Inc;
    if(layout == Qt::AlignVCenter) return Inc;
    if(layout == (Qt::AlignLeft | Qt::AlignVCenter)) return Text;
    if(layout == (Qt::AlignRight | Qt::AlignVCenter)) return Inc;
    if(layout == (Qt::AlignTop | Qt::AlignHCenter)) return Text;
    if(layout == (Qt::AlignBottom | Qt::AlignHCenter)) return Dec;

    return None;
  }

  FocusIndeces nextFocusIndex() const
  {
    const auto layout = q->alignmentHint(QskSpinBox::Layout);

    // [0  ][1   ][2  ][3   ]
    // [Dec][Text][Inc][None]
    using LUT = std::array<FocusIndeces,4>;
    if(layout == Qt::AlignLeft) return LUT{Inc,Dec,None,Text}[m_focusIndex];
    if(layout == Qt::AlignRight) return LUT{Inc,None,Text,Dec}[m_focusIndex];
    if(layout == Qt::AlignHCenter) return LUT{Text,Inc,None,Dec}[m_focusIndex];
    if(layout == Qt::AlignTop) return LUT{Inc,Dec,None,Text}[m_focusIndex];
    if(layout == Qt::AlignBottom) return LUT{Inc,None,Text,Dec}[m_focusIndex];
    if(layout == Qt::AlignVCenter) return LUT{None,Dec,Text,Inc}[m_focusIndex];
    if(layout == (Qt::AlignLeft | Qt::AlignVCenter)) return LUT{None,Inc,Dec,Text}[m_focusIndex];
    if(layout == (Qt::AlignRight | Qt::AlignVCenter)) return LUT{Text,None,Dec,Inc}[m_focusIndex];
    if(layout == (Qt::AlignTop | Qt::AlignHCenter)) return LUT{Inc,Dec,None,Text}[m_focusIndex];
    if(layout == (Qt::AlignBottom | Qt::AlignHCenter)) return LUT{Inc,None,Text,Dec}[m_focusIndex];

    return None;
  }

  FocusIndeces previousFocusIndex() const
  {
    const auto layout = q->alignmentHint(QskSpinBox::Layout);

    // [0  ][1   ][2  ][3   ]
    // [Dec][Text][Inc][None]
    using LUT = std::array<FocusIndeces,4>;
    if(layout == Qt::AlignLeft) return LUT{None,Dec,Text,Inc}[m_focusIndex];
    if(layout == Qt::AlignRight) return LUT{None,Inc,Dec,Text}[m_focusIndex];
    if(layout == Qt::AlignHCenter) return LUT{None,Dec,Text,Inc}[m_focusIndex];
    if(layout == Qt::AlignTop) return LUT{Text,None,Dec,Inc}[m_focusIndex];
    if(layout == Qt::AlignBottom) return LUT{None,Inc,Dec,Text}[m_focusIndex];
    if(layout == Qt::AlignVCenter) return LUT{Text,Inc,None,Dec}[m_focusIndex];
    if(layout == (Qt::AlignLeft | Qt::AlignVCenter)) return LUT{Inc,None,Text,Dec}[m_focusIndex];
    if(layout == (Qt::AlignRight | Qt::AlignVCenter)) return LUT{Inc,Dec,None,Text}[m_focusIndex];
    if(layout == (Qt::AlignTop | Qt::AlignHCenter)) return LUT{Text,None,Dec,Inc}[m_focusIndex];
    if(layout == (Qt::AlignBottom | Qt::AlignHCenter)) return LUT{None,Inc,Dec,Text}[m_focusIndex];

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
    Q_ASSERT(index == Dec || index == Text || index == Inc || index == None);
    if(index == Dec || index == Text || index == Inc || index == None)
    {
      m_focusIndex = index;
      Q_EMIT q->focusIndexChanged(m_focusIndex); // TODO register enum
    }
  }

  QRectF focusIndicatorRect() const
  {
    switch(m_focusIndex)
    {
    case PrivateData::FocusIndeces::Dec:
      return q->subControlRect(QskSpinBox::Dec);
    case PrivateData::FocusIndeces::Text:
      return q->subControlRect(QskSpinBox::TextPanel);
    case PrivateData::FocusIndeces::Inc:
      return q->subControlRect(QskSpinBox::Inc);
    default: return {};
    }
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
    , m_data( new PrivateData( this ) )
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

void QskSpinBox::hoverLeaveEvent(QHoverEvent* )
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

  if(subControlRect(QskSpinBox::Inc).contains( event->pos() ))
  {
    increment(+stepSize());

    if( focus )
    {
      m_data->setFocus(PrivateData::Inc);
    }

    return;
  }

  if(subControlRect(QskSpinBox::Dec).contains( event->pos() ))
  {
    increment(-stepSize());

    if( focus )
    {
      m_data->setFocus(PrivateData::Dec);
    }

    return;
  }

  if(subControlRect(QskSpinBox::TextPanel).contains( event->pos() ))
  {
    if( focus )
    {
      m_data->setFocus(PrivateData::Text);
    }

    return;
  }

  event->ignore();
}

void QskSpinBox::mousePressEvent(QMouseEvent *event)
{
  m_data->saveMousePosition( -1 * qskMousePosition( event ) );

  const auto focus = ( focusPolicy() & Qt::ClickFocus ) == Qt::ClickFocus && !QGuiApplication::styleHints()->setFocusOnTouchRelease();

  if(subControlRect(QskSpinBox::Inc).contains( event->pos() ))
  {
    if( focus )
    {
      m_data->setFocus(PrivateData::Inc);
    }
    return;
  }

  if(subControlRect(QskSpinBox::Dec).contains( event->pos() ))
  {
    if( focus )
    {
      m_data->setFocus(PrivateData::Dec);
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
      // TODO increment
      break;
    case Qt::Key_Minus:
    case Qt::Key_Down:
    case Qt::Key_Left:
      // TODO decrement
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
      if(m_data->focusIndex() == PrivateData::None)
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
