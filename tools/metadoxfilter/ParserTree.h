/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef _PARSER_TREE_H
#define _PARSER_TREE_H 1

#include "Interface.h"

class ParserTree
{
public:
    void beginScope( int token );
    void addText( const char* );
    void endScope();

    const std::vector< Interface >& interfaces() const
    {
        return m_interfaces;
    }

private:
    int scope() const;

    std::vector< Interface > m_interfaces;
    std::vector< int > m_scopeStack;
};

#endif
