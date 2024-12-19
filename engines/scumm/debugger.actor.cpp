#include "common/debug-channels.h"
#include "common/file.h"
#include "common/str.h"
#include "common/system.h"
#include "common/util.h"

#include "scumm/actor.h"
#include "scumm/boxes.h"
#include "scumm/debugger.h"
#include "scumm/imuse/imuse.h"
#include "scumm/imuse_digi/dimuse_engine.h"
#include "scumm/object.h"
#include "scumm/resource.h"
#include "scumm/scumm.h"
#include "scumm/sound.h"

namespace Scumm {
bool ScummDebugger::Cmd_Actor(int argc, const char **argv) {
	Actor *a;
	int actnum;
	int value = 0, value2 = 0;

	if (argc < 3) {
		debugPrintf("Syntax: actor <actornum> <command> <parameter>\n");
		debugPrintf("Valid commands: animvar|anim|condmask|costume|_elevation|ignoreboxes|name|x|y\n");
		return true;
	}

	actnum = atoi(argv[1]);
	if (actnum >= _vm->_numActors) {
		debugPrintf("Actor %d is out of range (range: 1 - %d)\n", actnum, _vm->_numActors);
		return true;
	}

	a = _vm->_actors[actnum];
	if (argc > 3)
		value = atoi(argv[3]);
	if (argc > 4)
		value2 = atoi(argv[4]);

	if (!strcmp(argv[2], "animvar")) {
		a->setAnimVar(value, value2);
		debugPrintf("Actor[%d].animVar[%d] = %d\n", actnum, value, a->getAnimVar(value));
	} else if (!strcmp(argv[2], "anim")) {
		a->animateActor(value);
		debugPrintf("Actor[%d].animateActor(%d)\n", actnum, value);
	} else if (!strcmp(argv[2], "ignoreboxes")) {
		a->_ignoreBoxes = (value > 0);
		debugPrintf("Actor[%d].ignoreBoxes = %d\n", actnum, a->_ignoreBoxes);
	} else if (!strcmp(argv[2], "x")) {
		a->putActor(value, a->getRealPos().y);
		debugPrintf("Actor[%d].x = %d\n", actnum, a->getRealPos().x);
		_vm->_fullRedraw = true;
	} else if (!strcmp(argv[2], "y")) {
		a->putActor(a->getRealPos().x, value);
		debugPrintf("Actor[%d].y = %d\n", actnum, a->getRealPos().y);
		_vm->_fullRedraw = true;
	} else if (!strcmp(argv[2], "_elevation")) {
		a->setElevation(value);
		debugPrintf("Actor[%d]._elevation = %d\n", actnum, a->getElevation());
		_vm->_fullRedraw = true;
	} else if (!strcmp(argv[2], "costume")) {
		if (value >= (int)_vm->_res->_types[rtCostume].size())
			debugPrintf("Costume not changed as %d exceeds max of %d\n", value, _vm->_res->_types[rtCostume].size());
		else {
			a->setActorCostume(value);
			_vm->_fullRedraw = true;
			debugPrintf("Actor[%d].costume = %d\n", actnum, a->_costume);
		}
	} else if (!strcmp(argv[2], "name")) {
		const byte *name = _vm->getObjOrActorName(_vm->actorToObj(actnum));
		if (!name)
			name = (const byte *)"(null)";
		debugPrintf("Name of actor %d: %s\n", actnum, name);
	} else if (!strcmp(argv[2], "condmask")) {
		if (argc > 3) {
			a->_heCondMask = value;
		}
		debugPrintf("Actor[%d]._heCondMask = 0x%X\n", actnum, a->_heCondMask);
	} else {
		debugPrintf("Unknown actor command '%s'\n", argv[2]);
	}

	return true;

}
}