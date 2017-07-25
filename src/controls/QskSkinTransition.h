#ifndef QSK_SKIN_TRANSITION_H
#define QSK_SKIN_TRANSITION_H

#include "QskGlobal.h"
#include "QskAspect.h"
#include "QskAnimationHint.h"

class QskAnimatorHint;
class QskSkin;
class QVariant;

class QSK_EXPORT QskSkinTransition
{
public:
    enum Type
    {
        Color = 1,
        Metric = 2,

        AllTypes = Color | Metric
    };

    QskSkinTransition();
    virtual ~QskSkinTransition();

    void setSourceSkin( QskSkin* );
    QskSkin* sourceSkin() const;

    void setTargetSkin( QskSkin* );
    QskSkin* targetSkin() const;

    void setAnimation( QskAnimationHint );
    QskAnimationHint animation() const;

    void setMask( Type );
    Type mask() const;

    void process();

    static bool isRunning();
    static QVariant animatedHint( QskAspect::Aspect );

protected:
    virtual void updateSkin( QskSkin*, QskSkin* );

private:
    QskSkin* m_skins[2];
    QskAnimationHint m_animation;
    Type m_mask : 2;
};

#endif
