/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include <cstdio>
#include <cstdlib>
#include "ParserTree.h"
#include "DoxDumper.h"

static ParserTree s_parserTree;

extern "C"
{
    // pingpong between C/C++

    extern int metadoxCompile( const char* );

    void beginScope( int token )
    {
        s_parserTree.beginScope( token );
    }

    void endScope()
    {
        s_parserTree.endScope();
    }

    void addText( const char* text )
    {
        if ( text )
            s_parserTree.addText( text );
    }
}

int main( int argc, const char* argv[] )
{
    const char* inFileName = nullptr;

    if ( argc == 2 )
        inFileName = argv[1];

    int ok = metadoxCompile( inFileName );
    if ( ok != 0 )
        return ok;

    DoxDumper dumper;
    for ( auto interface : s_parserTree.interfaces() )
    {
        ok = dumper.dump( interface, stdout );
        if ( ok != 0 )
            break;
    }

    return ok;
}

