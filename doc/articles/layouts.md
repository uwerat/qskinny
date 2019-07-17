# Qt Layouts

Writing a user interface, that automatically adjusts to different sizes, language,
font or theme changes is a challenge, that can be solved best with a layout system.
Such an implementation exists from the beginning for the [Qt framework]( https://www.qt.io )
and is still relevant for the desktop solution [Qt/Widgets]( https://doc.qt.io/qt-5/qtwidgets-index.html ).
It's ideas have been transferred to the [Qt/Graphics]( https://doc.qt.io/qt-5/qgraphicslayout.html )
module and in a restricted version to the newest technology [Qt/Quick](https://doc.qt.io/qt-5/qtquick-index.html).
Yet another variation can be found in the [QSkinny]( https://github.com/uwerat/qskinny ) library.

This article explains the Qt layout concept and compares the different implementations.

## Definitions

A layout calculates position and size ( = geometry ) for one or several
elements. Its algorithm relies on parameters, that are provided
from each element. Furthermore it also provides these parameters, so that it can
be used as an element in another layout.

A positioner finds the position without changing sizes.
Therefore every layout is a positioner, but a positioner is no layout.
To avoid confusion the term "positioner" will be used for the rest of this document
exclusivly for non layouts.

- minimum
- preferred
- maximum
- constraints
- policy
- explicit/implicit
- horizontal/vertical
- stretch
- more than one element per cell

+---------------+---------------+--------------------+
| Fruit         | Price         | Advantages         |
+===============+===============+====================+
| Bananas       | $1.34         | - built-in wrapper |
|               |               | - bright color     |
+---------------+---------------+--------------------+
| Oranges       | $2.10         | - cures scurvy     |
|               |               | - tasty            |
+---------------+---------------+--------------------+

~~~
{
  "firstName": "John",
  "lastName": "Smith",
  "age": 25
}
~~~
