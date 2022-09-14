#pragma once

#include <QskWindow.h>
#include <QskStackBox.h>

class BlurredBox;
class QskLinearBox;

class MainWindow : public QskWindow
{
    Q_OBJECT

  public:
    MainWindow();

  private:
    void createBackground();
    void createBlurDirectionsControls( BlurredBox* blurred, QskLinearBox* layout );
    void createBlurQualityControls( BlurredBox* blurred, QskLinearBox* layout );
    void createBlurSizeControls( BlurredBox* blurred, QskLinearBox* layout );
    void createBlurOpacityControls( BlurredBox* blurred, QskLinearBox* layout );
    void createBlurCornerRadiiControls( BlurredBox* blurred, QskLinearBox* layout );
    void createShortcutNote( QskLinearBox* layout );
    void createOverlay();

    QskStackBox* m_stack = nullptr;
};
