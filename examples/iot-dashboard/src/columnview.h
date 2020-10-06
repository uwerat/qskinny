/*
 *  SPDX-FileCopyrightText: 2019 Marco Martin <mart@kde.org>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */

#pragma once

#include <QQuickItem>
#include <QVariant>
#include <QPointer>

class ContentItem;
class ColumnView;

/**
 * This is an attached property to every item that is inserted in the ColumnView,
 * used to access the view and page information such as the position and informations for layouting, such as fillWidth
 * @since 2.7
 */
class ColumnViewAttached : public QObject
{
    Q_OBJECT

    /**
     * The index position of the column in the view, starting from 0
     */
    Q_PROPERTY(int index READ index WRITE setIndex NOTIFY indexChanged)

    /**
     * If true, the column will expand to take the whole viewport space minus reservedSpace
     */
    Q_PROPERTY(bool fillWidth READ fillWidth WRITE setFillWidth NOTIFY fillWidthChanged)

    /**
     * When a column is fillWidth, it will keep reservedSpace amount of pixels from going to fill the full viewport width
     */
    Q_PROPERTY(qreal reservedSpace READ reservedSpace WRITE setReservedSpace NOTIFY reservedSpaceChanged)

    /**
     * Like the same property of MouseArea, when this is true, the column view won't
     * try to manage events by itself when filtering from a child, not
     * disturbing user interaction
     */
    Q_PROPERTY(bool preventStealing READ preventStealing WRITE setPreventStealing NOTIFY preventStealingChanged)

    /**
     * If true the page will never go out of view, but will stay either
     * at the right or left side of the Columnview
     */
    Q_PROPERTY(bool pinned READ isPinned WRITE setPinned NOTIFY pinnedChanged)

    /**
     * The view this column belongs to
     */
    Q_PROPERTY(ColumnView *view READ view NOTIFY viewChanged)

public:
    ColumnViewAttached(QObject *parent = nullptr);
    ~ColumnViewAttached();

    void setIndex(int index);
    int  index() const;

    void setFillWidth(bool fill);
    bool fillWidth() const;

    qreal reservedSpace() const;
    void setReservedSpace(qreal space);

    ColumnView *view();
    void setView(ColumnView *view);

    //Private API, not for QML use
    QQuickItem *originalParent() const;
    void setOriginalParent(QQuickItem *parent);

    bool shouldDeleteOnRemove() const;
    void setShouldDeleteOnRemove(bool del);

    bool preventStealing() const;
    void setPreventStealing(bool prevent);

    bool isPinned() const;
    void setPinned(bool pinned);

Q_SIGNALS:
    void indexChanged();
    void fillWidthChanged();
    void reservedSpaceChanged();
    void viewChanged();
    void preventStealingChanged();
    void pinnedChanged();

private:
    int m_index = -1;
    bool m_fillWidth = false;
    qreal m_reservedSpace = 0;
    QPointer<ColumnView> m_view;
    QPointer<QQuickItem> m_originalParent;
    bool m_customFillWidth = false;
    bool m_customReservedSpace = false;
    bool m_shouldDeleteOnRemove = true;
    bool m_preventStealing = false;
    bool m_pinned = false;
};


/**
 * ColumnView is a container that lays out items horizontally in a row,
 * when not all items fit in the ColumnView, it will behave like a Flickable and will be a scrollable view which shows only a determined number of columns.
 * The columns can either all have the same fixed size (recommended),
 * size themselves with implicitWidth, or automatically expand to take all the available width: by default the last column will always be the expanding one.
 * Items inside the Columnview can access info of the view and set layouting hints via the Columnview attached property.
 *
 * This is the base for the implementation of PageRow
 * @since 2.7
 */
class ColumnView : public QQuickItem
{
    Q_OBJECT

    /**
     * The strategy to follow while automatically resizing the columns,
     * the enum can have the following values:
     * * FixedColumns: every column is fixed at the same width of the columnWidth property
     * * DynamicColumns: columns take their width from their implicitWidth
     * * SingleColumn: only one column at a time is shown, as wide as the viewport, eventual reservedSpace on the column's atttached property is ignored
     */
    Q_PROPERTY(ColumnResizeMode columnResizeMode READ columnResizeMode WRITE setColumnResizeMode NOTIFY columnResizeModeChanged)

    /**
     * The width of all columns when columnResizeMode is FixedColumns
     */
    Q_PROPERTY(qreal columnWidth READ columnWidth WRITE setColumnWidth NOTIFY columnWidthChanged)

    /**
     * How many columns this view containsItem*/
    Q_PROPERTY(int count READ count NOTIFY countChanged)

    /**
     * The position of the currently active column. The current column will also have keyboard focus
     */
    Q_PROPERTY(int currentIndex READ currentIndex WRITE setCurrentIndex NOTIFY currentIndexChanged)

    /**
     * The currently active column. The current column will also have keyboard focus
     */
    Q_PROPERTY(QQuickItem *currentItem READ currentItem NOTIFY currentItemChanged)

    /**
     * The main content item of this view: it's the parent of the column items
     */
    Q_PROPERTY(QQuickItem *contentItem READ contentItem CONSTANT)

    /**
     * The value of the horizontal scroll of the view, in pixels
     */
    Q_PROPERTY(qreal contentX READ contentX WRITE setContentX NOTIFY contentXChanged)

    /**
     * The compound width of all columns in the view
     */
    Q_PROPERTY(qreal contentWidth READ contentWidth NOTIFY contentWidthChanged)

    /**
     * The padding this will have at the top
     */
    Q_PROPERTY(qreal topPadding READ topPadding WRITE setTopPadding NOTIFY topPaddingChanged)

    /**
     * The padding this will have at the bottom
     */
    Q_PROPERTY(qreal bottomPadding READ bottomPadding WRITE setBottomPadding NOTIFY bottomPaddingChanged)

    /**
     * The duration for scrolling animations
     */
    Q_PROPERTY(int scrollDuration READ scrollDuration WRITE setScrollDuration NOTIFY scrollDurationChanged)

    /**
     * True if columns should be visually separed by a separator line
     */
    Q_PROPERTY(bool separatorVisible READ separatorVisible WRITE setSeparatorVisible NOTIFY separatorVisibleChanged)

    /**
     * The list of all visible column items that are at least partially in the viewport at any given moment
     */
    Q_PROPERTY(QList<QObject *> visibleItems READ visibleItems NOTIFY visibleItemsChanged)

    /**
     * The first of visibleItems provided from convenience
     */
    Q_PROPERTY(QQuickItem *firstVisibleItem READ firstVisibleItem NOTIFY firstVisibleItemChanged)

    /**
     * The last of visibleItems provided from convenience
     */
    Q_PROPERTY(QQuickItem *lastVisibleItem READ lastVisibleItem NOTIFY lastVisibleItemChanged)

    // Properties to make it similar to Flickable
    /**
     * True when the user is dragging around with touch gestures the view contents
     */
    Q_PROPERTY(bool dragging READ dragging NOTIFY draggingChanged)

    /**
     * True both when the user is dragging around with touch gestures the view contents or the view is animating
     */
    Q_PROPERTY(bool moving READ moving NOTIFY movingChanged)

    /**
     * True if it supports moving the contents by dragging
     */
    Q_PROPERTY(bool interactive READ interactive WRITE setInteractive NOTIFY interactiveChanged)

    // Default properties
    /**
     * Every column item the view contains
     */
    Q_PROPERTY(QQmlListProperty<QQuickItem> contentChildren READ contentChildren NOTIFY contentChildrenChanged FINAL)
    /**
     * every item declared inside the view, both visual and non-visual items
     */
    Q_PROPERTY(QQmlListProperty<QObject> contentData READ contentData  FINAL)
    Q_CLASSINFO("DefaultProperty", "contentData")

public:
    enum ColumnResizeMode {
        FixedColumns = 0,
        DynamicColumns,
        SingleColumn
    };
    Q_ENUM(ColumnResizeMode)

    ColumnView(QQuickItem *parent = nullptr);
    ~ColumnView();

    // QML property accessors
    ColumnResizeMode columnResizeMode() const;
    void setColumnResizeMode(ColumnResizeMode mode);

    qreal columnWidth() const;
    void setColumnWidth(qreal width);

    int currentIndex() const;
    void setCurrentIndex(int index);

    int scrollDuration() const;
    void setScrollDuration(int duration);

    bool separatorVisible() const;
    void setSeparatorVisible(bool visible);

    int count() const;

    qreal topPadding() const;
    void setTopPadding(qreal padding);

    qreal bottomPadding() const;
    void setBottomPadding(qreal padding);

    QQuickItem *currentItem();

    //NOTE: It's a QList<QObject *> as QML can't corectly build an Array out of QList<QQuickItem*>
    QList<QObject *> visibleItems() const;
    QQuickItem *firstVisibleItem() const;
    QQuickItem *lastVisibleItem() const;


    QQuickItem *contentItem() const;

    QQmlListProperty<QQuickItem> contentChildren();
    QQmlListProperty<QObject> contentData();

    bool dragging() const;
    bool moving() const;
    qreal contentWidth() const;

    qreal contentX() const;
    void setContentX(qreal x) const;

    bool interactive() const;
    void setInteractive(bool interactive);

    // Api not intended for QML use
    //can't do overloads in QML
    QQuickItem *removeItem(QQuickItem *item);
    QQuickItem *removeItem(int item);

    // QML attached property
    static ColumnViewAttached *qmlAttachedProperties(QObject *object);

public Q_SLOTS:
    /**
     * Pushes a new item at the end of the view
     * @param item the new item which will be reparented and managed
     */
    void addItem(QQuickItem *item);

    /**
     * Inserts a new item in the view at a given position.
     * The current Item will not be changed, currentIndex will be adjusted
     * accordingly if needed to keep the same current item.
     * @param pos the position we want the new item to be inserted in
     * @param item the new item which will be reparented and managed
     */
    void insertItem(int pos, QQuickItem *item);

    /**
     * Move an item inside the view.
     * The currentIndex property may be changed in order to keep currentItem the same.
     * @param from the old position
     * @param to the new position
     */
    void moveItem(int from, int to);

    /**
     * Removes an item from the view.
     * Items will be reparented to their old parent.
     * If they have JavaScript ownership and they didn't have an old parent, they will be destroyed.
     * CurrentIndex may be changed in order to keep the same currentItem
     * @param item it can either be a pointer of an item or an integer specifying the position to remove
     * @returns the item that has just been removed
     */
    QQuickItem *removeItem(const QVariant &item);

    /**
     * Removes all the items after item. Starting from the last column, every column will be removed until item is found, which will be left in place.
     * Items will be reparented to their old parent.
     * If they have JavaScript ownership and they didn't have an old parent, they will be destroyed
     * @param item the item which will be the new last one of the row.
     * @returns the last item that has been removed
     */
    QQuickItem *pop(QQuickItem *item);

    /**
     * Removes every item in the view.
     * Items will be reparented to their old parent.
     * If they have JavaScript ownership and they didn't have an old parent, they will be destroyed
     */
    void clear();

    /**
     * @returns true if the view contains the given item
     */
    bool containsItem(QQuickItem *item);

    /**
     * Returns the visible item containing the point x, y in content coordinates.
     * If there is no item at the point specified, or the item is not visible null is returned.
     */
    QQuickItem *itemAt(qreal x, qreal y);

protected:
    void classBegin() override;
    void componentComplete() override;
    void updatePolish() override;
    void itemChange(QQuickItem::ItemChange change, const QQuickItem::ItemChangeData &value) override;
    void geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry) override;
    bool childMouseEventFilter(QQuickItem *item, QEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseUngrabEvent() override;

Q_SIGNALS:
    /**
     * A new item has been inserted
     * @param position where the page has been inserted
     * @param item a pointer to the new item
     */
    void itemInserted(int position, QQuickItem *item);

    /**
     * An item has just been removed from the view
     * @param item a pointer to the item that has just been removed
     */
    void itemRemoved(QQuickItem *item);

    // Property notifiers
    void contentChildrenChanged();
    void columnResizeModeChanged();
    void columnWidthChanged();
    void currentIndexChanged();
    void currentItemChanged();
    void visibleItemsChanged();
    void countChanged();
    void draggingChanged();
    void movingChanged();
    void contentXChanged();
    void contentWidthChanged();
    void interactiveChanged();
    void scrollDurationChanged();
    void separatorVisibleChanged();
    void firstVisibleItemChanged();
    void lastVisibleItemChanged();
    void topPaddingChanged();
    void bottomPaddingChanged();

private:
    static void contentChildren_append(QQmlListProperty<QQuickItem> *prop, QQuickItem *object);
    static int contentChildren_count(QQmlListProperty<QQuickItem> *prop);
    static QQuickItem *contentChildren_at(QQmlListProperty<QQuickItem> *prop, int index);
    static void contentChildren_clear(QQmlListProperty<QQuickItem> *prop);

    static void contentData_append(QQmlListProperty<QObject> *prop, QObject *object);
    static int contentData_count(QQmlListProperty<QObject> *prop);
    static QObject *contentData_at(QQmlListProperty<QObject> *prop, int index);
    static void contentData_clear(QQmlListProperty<QObject> *prop);


    QList<QObject *> m_contentData;

    ContentItem *m_contentItem;
    QPointer<QQuickItem> m_currentItem;

    static QHash<QObject *, ColumnViewAttached *> m_attachedObjects;
    qreal m_oldMouseX = -1.0;
    qreal m_startMouseX = -1.0;
    int m_currentIndex = -1;
    qreal m_topPadding = 0;
    qreal m_bottomPadding = 0;

    bool m_mouseDown = false;
    bool m_interactive = true;
    bool m_dragging = false;
    bool m_moving = false;
    bool m_separatorVisible = true;
    bool m_complete = false;
};

QML_DECLARE_TYPEINFO(ColumnView, QML_HAS_ATTACHED_PROPERTIES)
