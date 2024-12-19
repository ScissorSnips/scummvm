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
bool ScummDebugger::Cmd_Room(int argc, const char **argv) {
	if (argc > 1) {
		int room = atoi(argv[1]);
		_vm->_actors[_vm->VAR(_vm->VAR_EGO)]->_room = room;
		_vm->_sound->stopAllSounds();
		_vm->startScene(room, nullptr, 0);
		_vm->_fullRedraw = true;
		return false;
	} else {
		debugPrintf("Current room: %d [%d] - use 'room <roomnum>' to switch\n", _vm->_currentRoom, _vm->_roomResource);
		return true;
	}
}
}