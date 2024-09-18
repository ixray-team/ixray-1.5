#include "stdafx.h"
#include "UIBoosterInfo.h"
#include "UIStatic.h"
#include "UIXmlInit.h"
#include "UIHelper.h"
#include "../Actor.h"
#include "../../xrEngine/string_table.h"

CUIBoosterInfo::CUIBoosterInfo()
{
	for(u32 i = 0; i < eBoostExplImmunity; ++i)
	{
		m_booster_items[i] = NULL;
	}
}

CUIBoosterInfo::~CUIBoosterInfo()
{
	delete_data(m_booster_items);
	xr_delete(m_Prop_line);
}

LPCSTR boost_influence_caption[] =
{
	"ui_inv_health",
	"ui_inv_bleeding",
	"ui_inv_power",
	"ui_inv_radiation",
	"ui_inv_satiety",
};

LPCSTR ef_boosters_section_names[] =
{
	"eat_health",
	"wounds_heal_perc",
	"eat_power",
	"eat_radiation",
	"eat_satiety",
};

LPCSTR ef_boosters_node_names[] =
{
	"boost_health",
	"boost_bleeding",
	"boost_power",
	"boost_radiation",
	"boost_satiety",
};

void CUIBoosterInfo::InitFromXml(CUIXml& xml)
{
	LPCSTR base	= "booster_params";
	XML_NODE* stored_root = xml.GetLocalRoot();
	XML_NODE* base_node   = xml.NavigateToNode( base, 0 );
	if(!base_node || !base)
		return;

	CUIXmlInit::InitWindow(xml, base, 0, this);
	xml.SetLocalRoot(base_node);
	
	m_Prop_line = xr_new<CUIStatic>();
	AttachChild(m_Prop_line);
	m_Prop_line->SetAutoDelete(false);	
	CUIXmlInit::InitStatic(xml, "prop_line", 0, m_Prop_line);

	for(u32 i = 0; i < eBoostExplImmunity; ++i)
	{
		m_booster_items[i] = xr_new<UIBoosterInfoItem>();
		m_booster_items[i]->Init(xml, ef_boosters_node_names[i]);
		m_booster_items[i]->SetAutoDelete(false);

		LPCSTR name = CStringTable().translate(boost_influence_caption[i]).c_str();
		m_booster_items[i]->SetCaption(name);

		xml.SetLocalRoot(base_node);
	}

	xml.SetLocalRoot( stored_root );
}

void CUIBoosterInfo::SetInfo( CInventoryItem& pInvItem )
{
	DetachAll();
	AttachChild( m_Prop_line );
	
	const shared_str& section = pInvItem.object().cNameSect();
	//CEatableItem* eatable = pInvItem.cast_eatable_item();
	CActor* actor = smart_cast<CActor*>( Level().CurrentViewEntity() );
	if ( !actor )
	{
		return;
	}


	float val = 0.0f;
	Fvector2 pos;
	float h = m_Prop_line->GetWndPos().y+m_Prop_line->GetWndSize().y;

	for (u32 i = 0; i < eBoostExplImmunity; ++i)
	{
		if(pSettings->line_exist(section.c_str(), ef_boosters_section_names[i]))
		{
			val	= pSettings->r_float(section, ef_boosters_section_names[i]);
			if(fis_zero(val))
				continue;
			if(i == _item_bleeding_restore_speed)
				m_booster_items[i]->SetValue(-val);
			else
				m_booster_items[i]->SetValue(val);


			pos.set(m_booster_items[i]->GetWndPos());
			pos.y = h;
			m_booster_items[i]->SetWndPos(pos);

			h += m_booster_items[i]->GetWndSize().y;
			AttachChild(m_booster_items[i]);
		}
	}

	SetHeight(h);
}

/// ----------------------------------------------------------------

UIBoosterInfoItem::UIBoosterInfoItem()
{
	m_caption				= NULL;
	m_value					= NULL;
	m_magnitude				= 1.0f;
	m_show_sign				= false;
	m_color_mode			= 0;
	
	m_unit_str._set			("");
	m_unit_str1._set		("");
	m_texture_minus._set	("");
	m_texture_plus._set		("");
}

UIBoosterInfoItem::~UIBoosterInfoItem()
{
}

void UIBoosterInfoItem::Init(CUIXml& xml, LPCSTR section)
{
	CUIXmlInit::InitWindow(xml, section, 0, this);
	xml.SetLocalRoot(xml.NavigateToNode(section));

	m_caption   = UIHelper::CreateStatic(xml, "caption", this);
	m_value     = UIHelper::CreateStatic(xml, "value",   this);
	m_magnitude = xml.ReadAttribFlt("value", 0, "magnitude", 1.0f);
	m_show_sign = (xml.ReadAttribInt("value", 0, "show_sign", 1) == 1);
	m_color_mode = xml.ReadAttribInt("value", 0, "color_mode", 0);
	
	LPCSTR unit_str = xml.ReadAttrib("value", 0, "unit_str", "");
	m_unit_str._set(CStringTable().translate(unit_str));
	unit_str = xml.ReadAttrib("value", 0, "unit_str1", "");
	m_unit_str1._set(CStringTable().translate(unit_str));
	
	LPCSTR texture_minus = xml.Read("texture_minus", 0, "");
	if(texture_minus && xr_strlen(texture_minus))
	{
		m_texture_minus._set(texture_minus);
		
		LPCSTR texture_plus = xml.Read("caption:texture", 0, "");
		m_texture_plus._set(texture_plus);
		VERIFY(m_texture_plus.size());
	}
}

void UIBoosterInfoItem::SetCaption(LPCSTR name)
{
	m_caption->SetText(name);
}

void UIBoosterInfoItem::SetValue(float value)
{
	value *= m_magnitude;
	string32 buf, str;
	if(m_show_sign)
		sprintf(buf, "%+.0f", value);
	else
		sprintf(buf, "%.0f", value);

	u32 red = color_rgba(255, 0, 0, 255);
	u32 green = color_rgba(0, 255, 0, 255);
	
	if (m_unit_str.size())
		xr_sprintf(str, "%s %s", buf, m_unit_str.c_str());
	else if(m_unit_str1.size())
		xr_sprintf(str,"%s%s", buf, m_unit_str1.c_str());
	else
		xr_sprintf(str,"%s", buf);

	bool positive = (value >= 0.0f);
	m_value->SetText(buf);

	if (m_color_mode == 2)
	{
		(value > 0)?m_value->SetTextColor(red):m_value->SetTextColor(green);
	}
	else if(m_color_mode == 1)
	{
		(value < 0)?m_value->SetTextColor(red):m_value->SetTextColor(green);
	}
	else
		m_value->SetTextColor(color_rgba(170,170,170,255));

	if(m_texture_minus.size())
	{
		if(positive)
			m_caption->InitTexture(m_texture_plus.c_str());
		else
			m_caption->InitTexture(m_texture_minus.c_str());
	}
}
