#pragma once

#include <QObject>

class QskRgbLiterals final : public QObject
{
    Q_OBJECT

  private Q_SLOTS:
    void parsing_data();
    void parsing();
    void qrgbLiterals_data();
    void qrgbLiterals();
    void colorLiterals_data();
    void colorLiterals();
};
