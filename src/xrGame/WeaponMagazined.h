#pragma once

#include "weapon.h"
#include "hudsound.h"
#include "ai_sounds.h"

class ENGINE_API CMotionDef;

//размер очереди считается бесконечность
//заканчиваем стрельбу, только, если кончились патроны
#define WEAPON_ININITE_QUEUE -1


class CWeaponMagazined: public CWeapon
{
private:
	typedef CWeapon inherited;
protected:
	//звук текущего выстрела
	shared_str		m_sSndShotCurrent;

	//дополнительная информация о глушителе
	LPCSTR			m_sSilencerFlameParticles;
	LPCSTR			m_sSilencerSmokeParticles;

	ESoundTypes		m_eSoundShow;
	ESoundTypes		m_eSoundHide;
	ESoundTypes		m_eSoundShot;
	ESoundTypes		m_eSoundEmptyClick;
	ESoundTypes		m_eSoundReload;
	ESoundTypes		m_eSoundClose;
	// General
	//кадр момента пересчета UpdateSounds
	u32				dwUpdateSounds_Frame;
protected:

	virtual void	switch2_Idle	();
	virtual void	switch2_Fire	();
	virtual void	switch2_Empty	();
	virtual void	switch2_Reload	();
	virtual void	switch2_Hiding	();
	virtual void	switch2_Hidden	();
	virtual void	switch2_Showing	();
	
	virtual void	OnShot			();	
	
	virtual void	OnEmptyClick	();

	virtual void	OnAnimationEnd	(u32 state);
	virtual void	OnStateSwitch	(u32 S);

	virtual void	UpdateSounds	();

	bool			TryReload		();

protected:
	virtual void	ReloadMagazine();
			void	ApplySilencerKoeffs();
			void	ResetSilencerKoeffs();

	virtual void	state_Fire		(float dt);
	virtual void	state_Misfire	(float dt);
public:
					CWeaponMagazined	(ESoundTypes eSoundType=SOUND_TYPE_WEAPON_SUBMACHINEGUN);
	virtual			~CWeaponMagazined	();

	virtual void	Load			(LPCSTR section);
			void	LoadSilencerKoeffs();
	virtual CWeaponMagazined*cast_weapon_magazined	()		 {return this;}

	virtual void	SetDefaults		();
	virtual void	FireStart		();
	virtual void	FireEnd			();
	virtual void	Reload			();
	

	virtual	void	UpdateCL		();
	virtual void	net_Destroy		();
	virtual void	net_Export		(NET_Packet& P);
	virtual void	net_Import		(NET_Packet& P);

	virtual void	OnH_A_Chield		();

	virtual bool	Attach(PIItem pIItem, bool b_send_event);
	virtual bool	Detach(const char* item_section_name, bool b_spawn_item);
			bool	DetachScope(const char* item_section_name, bool b_spawn_item);
	virtual bool	CanAttach(PIItem pIItem);
	virtual bool	CanDetach(const char* item_section_name);

	virtual void	InitAddons();

	virtual bool	Action			(s32 cmd, u32 flags);
	bool			IsAmmoAvailable	();
	virtual void	UnloadMagazine	(bool spawn_ammo = true);

	virtual void	GetBriefInfo				(xr_string& str_name, xr_string& icon_sect_name, xr_string& str_count, string16& fire_mode);

public:
	virtual bool	SwitchMode				();
	virtual bool	SingleShotMode			()			{return 1 == m_iQueueSize;}
	virtual void	SetQueueSize			(int size);
	IC		int		GetQueueSize			() const	{return m_iQueueSize;};
	virtual bool	StopedAfterQueueFired	()			{return m_bStopedAfterQueueFired; }
	virtual void	StopedAfterQueueFired	(bool value){m_bStopedAfterQueueFired = value; }

protected:
	//максимальный размер очереди, которой можно стрельнуть
	int				m_iQueueSize;
	//количество реально выстреляных патронов
	int				m_iShotNum;
	//после какого патрона, при непрерывной стрельбе, начинается отдача (сделано из-зи Абакана)
	int				m_iShootEffectorStart;
	Fvector			m_vStartPos, m_vStartDir;
	//флаг того, что мы остановились после того как выстреляли
	//ровно столько патронов, сколько было задано в m_iQueueSize
	bool			m_bStopedAfterQueueFired;
	//флаг того, что хотя бы один выстрел мы должны сделать
	//(даже если очень быстро нажали на курок и вызвалось FireEnd)
	bool			m_bFireSingleShot;
	//режимы стрельбы
	bool			m_bHasDifferentFireModes;
	xr_vector<s8>	m_aFireModes;
	int				m_iCurFireMode;
	int				m_iPrefferedFireMode;

	//переменная блокирует использование
	//только разных типов патронов
	bool m_bLockType;

public:
	virtual void	OnZoomIn			();
	virtual void	OnZoomOut			();
			void	OnNextFireMode		();
			void	OnPrevFireMode		();
			bool	HasFireModes		() { return m_bHasDifferentFireModes; };
	virtual	int		GetCurrentFireMode	() { return m_aFireModes[m_iCurFireMode]; };	

	virtual void	save				(NET_Packet &output_packet);
	virtual void	load				(IReader &input_packet);

protected:
	virtual bool	install_upgrade_impl( LPCSTR section, bool test );

protected:
	virtual bool	AllowFireWhileWorking() {return false;}
	virtual void	NeedAddSuffix(xr_string& M);

	//виртуальные функции для проигрывания анимации HUD
	virtual void	PlayAnimShow		();
	virtual void	PlayAnimHide		();
	virtual void	PlayAnimReload		();
	virtual void	PlayAnimShoot		();
	virtual void	PlayReloadSound		();

	virtual	int		ShotsFired			() { return m_iShotNum; }
	virtual float	GetWeaponDeterioration	();
	virtual bool	WeaponSoundExist	(LPCSTR section, LPCSTR sound_name);
};
