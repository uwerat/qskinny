%{
	#include "metadox.tab.h"
%}

whitespace      [ \t]
comment         "#".*
eol             \n
colon           :
end             END

%x SCOPEBEGIN
%x SCOPEBLOCK
%x SCOPELINE

%option yylineno

%%

[ \t\n]+ { 
    /* ignore */ 
}

{comment} { 
    /* ignore */ 
}

"INTERFACE" {
    return(INTERFACE);
}

"PROPERTY" {
    return(PROPERTY);
}

"ENUM" {
    return(ENUM);
}

"FUNCTION" {
    return(FUNCTION);
}

"INVOKABLE" {
    return(INVOKABLE);
}

"SIGNAL" {
    return(SIGNAL);
}

"SLOT" {
    return(SLOT);
}

"CLASS" {
    BEGIN SCOPEBEGIN;
    return(CLASS);
}

"BASE" {
    BEGIN SCOPEBEGIN;
    return(BASE);
}

"QMLCLASS" {
    BEGIN SCOPEBEGIN;
    return(QMLCLASS);
}

"QMLBASE" {
    BEGIN SCOPEBEGIN;
    return(QMLBASE);
}

"HEADER" {
    BEGIN SCOPEBEGIN;
    return(HEADER);
} 

"NAME" {
    BEGIN SCOPEBEGIN;
    return(NAME);
}

"TYPE" {
    BEGIN SCOPEBEGIN;
    return(TYPE);
}

"READ" {
    BEGIN SCOPEBEGIN;
    return(READ);
}

"WRITE" {
    BEGIN SCOPEBEGIN;
    return(WRITE);
}

"NOTIFY" {
    BEGIN SCOPEBEGIN;
    return(NOTIFY);
}

"SIGNATURE" {
    BEGIN SCOPEBEGIN;
    return(SIGNATURE);
}

"DESCRIPTION" {
    BEGIN SCOPEBEGIN;
    return(DESCRIPTION);
}

{end} {
    return(END);
}

<SCOPEBEGIN>{

    {whitespace}*{colon} {
        BEGIN SCOPELINE;
    }

    {whitespace}*{eol} {
        BEGIN SCOPEBLOCK;
    }
}

<SCOPELINE>{
    
    {eol} {
        BEGIN INITIAL;
        return(END);
    }
    
    .* {
        yylval.sval = yytext;
        return(TEXT);
    }
}

<SCOPEBLOCK>{

	^{whitespace}*{end}{whitespace}* {
		BEGIN INITIAL;
		return(END);
	}

    ^{whitespace}*{eol} {
		yylval.sval = "";
		return(TEXT);
    }

    {eol} {
    }

	.* {
        yylval.sval = yytext;
		return(TEXT);
	}
}
