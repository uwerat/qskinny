/******************************************************************************
 * Copyright (C) 2022 Edelhirsch Software GmbH
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#pragma once

#include <QVector>
#include <QskLinearBox.h>
#include <memory>

class QQuickItem;

class StoragePage final : public QskLinearBox
{
  public:
    QSK_SUBCONTROLS( Panel )
    explicit StoragePage( QQuickItem* parent = nullptr );

  private:
    struct Storage
    {
        struct Media
        {
            qreal pictures = 0;
            qreal music = 0;
            qreal videos = 0;
            qreal documents = 0;
            qreal others = 0;

            inline constexpr bool operator==( const Media& rhs ) const noexcept
            {
                return pictures == rhs.pictures && music == rhs.music && videos == rhs.videos &&
                       documents == rhs.documents && others == rhs.others;
            }

            inline constexpr qreal free() const noexcept
            {
                return 1.0 - pictures - music - videos - documents - others;
            }
        };

        QString title;
        QString description;
        Media distribution{};
    };

    void addRow( Storage storage );
};
