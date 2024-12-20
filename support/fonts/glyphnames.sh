#! /bin/sh
# dumps the glyph table from a icon font
otfinfo -u "$1" | sed -e 's/uni/0x/' -e 's/$/,/' | grep -v 0xFFFF
