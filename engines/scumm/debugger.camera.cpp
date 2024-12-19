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
bool ScummDebugger::Cmd_Camera(int argc, const char **argv) {
	debugPrintf("Camera: cur (%d,%d) - dest (%d,%d) - accel (%d,%d) -- last (%d,%d)\n",
		_vm->camera._cur.x, _vm->camera._cur.y, _vm->camera._dest.x, _vm->camera._dest.y,
		_vm->camera._accel.x, _vm->camera._accel.y, _vm->camera._last.x, _vm->camera._last.y);

	return true;
}
}