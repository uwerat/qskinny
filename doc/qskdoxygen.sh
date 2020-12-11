#! /bin/sh

doxygen

find html -name 'class_*.html' -exec sed -i \
    -e '/Detailed Description/d' \
    -e '0,/Definition at line/{//d;}' \
    -e 's/Inheritance diagram for .*://' \
    {} \;
