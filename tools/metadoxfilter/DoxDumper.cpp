/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "DoxDumper.h"
#include <cstdarg>

extern "C"
{
    #include "metadox.tab.h"
}

static inline const char* toCStr( const std::string& s )
{
    return s.empty() ? "" : s.c_str();
}

static inline std::string expandedString( const std::string& s,
    const std::string& search, const std::string& replace )
{
    using namespace std;

    string subject = s;

    size_t pos = 0;
    while ( ( pos = subject.find( search, pos ) ) != std::string::npos )
    {
        subject.replace( pos, search.length(), replace );
        pos += replace.length();
    }

    return subject;
}

DoxDumper::DoxDumper():
    m_file( nullptr )
{
}

std::string DoxDumper::expandedText(
    const Interface& interface, const std::string& s ) const
{
    using namespace std;

    string expanded;
    expanded = expandedString( s, "QUICKITEM", "QQuickItem *" );
    expanded = expandedString( expanded, "INHERITED", interface.baseName );
    expanded = expandedString( expanded, "CLASS", interface.className );

    return expanded;
}

std::string DoxDumper::expandedTextQml(
    const Interface& interface, const std::string& s ) const
{
    using namespace std;

    string expanded;
    expanded = expandedString( s, "QUICKITEM", "Item" );
    expanded = expandedString( expanded, "INHERITED", interface.qmlBaseName );
    expanded = expandedString( expanded, "CLASS", interface.qmlClassName );

    return expanded;
}

void DoxDumper::doxPrintf( const char* format, ... )
{
    fputs( "/*!\n", m_file );

    va_list args;
    va_start ( args, format );
    vprintf ( format, args );
    va_end ( args );

    fputs( "\n*/\n", m_file );
}

std::vector< std::string > DoxDumper::accessors(
    const Property& property ) const
{
    using namespace std;

    vector< string > accessors;
    if ( !property.read.empty() )
        accessors.push_back( property.read );

    if ( !property.write.empty() )
        accessors.push_back( property.write );

    if ( !property.notify.empty() )
        accessors.push_back( property.notify );

    return accessors;
}

std::string DoxDumper::accessorsAsString( const Property& property ) const
{
    std::vector< std::string > accessors = this->accessors( property );

    std::string s;
    for ( uint i = 0; i < accessors.size(); i++ )
    {
        if ( i > 0 )
            s += ", ";

        s += accessors[i];
    }

    return s;
}

int DoxDumper::dump( const Interface& interface, FILE* file )
{
    if ( !interface.className.empty() )
    {
        int ok = dumpCpp( interface, file );
        if ( ok != 0 )
            return ok;
    }

    if ( !interface.qmlClassName.empty() )
    {
        if ( !interface.className.empty() )
            printf( "\n\n/* ========= */\n\n" );
        int ok = dumpQml( interface, file );
        if ( ok != 0 )
            return ok;
    }

    return 0;
}

int DoxDumper::dumpCpp( const Interface& interface, FILE* file )
{
    m_file = file;

    if ( !interface.header.empty() )
        doxPrintf( "\\headerfile %s", toCStr( interface.header ) );

    if ( !interface.description.empty() )
    {
        const std::string expanded = expandedText( interface, interface.description );
        doxPrintf( "%s", toCStr( expanded ) );
    }

    // no base class, it is known from the real header
    fprintf( m_file, "class %s\n{\npublic:\n", toCStr( interface.className ) );

    for ( const auto property : interface.properties )
    {
        const std::string expanded = expandedText( interface, property.description );

        doxPrintf( "\\property %s %s\n\n%s\n\n\\accessors %s\n",
            toCStr( property.type ), toCStr( property.name ),
            toCStr( expanded ), toCStr( accessorsAsString( property ) ) );
    }

    for ( const auto enumdef : interface.enums )
    {
        const std::string expanded = expandedText( interface, enumdef.description );

        doxPrintf( "\\enum %s\n\n%s",
            toCStr( enumdef.className ), toCStr( expanded ) );
    }

    for ( const auto method : interface.methods )
    {
        const std::string expandedSignature =
            expandedText( interface, method.signature );

        const std::string expandedDescription =
            expandedText( interface, method.description );

        doxPrintf( "\\fn %s\n\n%s\n",
            toCStr( expandedSignature ), toCStr( expandedDescription ) );
    }

    fprintf( m_file, "};\n" );

    return 0;
}

int DoxDumper::dumpQml( const Interface& interface, FILE* file )
{
    m_file = file;

    if ( !interface.description.empty() )
    {
        const std::string expanded = expandedText( interface, interface.description );
        doxPrintf( "%s\n\n\t\\sa %s", toCStr( expanded ), toCStr( interface.className ) );
    }

    if ( interface.qmlBaseName.empty() )
    {
        fprintf( m_file, "class %s\n{\npublic:\n", toCStr( interface.qmlClassName ) );
    }
    else
    {
        fprintf( m_file, "class %s: public %s\n{\n",
            toCStr( interface.qmlClassName ), toCStr( interface.qmlBaseName ) );
    }

    for ( const auto property : interface.properties )
    {
        const std::string expanded = expandedText( interface, property.description );

        doxPrintf( "%s", toCStr( expanded ) );
        printf( "Q_PROPERTY( %s %s )\n\n",
            toCStr( property.type ), toCStr( property.name ) );
    }

    fprintf( m_file, "public:\n" );

    for ( const auto enumdef : interface.enums )
    {
        const std::string expanded = expandedText( interface, enumdef.description );

        doxPrintf( "\\enum %s\n\n%s",
            toCStr( enumdef.qmlClassName ), toCStr( expanded ) );
    }

    for ( const auto method : interface.methods )
    {
        if ( method.type == FUNCTION || method.type == SIGNAL )
            continue;

        const std::string expandedSignature =
            expandedTextQml( interface, method.signature );

        const std::string expandedDescription =
            expandedTextQml( interface, method.description );

        const std::string expanded = expandedText( interface, method.description );

        doxPrintf( "%s", toCStr( expandedDescription ) );
        printf( "%s;\n", toCStr( expandedSignature ) );
    }

    fprintf( m_file, "};\n" );

    return 0;
}
