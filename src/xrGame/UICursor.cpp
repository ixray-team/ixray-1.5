#include "stdafx.h"
#include "uicursor.h"

#include "../xrEngine/CustomHUD.h"
#include "UI.h"
#include "HUDManager.h"
#include "ui/UIStatic.h"
#include "ui/UIXmlInit.h"

constexpr auto C_DEFAULT = color_xrgb(0xff, 0xff, 0xff);

CUICursor::CUICursor()
:m_static(NULL)
{    
	bVisible				= false;
	vPos.set				(0.f,0.f);
	InitInternal			();
	Device.seqRender.Add	(this,-3/*2*/);
	Device.seqResolutionChanged.Add(this);
}
//--------------------------------------------------------------------
CUICursor::~CUICursor	()
{
	xr_delete				(m_static);
	Device.seqRender.Remove	(this);
	Device.seqResolutionChanged.Remove(this);
}

void CUICursor::OnScreenResolutionChanged()
{
	xr_delete					(m_static);
	InitInternal				();
}

void CUICursor::InitInternal()
{
	CUIXml xml_doc;
	xml_doc.Load				(CONFIG_PATH, UI_PATH, "cursor.xml");

	LPCSTR nodevalue			= xml_doc.Read("texture_name", 0, "ui\\ui_ani_cursor");

	m_static					= xr_new<CUIStatic>();
	m_static->InitTextureEx		(nodevalue, "hud\\cursor");

	float r_x					= xml_doc.ReadFlt("texture_rect_x", 0, 45.f);
	float r_y					= xml_doc.ReadFlt("texture_rect_y", 0, 45.f);

	Frect						rect;
	rect.set					(0.0f,0.0f, r_x, r_y);
	m_static->SetOriginalRect	(rect);
	Fvector2					sz;
	sz.set						(rect.rb);
	sz.x						*= UI().get_current_kx();

	float width					= xml_doc.ReadFlt("width", 0, 40.0f);
	float height				= xml_doc.ReadFlt("height", 0, 40.0f);
	width						*= UI().get_current_kx();
	BOOL stretch				= xml_doc.ReadInt("stretch", 0, TRUE);


	m_static->SetWndSize		(Fvector2().set(width, height));
	m_static->SetStretchTexture	(!!stretch);
}

//--------------------------------------------------------------------
u32 last_render_frame = 0;
void CUICursor::OnRender	()
{
	if( !IsVisible() ) return;
#ifdef DEBUG
	VERIFY(last_render_frame != Device.dwFrame);
	last_render_frame = Device.dwFrame;

	if(bDebug)
	{
	CGameFont* F		= UI().Font().pFontDI;
	F->SetAligment		(CGameFont::alCenter);
	F->SetHeight		(0.02f);
	F->OutSetI			(0.f,-0.9f);
	F->SetColor			(0xffffffff);
	Fvector2			pt = GetCursorPosition();
	F->OutNext			("%f-%f",pt.x, pt.y);
	}
#endif

	m_static->SetWndPos	(vPos);
	m_static->Update	();
	m_static->Draw		();
}

Fvector2 CUICursor::GetCursorPosition()
{
	return  vPos;
}

Fvector2 CUICursor::GetCursorPositionDelta()
{
	Fvector2 res_delta;

	res_delta.x = vPos.x - vPrevPos.x;
	res_delta.y = vPos.y - vPrevPos.y;
	return res_delta;
}

void CUICursor::UpdateCursorPosition()
{

	POINT		p;
	BOOL r		= GetCursorPos(&p);
	R_ASSERT	(r);

	vPrevPos = vPos;

	vPos.x			= (float)p.x * (UI_BASE_WIDTH/(float)Device.dwWidth);
	vPos.y			= (float)p.y * (UI_BASE_HEIGHT/(float)Device.dwHeight);
	clamp			(vPos.x, 0.f, UI_BASE_WIDTH);
	clamp			(vPos.y, 0.f, UI_BASE_HEIGHT);
}

void CUICursor::SetUICursorPosition(Fvector2 pos)
{
	vPos		= pos;
	POINT		p;
	p.x			= iFloor(vPos.x / (UI_BASE_WIDTH/(float)Device.dwWidth));
	p.y			= iFloor(vPos.y / (UI_BASE_HEIGHT/(float)Device.dwHeight));

	SetCursorPos(p.x, p.y);
}
