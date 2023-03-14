/******************************************************************************
 * Copyright (C) 2023 Edelhirsch Software GmbH
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#ifndef QSK_RGB_LITERALS_H
#define QSK_RGB_LITERALS_H

namespace QskRgb
{
    namespace literals
    {
        /// @brief converts HTML #RRGGBB[AA] color hex string to Qt's RGB 0xAARRGGBB integer
        /// @param str The string literal
        /// @param len The length of the literal
        /// @note non-hexadecimal characters are replaced with 0x0 value
        constexpr unsigned int operator""_rgba( const char* const str, const size_t len )
        {
            if ( len != 7 && len != 9 )
            {
                return 0;
            }

            unsigned int rgb = 0x00;

            // skip # character
            for ( size_t i = 1; i < len; ++i )
            {
                switch ( str[ i ] )
                {
                    case '0':
                        rgb = ( rgb << 4 ) | 0x0;
                        break;

                    case '1':
                        rgb = ( rgb << 4 ) | 0x1;
                        break;

                    case '2':
                        rgb = ( rgb << 4 ) | 0x2;
                        break;

                    case '3':
                        rgb = ( rgb << 4 ) | 0x3;
                        break;

                    case '4':
                        rgb = ( rgb << 4 ) | 0x4;
                        break;

                    case '5':
                        rgb = ( rgb << 4 ) | 0x5;
                        break;

                    case '6':
                        rgb = ( rgb << 4 ) | 0x6;
                        break;

                    case '7':
                        rgb = ( rgb << 4 ) | 0x7;
                        break;

                    case '8':
                        rgb = ( rgb << 4 ) | 0x8;
                        break;

                    case '9':
                        rgb = ( rgb << 4 ) | 0x9;
                        break;

                    case 'A':
                    case 'a':
                        rgb = ( rgb << 4 ) | 0xA;
                        break;

                    case 'B':
                    case 'b':
                        rgb = ( rgb << 4 ) | 0xB;
                        break;

                    case 'C':
                    case 'c':
                        rgb = ( rgb << 4 ) | 0xC;
                        break;

                    case 'D':
                    case 'd':
                        rgb = ( rgb << 4 ) | 0xD;
                        break;

                    case 'E':
                    case 'e':
                        rgb = ( rgb << 4 ) | 0xE;
                        break;

                    case 'F':
                    case 'f':
                        rgb = ( rgb << 4 ) | 0xF;
                        break;

                    default:
                        rgb = ( rgb << 4 ) | 0x0;
                        break;
                }
            }

            // add default 0xFF for alpha channel
            if ( len > 1 && len <= 7 )
            {
                rgb |= 0xFF000000;
            }

            // convert rrggbbaa to aarrggbb
            if ( len > 7 )
            {
                rgb = ( 0x000000FF & rgb ) << 24 | ( rgb >> 8 );
            }

            return rgb;
        }

        // example for rgb
        static_assert( "#123456"_rgba == 0xFF123456, "not constexpr" );
        // example for rgba
        static_assert( "#112233aa"_rgba == 0xAA112233, "not constexpr" );

        // bad examples
        static_assert( ""_rgba == 0x0, "not constexpr" );
        static_assert( "#"_rgba == 0x0, "not constexpr" );
        static_assert( "#1"_rgba == 0x0, "not constexpr" );
        static_assert( "#12"_rgba == 0x0, "not constexpr" );
        static_assert( "#123"_rgba == 0x0, "not constexpr" );
        static_assert( "#1234"_rgba == 0x0, "not constexpr" );
        static_assert( "#12345"_rgba == 0x0, "not constexpr" );
        static_assert( "#112233a"_rgba == 0x0, "not constexpr" );
        static_assert( "#fffffffff"_rgba == 0x0, "not constexpr" );
        static_assert( "#WTF01337"_rgba == 0x3700F013, "not constexpr" );
        static_assert( "#JUSTWHY?"_rgba == 0x0, "not constexpr" );
    }
}

#endif
