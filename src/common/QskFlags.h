/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_FLAGS_H_
#define QSK_FLAGS_H_

#include "QskGlobal.h"
#include <functional>

#define QSK_DECLARE_OPERATORS_FOR_FLAGS( Flags ) \
    constexpr inline Flags operator|( const Flags& a, const Flags& b ) noexcept \
    { \
        using underlying = typename std::underlying_type< Flags >::type; \
        return static_cast< Flags >( \
            static_cast< underlying >( a ) \
            | static_cast< underlying >( b ) ); \
    } \
    \
    inline Flags& operator|=( Flags& a, const Flags& b ) noexcept \
    { \
        return ( a = a | b ); \
    } \
    \
    constexpr inline Flags operator&( const Flags& a, const Flags& b ) noexcept \
    { \
        using underlying = typename std::underlying_type< Flags >::type; \
        return static_cast< Flags >( \
            static_cast< underlying >( a ) & static_cast< underlying >( b ) ); \
    } \
    \
    inline Flags& operator&=( Flags& a, const Flags& b ) noexcept \
    { \
        return ( a = static_cast< Flags >( a & b ) ); \
    } \
    \
    constexpr inline Flags operator~( const Flags& a ) noexcept \
    { \
        using underlying = typename std::underlying_type< Flags >::type; \
        return static_cast< Flags >( \
            ~static_cast< underlying >( a ) ); \
    } \
    \
    constexpr inline Flags operator^( const Flags& a, const Flags& b ) noexcept \
    { \
        using underlying = typename std::underlying_type< Flags >::type; \
        return static_cast< Flags >( \
            static_cast< underlying >( a ) ^ static_cast< underlying >( b ) ); \
    } \
    \
    inline Flags& operator^=( Flags& a, const Flags& b ) noexcept \
    { \
        return ( a = a ^ b ); \
    } \
    \
    constexpr inline Flags operator<<( const Flags& a, const int& b ) noexcept \
    { \
        using underlying = typename std::underlying_type< Flags >::type; \
        return static_cast< Flags >( static_cast< underlying >( a ) << b ); \
    } \
    \
    constexpr inline Flags operator>>( const Flags& a, const int& b ) noexcept \
    { \
        using underlying = typename std::underlying_type< Flags >::type; \
        return static_cast< Flags >( static_cast< underlying >( a ) >> b ); \
    } \
    \
    constexpr bool operator!( const Flags& a ) noexcept \
    { \
        using underlying = typename std::underlying_type< Flags >::type; \
        return !static_cast< underlying >( a ); \
    } \
    \
    namespace std \
    { \
        template< > struct hash< Flags > \
        { \
            constexpr size_t operator()( const Flags& element ) const noexcept \
            { \
                using underlying = typename std::underlying_type< Flags >::type; \
                return static_cast< underlying >( element ); \
            } \
        }; \
    }

#endif
