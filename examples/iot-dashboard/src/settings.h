/*
 *  SPDX-FileCopyrightText: 2016 Marco Martin <mart@kde.org>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef SETTINGS_H
#define SETTINGS_H

#include <QObject>
#include <QVariant>

/**
 * This class contains global kirigami settings about the current device setup
 * It is exposed to QML as the singleton "Settings"
 */
class Settings : public QObject
{
    Q_OBJECT

    /**
     * True if the system can dynamically enter in tablet mode
     * (or the device is actually a tablet).
     * such as transformable laptops that support keyboard detachment
     */
    Q_PROPERTY(bool tabletModeAvailable READ isTabletModeAvailable NOTIFY tabletModeAvailableChanged)

    /**
     * True if we are running on a small mobile device such as a mobile phone
     * This is used when we want to do specific adaptations to our UI for
     * small screen form factors, such as having bigger touch areas.
     */
    Q_PROPERTY(bool isMobile READ isMobile NOTIFY isMobileChanged)

    /**
     * True if the device we are running on is behaving like a tablet:
     * Note that this doesn't mean exactly a tablet form factor, but
     * that the preferred input mode for the device is the touch screen
     * and that pointer and keyboard are either secondary or not available.
     */
    Q_PROPERTY(bool tabletMode READ tabletMode NOTIFY tabletModeChanged)

    /**
     * True if the user in this moment is interacting with the app with the touch screen
     */
    Q_PROPERTY(bool hasTransientTouchInput READ hasTransientTouchInput NOTIFY hasTransientTouchInputChanged)

    /**
     * name of the QtQuickControls2 style we are using,
     * for instance org.kde.desktop, Plasma, Material, Universal etc
     */
    Q_PROPERTY(QString style READ style CONSTANT)

    //TODO: make this adapt without file watchers?
    /**
     * How many lines of text the mouse wheel should scroll
     */
    Q_PROPERTY(int mouseWheelScrollLines READ mouseWheelScrollLines CONSTANT)

    /**
     * @returns runtime information about the libraries in use
     *
     * @since 5.52
     * @since org.kde.kirigami 2.6
     */
    Q_PROPERTY(QStringList information READ information CONSTANT)

    /**
     * @returns application window icon, basically \QApplication::windowIcon()
     *
     * @since 5.62
     * @since org.kde.kirigami 2.10
     */
    Q_PROPERTY(QVariant applicationWindowIcon READ applicationWindowIcon CONSTANT)

public:
    Settings(QObject *parent = nullptr);
    ~Settings();

    void setTabletModeAvailable(bool mobile);
    bool isTabletModeAvailable() const;

    void setIsMobile(bool mobile);
    bool isMobile() const;

    void setTabletMode(bool tablet);
    bool tabletMode() const;

    void setTransientTouchInput(bool touch);
    bool hasTransientTouchInput() const;

    QString style() const;
    void setStyle(const QString &style);

    int mouseWheelScrollLines() const;

    QStringList information() const;

    QVariant applicationWindowIcon() const;

    static Settings *self();

protected:
    bool eventFilter(QObject *watched, QEvent *event) override;

Q_SIGNALS:
    void tabletModeAvailableChanged();
    void tabletModeChanged();
    void isMobileChanged();
    void hasTransientTouchInputChanged();

private:
    QString m_style;
    int m_scrollLines = 0;
    bool m_tabletModeAvailable : 1;
    bool m_mobile : 1;
    bool m_tabletMode : 1;
    bool m_hasTouchScreen : 1;
    bool m_hasTransientTouchInput : 1;
};

#endif
