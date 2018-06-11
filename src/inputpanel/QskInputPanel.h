/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_INPUT_ENGINE_H
#define QSK_INPUT_ENGINE_H

#include "QskGlobal.h"
#include <QObject>
#include <memory>

class QskTextPredictor;
class QskControl;
class QQuickItem;
class QLocale;

class QSK_EXPORT QskInputEngine : public QObject
{
    Q_OBJECT

    using Inherited = QObject;

public:
    QskInputEngine( QObject* parent = nullptr );
    virtual ~QskInputEngine() override;

    virtual void attachInputItem( QQuickItem* );
    virtual void updateInputPanel( Qt::InputMethodQueries );

    QskControl* panel( bool doCreate );
    virtual Qt::Alignment panelAlignment() const;

    virtual QQuickItem* inputProxy() const;
    virtual QQuickItem* inputItem() const;

public Q_SLOTS:
    void commitKey( int keyCode );
    void commitPredictiveText( int index );

Q_SIGNALS:
    void activeChanged();
    void localeChanged();

protected:
    virtual QskControl* createPanel() = 0;
    virtual void attachToPanel( QQuickItem* ) = 0;

    virtual void setPredictionEnabled( bool on );
    virtual void showPrediction( const QStringList& );

    void applyInput( bool success );
    void applyText( const QString&, bool isFinal );
    void applyKey( int keyCode );

private:
    void resetPredictor( const QLocale& );
    void updatePrediction();

    void updatePanel();

    void updateLocale( const QLocale& );

    class PrivateData;
    std::unique_ptr< PrivateData > m_data;
};

#endif
