/*
 *  SPDX-FileCopyrightText: 2016 Marco Martin <mart@kde.org>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */

import QtQuick 2.5
import QtQuick.Layouts 1.2
import QtQml.Models 2.2
import QtQuick.Templates 2.0 as T
import QtQuick.Controls 2.0 as QQC2
import org.kde.kirigami 2.7
import "private/globaltoolbar" as GlobalToolBar
import "templates" as KT

/**
 * PageRow implements a row-based navigation model, which can be used
 * with a set of interlinked information pages. Items are pushed in the
 * back of the row and the view scrolls until that row is visualized.
 * A PageRow can show a single page or a multiple set of columns, depending
 * on the window width: on a phone a single column should be fullscreen,
 * while on a tablet or a desktop more than one column should be visible.
 * @inherit QtQuick.Templates.Control
 */
T.Control {
    id: root

//BEGIN PROPERTIES
    /**
     * This property holds the number of items currently pushed onto the view
     */
    property alias depth: columnView.count

    /**
     * The last Page in the Row
     */
    readonly property Item lastItem: columnView.contentChildren.length > 0 ?  columnView.contentChildren[columnView.contentChildren.length - 1] : null

    /**
     * The currently visible Item
     */
    property alias currentItem: columnView.currentItem

    /**
     * the index of the currently visible Item
     */
    property alias currentIndex: columnView.currentIndex

    /**
     * The initial item when this PageRow is created
     */
    property variant initialPage

    /**
     * The main ColumnView of this Row
     */
    contentItem: columnView

    /**
     * columnView: Kirigami::ColumnView
     *
     * The ColumnView that this PageRow owns.
     * Generally, you shouldn't need to change
     * the value of this.
     *
     * @since 2.12
     */
    property alias columnView: columnView

    /**
     * items: list<Item>
     * All the items that are present in the PageRow
     * @since 2.6
     */
    property alias items: columnView.contentChildren;

    /**
     * visibleItems: list<Item>
     * All pages which are visible in the PageRow, excluding those which are scrolled away
     * @since 2.6
     */
    property alias visibleItems: columnView.visibleItems

    /**
     * firstVisibleItem: Item
     * The first at least partially visible page in the PageRow, pages before that one will be out of the viewport
     * @since 2.6
     */
    property alias firstVisibleItem: columnView.firstVisibleItem

    /**
     * lastVisibleItem: Item
     * The last at least partially visible page in the PageRow, pages after that one will be out of the viewport
     * @since 2.6
     */
    property alias lastVisibleItem: columnView.lastVisibleItem

    /**
     * The default width for a column
     * default is wide enough for 30 grid units.
     * Pages can override it with their Layout.fillWidth,
     * implicitWidth Layout.minimumWidth etc.
     */
    property int defaultColumnWidth: Units.gridUnit * 20

    /**
     * interactive: bool
     * If true it will be possible to go back/forward by dragging the
     * content themselves with a gesture.
     * Otherwise the only way to go back will be programmatically
     * default: true
     */
    property alias interactive: columnView.interactive

    /**
     * wideMode: bool
     * If true, the PageRow is wide enough that willshow more than one column at once
     * @since 5.37
     */
    readonly property bool wideMode: root.width >= root.defaultColumnWidth*2 && depth >= 2

    /**
     * separatorVisible: bool
     * True if the separator between pages should be visible
     * default: true
     * @since 5.38
     */
    property alias separatorVisible: columnView.separatorVisible

    /**
     * globalToolBar: grouped property
     * Controls the appearance of an optional global toolbar for the whole PageRow.
     * It's a grouped property comprised of the following properties:
     * * style (Kirigami.ApplicationHeaderStyle): can have the following values:
     *  * Auto: depending on application formfactor, it can behave automatically like other values, such as a Breadcrumb on mobile and ToolBar on desktop
     *  * Breadcrumb: it will show a breadcrumb of all the page titles in the stack, for easy navigation
     *  * Titles: each page will only have its own tile on top
     *  * TabBar: the global toolbar will look like a TabBar to select the pages
     *  * ToolBar: each page will have the title on top together buttons and menus to represent all of the page actions: not available on Mobile systems.
     *  * None: no global toolbar will be shown
     *
     * * actualStyle: this will represent the actual style of the toolbar: it can be different from style in the case style is Auto
     * * showNavigationButtons: OR flags combination of ApplicationHeaderStyle.ShowBackButton and ApplicationHeaderStyle.ShowForwardButton
     * * toolbarActionAlignment: How to horizontally align the actions when using the ToolBar style. Note that anything but Qt.AlignRight will cause the title to be hidden (default: Qt.AlignRight)
     * * minimumHeight (int): minimum height of the header, which will be resized when scrolling, only in Mobile mode (default: preferredHeight, sliding but no scaling)
     * * preferredHeight (int): the height the toolbar will usually have
     * * maximumHeight (int): the height the toolbar will have in mobile mode when the app is in reachable mode (default: preferredHeight * 1.5)
     * * leftReservedSpace (int, readonly): how many pixels are reserved at the left of the page toolbar (for navigation buttons or drawer handle)
     * * rightReservedSpace (int, readonly): how many pixels are reserved at the right of the page toolbar (drawer handle)
     *
     * @since 5.48
     */
    readonly property alias globalToolBar: globalToolBar

    implicitWidth: contentItem.implicitWidth + leftPadding + rightPadding
    implicitHeight: contentItem.implicitHeight + topPadding + bottomPadding
//END PROPERTIES

//BEGIN FUNCTIONS
    /**
     * Pushes a page on the stack.
     * The page can be defined as a component, item or string.
     * If an item is used then the page will get re-parented.
     * If a string is used then it is interpreted as a url that is used to load a page
     * component.
     * The last pushed page will become the current item.
     *
     * @param page The page can also be given as an array of pages.
     *     In this case all those pages will
     *     be pushed onto the stack. The items in the stack can be components, items or
     *     strings just like for single pages.
     *     Additionally an object can be used, which specifies a page and an optional
     *     properties property.
     *     This can be used to push multiple pages while still giving each of
     *     them properties.
     *     When an array is used the transition animation will only be to the last page.
     *
     * @param properties The properties argument is optional and allows defining a
     * map of properties to set on the page. If page is actually an array of pages, properties should also be an array of key/value maps
     * @return The new created page (or the last one if it was an array)
     */
    function push(page, properties) {
        var item = insertPage(depth, page, properties);
        currentIndex = depth - 1;
        return item;
    }

    /**
     * Inserts a new page or a list of new at an arbitrary position
     * The page can be defined as a component, item or string.
     * If an item is used then the page will get re-parented.
     * If a string is used then it is interpreted as a url that is used to load a page
     * component.
     * The current Page will not be changed, currentIndex will be adjusted
     * accordingly if needed to keep the same current page.
     *
     * @param page The page can also be given as an array of pages.
     *     In this case all those pages will
     *     be pushed onto the stack. The items in the stack can be components, items or
     *     strings just like for single pages.
     *     Additionally an object can be used, which specifies a page and an optional
     *     properties property.
     *     This can be used to push multiple pages while still giving each of
     *     them properties.
     *     When an array is used the transition animation will only be to the last page.
     *
     * @param properties The properties argument is optional and allows defining a
     * map of properties to set on the page. If page is actually an array of pages, properties should also be an array of key/value maps
     * @return The new created page (or the last one if it was an array)
     * @since 2.7
     */
    function insertPage(position, page, properties) {
        if (!page) {
            return null
        }
        //don't push again things already there
        if (page.createObject === undefined && typeof page != "string" && columnView.containsItem(page)) {
            print("The item " + page + " is already in the PageRow");
            return null;
        }

        position = Math.max(0, Math.min(depth, position));

        columnView.pop(columnView.currentItem);

        // figure out if more than one page is being pushed
        var pages;
        var propsArray = [];
        if (page instanceof Array) {
            pages = page;
            page = pages.pop();
            //compatibility with pre-qqc1 api, can probably be removed
            if (page.createObject === undefined && page.parent === undefined && typeof page != "string") {
                properties = properties || page.properties;
                page = page.page;
            }
        }
        if (properties instanceof Array) {
            propsArray = properties;
            properties = propsArray.pop();
        } else {
            propsArray = [properties];
        }

        // push any extra defined pages onto the stack
        if (pages) {
            var i;
            for (i = 0; i < pages.length; i++) {
                var tPage = pages[i];
                var tProps = propsArray[i];
                //compatibility with pre-qqc1 api, can probably be removed
                if (tPage.createObject === undefined && tPage.parent === undefined && typeof tPage != "string") {
                    if (columnView.containsItem(tPage)) {
                        print("The item " + page + " is already in the PageRow");
                        continue;
                    }
                    tProps = tPage.properties;
                    tPage = tPage.page;
                }

                var pageItem = pagesLogic.initAndInsertPage(position, tPage, tProps);
                ++position;
            }
        }

        // initialize the page
        var pageItem = pagesLogic.initAndInsertPage(position, page, properties);

        pagePushed(pageItem);

        return pageItem;
    }

    /**
     * Move the page at position fromPos to the new position toPos
     * If needed, currentIndex will be adjusted
     * in order to keep the same current page.
     * @since 2.7
     */
    function movePage(fromPos, toPos) {
        columnView.moveItem(fromPos, toPos);
    }

    /**
     * Remove the given page
     * @param page The page can be given both as integer position or by reference
     * @return The page that has just been removed
     * @since 2.7
     */
    function removePage(page) {
        if (depth == 0) {
            return null;
        }

        return columnView.removeItem(page);
    }

    /**
     * Pops a page off the stack.
     * @param page If page is specified then the stack is unwound to that page,
     * to unwind to the first page specify
     * page as null.
     * @return The page instance that was popped off the stack.
     */
    function pop(page) {
        if (depth == 0) {
            return null;
        }

        return columnView.pop(page);
    }

    /**
     * Emitted when a page has been inserted anywhere
     * @param position where the page has been inserted
     * @param page the new page
     * @since 2.7
     */
    signal pageInserted(int position, Item page)

    /**
     * Emitted when a page has been pushed to the bottom
     * @param page the new page
     * @since 2.5
     */
    signal pagePushed(Item page)

    /**
     * Emitted when a page has been removed from the row.
     * @param page the page that has been removed: at this point it's still valid,
     *           but may be auto deleted soon.
     * @since 2.5
     */
    signal pageRemoved(Item page)

    /**
     * Replaces a page on the stack.
     * @param page The page can also be given as an array of pages.
     *     In this case all those pages will
     *     be pushed onto the stack. The items in the stack can be components, items or
     *     strings just like for single pages.
     *     the current page and all pagest after it in the stack will be removed.
     *     Additionally an object can be used, which specifies a page and an optional
     *     properties property.
     *     This can be used to push multiple pages while still giving each of
     *     them properties.
     *     When an array is used the transition animation will only be to the last page.
     * @param properties The properties argument is optional and allows defining a
     * map of properties to set on the page.
     * @see push() for details.
     */
    function replace(page, properties) {
        if (currentIndex >= 1) {
            pop(columnView.contentChildren[currentIndex-1]);
        } else if (currentIndex == 0) {
            pop();
        } else {
            console.warn("There's no page to replace");
        }
        return push(page, properties);
    }

    /**
     * Clears the page stack.
     * Destroy (or reparent) all the pages contained.
     */
    function clear() {
        return columnView.clear();
    }

    /**
     * @return the page at idx
     * @param idx the depth of the page we want
     */
    function get(idx) {
        return columnView.contentChildren[idx];
    }

    /**
     * go back to the previous index and scroll to the left to show one more column
     */
    function flickBack() {
        if (depth > 1) {
            currentIndex = Math.max(0, currentIndex - 1);
        }
    }

    /**
     * layers: QtQuick.Controls.StackView
     * Access to the modal layers.
     * Sometimes an application needs a modal page that always covers all the rows.
     * For instance the full screen image of an image viewer or a settings page.
     * @since 5.38
     */
    property alias layers: layersStack
//END FUNCTIONS

    onInitialPageChanged: {
        if (initialPage) {
            clear();
            push(initialPage, null)
        }
    }
/*
    onActiveFocusChanged:  {
        if (activeFocus) {
            layersStack.currentItem.forceActiveFocus()
            if (columnView.activeFocus) {
                print("SSS"+columnView.currentItem)
                columnView.currentItem.forceActiveFocus();
            }
        }
    }
*/
    Keys.forwardTo: [currentItem]

    GlobalToolBar.PageRowGlobalToolBarStyleGroup {
        id: globalToolBar
        readonly property int leftReservedSpace: globalToolBarUI.item ? globalToolBarUI.item.leftReservedSpace : 0
        readonly property int rightReservedSpace: globalToolBarUI.item ? globalToolBarUI.item.rightReservedSpace : 0
        readonly property int height: globalToolBarUI.height
        readonly property Item leftHandleAnchor: globalToolBarUI.item ? globalToolBarUI.item.leftHandleAnchor : null
        readonly property Item rightHandleAnchor: globalToolBarUI.item ? globalToolBarUI.item.rightHandleAnchor : null
    }

    QQC2.StackView {
        id: layersStack
        z: 99
        anchors {
            fill: parent
        }
        //placeholder as initial item
        initialItem: columnView

        function clear () {
            //don't let it kill the main page row
            var d = layersStack.depth;
            for (var i = 1; i < d; ++i) {
                pop();
            }
        }

        popEnter: Transition {
            OpacityAnimator {
                from: 0
                to: 1
                duration: Units.longDuration
                easing.type: Easing.InOutCubic
            }
        }
        popExit: Transition {
            ParallelAnimation {
                OpacityAnimator {
                    from: 1
                    to: 0
                    duration: Units.longDuration
                    easing.type: Easing.InOutCubic
                }
                YAnimator {
                    from: 0
                    to: height/2
                    duration: Units.longDuration
                    easing.type: Easing.InCubic
                }
            }
        }

        pushEnter: Transition {
            ParallelAnimation {
                //NOTE: It's a PropertyAnimation instead of an Animator because with an animator the item will be visible for an instant before starting to fade
                PropertyAnimation {
                    property: "opacity"
                    from: 0
                    to: 1
                    duration: Units.longDuration
                    easing.type: Easing.InOutCubic
                }
                YAnimator {
                    from: height/2
                    to: 0
                    duration: Units.longDuration
                    easing.type: Easing.OutCubic
                }
            }
        }


        pushExit: Transition {
            OpacityAnimator {
                from: 1
                to: 0
                duration: Units.longDuration
                easing.type: Easing.InOutCubic
            }
        }

        replaceEnter: Transition {
            ParallelAnimation {
                OpacityAnimator {
                    from: 0
                    to: 1
                    duration: Units.longDuration
                    easing.type: Easing.InOutCubic
                }
                YAnimator {
                    from: height/2
                    to: 0
                    duration: Units.longDuration
                    easing.type: Easing.OutCubic
                }
            }
        }

        replaceExit: Transition {
            ParallelAnimation {
                OpacityAnimator {
                    from: 1
                    to: 0
                    duration: Units.longDuration
                    easing.type: Easing.InCubic
                }
                YAnimator {
                    from: 0
                    to: -height/2
                    duration: Units.longDuration
                    easing.type: Easing.InOutCubic
                }
            }
        }
    }

    Loader {
        id: globalToolBarUI
        anchors {
            left: parent.left
            top: parent.top
            right: parent.right
        }
        z: 100
        active: globalToolBar.actualStyle != ApplicationHeaderStyle.None || (firstVisibleItem && firstVisibleItem.globalToolBarStyle == ApplicationHeaderStyle.ToolBar)
        visible: active
        height: active ? implicitHeight : 0
        source: Qt.resolvedUrl("private/globaltoolbar/PageRowGlobalToolBarUI.qml");
    }

    QtObject {
        id: pagesLogic
        readonly property var componentCache: new Array()

        function initAndInsertPage(position, page, properties) {
            var pageComp;

            if (page.createObject) {
                // page defined as component
                pageComp = page;
            } else if (typeof page == "string") {
                // page defined as string (a url)
                pageComp = pagesLogic.componentCache[page];
                if (!pageComp) {
                    pageComp = pagesLogic.componentCache[page] = Qt.createComponent(page);
                }
            }
            if (pageComp) {
                // instantiate page from component
                // FIXME: parent directly to columnView or root?
                page = pageComp.createObject(null, properties || {});
                columnView.insertItem(position, page);

                if (pageComp.status === Component.Error) {
                    throw new Error("Error while loading page: " + pageComp.errorString());
                }
            } else {
                // copy properties to the page
                for (var prop in properties) {
                    if (properties.hasOwnProperty(prop)) {
                        page[prop] = properties[prop];
                    }
                }
                columnView.insertItem(position, page);
            }

            return page;
        }
    }

    ColumnView {
        id: columnView

        topPadding: globalToolBarUI.item && globalToolBarUI.item.breadcrumbVisible
                    ? globalToolBarUI.height : 0

        // Internal hidden api for Page
        readonly property Item __pageRow: root
        columnResizeMode: root.wideMode ? ColumnView.FixedColumns : ColumnView.SingleColumn
        columnWidth: root.defaultColumnWidth

        onItemInserted: root.pageInserted(position, item);
        onItemRemoved: root.pageRemoved(item);
    }

    Rectangle {
        anchors.bottom: parent.bottom
        height: Units.smallSpacing
        x: (columnView.width - width) * (columnView.contentX / (columnView.contentWidth - columnView.width))
        width: columnView.width * (columnView.width/columnView.contentWidth)
        color: Theme.textColor
        opacity: 0
        onXChanged: {
            opacity = 0.3
            scrollIndicatorTimer.restart();
        }
        Behavior on opacity {
            OpacityAnimator {
                duration: Units.longDuration
                easing.type: Easing.InOutQuad
            }
        }
        Timer {
            id: scrollIndicatorTimer
            interval: Units.longDuration * 4
            onTriggered: parent.opacity = 0;
        }
    }
}
