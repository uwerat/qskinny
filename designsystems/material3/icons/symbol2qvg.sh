#! /bin/sh

function symbol2qvg {
    fontfile=MaterialSymbolsOutlined.ttf
    glyph2qvg ${fontfile} 24 $1 qvg/$2.qvg
}

symbol2qvg 3067 check
symbol2qvg 3084 close

symbol2qvg 5022 arrow_drop_down
symbol2qvg 5023 arrow_drop_up

symbol2qvg 3082 add
symbol2qvg 3080 remove
