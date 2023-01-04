#include "StoragePage.h"
#include "CircularProgressBar.h"
#include "Box.h"
#include "EnergyMeter.h"
#include "Diagram.h"
#include "StorageMeter.h"
#include "StorageBar.h"
#include <QskAnimator.h>
#include <QskSkin.h>
#include <QskGraphicLabel.h>
#include <QskStackBox.h>
#include <QskTextLabel.h>
#include <QskGradient.h>
#include <QskGradientStop.h>
#include <QskBox.h>
#include <QskBoxShapeMetrics.h>
#include <QskPushButton.h>
#include <QTimer>

QSK_SUBCONTROL( StoragePage, Panel )

struct Media {
  qreal pictures = 0;
  qreal music = 0;
  qreal videos = 0;
  qreal documents = 0;
  qreal others = 0;

  inline constexpr bool operator==(const Media& rhs) const noexcept
  {
    return pictures == rhs.pictures && music == rhs.music && videos == rhs.videos && documents == rhs.documents && others == rhs.others;
  }

  inline constexpr qreal free() const noexcept
  {
    return 1.0 - pictures - music - videos - documents - others;
  }
};

StoragePage::StoragePage(QQuickItem * const parent) : QskLinearBox(Qt::Vertical, parent)
{
  const auto createProgressBar = [](const qreal value, QQuickItem* const parent)
  {    
    auto* const bar = new StorageMeter(parent);
    bar->setProgress(value);
    bar->setMinimumSize(16,16);
    bar->setMaximumSize(64,64);    
    bar->setSizePolicy( QskSizePolicy::Preferred, QskSizePolicy::Constrained );
    return bar;
  };

  const auto createLinearBox = [](Qt::Orientation orientation = Qt::Vertical, QskSizePolicy::Policy hp = QskSizePolicy::Fixed , QskSizePolicy::Policy vp = QskSizePolicy::Fixed, QQuickItem* const parent = nullptr){
    auto* const layout = new QskLinearBox(orientation, parent);
    layout->setSizePolicy( hp, vp );
    return layout;
  };

  const auto createRow = [&](const QString& titleText, const QString& subTitleText, const Media& media, QQuickItem* const parent){
    auto* const box = new Box("Network Storage", parent);
    auto* const layout = new QskLinearBox(Qt::Horizontal, box);
    auto* const left = new QskLinearBox(Qt::Vertical, layout);
    auto* const center = new QskLinearBox(Qt::Vertical, layout);
    auto* const right = new QskLinearBox(Qt::Vertical, layout);
    auto* const stack = new QskStackBox(left);
    stack->setAutoLayoutChildren(true);
    const auto percent = 1.0 - media.free();
    auto* const bar = createProgressBar(percent, createLinearBox(Qt::Vertical, QskSizePolicy::Preferred, QskSizePolicy::Preferred, stack));

    stack->addItem(bar);
    stack->setCurrentItem(bar);
    auto* title = new QskTextLabel(titleText, center);
    title->setFontRole(QskSkin::LargeFont);
    auto* subtitle = new QskTextLabel(subTitleText, center);
    subtitle->setFontRole(QskSkin::MediumFont);
    center->addSpacer(1,99);

    auto* const storageBar = new StorageBar(right);
    storageBar->setPictures(media.pictures);
    storageBar->setMusic(media.music);
    storageBar->setVideos(media.videos);
    storageBar->setDocuments(media.documents);
    storageBar->setOthers(media.others);

    auto* const mediaLegend = new QskLinearBox(Qt::Horizontal, right);
    mediaLegend->setSpacing(12);
    mediaLegend->addSpacer(1,999);
    auto* const sync = new QskPushButton("Update", mediaLegend);
    sync->setFontRoleHint(QskPushButton::Text, QskSkin::SmallFont);
    connect(sync, &QskPushButton::clicked, storageBar, [bar, storageBar, media](){
      struct PropertyAnimator final : public QskAnimator
      {
        void advance( qreal value ) override { callback(value); }
        void done() override { delete this; }
        std::function<void(qreal)> callback = [](qreal){};
      };

      auto* const animator = new PropertyAnimator();
      animator->setEasingCurve(QEasingCurve::InQuad);
      animator->setWindow(storageBar->window());
      animator->callback = [bar, storageBar, media](qreal v){
        bar->setProgress((1.0 - media.free()) * v);
        storageBar->setPictures(media.pictures * v);
        storageBar->setMusic(media.music * v);
        storageBar->setVideos(media.videos * v);
        storageBar->setDocuments(media.documents * v);
        storageBar->setOthers(media.others * v);
      };
      animator->setDuration(400);
      animator->start();
    });

    using S = StorageBar;
    const auto subcontrols = QVector<QskAspect>{S::Pictures, S::Music, S::Videos, S::Documents, S::Others, S::Free};
    const auto subcontrolNames = QStringList{"Picture", "Music", "Videos", "Documents", "Others", "Free"};
    Q_ASSERT(subcontrolNames.size() == subcontrols.size());

    for(int i = 0; i < subcontrolNames.size(); ++i)
    {
      auto* const dot = new QskBox(mediaLegend);
      dot->setBoxShapeHint(QskBox::Panel,{4});
      dot->setMinimumSize(8,8);
      dot->setMaximumSize(8,8);
      const auto color = storageBar->color(subcontrols[i]);
      dot->setGradientHint(QskBox::Panel, {color.lighter(), color});
      auto* const label = new QskTextLabel(subcontrolNames[i], mediaLegend);
      label->setFontRole(QskSkin::SmallFont);
    }

    layout->setStretchFactor(left,1);
    layout->setStretchFactor(center,2);
    layout->setStretchFactor(right,5);
    return layout;
  };

  setPanel( true );
  setSizePolicy( QskSizePolicy::Expanding, QskSizePolicy::Expanding );
  setDefaultAlignment( Qt::AlignTop );
  setSpacing( 24 );
  setMargins(30);
  setSizePolicy(Qt::Horizontal, QskSizePolicy::Minimum);

  setSubcontrolProxy( QskBox::Panel, StoragePage::Panel );

  createRow("Backup (B:)","Used for daily backups", Media{0.1,0.1,0.1,0.02, 0.01}, this);
  createRow("Share (S:)","Used for sharing files publicly",Media{0.05,0.05,0.2,0.2,0.01}, this);
  createRow("Exchange (X:)","Used for exchanging large files", Media{0.1,0.1,0.1,0.1,0.5}, this);
  addSpacer(1, 99);
}
