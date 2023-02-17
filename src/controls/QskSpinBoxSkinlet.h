/******************************************************************************
 * Copyright (C) 2023 Edelhirsch Software GmbH
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#pragma once

#include <QskSkinlet.h>

////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief
/// This skinlet's purpose is to draw a QskSpinBox instance.
/// @details
/// In order to manage individual subcontrol states this skinlet uses subcontrol sampling. Although
/// it is most usefull when dealing with dynamic or large numbers of subcontrols, it is a strategy
/// to index the subcontrol in order to have individual states instead of one collective state on
/// the skinnable object.
/// @note The placement and dimensions of all subcontrols depend on the following subctrontrol
/// aspects:
/// - QskSpinBox::Layout's alignment hint ( which affects the positions of all controls )
/// - QskSpinBox::Layout's spacing hint
/// - QskSpinBox::IncrementPanel's strut size hint
/// - QskSpinBox::DecrementPanel's strut size hint
/// - QskSpinBox::TextPanel's strut size hint
////////////////////////////////////////////////////////////////////////////////////////////////////
class QSK_EXPORT QskSpinBoxSkinlet : public QskSkinlet
{
    Q_GADGET
    using Inherited = QskSkinlet;

  public:
    /// @brief C-TOR defining the correct node's role order (e.g. panel before text)
    Q_INVOKABLE QskSpinBoxSkinlet( QskSkin* = nullptr );

    /// @brief Roles for the subcontrols.
    enum NodeRole
    {
        IncrementPanel, ///< Identifier for the increment button's panel.
        IncrementText,  ///< Identifier for the increment button's text.
        DecrementPanel, ///< Identifier for the decrement button's panel.
        DecrementText,  ///< Identifier for the decrement button's text.
        TextPanel,      ///< Identifier for the text's panel.
        TextText,       ///< Identifier for the text's glyphs.
        RoleCount       ///< Number of all roles in this skinlet.
    };

  protected:
    /// @brief Getter for the number of samples in this skinlet.
    /// @param skinnable The skinnable object.
    /// @param subControl The skinnable object's subcontrol.
    /// @returns Returns the number of samples.
    /// @retval Returns 1 since each subcontrol a sample;
    int sampleCount(
        const QskSkinnable* skinnable, QskAspect::Subcontrol subControl ) const override;

    /// @brief Getter for a subcontrol's sample rectangle.
    /// @param skinnable The skinnable object.
    /// @param rect The skinnable object's content rectangle.
    /// @param subControl The skinnable object's subcontrol.
    /// @param index The skinnable object's subcontrol sample index.
    /// @returns Returns the subcontrol's rectangle within the @p skinnable's content rectangle.
    QRectF sampleRect( const QskSkinnable* skinnable, const QRectF& rect,
        QskAspect::Subcontrol subControl, int index ) const override;

    /// @brief Getter for a subcontrol's sample states.
    /// @param skinnable The skinnable object.
    /// @param subControl The skinnable object's subcontrol.
    /// @param index The skinnable object's subcontrol sample index.
    /// @return Returns the states of the subcontrol's sample at the given @p index.
    /// @details Sets or unsets the @c pressed, @c hovered, @c focused bits in the returned states
    /// object.
    QskAspect::States sampleStates(
        const QskSkinnable* skinnable, QskAspect::Subcontrol subControl, int index ) const override;

    /// @brief Getter for the skinnable object's size hints.
    /// @param skinnable The skinnable object.
    /// @param sizeHint The size hint.
    /// @param rect The skinnable object's available rectangle.
    /// @details Calculates the minimum, maximum and preferred size of the skinnable.
    QSizeF sizeHint(
        const QskSkinnable* skinnable, Qt::SizeHint sizeHint, const QSizeF& rect ) const override;

    /// @brief Getter for the subcontrol's rectangle.
    /// @param skinnable The skinnable object.
    /// @param rect The skinnable object's content rectangle.
    /// @param subControl The skinnable object's subcontrol.
    /// @returns Returns the subcontrol's rectangle in the skinnable's content rectangle.
    QRectF subControlRect( const QskSkinnable* skinnable, const QRectF& rect,
        QskAspect::Subcontrol subControl ) const override;

    /// @brief Updates the scene graph @p node for the given @p role
    /// @param skinnable The skinnable object.
    /// @param role The node's role number ( see: QskSpinBoxSkinlet::NodeRole ).
    /// @param node The scene graph node for the given @p role.
    /// @returns Returns a new or updated scene graph node for the given @p role.
    /// @details This functions updates the text nodes and mediates updates for sampled
    /// subcontrols to QskSpinBoxSkinlet::updateSampleNode.
    /// @see QskSpinBoxSkinlet::NodeRole
    QSGNode* updateSubNode(
        const QskSkinnable* skinnable, quint8 role, QSGNode* node ) const override;

    /// @brief Updates the scene graph @p node for the given @p subControl's sample @p index
    /// @param skinnable The skinnable object.
    /// @param subControl The skinnable object's subcontrol.
    /// @param index The skinnable object's subcontrol sample index.
    /// @param node The scene graph node for the @p subControl's sample @p index.
    /// @returns Returns a new or updated scene graph node for the given @p subControl's sample @p
    /// index.
    QSGNode* updateSampleNode( const QskSkinnable* skinnable, QskAspect::Subcontrol subControl,
        int index, QSGNode* node ) const override;
};
