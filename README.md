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

The code already provides a solid fundament for an automotive GUI with
currently ~300K lines of pure C++ code. As expected it results
in a good startup performance and a low memory footprint.

Nontheless QSkinny is lacking in areas like documentation or appealing
default skins. The QML API has not been completed after reaching a
proof of concept state. Furthermore the current selection of the implemented
controls is limited to the needs of the driving projects.

QSkinny is supposed to run on all platforms being supported by Qt/Quick.
But so far only Linux is actively tested. 
It might support all versions Qt >= 5.6, but you can rely on:

- Qt 5.6
- current long term supported ( LTS ) version of Qt
- current version of Qt

If you want to know more about QSkinny - or even like to give it a specific
direction - please contact support@qskinny.org.

[![FendtOne](https://www.fendt.com/int/geneva-assets/widget/28291/news-3-low.jpg)](https://www.fendt.com/int/fendt-700-vario-12894.html)<br />
( Photo: Copyright AGCO GmbH )
