/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef DOX_DUMPER_H
#define DOX_DUMPER_H

#include "Interface.h"
#include <cstdio>

class DoxDumper
{
  public:
    DoxDumper();
    int dump( const Interface&, FILE* );

    int dumpCpp( const Interface&, FILE* );
    int dumpQml( const Interface&, FILE* );

  private:
    void doxPrintf( const char* format, ... );

    std::string expandedText(
        const Interface&, const std::string& ) const;

    std::string expandedTextQml(
        const Interface&, const std::string& ) const;

    std::vector< std::string > accessors( const Property& ) const;
    std::string accessorsAsString( const Property& ) const;

    FILE* m_file;
};

#endif
