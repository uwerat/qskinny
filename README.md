QSkinny
-------

The (Q)Skinny library contains a set of lightweight Qt Quick Controls.

It can be used from C++ and/or QML application code. 

Doing the implementation in C++ allows to make use of the "scene graph"
classes. Building controls from scene graph nodes allows for a lighter implementation
than found with stacking "heavier" objects like QObject or QQuickItem.
Offering a full featured C++ API allows the application code to benefit
from following the same strategies.

Being "skinny" also means a design that separates concerns between
the API and logic of the controls themselves, the styling of these controls,
and the delegated rendering of the controls to the screen.

The current selection of implemented controls is driven by the needs of specific
projects and therefore may feel a bit random. Conceptually, though, any type of
control fits into QSkinny, as long as it is usable from C++ (as opposed to only
QML).

