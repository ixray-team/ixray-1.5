#pragma once

#include "../../xrEngine/ILoadingScreen.h"
#include "UIStatic.h"
#include "UIWindow.h"
#include "ui/UIProgressBar.h"

class CApplication;

class UILoadingScreen : public ILoadingScreen, public CUIWindow {
    CUIProgressBar* loadingProgress;
    CUIStatic* loadingLogo;

    CUIStatic* loadingStage;

public:
    UILoadingScreen();

    void Initialize() override;

    void Update(const int stagesCompleted, const int stagesTotal) override;
    void ForceFinish() override;

    void SetLevelLogo(const char* name) const override;
    void SetStageTitle(const char* title) const override;
};