/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_SHORTCUT_H
#define QSK_SHORTCUT_H

#include "QskGlobal.h"

#include <qnamespace.h>
#include <qobject.h>
#include <qqmlparserstatus.h>

#include <memory>

class QKeySequence;

/*
    For QML, with C++ there is also QskShortcutMap that does
    not need to create QObjects per shortcut
 */
class QSK_EXPORT QskShortcut : public QObject, public QQmlParserStatus
{
    Q_OBJECT
    Q_INTERFACES( QQmlParserStatus )

    Q_PROPERTY( QVariant sequence READ sequenceVariant
        WRITE setSequenceVariant NOTIFY sequenceChanged FINAL)

    Q_PROPERTY( Qt::ShortcutContext context READ context
        WRITE setContext NOTIFY contextChanged )

    Q_PROPERTY( bool enabled READ isEnabled
        WRITE setEnabled NOTIFY enabledChanged )

    Q_PROPERTY( bool autoRepeat READ autoRepeat
        WRITE setAutoRepeat NOTIFY autoRepeatChanged )

    Q_PROPERTY( int shortcutId READ shortcutId
        NOTIFY shortcutIdChanged )

    using Inherited = QObject;

  public:
    QskShortcut( QObject* parent = nullptr );

    QskShortcut( const QKeySequence&, QObject* = nullptr );
    QskShortcut( const QKeySequence&, Qt::ShortcutContext, QObject* = nullptr );

    ~QskShortcut() override;

    int shortcutId() const;

    void setSequence( const QKeySequence& );
    QKeySequence sequence() const;

    // for QML
    void setSequenceVariant( const QVariant& );
    QVariant sequenceVariant() const;

    Qt::ShortcutContext context() const;
    void setContext( Qt::ShortcutContext );

    void setEnabled( bool );
    bool isEnabled() const;

    void setAutoRepeat( bool );
    bool autoRepeat() const;

    virtual bool isFocusInScope() const;

  Q_SIGNALS:
    void sequenceChanged();
    void contextChanged();
    void enabledChanged();
    void autoRepeatChanged();

    void activated();
    void activatedAmbiguously();

    int shortcutIdChanged( int ) const;

  protected:
    bool event( QEvent* ) override;
    void classBegin() override;
    void componentComplete() override;

  private:
    class PrivateData;
    std::unique_ptr< PrivateData > m_data;
};

#endif
