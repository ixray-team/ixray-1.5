////////////////////////////////////////////////////////////////////////////
//	Module 		: UIActorMenu_action.cpp
//	Created 	: 14.10.2008
//	Author		: Evgeniy Sokolov (sea)
//	Description : UI ActorMenu actions implementation
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "UIActorMenu.h"
#include "../actor.h"
#include "../HUDManager.h"
#include "game_cl_base.h"

#include "UICursor.h"
#include "UICellItem.h"
#include "UIItemInfo.h"
#include "UIDragDropListEx.h"
#include "UIInventoryUpgradeWnd.h"
#include "UIMessageBoxEx.h"
#include "UIPropertiesBox.h"
#include "UIDialogWnd.h"

#ifdef DEBUG
#include <dinput.h>
#endif


bool  CUIActorMenu::AllowItemDrops(EDDListType from, EDDListType to)
{
	xr_vector<EDDListType>& v = m_allowed_drops[to];
	xr_vector<EDDListType>::iterator it = std::find(v.begin(), v.end(), from);
	
	return(it!=v.end());
}

class CUITrashIcon :public ICustomDrawDragItem
{
	CUIStatic			m_icon;
public:
	CUITrashIcon		()
	{
		m_icon.SetWndSize		(Fvector2().set(29.0f*UI()->get_current_kx(), 36.0f));
		m_icon.SetStretchTexture(true);
//		m_icon.SetAlignment		(waCenter);
		m_icon.InitTexture		("ui_inGame2_inv_trash");
	}
	virtual void		OnDraw		(CUIDragItem* drag_item)
	{
		Fvector2 pos			= drag_item->GetWndPos();
		Fvector2 icon_sz		= m_icon.GetWndSize();
		Fvector2 drag_sz		= drag_item->GetWndSize();

		pos.x			-= icon_sz.x;
		pos.y			+= drag_sz.y;

		m_icon.SetWndPos(pos);
//		m_icon.SetWndSize(sz);
		m_icon.Draw		();
	}

};

void CUIActorMenu::OnDragItemOnTrash(CUIDragItem* item, bool b_receive)
{
	if (b_receive && !CurrentIItem()->IsQuestItem())
		item->SetCustomDraw(xr_new<CUITrashIcon>());
	else
		item->SetCustomDraw(NULL);
}

bool CUIActorMenu::OnItemDrop(CUICellItem* itm)
{
	InfoCurItem( NULL );
	CUIDragDropListEx*	old_owner		= itm->OwnerList();
	CUIDragDropListEx*	new_owner		= CUIDragDropListEx::m_drag_item->BackList();
	if ( old_owner==new_owner || !old_owner || !new_owner )
	{
		return false;
	}

	EDDListType t_new		= GetListType(new_owner);
	EDDListType t_old		= GetListType(old_owner);
	
	if ( !AllowItemDrops(t_old, t_new) )
	{
		Msg("incorrect action [%d]->[%d]",t_old, t_new);
		return true;
	}
	switch(t_new)
	{
		case iTrashSlot:
		{
			if(CurrentIItem()->IsQuestItem())
				return true;

			SendEvent_Item_Drop		(CurrentIItem(), m_pActorInvOwner->object_id());
			SetCurrentItem			(NULL);
		}break;
		case iActorSlot:
		{
			if(GetSlotList(CurrentIItem()->GetSlot())==new_owner)
				ToSlot	(itm, true);
		}break;
		case iActorBag:
		{
			ToBag	(itm, true);
		}break;
		case iActorBelt:
		{
			ToBelt	(itm, true);
		}break;
		case iActorTrade:
		{
			ToActorTrade(itm, true);
		}break;
		case iPartnerTrade:
		{
			if(t_old!=iPartnerTradeBag)	
				return false;
			ToPartnerTrade(itm, true);
		}break;
		case iPartnerTradeBag:
		{
			if(t_old!=iPartnerTrade)	
				return false;
			ToPartnerTradeBag(itm, true);
		}break;
		case iDeadBodyBag:
		{
			ToDeadBodyBag(itm, true);
		}break;
	};

	OnItemDropped			(CurrentIItem(), new_owner, old_owner);
	
	UpdateItemsPlace();

	return true;
}

bool CUIActorMenu::OnItemStartDrag(CUICellItem* itm)
{
	InfoCurItem( NULL );
	return false; //default behaviour
}

bool CUIActorMenu::OnItemDbClick(CUICellItem* itm)
{
	InfoCurItem( NULL );
	CUIDragDropListEx*	old_owner		= itm->OwnerList();
	EDDListType t_old					= GetListType(old_owner);

	switch ( t_old )
	{
		case iActorSlot:
		{
			if ( m_currMenuMode == mmDeadBodySearch )
				ToDeadBodyBag	( itm, false );
			else
				ToBag			( itm, false );
			break;
		}
		case iActorBag:
		{
			if ( m_currMenuMode == mmTrade )
			{
				ToActorTrade( itm, false );
				break;
			}else
			if ( m_currMenuMode == mmDeadBodySearch )
			{
				ToDeadBodyBag( itm, false );
				break;
			}
			if(m_currMenuMode!=mmUpgrade && TryUseItem( itm ))
			{
				m_item_info_view = false;
				break;
			}
			if ( TryActiveSlot( itm ) )
			{
				break;
			}
			if ( !ToSlot( itm, false ) )
			{
				if ( !ToBelt( itm, false ) )
				{
					ToSlot( itm, true );
				}
			}
			break;
		}
		case iActorBelt:
		{
			ToBag( itm, false );
			break;
		}
		case iActorTrade:
		{
			ToBag( itm, false );
			break;
		}
		case iPartnerTradeBag:
		{
			ToPartnerTrade( itm, false );
			break;
		}
		case iPartnerTrade:
		{
			ToPartnerTradeBag( itm, false );
			break;
		}
		case iDeadBodyBag:
		{
			ToBag( itm, false );
			break;
		}

	}; //switch 

	UpdateItemsPlace();
	return true;
}

bool CUIActorMenu::OnItemSelected(CUICellItem* itm)
{
	SetCurrentItem		(itm);
	InfoCurItem			(NULL);
	return				false;
}

bool CUIActorMenu::OnItemRButtonClick(CUICellItem* itm)
{
	SetCurrentItem( itm );
	InfoCurItem( NULL );
	ActivatePropertiesBox();
	m_item_info_view = false;
	return false;
}

bool CUIActorMenu::OnItemFocusReceive(CUICellItem* itm)
{
	InfoCurItem( NULL );
	m_item_info_view = true;

	itm->m_selected = true;
	set_highlight_item(itm);
	return true;
}

bool CUIActorMenu::OnItemFocusLost(CUICellItem* itm)
{
	if ( itm )
	{
		itm->m_selected = false;
	}
	InfoCurItem( NULL );
	clear_highlight_lists();
	return true;
}

bool CUIActorMenu::OnItemFocusedUpdate(CUICellItem* itm)
{
	if ( itm )
	{
		itm->m_selected = true;
		if ( m_highlight_clear )
		{
			set_highlight_item( itm );
		}
	}
	VERIFY( m_ItemInfo );
	if ( Device.dwTimeGlobal < itm->FocusReceiveTime() + m_ItemInfo->delay )
	{
		return true; //false
	}
	if ( CUIDragDropListEx::m_drag_item || m_UIPropertiesBox->IsShown() || !m_item_info_view )
	{
		return true;
	}	
	
	InfoCurItem( itm );
	return true;
}

bool CUIActorMenu::OnMouseAction( float x, float y, EUIMessages mouse_action )
{
	inherited::OnMouseAction( x, y, mouse_action );
	return true; // no click`s
}

bool CUIActorMenu::OnKeyboardAction(int dik, EUIMessages keyboard_action)
{
	InfoCurItem( NULL );
	if ( is_binded(kDROP, dik) )
	{
		bool isDroppableItem = WINDOW_KEY_PRESSED == keyboard_action && CurrentIItem() && !CurrentIItem()->IsQuestItem()
			&& CurrentIItem()->parent_id() == m_pActorInvOwner->object_id();
		if (isDroppableItem)
		{

			SendEvent_Item_Drop		(CurrentIItem(), m_pActorInvOwner->object_id());
			SetCurrentItem			(NULL);
		}
		return true;
	}
	
	if ( is_binded(kSPRINT_TOGGLE, dik) )
	{
		if ( WINDOW_KEY_PRESSED == keyboard_action )
		{
			OnPressUserKey();
		}
		return true;
	}	
	
	if ( is_binded(kUSE, dik) )
	{
		if ( WINDOW_KEY_PRESSED == keyboard_action )
		{
			GetHolder()->StartStopMenu( this, true );
		}
		return true;
	}	

#ifdef DEBUG
	if (WINDOW_KEY_PRESSED == keyboard_action)
	{
		{
			if (DIK_NUMPAD7 == dik && CurrentIItem() && CurrentIItem()->IsUsingCondition())
			{
				CurrentIItem()->ChangeCondition(-0.05f);
				m_pCurrentCellItem->UpdateConditionProgressBar();
			}
			else if (DIK_NUMPAD8 == dik && CurrentIItem() && CurrentIItem()->IsUsingCondition())
			{
				CurrentIItem()->ChangeCondition(0.05f);
				m_pCurrentCellItem->UpdateConditionProgressBar();
			}
		}
	}
#endif
	if( inherited::OnKeyboardAction(dik,keyboard_action) )return true;

	return false;
}

void CUIActorMenu::OnMouseMove()
{
	//SetInfoItem( NULL );
	inherited::OnMouseMove		();
}

void CUIActorMenu::OnPressUserKey()
{
	switch ( m_currMenuMode )
	{
	case mmUndefined:		break;
	case mmInventory:		break;
	case mmTrade:			
		OnBtnPerformTrade( this, 0 );
		break;
	case mmUpgrade:			
		TrySetCurUpgrade();
		break;
	case mmDeadBodySearch:	
		TakeAllFromPartner( this, 0 );
		break;
	default:
		R_ASSERT(0);
		break;
	}
}

void CUIActorMenu::OnBtnExitClicked(CUIWindow* w, void* d)
{
	GetHolder()->StartStopMenu			(this,true);
}

void CUIActorMenu::CallMessageBoxYesNo( LPCSTR text )
{
	m_message_box_yes_no->SetText( text );
	m_message_box_yes_no->func_on_ok = CUIWndCallback::void_function( this, &CUIActorMenu::OnMesBoxYes );
	HUD().GetUI()->StartStopMenu( m_message_box_yes_no, false );
}

void CUIActorMenu::CallMessageBoxOK( LPCSTR text )
{
	m_message_box_ok->SetText( text );
	HUD().GetUI()->StartStopMenu( m_message_box_ok, false );
}

void CUIActorMenu::OnMesBoxYes( CUIWindow*, void* )
{
	switch( m_currMenuMode )
	{
	case mmUndefined:
		break;
	case mmInventory:
		break;
	case mmTrade:
		break;
	case mmUpgrade:
		if ( m_repair_mode )
		{
			RepairEffect_CurItem();
			m_repair_mode = false;
		}
		else
		{
			m_pUpgradeWnd->OnMesBoxYes();
		}
		break;
	case mmDeadBodySearch:
		break;
	default:
		R_ASSERT(0);
		break;
	}
	UpdateItemsPlace();
}