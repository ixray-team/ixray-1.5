#include "stdafx.h"
#include "uiprogressbar.h"

CUIProgressBar::CUIProgressBar(void)
{
	m_MinPos				= 1.0f;
	m_MaxPos				= 1.0f+EPS;

	Enable					(false);

	m_bBackgroundPresent	= false;
	m_bUseColor				= false;
	m_bUseMidColor			= false;

	AttachChild				(&m_UIBackgroundItem);
	AttachChild				(&m_UIProgressItem);
	m_ProgressPos.x			= 0.0f;
	m_ProgressPos.y			= 0.0f;
	m_inertion				= 0.0f;
	m_last_render_frame		= u32(-1);
	m_orient_mode			= om_horz;
}

CUIProgressBar::~CUIProgressBar(void)
{
}

void CUIProgressBar::InitProgressBar(Fvector2 pos, Fvector2 size, EOrientMode mode)
{
	m_orient_mode			= mode;
	SetWndPos				(pos);
	SetWndSize				(size);
	UpdateProgressBar		();
}

void CUIProgressBar::UpdateProgressBar()
{
	if( fsimilar(m_MaxPos,m_MinPos) ) m_MaxPos	+= EPS;

	float progressbar_unit = 1/(m_MaxPos-m_MinPos);

	float fCurrentLength = m_ProgressPos.x*progressbar_unit;

	if ( m_orient_mode == om_horz || m_orient_mode == om_back )
	{
		m_CurrentLength = GetWidth()*fCurrentLength;
	}
	else if ( m_orient_mode == om_vert || m_orient_mode == om_down )
	{
		m_CurrentLength = GetHeight()*fCurrentLength;
	}
	else
	{
		m_CurrentLength = 0.0f;
	}

	if(m_bUseColor){
		Fcolor curr;
		if (m_bUseMidColor)
			curr.lerp(m_minColor, m_middleColor, m_maxColor, fCurrentLength);
		else
			curr.lerp(m_minColor, m_maxColor, fCurrentLength);
		m_UIProgressItem.GetStaticItem		()->SetColor			(curr);
	}
}

void CUIProgressBar::SetProgressPos(float _Pos)				
{ 
	m_ProgressPos.y		= _Pos; 
	clamp(m_ProgressPos.y,m_MinPos,m_MaxPos);

/*	if(m_last_render_frame+1 != Device.dwFrame)
		m_ProgressPos.x = m_ProgressPos.y;
*/
	UpdateProgressBar	();
}

float _sign(const float& v)
{
	return (v>0.0f)?+1.0f:-1.0f;
}
void CUIProgressBar::Update()
{
	inherited::Update();
	if(!fsimilar(m_ProgressPos.x, m_ProgressPos.y))
	{
		if( fsimilar(m_MaxPos,m_MinPos) ) m_MaxPos	+= EPS;	//hack ^(
		float _diff				= m_ProgressPos.y - m_ProgressPos.x;
		
		float _length			= (m_MaxPos-m_MinPos);
		float _val				= _length*(1.0f-m_inertion)*Device.fTimeDelta;

		_val					= _min(_abs(_val), _abs(_diff) );
		_val					*= _sign(_diff);
		m_ProgressPos.x			+= _val;
		UpdateProgressBar		();
	}
}

void CUIProgressBar::Draw()
{
	Frect					rect;
	GetAbsoluteRect			(rect);

	if(m_bBackgroundPresent){
		UI()->PushScissor	(rect);		
		m_UIBackgroundItem.Draw();
		UI()->PopScissor	();
	}

	Frect progress_rect;

	switch ( m_orient_mode )
	{
	case om_horz:
		progress_rect.set	( 0, 0, m_CurrentLength, GetHeight() );
		break;
	case om_vert:
		progress_rect.set	( 0, GetHeight() - m_CurrentLength, GetWidth(), GetHeight() );
		break;
	case om_back:
		progress_rect.set	( GetWidth() - m_CurrentLength * 1.01f, 0, GetWidth(), GetHeight() );
	    break;
	case om_down:
		progress_rect.set	( 0, 0, GetWidth(), m_CurrentLength );
	    break;
	default:
		NODEFAULT;
		break;
	}

	if(m_CurrentLength>0){
		Fvector2 pos		= m_UIProgressItem.GetWndPos();	
		progress_rect.add	(rect.left + pos.x,rect.top + pos.y);

		UI()->PushScissor	(progress_rect);
		m_UIProgressItem.Draw();
		UI()->PopScissor	();
	}
	m_last_render_frame	= Device.dwFrame;
}