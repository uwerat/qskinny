/*
 * SPDX-FileCopyrightText: 2020 Arjen Hiemstra <ahiemstra@heimr.nl>
 * 
 * SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#ifndef TOOLBARLAYOUT_H
#define TOOLBARLAYOUT_H

#include <memory>
#include <QQuickItem>

/**
 * Attached property for ToolBarLayout delegates.
 */
class ToolBarLayoutAttached : public QObject
{
    Q_OBJECT
    /**
     * The action this delegate was created for.
     */
    Q_PROPERTY(QObject *action READ action CONSTANT)
public:
    ToolBarLayoutAttached(QObject *parent = nullptr);

    QObject *action() const;
    void setAction(QObject *action);

private:
    QObject *m_action = nullptr;
};

/**
 * An item that creates delegates for actions and lays them out in a row.
 *
 * This effectively combines RowLayout and Repeater in a single item, with the
 * addition of some extra performance enhancing tweaks. It will create instances
 * of ::fullDelegate and ::itemDelegate for each action in ::actions . These are
 * then positioned horizontally. Any action that ends up being placed outside
 * the width of the item is hidden and will be part of ::hiddenActions.
 *
 * The items created as delegates are always created asynchronously, to avoid
 * creation lag spikes. Each delegate has access to the action it was created
 * for through the ToolBarLayoutAttached attached property.
 */
class ToolBarLayout : public QQuickItem
{
    Q_OBJECT
    /**
     * The actions this layout should create delegates for.
     */
    Q_PROPERTY(QQmlListProperty<QObject> actions READ actionsProperty NOTIFY actionsChanged)
    /**
     * A list of actions that do not fit in the current view and are thus hidden.
     */
    Q_PROPERTY(QList<QObject*> hiddenActions READ hiddenActions NOTIFY hiddenActionsChanged)
    /**
     * A component that is used to create full-size delegates from.
     *
     * Each delegate has three states, it can be full-size, icon-only or hidden.
     * By default, the full-size delegate is used. When the action has the
     * DisplayHint::IconOnly hint set, it will always use the iconDelegate. When
     * it has the DisplayHint::KeepVisible hint set, it will use the full-size
     * delegate when it fits. If not, it will use the iconDelegate, unless even
     * that does not fit, in which case it will still be hidden.
     */
    Q_PROPERTY(QQmlComponent *fullDelegate READ fullDelegate WRITE setFullDelegate NOTIFY fullDelegateChanged)
    /**
     * A component that is used to create icon-only delegates from.
     *
     * \sa fullDelegate
     */
    Q_PROPERTY(QQmlComponent *iconDelegate READ iconDelegate WRITE setIconDelegate NOTIFY iconDelegateChanged)
    /**
     * A component that is used to create the "more button" item from.
     *
     * The more button is shown when there are actions that do not fit the
     * current view. It is intended to have functionality to show these hidden
     * actions, like popup a menu with them showing.
     */
    Q_PROPERTY(QQmlComponent *moreButton READ moreButton WRITE setMoreButton NOTIFY moreButtonChanged)
    /**
     * The amount of spacing between individual delegates.
     */
    Q_PROPERTY(qreal spacing READ spacing WRITE setSpacing NOTIFY spacingChanged)
    /**
     * How to align the delegates within this layout.
     *
     * When there is more space available than required by the visible delegates,
     * we need to determine how to place the delegates. This property determines
     * how to do that. Note that the moreButton, if visible, will always be
     * placed at the end of the layout.
     */
    Q_PROPERTY(Qt::Alignment alignment READ alignment WRITE setAlignment NOTIFY alignmentChanged)
    /**
     * The combined width of visible delegates in this layout.
     */
    Q_PROPERTY(qreal visibleWidth READ visibleWidth NOTIFY visibleWidthChanged)
    /**
     * The minimum width this layout can have.
     *
     * This is equal to the width of the moreButton.
     */
    Q_PROPERTY(qreal minimumWidth READ minimumWidth NOTIFY minimumWidthChanged)
    /**
     * Which direction to layout in.
     *
     * This is primarily intended to support right-to-left layouts. When set to
     * LeftToRight, delegates will be layout with the first item on the left and
     * following items to the right of that. The more button will be placed at
     * the rightmost position. Alignment flags work normally.
     *
     * When set to RightToLeft, delegates will be layout with the first item on
     * the right and following items to the left of that. The more button will
     * be placed at the leftmost position. Alignment flags are inverted, so
     * AlignLeft will align items to the right, and vice-versa.
     */
    Q_PROPERTY(Qt::LayoutDirection layoutDirection READ layoutDirection WRITE setLayoutDirection NOTIFY layoutDirectionChanged)
    /**
     * How to handle items that do not match the toolbar's height.
     *
     * When toolbar items do not match the height of the toolbar, there are
     * several ways we can deal with this. This property sets the preferred way.
     *
     * The default is HeightMode::ConstrainIfLarger .
     *
     * \sa HeightMode
     */
    Q_PROPERTY(HeightMode heightMode READ heightMode WRITE setHeightMode NOTIFY heightModeChanged)

public:
    using ActionsProperty = QQmlListProperty<QObject>;

    /**
     * An enum describing several modes that can be used to deal with items with
     * a height that does not match the toolbar's height.
     */
    enum HeightMode {
        AlwaysCenter, ///< Always center items, allowing them to go outside the bounds of the layout if they are larger.
        AlwaysFill, ///< Always match the height of the layout. Larger items will be reduced in height, smaller items will be increased.
        ConstrainIfLarger, ///< If the item is larger than the toolbar, reduce its height. Otherwise center it in the toolbar.
    };
    Q_ENUM(HeightMode)

    ToolBarLayout(QQuickItem *parent = nullptr);
    ~ToolBarLayout();

    ActionsProperty actionsProperty() const;
    /**
     * Add an action to the list of actions.
     *
     * \param action The action to add.
     */
    void addAction(QObject *action);
    /**
     * Remove an action from the list of actions.
     *
     * \param action The action to remove.
     */
    void removeAction(QObject *action);
    /**
     * Clear the list of actions.
     */
    void clearActions();
    Q_SIGNAL void actionsChanged();

    QList<QObject*> hiddenActions() const;
    Q_SIGNAL void hiddenActionsChanged();

    QQmlComponent *fullDelegate() const;
    void setFullDelegate(QQmlComponent *newFullDelegate);
    Q_SIGNAL void fullDelegateChanged();

    QQmlComponent *iconDelegate() const;
    void setIconDelegate(QQmlComponent *newIconDelegate);
    Q_SIGNAL void iconDelegateChanged();

    QQmlComponent *moreButton() const;
    void setMoreButton(QQmlComponent *newMoreButton);
    Q_SIGNAL void moreButtonChanged();

    qreal spacing() const;
    void setSpacing(qreal newSpacing);
    Q_SIGNAL void spacingChanged();

    Qt::Alignment alignment() const;
    void setAlignment(Qt::Alignment newAlignment);
    Q_SIGNAL void alignmentChanged();

    qreal visibleWidth() const;
    Q_SIGNAL void visibleWidthChanged();

    qreal minimumWidth() const;
    Q_SIGNAL void minimumWidthChanged();

    Qt::LayoutDirection layoutDirection() const;
    void setLayoutDirection(Qt::LayoutDirection &newLayoutDirection);
    Q_SIGNAL void layoutDirectionChanged();

    HeightMode heightMode() const;
    void setHeightMode(HeightMode newHeightMode);
    Q_SIGNAL void heightModeChanged();

    /**
     * Queue a relayout of this layout.
     *
     * \note The layouting happens during the next scene graph polishing phase.
     */
    Q_SLOT void relayout();

    static ToolBarLayoutAttached *qmlAttachedProperties(QObject *object)
    {
        return new ToolBarLayoutAttached(object);
    }

protected:
    void componentComplete() override;
    void geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry) override;
    void itemChange(QQuickItem::ItemChange change, const QQuickItem::ItemChangeData &data) override;
    void updatePolish() override;

private:
    class Private;
    const std::unique_ptr<Private> d;
};

QML_DECLARE_TYPEINFO(ToolBarLayout, QML_HAS_ATTACHED_PROPERTIES)

#endif // TOOLBARLAYOUT_H
