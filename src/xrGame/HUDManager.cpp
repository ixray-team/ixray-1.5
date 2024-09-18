#include "stdafx.h"
#include "HUDManager.h"
#include "hudtarget.h"
#include "actor.h"
#include "../xrEngine/igame_level.h"
#include "../xrEngine/xr_input.h"
#include "GamePersistent.h"
#include "MainMenu.h"
#include "grenade.h"
#include "spectator.h"
#include "Car.h"
#include "UIGameCustom.h"
#ifdef	DEBUG
#include "phdebug.h"
#endif
#include "UIFontDefines.h"

//--------------------------------------------------------------------
CHUDManager::CHUDManager() : m_Renderable(true), pUI(NULL), m_pHUDTarget(xr_new<CHUDTarget>())
{ 
	OnDisconnected();
}
//--------------------------------------------------------------------
CHUDManager::~CHUDManager()
{
	xr_delete(pUI);
	xr_delete(m_pHUDTarget);
	b_online = false;
}

//--------------------------------------------------------------------

void CHUDManager::Load()
{
	if(pUI){
		pUI->Load			( pUI->UIGame() );
		return;
	}
	pUI					= xr_new<CUI> (this);
	pUI->Load			(NULL);
	OnDisconnected		();
}
//--------------------------------------------------------------------
void CHUDManager::OnFrame()
{
	if ( !m_Renderable )
	{
		return;
	}

	if(!b_online)					return;
	if (pUI) pUI->UIOnFrame();
	m_pHUDTarget->CursorOnFrame();
}
//--------------------------------------------------------------------

ENGINE_API extern float psHUD_FOV;

void CHUDManager::Render_First()
{
	if ( !m_Renderable )
	{
		return;
	}

	if (!psHUD_Flags.is(HUD_WEAPON|HUD_WEAPON_RT|HUD_WEAPON_RT2))return;
	if (0==pUI)						return;
	CObject*	O					= g_pGameLevel->CurrentViewEntity();
	if (0==O)						return;
	CActor*		A					= smart_cast<CActor*> (O);
	if (!A)							return;
	if (A && !A->HUDview())			return;

	// only shadow 
	::Render->set_Invisible			(TRUE);
	::Render->set_Object			(O->H_Root());
	O->renderable_Render			();
	::Render->set_Invisible			(FALSE);
}

void CHUDManager::Render_Last()
{
	if ( !m_Renderable )
	{
		return;
	}

	if (!psHUD_Flags.is(HUD_WEAPON|HUD_WEAPON_RT|HUD_WEAPON_RT2))return;
	if (0==pUI)						return;
	CObject*	O					= g_pGameLevel->CurrentViewEntity();
	if (0==O)						return;
	CActor*		A					= smart_cast<CActor*> (O);
	if (A && !A->HUDview())			return;
	if( smart_cast<CCar*>(O) || smart_cast<CSpectator*>(O) )
	{
		return;
	}

	// hud itself
	::Render->set_HUD				(TRUE);
	::Render->set_Object			(O->H_Root());
	O->OnHUDDraw					(this);
	::Render->set_HUD				(FALSE);
}

#include "player_hud.h"
bool   CHUDManager::RenderActiveItemUIQuery()
{
	if ( !m_Renderable )
	{
		return false;
	}

	if (!psHUD_Flags.is(HUD_WEAPON|HUD_WEAPON_RT|HUD_WEAPON_RT2))return false;
	return (g_player_hud && g_player_hud->render_item_ui_query() );
}

void   CHUDManager::RenderActiveItemUI()
{
	if ( !m_Renderable )
	{
		return;
	}

	g_player_hud->render_item_ui		();
}

extern ENGINE_API BOOL bShowPauseString;
//отрисовка элементов интерфейса
#include "../xrEngine/string_table.h"
void  CHUDManager::RenderUI()
{
	if ( !m_Renderable )
	{
		return;
	}

	if(!b_online)					return;

	BOOL bAlready					= FALSE;
	if (true || psHUD_Flags.is(HUD_DRAW | HUD_DRAW_RT))
	{
		HitMarker.Render			();
		bAlready					= ! (pUI && !pUI->Render());
		UI().RenderFont();
	}

	if (/* psHUD_Flags.is(HUD_CROSSHAIR|HUD_CROSSHAIR_RT|HUD_CROSSHAIR_RT2) &&*/ !bAlready)	
		m_pHUDTarget->Render();


	if( Device.Paused() && bShowPauseString){
		CGameFont* pFont	= UI().Font().GetFont(GRAFFITI50_FONT_NAME);
		pFont->SetColor		(0x80FF0000	);
		LPCSTR _str			= CStringTable().translate("st_game_paused").c_str();
		
		Fvector2			_pos;
		_pos.set			(UI_BASE_WIDTH/2.0f, UI_BASE_HEIGHT/2.0f);
		UI().ClientToScreenScaled(_pos);
		pFont->SetAligment	(CGameFont::alCenter);
		pFont->Out			(_pos.x, _pos.y, _str);
		pFont->OnRender		();
	}

}

void CHUDManager::OnEvent(EVENT E, u64 P1, u64 P2)
{
}

collide::rq_result&	CHUDManager::GetCurrentRayQuery	() 
{
	return m_pHUDTarget->GetRQ();
}

void CHUDManager::SetCrosshairDisp	(float dispf, float disps)
{	
	m_pHUDTarget->GetHUDCrosshair().SetDispersion(psHUD_Flags.test(HUD_CROSSHAIR_DYNAMIC) ? dispf : disps);
}

#ifdef DEBUG
void CHUDManager::SetFirstBulletCrosshairDisp(float fbdispf)
{
	m_pHUDTarget->GetHUDCrosshair().SetFirstBulletDispertion(fbdispf);
}
#endif

void  CHUDManager::ShowCrosshair(bool show)
{
	m_pHUDTarget->ShowCrosshair	(show);
}

void CHUDManager::HitMarked( int idx, float power, const Fvector& dir )
{
	HitMarker.Hit			(dir);
	clamp					(power,0.0f,1.0f);
	pInput->feedback		(u16(iFloor(u16(-1)*power)), u16(iFloor(u16(-1)*power)), 0.5f);
}

bool CHUDManager::AddGrenade_ForMark( CGrenade* grn )
{
	return HitMarker.AddGrenade_ForMark( grn );
}

void CHUDManager::Update_GrenadeView( Fvector& pos_actor )
{
	HitMarker.Update_GrenadeView( pos_actor );
}

void CHUDManager::SetHitmarkType( LPCSTR tex_name )
{
	HitMarker.InitShader( tex_name );
}

void CHUDManager::SetGrenadeMarkType( LPCSTR tex_name )
{
	HitMarker.InitShader_Grenade( tex_name );
}

// ------------------------------------------------------------------------------------

#include "ui\UIMainInGameWnd.h"
extern CUIXml*			pWpnScopeXml;

void CHUDManager::OnScreenResolutionChanged()
{
	if(pUI->UIGame())
		pUI->UIGame()->HideShownDialogs	();

	xr_delete							(pWpnScopeXml);
	xr_delete							(pUI->UIMainIngameWnd);

	pUI->UIMainIngameWnd				= xr_new<CUIMainIngameWnd>	();
	pUI->UIMainIngameWnd->Init			();
	pUI->UnLoad							();
	pUI->Load							(pUI->UIGame());
	pUI->OnConnected					();
	GetUICursor().OnScreenResolutionChanged();
}

void CHUDManager::OnDisconnected()
{
	b_online				= false;
	if(pUI)
		Device.seqFrame.Remove	(pUI);
}

void CHUDManager::OnConnected()
{
	if(b_online)			return;
	b_online				= true;
	if(pUI){
		Device.seqFrame.Add	(pUI,REG_PRIORITY_LOW-1000);
	}
}

void CHUDManager::net_Relcase( CObject* obj )
{
	HitMarker.net_Relcase		( obj );
	
	VERIFY						( m_pHUDTarget );
	m_pHUDTarget->net_Relcase	( obj );
#ifdef	DEBUG
	DBG_PH_NetRelcase( obj );
#endif
}
