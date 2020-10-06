/*
 *  SPDX-FileCopyrightText: 2017 Marco Martin <mart@kde.org>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef FORMLAYOUTATTACHED_H
#define FORMLAYOUTATTACHED_H

#include <QtQml>
#include <QObject>

class QQuickItem;

/**
 * This attached property contains the information for decorating a FormLayout:
 * It contains the text labels of fields and information about sections.
 * @code
 * import org.kde.kirigami 2.3 as Kirigami
 * Kirigami.FormLayout {
 *    TextField {
 *       Kirigami.FormData.label: "Label:"
 *    }
 *    TextField {
 *       Kirigami.FormData.label: "Label:"
 *    }
 * }
 * @endcode
 * @since 2.3
 */
class FormLayoutAttached : public QObject
{
    Q_OBJECT
    /**
     * The label for a form layout field
     */
    Q_PROPERTY(QString label READ label WRITE setLabel NOTIFY labelChanged)
    /**
     * If true the FormLayout item is a section separator, a section separator
     * may have different looks:
     * * To make it just a space between two fields, just put an empty item with isSection:
     * @code
     * TextField {
     *     Kirigami.FormData.label: "Label:"
     * }
     * Item {
     *     Kirigami.FormData.isSection: true
     * }
     * TextField {
     *     Kirigami.FormData.label: "Label:"
     * }
     * @endcode
     *
     * * To make it a space with a section title:
     * @code
     * TextField {
     *     Kirigami.FormData.label: "Label:"
     * }
     * Item {
     *     Kirigami.FormData.label: "Section Title"
     *     Kirigami.FormData.isSection: true
     * }
     * TextField {
     *     Kirigami.FormData.label: "Label:"
     * }
     * @endcode
     *
     * * To make it a space with a section title and a separator line:
     * @code
     * TextField {
     *     Kirigami.FormData.label: "Label:"
     * }
     * Kirigami.Separator {
     *     Kirigami.FormData.label: "Section Title"
     *     Kirigami.FormData.isSection: true
     * }
     * TextField {
     *     Kirigami.FormData.label: "Label:"
     * }
     * @endcode
     */
    Q_PROPERTY(bool isSection READ isSection WRITE setIsSection NOTIFY isSectionChanged)

    /**
     * If true a checkbox is prepended to the FormLayout item.
     */
    Q_PROPERTY(bool checkable READ checkable WRITE setCheckable NOTIFY checkableChanged)

    /**
     * This property is true when the checkbox of the FormLayout item is checked, @see checkable.
     */
    Q_PROPERTY(bool checked READ checked WRITE setChecked NOTIFY checkedChanged)

    /**
     * This property holds whether the label and the checkbox of the FormLayout item receive mouse and keyboard events.
     */
    Q_PROPERTY(bool enabled READ enabled WRITE setEnabled NOTIFY enabledChanged)

    /**
     * The Item the label will be considered a "Buddy" for,
     * which will be the parent item the attached property is in.
     * A buddy item is useful for instance when the label has a keyboard accelerator,
     * which on triggered will be given active keyboard focus to.
     */
    Q_PROPERTY(QQuickItem *buddyFor READ buddyFor WRITE setBuddyFor NOTIFY buddyForChanged)

public:

    explicit FormLayoutAttached(QObject *parent = nullptr);
    ~FormLayoutAttached() override;

    void setLabel(const QString &text);
    QString label() const;

    QString decoratedLabel() const;

    void setIsSection(bool section);
    bool isSection() const;

    void setCheckable(bool checkable);
    bool checkable() const;

    void setChecked(bool checked);
    bool checked() const;

    void setEnabled(bool enabled);
    bool enabled() const;

    QQuickItem *buddyFor() const;
    void setBuddyFor(QQuickItem *buddyfor);

    //QML attached property
    static FormLayoutAttached *qmlAttachedProperties(QObject *object);

Q_SIGNALS:
    void labelChanged();
    void isSectionChanged();
    void checkableChanged();
    void checkedChanged();
    void enabledChanged();
    void buddyForChanged();

private:
    QString m_label;
    QString m_actualDecoratedLabel;
    QString m_decoratedLabel;
    QPointer <QQuickItem> m_buddyFor;
    bool m_isSection = false;
    bool m_checkable = false;
    bool m_checked = false;
    bool m_enabled = true;
};

QML_DECLARE_TYPEINFO(FormLayoutAttached, QML_HAS_ATTACHED_PROPERTIES)

#endif // FORMLAYOUTATTACHED_H
