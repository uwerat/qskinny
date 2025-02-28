#! /bin/sh

function symbol2qvg {
    fontfile=FluentSystemIcons-Regular.ttf
    glyph2qvg ${fontfile} 12 $1 qvg/$2.qvg
}


symbol2qvg 1724 checkmark
symbol2qvg 1792 chevron_down
symbol2qvg 1816 chevron_up
