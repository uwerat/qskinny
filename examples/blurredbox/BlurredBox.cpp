#include "BlurredBox.h"
#include "BlurredBoxSkinlet.h"

#include <QQuickWindow>

QSK_SUBCONTROL( BlurredBox, Panel )

class BlurredBoxSkinlet;

BlurredBox::BlurredBox( QQuickItem* parentItem )
    : QskBox( parentItem )
{
    auto* const skinlet = new BlurredBoxSkinlet();
    skinlet->setOwnedBySkinnable( true );
    setSkinlet( skinlet );
    setPolishOnResize( true );
}

BlurredBox::~BlurredBox() = default;

QRectF BlurredBox::rectOnScreen() const
{
    return mapRectToScene( contentsRect() );
}

QRectF BlurredBox::rectOfScreen() const
{
    // find root node and get its rectangle
    auto* root = parentItem();

    while ( root && root->parentItem() )
    {
        root = root->parentItem();
    }

    const auto rootRect = this->window()->contentItem()->boundingRect();
    return rootRect;
}

void BlurredBox::geometryChangeEvent( QskGeometryChangeEvent* e )
{
    Inherited::geometryChangeEvent( e );
    update();
}

float BlurredBox::blurDirections() const
{
    return m_blurDirections;
}

void BlurredBox::setBlurDirections( float newBlurDirections )
{
    if ( qFuzzyCompare( m_blurDirections, newBlurDirections ) )
    {
        return;
    }

    m_blurDirections = newBlurDirections;
    update();
    Q_EMIT blurDirectionsChanged( m_blurDirections );
}

float BlurredBox::blurQuality() const
{
    return m_blurQuality;
}

void BlurredBox::setBlurQuality( float newBlurQuality )
{
    if ( qFuzzyCompare( m_blurQuality, newBlurQuality ) )
    {
        return;
    }

    m_blurQuality = newBlurQuality;
    update();
    Q_EMIT blurQualityChanged( m_blurQuality );
}

float BlurredBox::blurSize() const
{
    return m_blurSize;
}

void BlurredBox::setBlurSize( float newBlurSize )
{
    if ( qFuzzyCompare( m_blurSize, newBlurSize ) )
    {
        return;
    }

    m_blurSize = newBlurSize;
    update();
    Q_EMIT blurSizeChanged( m_blurSize );
}

#include "moc_BlurredBox.cpp"
