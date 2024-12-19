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
bool ScummDebugger::Cmd_Object(int argc, const char **argv) {
	int i;
	int obj;

	if (argc < 3) {
		debugPrintf("Syntax: object <objectnum> <command> <parameter>\n");
		return true;
	}

	obj = atoi(argv[1]);
	if (_vm->_game.version != 0 && obj >= _vm->_numGlobalObjects) {
		debugPrintf("Object %d is out of range (range: 1 - %d)\n", obj, _vm->_numGlobalObjects);
		return true;
	}

	if (!strcmp(argv[2], "pickup")) {
		for (i = 0; i < _vm->_numInventory; i++) {
			if (_vm->_inventory[i] == (uint16)obj) {
				_vm->putOwner(obj, _vm->VAR(_vm->VAR_EGO));
				_vm->runInventoryScript(obj);
				return true;
			}
		}

		if (argc == 3)
			_vm->addObjectToInventory(obj, _vm->_currentRoom);
		else
			_vm->addObjectToInventory(obj, atoi(argv[3]));

		_vm->putOwner(obj, _vm->VAR(_vm->VAR_EGO));
		_vm->putClass(obj, kObjectClassUntouchable, 1);
		_vm->putState(obj, 1);
		_vm->markObjectRectAsDirty(obj);
		_vm->clearDrawObjectQueue();
		_vm->runInventoryScript(obj);
	} else if (!strcmp(argv[2], "state")) {
		if (argc == 4) {
			_vm->putState(obj, atoi(argv[3]));
			//is BgNeedsRedraw enough?
			_vm->_bgNeedsRedraw = true;
		} else {
			debugPrintf("State of object %d: %d\n", obj, _vm->getState(obj));
		}
	} else if (!strcmp(argv[2], "name")) {
		const byte *name = _vm->getObjOrActorName(obj);
		if (!name)
			name = (const byte *)"(null)";
		debugPrintf("Name of object %d: %s\n", obj, name);
	} else {
		debugPrintf("Unknown object command '%s'\nUse <pickup | state | name> as command\n", argv[2]);
	}

	return true;
}
}