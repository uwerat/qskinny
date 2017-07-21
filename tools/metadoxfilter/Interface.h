/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef _INTERFACE_H
#define _INTERFACE_H 1

#include <vector>
#include <string>

class Property
{
public:
    std::string name;
    std::string type;

    std::string read;
    std::string write;
    std::string notify;

    std::string description;
};

class Enum
{
public:
    std::string className;
    std::string qmlClassName;
    std::string description;
};

class Method
{
public:
    Method( int methodType = -1 ):
        type( methodType )
    {
    }

    int type;
    std::string signature;
    std::string description;
};

class Interface
{
public:
    std::string className;
    std::string baseName;
    std::string qmlClassName;
    std::string qmlBaseName;
    std::string header;

    std::string description;

    std::vector< Property > properties;
    std::vector< Enum > enums;
    std::vector< Method > methods;
};

#endif
