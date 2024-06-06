#pragma once
#include "weaponcustompistol.h"

class CWeaponPistol :
	public CWeaponCustomPistol
{
	typedef CWeaponCustomPistol inherited;
public:
					CWeaponPistol	(){};
	virtual			~CWeaponPistol	(){};
protected:	
	virtual bool	AllowFireWhileWorking() {return true;}
};
