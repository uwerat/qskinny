/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "ParserTree.h"
#include <cstdio>
#include <algorithm>
#include <regex>
#include <cassert>

extern "C"
{
    #include "metadox.tab.h"
}

static std::string trimmedString( const char* text )
{
    using namespace std;

    string s( text );

    s.erase( s.begin(), find_if( s.begin(), s.end(), bind1st( not_equal_to< char >(), ' ' ) ) );
    s.erase( find_if( s.rbegin(), s.rend(), bind1st( not_equal_to< char >(), ' ' ) ).base(), s.end() );

    return s;
}

void ParserTree::beginScope( int token )
{
    m_scopeStack.push_back( token );

    switch( token )
    {
        case INTERFACE:
        {
            m_interfaces.emplace_back();
            break;
        }
        case PROPERTY:
        {
            m_interfaces.back().properties.emplace_back();
            break;
        }
        case ENUM:
        {
            m_interfaces.back().enums.emplace_back();
            break;
        }
        case FUNCTION:
        case INVOKABLE:
        case SIGNAL:
        case SLOT:
        {
            m_interfaces.back().methods.emplace_back( token );
            break;
        }
        default:
            ;
    }
}

void ParserTree::endScope()
{
    m_scopeStack.pop_back();
}

int ParserTree::scope() const
{
    return m_scopeStack.empty() ? -1 : m_scopeStack.back();
}

void ParserTree::addText( const char* text )
{
    Interface& interface = m_interfaces.back();

    int outerScope = -1;
    if ( m_scopeStack.size() > 1 )
        outerScope = m_scopeStack[m_scopeStack.size() - 2];

    if ( scope() == DESCRIPTION )
    {
        std::string* description = nullptr;

        switch( outerScope )
        {
            case INTERFACE:
            {
                description = &interface.description;
                break;
            }
            case PROPERTY:
            {
                assert( !interface.properties.empty() );
                description = &interface.properties.back().description;
                break;
            }
            case ENUM:
            {
                assert( !interface.enums.empty() );
                description = &interface.enums.back().description;
                break;
            }
            default:
            {
                assert( !interface.methods.empty() );
                description = &interface.methods.back().description;
            }
        }

        if ( description )
        {
            std::string& d = *description;
            if ( !d.empty() )
                d += '\n';

            if ( text )
                d += text;
        }

        return;
    }

    const std::string trimmed = trimmedString( text );

    switch( scope() )
    {
        // interface
        case CLASS:
        {
            if ( outerScope == ENUM )
            {
                assert( !interface.enums.empty() );
                interface.enums.back().className = trimmed;
            }
            else
            {
                interface.className = trimmed;
            }

            break;
        }
        case BASE:
        {
            interface.baseName = trimmed;
            break;
        }
        case QMLCLASS:
        {
            if ( outerScope == ENUM )
            {
                assert( !interface.enums.empty() );
                interface.enums.back().qmlClassName = trimmed;
            }
            else
            {
                interface.qmlClassName = trimmed;
            }

            break;
        }
        case QMLBASE:
        {
            interface.qmlBaseName = trimmed;
            break;
        }
        case HEADER:
        {
            interface.header = trimmed;
            break;
        }

        // property
        case NAME:
        {
            assert( !interface.properties.empty() );
            interface.properties.back().name = trimmed;
            break;
        }
        case TYPE:
        {
            assert( !interface.properties.empty() );
            interface.properties.back().type = trimmed;
            break;
        }
        case READ:
        {
            assert( !interface.properties.empty() );
            interface.properties.back().read = trimmed;
            break;
        }
        case WRITE:
        {
            assert( !interface.properties.empty() );
            interface.properties.back().write = trimmed;
            break;
        }
        case NOTIFY:
        {
            assert( !interface.properties.empty() );
            interface.properties.back().notify = trimmed;
            break;
        }

        // method
        case SIGNATURE:
        {
            assert( !interface.methods.empty() );

            std::string& signature = interface.methods.back().signature;

            if ( !signature.empty() )
                signature += "\n\t";

            signature += trimmed;
            break;
        }
    }
}
