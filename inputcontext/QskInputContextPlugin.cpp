/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include <qpa/qplatforminputcontextplugin_p.h>

#include "QskInputContext.h"
#include "QskPinyinTextPredictor.h"
#include "QskHunspellTextPredictor.h"

#include <QLocale>

class QskInputContextPlugin final : public QPlatformInputContextPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA( IID QPlatformInputContextFactoryInterface_iid FILE "metadata.json" )

public:
    virtual QPlatformInputContext* create(
        const QString& system, const QStringList& ) override
    {
        if ( system.compare( QStringLiteral( "skinny" ), Qt::CaseInsensitive ) == 0 )
        {
            auto context = new QskInputContext();

#if 0
            context->registerPredictor( QLocale(),
                new QskHunspellTextPredictor( this ) );
#endif

#if 0
            context->registerPredictor(
                QLocale::Chinese, new QskPinyinTextPredictor( this ) );
#endif
            
            return context;
        }

        return nullptr;
    }
};

#include "QskInputContextPlugin.moc"
