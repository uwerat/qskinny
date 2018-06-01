/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_INPUT_MANAGER_H
#define QSK_INPUT_MANAGER_H

#include "QskGlobal.h"
#include <QObject>
#include <memory>

class QskInputEngine;
class QskControl;
class QQuickItem;
class QLocale;

class QSK_EXPORT QskInputManager : public QObject
{
    Q_OBJECT

    using Inherited = QObject;

public:
    QskInputManager( QObject* parent = nullptr );
    virtual ~QskInputManager() override;

    virtual void attachInputItem( QQuickItem* );
    virtual void processInputMethodQueries( Qt::InputMethodQueries );

    QskControl* panel( bool doCreate );
    virtual Qt::Alignment panelAlignment() const;

    virtual QQuickItem* inputProxy() const;
    virtual QQuickItem* inputItem() const;

protected:
    virtual QskControl* createPanel();
    virtual QskInputEngine* createEngine();

    void updatePredictor();
    void updateEngine( const QLocale& );

private:
    void applyInput( bool success );
    void applyText( const QString&, bool isFinal );
    void applyKey( int keyCode );

    class PrivateData;
    std::unique_ptr< PrivateData > m_data;
};

#endif
