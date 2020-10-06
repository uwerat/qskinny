/*
 *  SPDX-FileCopyrightText: 2016 Marco Martin <mart@kde.org>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */

import QtQuick 2.7
import QtQml 2.7
import QtQuick.Controls 2.5 as Controls
import org.kde.kirigami 2.11 as Kirigami

/**
 * An action used to load Pages coming from a common PagePool
 * in a PageRow or QtQuickControls2 StackView
 *
 * @inherit Action
 */
Kirigami.Action {
    id: root

    /**
     * page: string
     * Url or filename of the page this action will load
     */
    property string page

    /**
     * pagePool: Kirigami.PagePool
     * The PagePool used by this PagePoolAction.
     * PagePool will make sure only one instance of the page identified by the page url will be created and reused.
     * PagePool's lastLoaderUrl property will be used to control the mutual
     * exclusivity of the checked state of the PagePoolAction instances
     * sharing the same PagePool
     */
    property Kirigami.PagePool pagePool

    /**
     * pageStack: Kirigami.PageRow or QtQuickControls2 StackView
     * The component that will instantiate the pages, which has to work with a stack logic.
     * Kirigami.PageRow is recommended, but will work with QtQuicControls2 StackView as well.
     * By default this property is binded to ApplicationWindow's global
     * pageStack, which is a PageRow by default.
     */
    property Item pageStack: typeof applicationWindow != undefined ? applicationWindow().pageStack : null

    /**
     * basePage: Kirigami.Page
     * The page of pageStack new pages will be pushed after.
     * All pages present after the given basePage will be removed from the pageStack
     */
    property Controls.Page basePage

    /**
      * initialProperties: JavaScript Object
      * The initialProperties object specifies a map of initial property values for the created page
      * when it is pushed onto the Kirigami.PagePool.
      */
    property var initialProperties

    /** useLayers: bool
      * @since 5.70
      * @since org.kde.kirigami 2.12
      * When true the PagePoolAction will use the layers property of the pageStack.
      * This is intended for use with PageRow layers to allow PagePoolActions to
      * push context-specific pages onto the layers stack. 
      */
    property bool useLayers: false

    /**
      * Retrieve the page item held in the PagePool or null if it has not been loaded yet.
      */
    function pageItem() {
        return pagePool.pageForUrl(page)
    }

    /**
      * Return true if the page has been loaded and placed on pageStack.layers
      * and useLayers is true, otherwise returns null.
      */
    function layerContainsPage() {
        if (!useLayers || !pageStack.hasOwnProperty("layers")) return false

        var found = pageStack.layers.find((item, index) => {
            return item === pagePool.pageForUrl(page)
        })
        return found ? true: false
    }

    /**
      * Return true if the page has been loaded and placed on the pageStack,
      * otherwise returns null.
      */
    function stackContainsPage() {
        if (useLayers) return false
        return pageStack.columnView.containsItem(pagePool.pageForUrl(page))
    }

    checkable: true

    onTriggered: {
        if (page.length == 0 || !pagePool || !pageStack) {
            return;
        }

        // User intends to "go back" to this layer.
        if (layerContainsPage() && pageItem() !== pageStack.layers.currentItem) {
            pageStack.layers.replace(pageItem(), pageItem()) // force pop above
            return
        }

        // User intends to "go back" to this page.
        if (stackContainsPage()) {
            if (pageStack.hasOwnProperty("layers")) {
                pageStack.layers.clear()
            }
        }

        let pageStack_ = useLayers ? pageStack.layers : pageStack

        if (initialProperties && typeof(initialProperties) !== "object") {
            console.warn("initialProperties must be of type object");
            return;
        }

        if (!pageStack_.hasOwnProperty("pop") || typeof pageStack_.pop !== "function" || !pageStack_.hasOwnProperty("push") || typeof pageStack_.push !== "function") {
            return;
        }

        if (pagePool.isLocalUrl(page)) {
            if (basePage) {
                pageStack_.pop(basePage);

            } else if (!useLayers) {
                pageStack_.clear();
            }

            pageStack_.push(initialProperties ?
                               pagePool.loadPageWithProperties(page, initialProperties) :
                               pagePool.loadPage(page));
                               
        } else {
            var callback = function(item) {
                if (basePage) {
                    pageStack_.pop(basePage);

                } else if (!useLayers) {
                    pageStack_.clear();
                }
                pageStack_.push(item);
            };

            if (initialProperties) {
                pagePool.loadPage(page, initialProperties, callback);

            } else {
                pagePool.loadPage(page, callback);
            }
        }
    }

    // Exposing this as a property is required as Action does not have a default property
    property QtObject _private: QtObject {
        id: _private

        function setChecked(checked) {
            root.checked = checked
        }

        function clearLayers() {
            pageStack.layers.clear()
        }
        
        property list<Connections> connections: [
            Connections {
                target: pageStack

                onCurrentItemChanged: {
                    if (root.useLayers) {
                        if (root.layerContainsPage()) {
                            _private.clearLayers()
                        }
                        if (root.checkable)
                            _private.setChecked(false);

                    } else {
                        if (root.checkable)
                            _private.setChecked(root.stackContainsPage());
                    }
                }
            },
            Connections {
                enabled: pageStack.hasOwnProperty("layers")
                target: pageStack.layers

                onCurrentItemChanged: {
                    if (root.useLayers && root.checkable) {
                        _private.setChecked(root.layerContainsPage());

                    } else {
                        if (pageStack.layers.depth == 1 && root.stackContainsPage()) {
                            _private.setChecked(true)
                        }
                    }
                }
            }
        ]
    }
}
