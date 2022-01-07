#include "QskPlatform.h"

#include <qguiapplication.h>
#include <qscreen.h>

QSK_QT_PRIVATE_BEGIN
#include <private/qguiapplication_p.h>

#if QT_VERSION < QT_VERSION_CHECK( 5, 8, 0 )
    #ifndef foreach
        // qhighdpiscaling_p.h needs it
        #define foreach Q_FOREACH
    #endif
#endif

#include <private/qhighdpiscaling_p.h>
QSK_QT_PRIVATE_END

#include <qpa/qplatformintegration.h>
#include <qpa/qplatformscreen.h>

qreal qskDpiScaled( qreal value )
{
    static qreal factor = 0.0;

    if ( factor <= 0.0 )
    {
        if ( const auto screen = QGuiApplication::primaryScreen() )
            factor = screen->logicalDotsPerInchX();
        else
            factor = 100.0;

        factor /= 96.0;
    }

    return value * factor;
}

qreal qskGlobalScaleFactor()
{
    // The value of QT_SCALE_FACTOR
    const QScreen* noScreen = nullptr;
    return QHighDpiScaling::factor( noScreen );
}

const QPlatformIntegration* qskPlatformIntegration()
{
    return QGuiApplicationPrivate::platformIntegration();
}

bool qskMaybeDesktopPlatform()
{
    // this is what QC2 is doing for menus ?

#if QT_VERSION >= QT_VERSION_CHECK( 5, 15, 0 )
#if !QT_CONFIG(cursor)
    return false;
#endif
#endif

    if ( const auto platform = QGuiApplicationPrivate::platformIntegration() )
        return platform->hasCapability( QPlatformIntegration::MultipleWindows );

    return false;
}

QRect qskPlatformScreenGeometry( const QScreen* screen )
{
    if ( screen == nullptr )
        return QRect();

    return screen->handle()->geometry();
}


