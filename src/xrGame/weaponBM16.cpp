#include "stdafx.h"
#include "weaponBM16.h"

CWeaponBM16::~CWeaponBM16()
{
}

void CWeaponBM16::Load	(LPCSTR section)
{
	inherited::Load		(section);
	m_sounds.LoadSound	(section, "snd_reload_1", "sndReload1", true, m_eSoundShot);
}

void CWeaponBM16::PlayReloadSound()
{
	if(m_magazine.size()==1)	
		PlaySound	("sndReload1",get_LastFP());
	else						
		PlaySound	("sndReload",get_LastFP());
}

void CWeaponBM16::PlayAnimShoot()
{
	PlayHUDMotion("anm_shot", true, this, eFire);
}

void CWeaponBM16::PlayAnimReload()
{
	if (m_magazine.size() == 1 || !HaveCartridgeInInventory(2))
		PlayHUDMotion("anm_reload_1", true, this, eReload);
	else
		PlayHUDMotion("anm_reload_2", true, this, eReload);
}

void CWeaponBM16::NeedAddSuffix(xr_string& M)
{
	xr_string ac;
	ac = std::to_string(iAmmoElapsed);

	if (IsZoomed())
		AddSuffix(M, "_aim", ac);

	if (IsMisfire())
		AddSuffix(M, "_misfire", ac);

	 AddSuffix(M, "_", ac);
}