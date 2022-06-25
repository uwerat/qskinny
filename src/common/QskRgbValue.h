/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_RGB_VALUE_H
#define QSK_RGB_VALUE_H

#include "QskGlobal.h"
#include <qcolor.h>

#define QSK_RGB_VALUES \
    /* Web colors */ \
    RGBVALUE( AliceBlue, 0xfff0f8ff ) \
    RGBVALUE( AntiqueWhite, 0xfffaebd7 ) \
    RGBVALUE( Aqua, 0xff00ffff ) \
    RGBVALUE( Aquamarine, 0xff7fffd4 ) \
    RGBVALUE( Azure, 0xfff0ffff ) \
    RGBVALUE( Beige, 0xfff5f5dc ) \
    RGBVALUE( Bisque, 0xffffe4c4 ) \
    RGBVALUE( Black, 0xff000000 ) \
    RGBVALUE( BlanchedAlmond, 0xffffebcd ) \
    RGBVALUE( Blue, 0xff0000ff ) \
    RGBVALUE( BlueViolet, 0xff8a2be2 ) \
    RGBVALUE( Brown, 0xffa52a2a ) \
    RGBVALUE( Burlywood, 0xffdeb887 ) \
    RGBVALUE( CadetBlue, 0xff5f9ea0 ) \
    RGBVALUE( Chartreuse, 0xff7fff00 ) \
    RGBVALUE( Chocolate, 0xffd2691e ) \
    RGBVALUE( Coral, 0xffff7f50 ) \
    RGBVALUE( CornflowerBlue, 0xff6495ed ) \
    RGBVALUE( Cornsilk, 0xfffff8dc ) \
    RGBVALUE( Crimson, 0xffdc143c ) \
    RGBVALUE( Cyan, 0xff00ffff ) \
    RGBVALUE( DarkBlue, 0xff00008b ) \
    RGBVALUE( DarkCyan, 0xff008b8b ) \
    RGBVALUE( DarkGoldenrod, 0xffb8860b ) \
    RGBVALUE( DarkGray, 0xffa9a9a9 ) \
    RGBVALUE( DarkGreen, 0xff006400 ) \
    RGBVALUE( DarkGrey, 0xffa9a9a9 ) \
    RGBVALUE( DarkKhaki, 0xffbdb76b ) \
    RGBVALUE( DarkMagenta, 0xff8b008b ) \
    RGBVALUE( DarkOliveGreen, 0xff556b2f ) \
    RGBVALUE( DarkOrange, 0xffff8c00 ) \
    RGBVALUE( DarkOrchid, 0xff9932cc ) \
    RGBVALUE( DarkRed, 0xff8b0000 ) \
    RGBVALUE( DarkSalmon, 0xffe9967a ) \
    RGBVALUE( DarkSeaGreen, 0xff8fbc8f ) \
    RGBVALUE( DarkSlateBlue, 0xff483d8b ) \
    RGBVALUE( DarkSlateGray, 0xff2f4f4f ) \
    RGBVALUE( DarkSlateGrey, 0xff2f4f4f ) \
    RGBVALUE( DarkTurquoise, 0xff00ced1 ) \
    RGBVALUE( DarkViolet, 0xff9400d3 ) \
    RGBVALUE( DeepPink, 0xffff1493 ) \
    RGBVALUE( DeepSkyBlue, 0xff00bfff ) \
    RGBVALUE( DimGray, 0xff696969 ) \
    RGBVALUE( DimGrey, 0xff696969 ) \
    RGBVALUE( DodgerBlue, 0xff1e90ff ) \
    RGBVALUE( FireBrick, 0xffb22222 ) \
    RGBVALUE( FloralWhite, 0xfffffaf0 ) \
    RGBVALUE( ForestGreen, 0xff228b22 ) \
    RGBVALUE( Fuchsia, 0xffff00ff ) \
    RGBVALUE( Gainsboro, 0xffdcdcdc ) \
    RGBVALUE( GhostWhite, 0xfff8f8ff ) \
    RGBVALUE( Gold, 0xffffd700 ) \
    RGBVALUE( Goldenrod, 0xffdaa520 ) \
    RGBVALUE( Gray, 0xff808080 ) \
    RGBVALUE( Green, 0xff008000 ) \
    RGBVALUE( GreenYellow, 0xffadff2f ) \
    RGBVALUE( Grey, 0xff808080 ) \
    RGBVALUE( Honeydew, 0xfff0fff0 ) \
    RGBVALUE( HotPink, 0xffff69b4 ) \
    RGBVALUE( IndianRed, 0xffcd5c5c ) \
    RGBVALUE( Indigo, 0xff4b0082 ) \
    RGBVALUE( Ivory, 0xfffffff0 ) \
    RGBVALUE( Khaki, 0xfff0e68c ) \
    RGBVALUE( Lavender, 0xffe6e6fa ) \
    RGBVALUE( LavenderBlush, 0xfffff0f5 ) \
    RGBVALUE( LawnGreen, 0xff7cfc00 ) \
    RGBVALUE( LemonChiffon, 0xfffffacd ) \
    RGBVALUE( LightBlue, 0xffadd8e6 ) \
    RGBVALUE( LightCoral, 0xfff08080 ) \
    RGBVALUE( LightCyan, 0xffe0ffff ) \
    RGBVALUE( LightGoldenrodYellow, 0xfffafad2 ) \
    RGBVALUE( LightGray, 0xffd3d3d3 ) \
    RGBVALUE( LightGreen, 0xff90ee90 ) \
    RGBVALUE( LightGrey, 0xffd3d3d3 ) \
    RGBVALUE( LightPink, 0xffffb6c1 ) \
    RGBVALUE( LightSalmon, 0xffffa07a ) \
    RGBVALUE( LightSeaGreen, 0xff20b2aa ) \
    RGBVALUE( LightSkyBlue, 0xff87cefa ) \
    RGBVALUE( LightSlateGray, 0xff778899 ) \
    RGBVALUE( LightSlateGrey, 0xff778899 ) \
    RGBVALUE( LightSteelBlue, 0xffb0c4de ) \
    RGBVALUE( LightYellow, 0xffffffe0 ) \
    RGBVALUE( Lime, 0xff00ff00 ) \
    RGBVALUE( Limegreen, 0xff32cd32 ) \
    RGBVALUE( Linen, 0xfffaf0e6 ) \
    RGBVALUE( Magenta, 0xffff00ff ) \
    RGBVALUE( Maroon, 0xff800000 ) \
    RGBVALUE( MediumAquamarine, 0xff66cdaa ) \
    RGBVALUE( MediumBlue, 0xff0000cd ) \
    RGBVALUE( MediumOrchid, 0xffba55d3 ) \
    RGBVALUE( MediumPurple, 0xff9370db ) \
    RGBVALUE( MediumSeaGreen, 0xff3cb371 ) \
    RGBVALUE( MediumSlateBlue, 0xff7b68ee ) \
    RGBVALUE( MediumSpringGreen, 0xff00fa9a ) \
    RGBVALUE( MediumTurquoise, 0xff48d1cc ) \
    RGBVALUE( MediumVioletRed, 0xffc71585 ) \
    RGBVALUE( MidnightBlue, 0xff191970 ) \
    RGBVALUE( MintCream, 0xfff5fffa ) \
    RGBVALUE( MistyRose, 0xffffe4e1 ) \
    RGBVALUE( Moccasin, 0xffffe4b5 ) \
    RGBVALUE( NavajoWhite, 0xffffdead ) \
    RGBVALUE( Navy, 0xff000080 ) \
    RGBVALUE( OldLace, 0xfffdf5e6 ) \
    RGBVALUE( Olive, 0xff808000 ) \
    RGBVALUE( OliveDrab, 0xff6b8e23 ) \
    RGBVALUE( Orange, 0xffffa500 ) \
    RGBVALUE( OrangeRed, 0xffff4500 ) \
    RGBVALUE( Orchid, 0xffda70d6 ) \
    RGBVALUE( PaleGoldenrod, 0xffeee8aa ) \
    RGBVALUE( PaleGreen, 0xff98fb98 ) \
    RGBVALUE( PaleTurquoise, 0xffafeeee ) \
    RGBVALUE( PaleVioletRed, 0xffdb7093 ) \
    RGBVALUE( PapayaWhip, 0xffffefd5 ) \
    RGBVALUE( PeachPuff, 0xffffdab9 ) \
    RGBVALUE( Peru, 0xffcd853f ) \
    RGBVALUE( Pink, 0xffffc0cb ) \
    RGBVALUE( Plum, 0xffdda0dd ) \
    RGBVALUE( PowderBlue, 0xffb0e0e6 ) \
    RGBVALUE( Purple, 0xff800080 ) \
    RGBVALUE( Red, 0xffff0000 ) \
    RGBVALUE( RosyBrown, 0xffbc8f8f ) \
    RGBVALUE( RoyalBlue, 0xff4169e1 ) \
    RGBVALUE( SaddleBrown, 0xff8b4513 ) \
    RGBVALUE( Salmon, 0xfffa8072 ) \
    RGBVALUE( SandyBrown, 0xfff4a460 ) \
    RGBVALUE( SeaGreen, 0xff2e8b57 ) \
    RGBVALUE( Seashell, 0xfffff5ee ) \
    RGBVALUE( Sienna, 0xffa0522d ) \
    RGBVALUE( Silver, 0xffc0c0c0 ) \
    RGBVALUE( SkyBlue, 0xff87ceeb ) \
    RGBVALUE( SlateBlue, 0xff6a5acd ) \
    RGBVALUE( SlateGrey, 0xff708090 ) \
    RGBVALUE( Snow, 0xfffffafa ) \
    RGBVALUE( SpringGreen, 0xff00ff7f ) \
    RGBVALUE( SteelBlue, 0xff4682b4 ) \
    RGBVALUE( Tan, 0xffd2b48c ) \
    RGBVALUE( Teal, 0xff008080 ) \
    RGBVALUE( Thistle, 0xffd8bfd8 ) \
    RGBVALUE( Tomato, 0xffff6347 ) \
    RGBVALUE( Turquoise, 0xff40e0d0 ) \
    RGBVALUE( Violet, 0xffee82ee ) \
    RGBVALUE( Wheat, 0xfff5deb3 ) \
    RGBVALUE( WhiteSmoke, 0xfff5f5f5 ) \
    RGBVALUE( Yellow, 0xffffff00 ) \
    RGBVALUE( YellowGreen, 0xff9acd32 ) \
    RGBVALUE( White, 0xffffffff ) \
    /* Material colors from https://material.google.com/style/color.html */ \
    RGBVALUE( Red50, 0xffffebee ) \
    RGBVALUE( Red100, 0xffffcdd2 ) \
    RGBVALUE( Red200, 0xffef9a9a ) \
    RGBVALUE( Red300, 0xffe57373 ) \
    RGBVALUE( Red400, 0xffef5350 ) \
    RGBVALUE( Red500, 0xfff44336 ) \
    RGBVALUE( Red600, 0xffe53935 ) \
    RGBVALUE( Red700, 0xffd32f2f ) \
    RGBVALUE( Red800, 0xffc62828 ) \
    RGBVALUE( Red900, 0xffb71c1c ) \
    RGBVALUE( RedA100, 0xffff8a80 ) \
    RGBVALUE( RedA200, 0xffff5252 ) \
    RGBVALUE( RedA400, 0xffff1744 ) \
    RGBVALUE( RedA700, 0xffd50000 ) \
    \
    RGBVALUE( Pink50, 0xfffce4ec ) \
    RGBVALUE( Pink100, 0xfff8bbd0 ) \
    RGBVALUE( Pink200, 0xfff48fb1 ) \
    RGBVALUE( Pink300, 0xfff06292 ) \
    RGBVALUE( Pink400, 0xffec407a ) \
    RGBVALUE( Pink500, 0xffe91e63 ) \
    RGBVALUE( Pink600, 0xffd81b60 ) \
    RGBVALUE( Pink700, 0xffc2185b ) \
    RGBVALUE( Pink800, 0xffad1457 ) \
    RGBVALUE( Pink900, 0xff880e4f ) \
    RGBVALUE( PinkA100, 0xffff80ab ) \
    RGBVALUE( PinkA200, 0xffff4081 ) \
    RGBVALUE( PinkA400, 0xfff50057 ) \
    RGBVALUE( PinkA700, 0xffc51162 ) \
    \
    RGBVALUE( Purple50, 0xfff3e5f5 ) \
    RGBVALUE( Purple100, 0xffe1bee7 ) \
    RGBVALUE( Purple200, 0xffce93d8 ) \
    RGBVALUE( Purple300, 0xffba68c8 ) \
    RGBVALUE( Purple400, 0xffab47bc ) \
    RGBVALUE( Purple500, 0xff9c27b0 ) \
    RGBVALUE( Purple600, 0xff8e24aa ) \
    RGBVALUE( Purple700, 0xff7b1fa2 ) \
    RGBVALUE( Purple800, 0xff6a1b9a ) \
    RGBVALUE( Purple900, 0xff4a148c ) \
    RGBVALUE( PurpleA100, 0xffea80fc ) \
    RGBVALUE( PurpleA200, 0xffe040fb ) \
    RGBVALUE( PurpleA400, 0xffd500f9 ) \
    RGBVALUE( PurpleA700, 0xffaa00ff ) \
    \
    RGBVALUE( DeepPurple50, 0xffede7f6 ) \
    RGBVALUE( DeepPurple100, 0xffd1c4e9 ) \
    RGBVALUE( DeepPurple200, 0xffb39ddb ) \
    RGBVALUE( DeepPurple300, 0xff9575cd ) \
    RGBVALUE( DeepPurple400, 0xff7e57c2 ) \
    RGBVALUE( DeepPurple500, 0xff673ab7 ) \
    RGBVALUE( DeepPurple600, 0xff5e35b1 ) \
    RGBVALUE( DeepPurple700, 0xff512da8 ) \
    RGBVALUE( DeepPurple800, 0xff4527a0 ) \
    RGBVALUE( DeepPurple900, 0xff311b92 ) \
    RGBVALUE( DeepPurpleA100, 0xffb388ff ) \
    RGBVALUE( DeepPurpleA200, 0xff7c4dff ) \
    RGBVALUE( DeepPurpleA400, 0xff651fff ) \
    RGBVALUE( DeepPurpleA700, 0xff6200ea ) \
    \
    RGBVALUE( Indigo50, 0xffe8eaf6 ) \
    RGBVALUE( Indigo100, 0xffc5cae9 ) \
    RGBVALUE( Indigo200, 0xff9fa8da ) \
    RGBVALUE( Indigo300, 0xff7986cb ) \
    RGBVALUE( Indigo400, 0xff5c6bc0 ) \
    RGBVALUE( Indigo500, 0xff3f51b5 ) \
    RGBVALUE( Indigo600, 0xff3949ab ) \
    RGBVALUE( Indigo700, 0xff303f9f ) \
    RGBVALUE( Indigo800, 0xff283593 ) \
    RGBVALUE( Indigo900, 0xff1a237e ) \
    RGBVALUE( IndigoA100, 0xff8c9eff ) \
    RGBVALUE( IndigoA200, 0xff536dfe ) \
    RGBVALUE( IndigoA400, 0xff3d5afe ) \
    RGBVALUE( IndigoA700, 0xff304ffe ) \
    \
    RGBVALUE( Blue50, 0xffe3f2fd ) \
    RGBVALUE( Blue100, 0xffbbdefb ) \
    RGBVALUE( Blue200, 0xff90caf9 ) \
    RGBVALUE( Blue300, 0xff64b5f6 ) \
    RGBVALUE( Blue400, 0xff42a5f5 ) \
    RGBVALUE( Blue500, 0xff2196f3 ) \
    RGBVALUE( Blue600, 0xff1e88e5 ) \
    RGBVALUE( Blue700, 0xff1976d2 ) \
    RGBVALUE( Blue800, 0xff1565c0 ) \
    RGBVALUE( Blue900, 0xff0d47a1 ) \
    RGBVALUE( BlueA100, 0xff82b1ff ) \
    RGBVALUE( BlueA200, 0xff448aff ) \
    RGBVALUE( BlueA400, 0xff2979ff ) \
    RGBVALUE( BlueA700, 0xff2962ff ) \
    \
    RGBVALUE( LightBlue50, 0xffe1f5fe ) \
    RGBVALUE( LightBlue100, 0xffb3e5fc ) \
    RGBVALUE( LightBlue200, 0xff81d4fa ) \
    RGBVALUE( LightBlue300, 0xff4fc3f7 ) \
    RGBVALUE( LightBlue400, 0xff29b6f6 ) \
    RGBVALUE( LightBlue500, 0xff03a9f4 ) \
    RGBVALUE( LightBlue600, 0xff039be5 ) \
    RGBVALUE( LightBlue700, 0xff0288d1 ) \
    RGBVALUE( LightBlue800, 0xff0277bd ) \
    RGBVALUE( LightBlue900, 0xff01579b ) \
    RGBVALUE( LightBlueA100, 0xff80d8ff ) \
    RGBVALUE( LightBlueA200, 0xff40c4ff ) \
    RGBVALUE( LightBlueA400, 0xff00b0ff ) \
    RGBVALUE( LightBlueA700, 0xff0091ea ) \
    \
    RGBVALUE( Cyan50, 0xffe0f7fa ) \
    RGBVALUE( Cyan100, 0xffb2ebf2 ) \
    RGBVALUE( Cyan200, 0xff80deea ) \
    RGBVALUE( Cyan300, 0xff4dd0e1 ) \
    RGBVALUE( Cyan400, 0xff26c6da ) \
    RGBVALUE( Cyan500, 0xff00bcd4 ) \
    RGBVALUE( Cyan600, 0xff00acc1 ) \
    RGBVALUE( Cyan700, 0xff0097a7 ) \
    RGBVALUE( Cyan800, 0xff00838f ) \
    RGBVALUE( Cyan900, 0xff006064 ) \
    RGBVALUE( CyanA100, 0xff84ffff ) \
    RGBVALUE( CyanA200, 0xff18ffff ) \
    RGBVALUE( CyanA400, 0xff00e5ff ) \
    RGBVALUE( CyanA700, 0xff00b8d4 ) \
    \
    RGBVALUE( Teal50, 0xffe0f2f1 ) \
    RGBVALUE( Teal100, 0xffb2dfdb ) \
    RGBVALUE( Teal200, 0xff80cbc4 ) \
    RGBVALUE( Teal300, 0xff4db6ac ) \
    RGBVALUE( Teal400, 0xff26a69a ) \
    RGBVALUE( Teal500, 0xff009688 ) \
    RGBVALUE( Teal600, 0xff00897b ) \
    RGBVALUE( Teal700, 0xff00796b ) \
    RGBVALUE( Teal800, 0xff00695c ) \
    RGBVALUE( Teal900, 0xff004d40 ) \
    RGBVALUE( TealA100, 0xffa7ffeb ) \
    RGBVALUE( TealA200, 0xff64ffda ) \
    RGBVALUE( TealA400, 0xff1de9b6 ) \
    RGBVALUE( TealA700, 0xff00bfa5 ) \
    \
    RGBVALUE( Green50, 0xffe8f5e9 ) \
    RGBVALUE( Green100, 0xffc8e6c9 ) \
    RGBVALUE( Green200, 0xffa5d6a7 ) \
    RGBVALUE( Green300, 0xff81c784 ) \
    RGBVALUE( Green400, 0xff66bb6a ) \
    RGBVALUE( Green500, 0xff4caf50 ) \
    RGBVALUE( Green600, 0xff43a047 ) \
    RGBVALUE( Green700, 0xff388e3c ) \
    RGBVALUE( Green800, 0xff2e7d32 ) \
    RGBVALUE( Green900, 0xff1b5e20 ) \
    RGBVALUE( GreenA100, 0xffb9f6ca ) \
    RGBVALUE( GreenA200, 0xff69f0ae ) \
    RGBVALUE( GreenA400, 0xff00e676 ) \
    RGBVALUE( GreenA700, 0xff00c853 ) \
    \
    RGBVALUE( LightGreen50, 0xfff1f8e9 ) \
    RGBVALUE( LightGreen100, 0xffdcedc8 ) \
    RGBVALUE( LightGreen200, 0xffc5e1a5 ) \
    RGBVALUE( LightGreen300, 0xffaed581 ) \
    RGBVALUE( LightGreen400, 0xff9ccc65 ) \
    RGBVALUE( LightGreen500, 0xff8bc34a ) \
    RGBVALUE( LightGreen600, 0xff7cb342 ) \
    RGBVALUE( LightGreen700, 0xff689f38 ) \
    RGBVALUE( LightGreen800, 0xff558b2f ) \
    RGBVALUE( LightGreen900, 0xff33691e ) \
    RGBVALUE( LightGreenA100, 0xffccff90 ) \
    RGBVALUE( LightGreenA200, 0xffb2ff59 ) \
    RGBVALUE( LightGreenA400, 0xff76ff03 ) \
    RGBVALUE( LightGreenA700, 0xff64dd17 ) \
    \
    RGBVALUE( Lime50, 0xfff9fbe7 ) \
    RGBVALUE( Lime100, 0xfff0f4c3 ) \
    RGBVALUE( Lime200, 0xffe6ee9c ) \
    RGBVALUE( Lime300, 0xffdce775 ) \
    RGBVALUE( Lime400, 0xffd4e157 ) \
    RGBVALUE( Lime500, 0xffcddc39 ) \
    RGBVALUE( Lime600, 0xffc0ca33 ) \
    RGBVALUE( Lime700, 0xffafb42b ) \
    RGBVALUE( Lime800, 0xff9e9d24 ) \
    RGBVALUE( Lime900, 0xff827717 ) \
    RGBVALUE( LimeA100, 0xfff4ff81 ) \
    RGBVALUE( LimeA200, 0xffeeff41 ) \
    RGBVALUE( LimeA400, 0xffc6ff00 ) \
    RGBVALUE( LimeA700, 0xffaeea00 ) \
    \
    RGBVALUE( Yellow50, 0xfffffde7 ) \
    RGBVALUE( Yellow100, 0xfffff9c4 ) \
    RGBVALUE( Yellow200, 0xfffff59d ) \
    RGBVALUE( Yellow300, 0xfffff176 ) \
    RGBVALUE( Yellow400, 0xffffee58 ) \
    RGBVALUE( Yellow500, 0xffffeb3b ) \
    RGBVALUE( Yellow600, 0xfffdd835 ) \
    RGBVALUE( Yellow700, 0xfffbc02d ) \
    RGBVALUE( Yellow800, 0xfff9a825 ) \
    RGBVALUE( Yellow900, 0xfff57f17 ) \
    RGBVALUE( YellowA100, 0xffffff8d ) \
    RGBVALUE( YellowA200, 0xffffff00 ) \
    RGBVALUE( YellowA400, 0xffffea00 ) \
    RGBVALUE( YellowA700, 0xffffd600 ) \
    \
    RGBVALUE( Amber50, 0xfffff8e1 ) \
    RGBVALUE( Amber100, 0xffffecb3 ) \
    RGBVALUE( Amber200, 0xffffe082 ) \
    RGBVALUE( Amber300, 0xffffd54f ) \
    RGBVALUE( Amber400, 0xffffca28 ) \
    RGBVALUE( Amber500, 0xffffc107 ) \
    RGBVALUE( Amber600, 0xffffb300 ) \
    RGBVALUE( Amber700, 0xffffa000 ) \
    RGBVALUE( Amber800, 0xffff8f00 ) \
    RGBVALUE( Amber900, 0xffff6f00 ) \
    RGBVALUE( AmberA100, 0xffffe57f ) \
    RGBVALUE( AmberA200, 0xffffd740 ) \
    RGBVALUE( AmberA400, 0xffffc400 ) \
    RGBVALUE( AmberA700, 0xffffab00 ) \
    \
    RGBVALUE( Orange50, 0xfffff3e0 ) \
    RGBVALUE( Orange100, 0xffffe0b2 ) \
    RGBVALUE( Orange200, 0xffffcc80 ) \
    RGBVALUE( Orange300, 0xffffb74d ) \
    RGBVALUE( Orange400, 0xffffa726 ) \
    RGBVALUE( Orange500, 0xffff9800 ) \
    RGBVALUE( Orange600, 0xfffb8c00 ) \
    RGBVALUE( Orange700, 0xfff57c00 ) \
    RGBVALUE( Orange800, 0xffef6c00 ) \
    RGBVALUE( Orange900, 0xffe65100 ) \
    RGBVALUE( OrangeA100, 0xffffd180 ) \
    RGBVALUE( OrangeA200, 0xffffab40 ) \
    RGBVALUE( OrangeA400, 0xffff9100 ) \
    RGBVALUE( OrangeA700, 0xffff6d00 ) \
    \
    RGBVALUE( DeepOrange50, 0xfffbe9e7 ) \
    RGBVALUE( DeepOrange100, 0xffffccbc ) \
    RGBVALUE( DeepOrange200, 0xffffab91 ) \
    RGBVALUE( DeepOrange300, 0xffff8a65 ) \
    RGBVALUE( DeepOrange400, 0xffff7043 ) \
    RGBVALUE( DeepOrange500, 0xffff5722 ) \
    RGBVALUE( DeepOrange600, 0xfff4511e ) \
    RGBVALUE( DeepOrange700, 0xffe64a19 ) \
    RGBVALUE( DeepOrange800, 0xffd84315 ) \
    RGBVALUE( DeepOrange900, 0xffbf360c ) \
    RGBVALUE( DeepOrangeA100, 0xffff9e80 ) \
    RGBVALUE( DeepOrangeA200, 0xffff6e40 ) \
    RGBVALUE( DeepOrangeA400, 0xffff3d00 ) \
    RGBVALUE( DeepOrangeA700, 0xffdd2c00 ) \
    \
    RGBVALUE( Brown50, 0xffefebe9 ) \
    RGBVALUE( Brown100, 0xffd7ccc8 ) \
    RGBVALUE( Brown200, 0xffbcaaa4 ) \
    RGBVALUE( Brown300, 0xffa1887f ) \
    RGBVALUE( Brown400, 0xff8d6e63 ) \
    RGBVALUE( Brown500, 0xff795548 ) \
    RGBVALUE( Brown600, 0xff6d4c41 ) \
    RGBVALUE( Brown700, 0xff5d4037 ) \
    RGBVALUE( Brown800, 0xff4e342e ) \
    RGBVALUE( Brown900, 0xff3e2723 ) \
    \
    RGBVALUE( Grey50, 0xfffafafa ) \
    RGBVALUE( Grey100, 0xfff5f5f5 ) \
    RGBVALUE( Grey200, 0xffeeeeee ) \
    RGBVALUE( Grey300, 0xffe0e0e0 ) \
    RGBVALUE( Grey400, 0xffbdbdbd ) \
    RGBVALUE( Grey500, 0xff9e9e9e ) \
    RGBVALUE( Grey600, 0xff757575 ) \
    RGBVALUE( Grey700, 0xff616161 ) \
    RGBVALUE( Grey800, 0xff424242 ) \
    RGBVALUE( Grey900, 0xff212121 ) \
    \
    RGBVALUE( BlueGrey50, 0xffeceff1 ) \
    RGBVALUE( BlueGrey100, 0xffcfd8dc ) \
    RGBVALUE( BlueGrey200, 0xffb0bec5 ) \
    RGBVALUE( BlueGrey300, 0xff90a4ae ) \
    RGBVALUE( BlueGrey400, 0xff78909c ) \
    RGBVALUE( BlueGrey500, 0xff607d8b ) \
    RGBVALUE( BlueGrey600, 0xff546e7a ) \
    RGBVALUE( BlueGrey700, 0xff455a64 ) \
    RGBVALUE( BlueGrey800, 0xff37474f ) \
    RGBVALUE( BlueGrey900, 0xff263238 ) \
    \
    RGBVALUE( Transparent, 0x00000000 ) \
    RGBVALUE( AlphaMask, 0xff000000 ) \
    RGBVALUE( ColorMask, 0x00ffffff )

namespace QskRgb
{
#define RGBVALUE( name, value ) static constexpr const QRgb name = value;
    QSK_RGB_VALUES
#undef RGBVALUE

    QSK_EXPORT QRgb rgb( Qt::GlobalColor );
    QSK_EXPORT QRgb interpolated( QRgb rgb1, QRgb rgb2, qreal ratio );
    QSK_EXPORT QColor interpolated( const QColor& c1, const QColor& c2, qreal ratio );

    inline QColor toTransparent( const QColor& c1, int alpha = 0 )
    {
        const auto alphaU = static_cast< uint >( alpha );
        return QColor::fromRgba( ( c1.rgb() & ColorMask ) | ( ( alphaU & 0xffu ) << 24 ) );
    }

    inline QColor toTransparent( Qt::GlobalColor color, int alpha = 0 )
    {
        return toTransparent( QColor( color ), alpha );
    }

    inline constexpr QRgb toTransparent( QRgb rgb, int alpha = 0 ) noexcept
    {
        return ( rgb & ColorMask ) | ( ( static_cast< uint >( alpha ) & 0xffu ) << 24 );
    }

    inline QColor toTransparentF( const QColor& color, qreal opacity )
    {
        return toTransparent( color, qRound( opacity * 255 ) );
    }

    inline QColor toTransparentF( Qt::GlobalColor color, qreal opacity )
    {
        return toTransparent( QColor( color ), qRound( opacity * 255 ) );
    }

    inline constexpr QRgb toTransparentF( QRgb rgb, qreal opacity ) noexcept
    {
        return toTransparent( rgb, qRound( opacity * 255 ) );
    }

    QSK_EXPORT QRgb lighter( QRgb, int factor = 150 ) noexcept;
    QSK_EXPORT QRgb darker( QRgb, int factor = 200 ) noexcept;
}

#ifndef QT_NO_DEBUG_STREAM

class QDebug;

namespace QskRgb
{
    QSK_EXPORT void debugColor( QDebug, const QColor& );
    QSK_EXPORT void debugColor( QDebug, QRgb );
}

#endif

#endif
