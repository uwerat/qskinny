%{
#include <stdio.h>

extern void beginScope( int );
extern void endScope();
extern void addText( const char* );

%}

%error-verbose

// Symbols.
%union
{
    char    *sval;
};

%token <sval> TEXT
%token INTERFACE
%token CLASS
%token BASE
%token QMLCLASS
%token QMLBASE
%token HEADER
%token NAME
%token TYPE
%token READ
%token WRITE
%token NOTIFY
%token ENUM
%token PROPERTY
%token FUNCTION
%token INVOKABLE
%token SIGNAL
%token SLOT
%token SIGNATURE
%token DESCRIPTION
%token END

%start Interfaces
%%

Interfaces:
    | Interfaces Interface;

Interface: 
    INTERFACE { beginScope( INTERFACE ); }
    InterfaceParts 
    End

InterfaceParts:
    | InterfaceParts InterfacePart

InterfacePart:
    Class | Base | QmlClass | QmlBase | Header | Description
        | Enum | Property | Function | Invokable | Slots | Signals

Enum:
    ENUM { beginScope( ENUM ); }
    EnumParts 
    End

EnumParts:
    | EnumParts EnumPart

EnumPart:
    Class | QmlClass | Description

Property:
    PROPERTY { beginScope( PROPERTY ); }
    PropertyParts 
    End

PropertyParts:
    | PropertyParts PropertyPart

PropertyPart:
    Name | Type | Read | Write | Notify | Description

Function:
    FUNCTION { beginScope( FUNCTION ); }
    FunctionParts 
    End 

Invokable:
    INVOKABLE { beginScope( INVOKABLE ); }
    FunctionParts 
    End 

Slots:
    SLOT { beginScope( SLOT ); }
    FunctionParts 
    End 

Signals:
    SIGNAL { beginScope( SIGNAL ); }
    FunctionParts 
    End 

FunctionParts:
    Signature Description

Signature:
	SIGNATURE { beginScope( SIGNATURE ); }
    Textlines 
    End

Description:
	DESCRIPTION { beginScope( DESCRIPTION ); }
    Textlines 
    End

Class:
    CLASS { beginScope( CLASS ); }
    Textline 
    End

Base:
    BASE { beginScope( BASE ); }
    Textline 
    End

QmlClass:
    QMLCLASS { beginScope( QMLCLASS ); }
    Textline 
    End

QmlBase:
    QMLBASE { beginScope( QMLBASE ); }
    Textline 
    End

Header:
    HEADER { beginScope( HEADER ); }
    Textline 
    End

Name:
    NAME { beginScope( NAME ); }
    Textline 
    End

Type:
    TYPE { beginScope( TYPE ); }
    Textline 
    End

Read:
    READ { beginScope( READ ); }
    Textline 
    End

Write:
    WRITE { beginScope( WRITE ); }
    Textline 
    End

Notify:
    NOTIFY { beginScope( NOTIFY ); }
    Textline 
    End

Textlines:
    | Textlines Textline

Textline:
    TEXT { addText( $1 );  }

End:
    END { endScope(); }

%%

#include "metadox.lex.h"
#include "metadox.tab.h"
#include <stdio.h>
#include <stdlib.h>


int yyerror( const char *s ) 
{
    printf("Error at line %d: %s\n", yylineno, s);
    return 0;
}

int metadoxCompile( const char* fileName )
{
    int ok;
    FILE *fp = 0;

    /*yydebug = 1; */
    if ( fileName )
    {
        fp = fopen( fileName, "r" );
        yyin = fp;
        ok = yyparse();
        fclose(fp);
    }
    else
    {
        ok = yyparse();
    }

    return ok;
}


