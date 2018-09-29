QSkinny
-------

The (Q)Skinny library is a framework built on top of the Qt scene graph
and very few core classes from Qt/Quick. It offers a set of lightweight controls,
that can be used from C++ and/or QML.

Doing the implementation in C++ allows to make use of the "scene graph"
classes. Building controls from scene graph nodes allows for a lighter implementation
than found with stacking "heavier" objects like QObject or QQuickItem.
Offering a full featured C++ API allows the application code to benefit
from following the same strategies.

Being "skinny" also means a design that separates concerns between
the API and logic of the controls themselves, the styling of these controls,
and the delegated rendering of the controls to the screen.

The code already provides a solid fundament for an application with
currently ~150K lines of pure C++/GUI code. As expected it results
in a good startup performance a nd low memory footprint.

Nontheless the current state of the project is lacking in areas like
documentation or offering appealing default skins. Furthermore
the current selection of implemented controls is limited by the needs
of the driving projects.

QSkinny is sopposed to run in all environments, that are supported by Qt/Quick,
but only Linux is actively tested so far.

Currently it supports all version of Qt >= 5.6, but it is intended to always support:

- Qt 5.6
- The current long term supported ( LTS ) version of Qt
- The current version of Qt

If you want to know more about the (Q)Skinny or even like to give it a specific
direction you can contact support@qskinny.org.

This is a screenshot of a sample automotive UI (see examples/automotive):

![Automotive screenshot](https://github.com/uwerat/qskinny/blob/master/doc/automotive-screenshot.jpg?raw=true)
