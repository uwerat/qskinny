/*
 *  SPDX-FileCopyrightText: 2017 Marco Martin <mart@kde.org>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "mnemonicattached.h"
#include <QQuickItem>
#include <QQuickWindow>
#include <QQuickRenderControl>
#include <QDebug>

QHash<QKeySequence, MnemonicAttached *> MnemonicAttached::s_sequenceToObject = QHash<QKeySequence, MnemonicAttached *>();

// If pos points to alphanumeric X in "...(X)...", which is preceded or
// followed only by non-alphanumerics, then "(X)" gets removed.
static QString removeReducedCJKAccMark(const QString &label, int pos)
{
    if (pos > 0 && pos + 1 < label.length()
            && label[pos - 1] == QLatin1Char('(') && label[pos + 1] == QLatin1Char(')')
            && label[pos].isLetterOrNumber()) {
        // Check if at start or end, ignoring non-alphanumerics.
        int len = label.length();
        int p1 = pos - 2;
        while (p1 >= 0 && !label[p1].isLetterOrNumber()) {
            --p1;
        }
        ++p1;
        int p2 = pos + 2;
        while (p2 < len && !label[p2].isLetterOrNumber()) {
            ++p2;
        }
        --p2;

        if (p1 == 0) {
            return label.leftRef(pos - 1) + label.midRef(p2 + 1);
        } else if (p2 + 1 == len) {
            return label.leftRef(p1) + label.midRef(pos + 2);
        }
    }
    return label;
}

QString removeAcceleratorMarker(const QString &label_)
{
    QString label = label_;

    int p = 0;
    bool accmarkRemoved = false;
    while (true) {
        p = label.indexOf(QLatin1Char('&'), p);
        if (p < 0 || p + 1 == label.length()) {
            break;
        }

        if (label[p + 1].isLetterOrNumber()) {
            // Valid accelerator.
            label = QString(label.leftRef(p) + label.midRef(p + 1));

            // May have been an accelerator in CJK-style "(&X)"
            // at the start or end of text.
            label = removeReducedCJKAccMark(label, p);

            accmarkRemoved = true;
        } else if (label[p + 1] == QLatin1Char('&')) {
            // Escaped accelerator marker.
            label = QString(label.leftRef(p) + label.midRef(p + 1));
        }

        ++p;
    }

    // If no marker was removed, and there are CJK characters in the label,
    // also try to remove reduced CJK marker -- something may have removed
    // ampersand beforehand.
    if (!accmarkRemoved) {
        bool hasCJK = false;
        for (const QChar c : qAsConst(label)) {
            if (c.unicode() >= 0x2e00) { // rough, but should be sufficient
                hasCJK = true;
                break;
            }
        }
        if (hasCJK) {
            p = 0;
            while (true) {
                p = label.indexOf(QLatin1Char('('), p);
                if (p < 0) {
                    break;
                }
                label = removeReducedCJKAccMark(label, p + 1);
                ++p;
            }
        }
    }

    return label;
}


MnemonicAttached::MnemonicAttached(QObject *parent)
    : QObject(parent)
{
    QQuickItem *parentItem = qobject_cast<QQuickItem *>(parent);
    if (parentItem) {
        if (parentItem->window()) {
            m_window = parentItem->window();
            m_window->installEventFilter(this);
        }
        connect(parentItem, &QQuickItem::windowChanged, this,
                [this](QQuickWindow *window) {
            if (m_window) {
                QWindow *renderWindow = QQuickRenderControl::renderWindowFor(m_window);
                if (renderWindow) {
                    renderWindow->removeEventFilter(this);
                } else {
                    m_window->removeEventFilter(this);
                }
            }
            m_window = window;
            if (m_window) {
                QWindow *renderWindow = QQuickRenderControl::renderWindowFor(m_window);
                //renderWindow means the widget is rendering somewhere else, like a QQuickWidget
                if (renderWindow && renderWindow != m_window) {
                    renderWindow->installEventFilter(this);
                } else {
                    m_window->installEventFilter(this);
                }
            }
        });
    }
}

MnemonicAttached::~MnemonicAttached()
{
    s_sequenceToObject.remove(m_sequence);
}

bool MnemonicAttached::eventFilter(QObject *watched, QEvent *e)
{
    Q_UNUSED(watched)

    if (m_richTextLabel.isEmpty()) {
        return false;
    }

    if (e->type() == QEvent::KeyPress) {
        QKeyEvent *ke = static_cast<QKeyEvent *>(e);
        if (ke->key() == Qt::Key_Alt) {
            m_actualRichTextLabel = m_richTextLabel;
            emit richTextLabelChanged();
            m_active = true;
            emit activeChanged();
        }

    } else if (e->type() == QEvent::KeyRelease) {
        QKeyEvent *ke = static_cast<QKeyEvent *>(e);
        if (ke->key() == Qt::Key_Alt) {
            m_actualRichTextLabel = m_label;
            m_actualRichTextLabel = removeAcceleratorMarker(m_actualRichTextLabel);
            emit richTextLabelChanged();
            m_active = false;
            emit activeChanged();
        }
    }
    return false;
}

//Algorythm adapted from KAccelString
void MnemonicAttached::calculateWeights()
{
    m_weights.clear();

    int pos = 0;
    bool start_character = true;
    bool wanted_character = false;

    while (pos < m_label.length()) {
        QChar c = m_label[pos];

        // skip non typeable characters
        if (!c.isLetterOrNumber() && c != QLatin1Char('&')) {
            start_character = true;
            ++pos;
            continue;
        }

        int weight = 1;

        // add special weight to first character
        if (pos == 0) {
            weight += FIRST_CHARACTER_EXTRA_WEIGHT;
        // add weight to word beginnings
        } else if (start_character) {
            weight += WORD_BEGINNING_EXTRA_WEIGHT;
            start_character = false;
        }

        // add weight to characters that have an & beforehand
        if (wanted_character) {
            weight += WANTED_ACCEL_EXTRA_WEIGHT;
            wanted_character = false;
        }

        // add decreasing weight to left characters
        if (pos < 50) {
            weight += (50 - pos);
        }

        // try to preserve the wanted accelerators
        if (c == QLatin1Char('&')
            && (pos != m_label.length() - 1
                && m_label[pos + 1] != QLatin1Char('&')
                && m_label[pos + 1].isLetterOrNumber())) {
            wanted_character = true;
            ++pos;
            continue;
        }

        while (m_weights.contains(weight)) {
            ++weight;
        }

        if (c != QLatin1Char('&')) {
            m_weights[weight] = c;
        }

        ++pos;
    }

    //update our maximum weight
    if (m_weights.isEmpty()) {
        m_weight = m_baseWeight;
    } else {
        m_weight = m_baseWeight + (m_weights.cend()-1).key();
    }
}

void MnemonicAttached::updateSequence()
{
    if (!m_sequence.isEmpty()) {
        s_sequenceToObject.remove(m_sequence);
        m_sequence = {};
    }

    calculateWeights();

    // Preserve strings like "One & Two" where & is not an accelerator escape
    const QString text = label().replace(QStringLiteral("& "), QStringLiteral("&& "));

    if (!m_enabled) {
        m_actualRichTextLabel = text;
        m_actualRichTextLabel = removeAcceleratorMarker(m_actualRichTextLabel);
        //was the label already completely plain text? try to limit signal emission
        if (m_mnemonicLabel != m_actualRichTextLabel) {
            m_mnemonicLabel = m_actualRichTextLabel;
            emit mnemonicLabelChanged();
            emit richTextLabelChanged();
        }
        return;
    }

    if (!m_weights.isEmpty()) {
        QMap<int, QChar>::const_iterator i = m_weights.constEnd();
        do {
            --i;
            QChar c = i.value();

            QKeySequence ks(QStringLiteral("Alt+") % c);
            MnemonicAttached *otherMa = s_sequenceToObject.value(ks);
            Q_ASSERT(otherMa != this);
            if (!otherMa || otherMa->m_weight < m_weight) {
                //the old shortcut is less valuable than the current: remove it
                if (otherMa) {
                    s_sequenceToObject.remove(otherMa->sequence());
                    otherMa->m_sequence = {};
                }

                s_sequenceToObject[ks] = this;
                m_sequence = ks;
                m_richTextLabel = text;
                m_richTextLabel.replace(QRegularExpression(QLatin1String("\\&([^\\&])")), QStringLiteral("\\1"));
                m_actualRichTextLabel = m_richTextLabel;
                m_mnemonicLabel = text;
                const int mnemonicPos = m_mnemonicLabel.indexOf(c);

                if (mnemonicPos > -1 && (mnemonicPos == 0 || m_mnemonicLabel[mnemonicPos - 1] != QLatin1Char('&'))) {
                    m_mnemonicLabel.replace(mnemonicPos, 1, QStringLiteral("&") % c);
                }

                const int richTextPos = m_richTextLabel.indexOf(c);
                if (richTextPos > -1) {
                    m_richTextLabel.replace(richTextPos, 1, QLatin1String("<u>") % c % QLatin1String("</u>"));
                }

                //remap the sequence of the previous shortcut
                if (otherMa) {
                    otherMa->updateSequence();
                }

                break;
            }
        } while (i != m_weights.constBegin());
    }

    if (!m_sequence.isEmpty()) {
        emit sequenceChanged();
    } else {
        m_actualRichTextLabel = text;
        m_actualRichTextLabel = removeAcceleratorMarker(m_actualRichTextLabel);
        m_mnemonicLabel = m_actualRichTextLabel;
        
    }

    emit richTextLabelChanged();
    emit mnemonicLabelChanged();
}

void MnemonicAttached::setLabel(const QString &text)
{
    if (m_label == text) {
        return;
    }

    m_label = text;
    updateSequence();
    emit labelChanged();
}

QString MnemonicAttached::richTextLabel() const
{
    return !m_actualRichTextLabel.isEmpty() ? m_actualRichTextLabel : m_label;
}

QString MnemonicAttached::mnemonicLabel() const
{
    return m_mnemonicLabel;
}

QString MnemonicAttached::label() const
{
    return m_label;
}

void MnemonicAttached::setEnabled(bool enabled)
{
    if (m_enabled == enabled) {
        return;
    }

    m_enabled = enabled;
    updateSequence();
    emit enabledChanged();
}

bool MnemonicAttached::enabled() const
{
    return m_enabled;
}

void MnemonicAttached::setControlType(MnemonicAttached::ControlType controlType)
{
    if (m_controlType == controlType) {
        return;
    }

    m_controlType = controlType;

    switch (controlType) {
    case ActionElement:
        m_baseWeight = ACTION_ELEMENT_WEIGHT;
        break;
    case DialogButton:
        m_baseWeight = DIALOG_BUTTON_EXTRA_WEIGHT;
        break;
    case MenuItem:
        m_baseWeight = MENU_ITEM_WEIGHT;
        break;
    case FormLabel:
        m_baseWeight = FORM_LABEL_WEIGHT;
        break;
    default:
        m_baseWeight = SECONDARY_CONTROL_WEIGHT;
        break;
    }
    //update our maximum weight
    if (m_weights.isEmpty()) {
        m_weight = m_baseWeight;
    } else {
        m_weight = m_baseWeight + (m_weights.constEnd() - 1).key();
    }
    emit controlTypeChanged();
}

MnemonicAttached::ControlType MnemonicAttached::controlType() const
{
    return m_controlType;
}

QKeySequence MnemonicAttached::sequence()
{
    return m_sequence;
}

bool MnemonicAttached::active() const
{
    return m_active;
}

MnemonicAttached *MnemonicAttached::qmlAttachedProperties(QObject *object)
{
    return new MnemonicAttached(object);
}

#include "moc_mnemonicattached.cpp"
