#ifndef __X_RAY_H__
#define __X_RAY_H__

// refs
class ENGINE_API CGameFont;

class ILoadingScreen;

#include "../Include/xrRender/FactoryPtr.h"

// definition
class ENGINE_API CApplication	:
	public pureFrame,
	public IEventReceiver
{
	// levels
	struct					sLevelInfo
	{
		char*				folder;
		char*				name;
	};
	string256				app_title;
private:
	ILoadingScreen* loadingScreen;

	int		max_load_stage;

	int						load_stage;

	u32						ll_dwReference;
private:
	EVENT					eQuit;
	EVENT					eStart;
	EVENT					eStartLoad;
	EVENT					eDisconnect;
	EVENT					eConsole;

	void					Level_Append		(LPCSTR lname);
public:

	// Levels
	xr_vector<sLevelInfo>	Levels;
	u32						Level_Current;
	void					Level_Scan			();
	int						Level_ID			(LPCSTR name, LPCSTR ver, bool bSet);
	void					Level_Set			(u32 ID);
	void					LoadAllArchives		();
	CInifile*				GetArchiveHeader	(LPCSTR name, LPCSTR ver);

	// Loading
	void					LoadBegin			(shared_str lang);
	void					LoadEnd				();
	void					LoadTitleInt		(LPCSTR str);
	void					LoadSwitch			();
	void					LoadDraw			();
	void					LoadForceFinish		();

	virtual	void			OnEvent				(EVENT E, u64 P1, u64 P2);

	// Other
							CApplication		();
	virtual					~CApplication		();

	virtual void	_BCL	OnFrame				();
			void			load_draw_internal	();
			void			SetLoadingScreen	(ILoadingScreen* newScreen);
			void			DestroyLoadingScreen();
};

extern ENGINE_API	CApplication*	pApp;

#endif //__XR_BASE_H__
