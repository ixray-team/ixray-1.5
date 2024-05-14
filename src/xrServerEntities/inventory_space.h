#pragma once

#define CMD_START	(1<<0)
#define CMD_STOP	(1<<1)

enum {
	NO_ACTIVE_SLOT = 0xffffffff,
	KNIFE_SLOT = 0,
	PISTOL_SLOT,
	RIFLE_SLOT,
	GRENADE_SLOT,
	APPARATUS_SLOT,
	BOLT_SLOT,
	OUTFIT_SLOT,
	PDA_SLOT,
	DETECTOR_SLOT,
	TORCH_SLOT,
	ARTEFACT_SLOT,
	FLARE_SLOT,
	LAST_SLOT = FLARE_SLOT
};


#define RUCK_HEIGHT			280
#define RUCK_WIDTH			7

class CInventoryItem;
class CInventory;

typedef CInventoryItem*				PIItem;
typedef xr_vector<PIItem>			TIItemContainer;


enum EItemPlace
{			
	eItemPlaceUndefined,
	eItemPlaceSlot,
	eItemPlaceBelt,
	eItemPlaceRuck
};

extern u32	INV_STATE_LADDER;
extern u32	INV_STATE_CAR;
extern u32	INV_STATE_BLOCK_ALL;
extern u32	INV_STATE_INV_WND;
extern u32	INV_STATE_BUY_MENU;
