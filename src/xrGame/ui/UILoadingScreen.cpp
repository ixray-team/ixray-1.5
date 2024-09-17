#include "StdAfx.h"
#include "UILoadingScreen.h"

#include "../../xrEngine/GameFont.h"
#include "UIHelper.h"
#include "ui/UIXmlInit.h"

UILoadingScreen::UILoadingScreen()
    : loadingProgress(nullptr),
      loadingLogo(nullptr), loadingStage(nullptr) {
    UILoadingScreen::Initialize();
}

void UILoadingScreen::Initialize() {
    CUIXml uiXml;
    uiXml.Load(CONFIG_PATH, UI_PATH, "ui_mm_loading_screen.xml");

    const auto loadProgressBar = [&]() 
        {
        loadingProgress = UIHelper::CreateProgressBar(uiXml, "loading_progress", this);
    };

    const auto loadBackground = [&] { CUIXmlInit::InitWindow(uiXml, "background", 0, this); };

    const auto node =
        uiXml.NavigateToNodeWithAttribute("loading_progress", "under_background", "0");
    if (node) {
        loadBackground();
        loadProgressBar();
    } else {
        loadProgressBar();
        loadBackground();
    }

    loadingLogo = UIHelper::CreateStatic(uiXml, "loading_logo", this);
    loadingStage = UIHelper::CreateStatic(uiXml, "loading_stage", this);
}

void UILoadingScreen::Update(const int stagesCompleted, const int stagesTotal) {
    const float progress = float(stagesCompleted) / stagesTotal * loadingProgress->GetRange_max();
    if (loadingProgress->GetProgressPos() < progress)
        loadingProgress->SetProgressPos(progress);

    CUIWindow::Update();
    Draw();
}

void UILoadingScreen::ForceFinish() {
    loadingProgress->SetProgressPos(loadingProgress->GetRange_max());
}

void UILoadingScreen::SetLevelLogo(const char* name) const { loadingLogo->InitTexture(name); }

void UILoadingScreen::SetStageTitle(const char* title) const {
    loadingStage->SetText(title);
}
