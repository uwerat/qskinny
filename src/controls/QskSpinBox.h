/******************************************************************************
 * Copyright (C) 2023 Edelhirsch Software GmbH
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#pragma once

#include <QskBoundedValueInput.h>

////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief
/// This control allows the user to increment and decrement a floating point value.
/// @details
/// The incement and decrement step size is configurable and the value's range can be limited
/// through an inclusive interval [min,max]. The value is being displayed on a readonly text label
/// surrounded by an increment and decrement button.
///
/// - The value can be increased by:
///  - clicking the increment button
///  - pressing the plus, right or up key
///  - scrolling up the mouse wheel
///  - focusing the increment button and clicking space or select
/// - The value can be decreased by:
///  - clicking the decrement button
///  - pressing the minus, left or down key
///  - scrolling down the mouse wheel
///  - focusing the decrement button and clicking space or select
////////////////////////////////////////////////////////////////////////////////////////////////////
class QSK_EXPORT QskSpinBox : public QskBoundedValueInput
{
    Q_OBJECT
    using Inherited = QskBoundedValueInput;

  public:
    /// Focus indeces for the visual subcontrols
    enum FocusIndeces : int
    {
        Decrement = 0, ///< the decrement buttons index
        Textbox = 1,   ///< the textbox' index
        Increment = 2, ///< the increment button's index
        None = 3       ///< index for when no subcontrol is focused (e.g. focus in/out )
    };
    Q_ENUM( FocusIndeces )

    /// The currently focused subcontrol's index
    Q_PROPERTY( FocusIndeces focusIndex READ focusIndex NOTIFY focusIndexChanged )
    QSK_SUBCONTROLS( IncrementPanel ) ///< Use this to style the increment button.
    QSK_SUBCONTROLS( DecrementPanel ) ///< Use this to style the decrement button.
    QSK_SUBCONTROLS( IncrementText )  ///< Use this to style the increment button's text.
    QSK_SUBCONTROLS( DecrementText )  ///< Use this to style the decrement button's text.
    QSK_SUBCONTROLS( TextPanel )      ///< Use this to style the text's panel.
    QSK_SUBCONTROLS( Text )           ///< Use this to style the text (e.g. font role).
    QSK_SUBCONTROLS( Layout )         ///< Use this to style the spinbox's controls layout.
    QSK_STATES( Pressed )

    /// @brief C-TOR
    /// @param parent This object's parent
    explicit QskSpinBox( QQuickItem* parent = nullptr );

    /// @brief D-TOR defaulted but required for std::unique_ptr
    ~QskSpinBox() override;

    /// @brief Getter for property focusIndex.
    /// @returns Returns the currently focused subcontrol's index.
    /// @retval Return FocusIndeces::None if no subcontrol is currently focused.
    FocusIndeces focusIndex() const;

  Q_SIGNALS:
    /// @brief Emitted when the property @c focusIndex changed.
    void focusIndexChanged( int index );

  private:
    void hoverEnterEvent( QHoverEvent* event ) override;
    void hoverLeaveEvent( QHoverEvent* event ) override;
    void hoverMoveEvent( QHoverEvent* event ) override;

    void mouseReleaseEvent( QMouseEvent* event ) override;
    void mousePressEvent( QMouseEvent* event ) override;

    void keyPressEvent( QKeyEvent* event ) override;
    void keyReleaseEvent( QKeyEvent* event ) override;

    void focusInEvent( QFocusEvent* event ) override;
    QRectF focusIndicatorRect() const override;

    class PrivateData;
    std::unique_ptr< PrivateData > m_data;
};
