/*
 *  SPDX-FileCopyrightText: 2018 Marco Martin <mart@kde.org>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef SCENEPOSITIONATTACHED_H
#define SCENEPOSITIONATTACHED_H

#include <QtQml>
#include <QObject>

class QQuickItem;

/**
 * This attached property contains the information about the scene position of the item:
 * Its global x and y coordinates will update automatically and can be binded
 * @code
 * import org.kde.kirigami 2.5 as Kirigami
 * Text {
 *    text: ScenePosition.x
 * }
 * @endcode
 * @since 2.3
 */
class ScenePositionAttached : public QObject
{
    Q_OBJECT
    /**
     * The global scene X position
     */
    Q_PROPERTY(int x READ x NOTIFY xChanged)

    /**
     * The global scene Y position
     */
    Q_PROPERTY(int y READ y NOTIFY yChanged)

public:

    explicit ScenePositionAttached(QObject *parent = nullptr);
    ~ScenePositionAttached() override;

    int x() const;
    int y() const;

    //QML attached property
    static ScenePositionAttached *qmlAttachedProperties(QObject *object);

Q_SIGNALS:
    void xChanged();
    void yChanged();

private:
    void connectAncestors(QQuickItem *item);

    QQuickItem *m_item = nullptr;
    QList<QQuickItem *> m_ancestors;
};

QML_DECLARE_TYPEINFO(ScenePositionAttached, QML_HAS_ATTACHED_PROPERTIES)

#endif // SCENEPOSITIONATTACHED_H
