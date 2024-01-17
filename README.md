# QSkinny

![](https://github.com/uwerat/qskinny/workflows/CI/badge.svg)

[![CMake Build Matrix](https://github.com/vrcomputing/qskinny/actions/workflows/cmake.yml/badge.svg)](https://github.com/vrcomputing/qskinny/actions/workflows/cmake.yml)

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

Nontheless QSkinny is lacking in areas like documentation.
The QML API has not been completed after reaching a proof of concept state.

QSkinny is supposed to run on all platforms being supported by Qt/Quick.
It might support other versions of Qt, but you can rely on:

- Qt 5.15
- current long term supported ( LTS ) version of Qt ( at the moment Qt 6.5.x )
- current version of Qt

How to build, install and use QSkinny is described in this
[tutorial](doc/tutorials/03-writing-your-first-application.asciidoc).

If you want to know more about QSkinny - or even like to give it a specific
direction - please contact support@qskinny.org.

![IOT dashboard](/doc/images/iotdashboard.png)


# Contributing to QSkinny

QSkinny is licensed under the BSD 3 Clause License ( https://opensource.org/license/bsd-3-clause ). However we want to be able to offer more licenses on request.

If you are fine with these terms, you are welcome to contribute to QSkinny by
completing the following steps:

1. Agree to the terms of the [Contributor Agreement](https://gist.github.com/uwerat/76b2f3df5e246cc468231f55f1d35e47#file-qskinny-cla-md).
  You can create a pull request first, then the licensing bot will check
  automatically whether you already signed the Agreement or not. If you have not
  signed it yet, it will take you to the Agreement to agree.
1. Submit your pull request for review.
