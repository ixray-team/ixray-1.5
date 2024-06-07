////////////////////////////////////////////////////////////////////////////
//	Module 		: actor_script.cpp
//	Created 	: 17.01.2008
//  Modified 	: 17.01.2008
//	Author		: Dmitriy Iassenev
//	Description : actor script export
////////////////////////////////////////////////////////////////////////////

#include "pch_script.h"
#include "actor.h"
#include "level_changer.h"

using namespace luabind;

#pragma optimize("s",on)
void CActor::script_register(lua_State *L)
{
	module(L)
	[
		class_<CActor,CGameObject>("CActor")
			.def(constructor<>()),

		class_<CLevelChanger,CGameObject>("CLevelChanger")
			.def(constructor<>()),

			class_<enum_exporter<EMovementStates>>("EMovementStates").enum_("emovementstates")
			[
				value("eOld", eOld),
					value("eWishful", eWishful),
					value("eReal", eReal)
			],

			class_<enum_exporter<EMoveCommand>>("EMoveCommand").enum_("emovecommand")
			[
				value("mcFwd", mcFwd),
					value("mcBack", mcBack),
					value("mcLStrafe", mcLStrafe),
					value("mcRStrafe", mcRStrafe),
					value("mcCrouch", mcCrouch),
					value("mcAccel", mcAccel),
					value("mcTurn", mcTurn),
					value("mcJump", mcJump),
					value("mcFall", mcFall),
					value("mcLanding", mcLanding),
					value("mcLanding2", mcLanding2),
					value("mcClimb", mcClimb),
					value("mcSprint", mcSprint),
					value("mcLLookout", mcLLookout),
					value("mcRLookout", mcRLookout),
					value("mcAnyMove", mcAnyMove),
					value("mcAnyAction", mcAnyAction),
					value("mcAnyState", mcAnyState),
					value("mcLookout", mcLookout)
			]
	];
}
