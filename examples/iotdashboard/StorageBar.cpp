#include "StorageBar.h"
#include <QskBox.h>
#include <QskBoxShapeMetrics.h>
#include <QskSkinlet.h>
#include <QskAnimator.h>

QSK_SUBCONTROL( StorageBar, Pictures)
QSK_SUBCONTROL( StorageBar, Music)
QSK_SUBCONTROL( StorageBar, Videos)
QSK_SUBCONTROL( StorageBar, Documents)
QSK_SUBCONTROL( StorageBar, Others )
QSK_SUBCONTROL( StorageBar, Free )

class StorageBarSkinlet final : public QskSkinlet
{
  Q_GADGET
public:

  using Inherited = QskSkinlet;
  enum NodeRole { Pictures, Music, Videos, Documents, Others, Free };
  Q_INVOKABLE StorageBarSkinlet( QskSkin* skin = nullptr ) : Inherited(skin)
  {
    setNodeRoles( { Pictures, Music, Videos, Documents, Others, Free } );
    setOwnedBySkinnable(true);
  }

private:

  QRectF subControlRect( const QskSkinnable* skinnable, const QRectF& contentsRect, QskAspect::Subcontrol subControl ) const override
  {
    using S = StorageBar;
    const auto bar = static_cast< const S* >( skinnable );

    auto x = contentsRect.x();
    const auto y = contentsRect.y();
    const auto h = contentsRect.height();

    const QRectF p {x, y, contentsRect.width() * bar->pictures(), h};
    if(subControl == S::Pictures){ return p; }
    x += p.width();

    const QRectF m {x, y, contentsRect.width() * bar->music(), h};
    if(subControl == S::Music){ return m; }
    x += m.width();

    const QRectF v {x, y, contentsRect.width() * bar->videos(), h};
    if(subControl == S::Videos){ return v; }
    x += v.width();

    const QRectF d {x, y, contentsRect.width() * bar->documents(), h};
    if(subControl == S::Documents){ return d; }
    x += d.width();

    const QRectF o {x, y, contentsRect.width() * bar->others(), h};
    if(subControl == S::Others){ return o; }
    x += o.width();

    const QRectF f {x, y, contentsRect.width() * bar->free(), h};
    if(subControl == S::Free){ return f; }

    return Inherited::subControlRect( skinnable, contentsRect, subControl );
  }

  QSGNode* updateSubNode( const QskSkinnable* skinnable, quint8 nodeRole, QSGNode* node) const override
  {
    using S = StorageBar;
    const auto bar = static_cast< const S* >( skinnable );

    if ( nodeRole == Pictures)
    {
      const QskGradient g( { { 0.0, bar->color(S::Pictures).lighter() },{ 1.0, bar->color(S::Pictures) } } );
      return updateBoxNode( bar, node, bar->subControlRect( S::Pictures ), g, S::Pictures );
    }

    if ( nodeRole == Music)
    {
      const QskGradient g( { { 0.0, bar->color(S::Music).lighter() },{ 1.0, bar->color(S::Music) } } );
      return updateBoxNode( bar, node, bar->subControlRect( S::Music ), g, S::Music );
    }

    if ( nodeRole == Videos)
    {
      const QskGradient g( { { 0.0, bar->color(S::Videos).lighter() },{ 1.0, bar->color(S::Videos) } } );
      return updateBoxNode( bar, node, bar->subControlRect( S::Videos ), g, S::Videos );
    }

    if ( nodeRole == Documents)
    {
      const QskGradient g( { { 0.0, bar->color(S::Documents).lighter() },{ 1.0, bar->color(S::Documents) } } );
      return updateBoxNode( bar, node, bar->subControlRect( S::Documents ), g, S::Documents );
    }

    if ( nodeRole == Others)
    {
      const QskGradient g( { { 0.0, bar->color(S::Others).lighter() },{ 1.0, bar->color(S::Others) } } );
      return updateBoxNode( bar, node, bar->subControlRect( S::Others ), g, S::Others );
    }

    if ( nodeRole == Free)
    {
      const QskGradient g( { { 0.0, bar->color(S::Free).lighter() },{ 1.0, bar->color(S::Free)} } );
      return updateBoxNode( bar, node, bar->subControlRect( S::Free ), g, S::Free );
    }

    return Inherited::updateSubNode( skinnable, nodeRole, node );
  }
};

StorageBar::StorageBar(QskQuickItem * const parent) : Inherited(parent)
{
  setSkinlet(new StorageBarSkinlet());

  using S = StorageBar;

  // TODO move into skin?
  setColor(S::Pictures, "#FFBE0B");
  setColor(S::Music,"#FB5607");
  setColor(S::Videos,"#FF006E");
  setColor(S::Documents, "#8338EC");
  setColor(S::Others, "#3A86FF");
  setColor(S::Free, "lightgray");

  static constexpr qreal size = 16.0;
  static constexpr qreal radius = size / 2.0;

  setMinimumSize(-1, size);
  setMaximumSize(-1, size);

  setBoxShapeHint(S::Pictures, {radius, 0.0, radius, 0.0});
  setBoxShapeHint(S::Free, { 0.0, radius, 0.0, radius});
}

qreal StorageBar::pictures() const
{
  return m_pictures;
}

void StorageBar::setPictures(qreal newPictures)
{
  if (qFuzzyCompare(m_pictures, newPictures))
   {return;}

  m_pictures = newPictures;
  Q_EMIT picturesChanged(m_pictures);
  update();
}

qreal StorageBar::music() const
{
  return m_music;
}

void StorageBar::setMusic(qreal newMusic)
{
  if (qFuzzyCompare(m_music, newMusic))
    {return;}
  m_music = newMusic;
  Q_EMIT musicChanged(m_music);
  update();
}

qreal StorageBar::videos() const
{
  return m_videos;
}

void StorageBar::setVideos(qreal newVideos)
{
  if (qFuzzyCompare(m_videos, newVideos))
  {return;}
  m_videos = newVideos;
  Q_EMIT videosChanged(m_videos);
  update();
}

qreal StorageBar::documents() const
{
  return m_documents;
}

void StorageBar::setDocuments(qreal newDocuments)
{
  if (qFuzzyCompare(m_documents, newDocuments))
    {return;}
  m_documents = newDocuments;
  Q_EMIT documentsChanged(m_documents);
  update();
}

qreal StorageBar::others() const
{
  return m_others;
}

void StorageBar::setOthers(qreal newOthers)
{
  if (qFuzzyCompare(m_others, newOthers))
   {return;}
  m_others = newOthers;
  Q_EMIT othersChanged(m_others);
  update();
}

qreal StorageBar::free() const
{
  return 1.0 - m_pictures - m_music - m_videos - m_documents - m_others;
}
