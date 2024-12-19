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
bool ScummDebugger::Cmd_PrintActor(int argc, const char **argv) {
	int i;
	Actor *a;

	debugPrintf("+----------------------------------------------------------------------------+\n");
	debugPrintf("|# |            name            |  x |  y | w | h |elev|cos|box|mov| zp|frm|scl|dir|   cls   |\n");
	debugPrintf("+--+----------------------------+----+----+---+---+----+---+---+---+---+---+---+---+---------+\n");
	for (i = 1; i < _vm->_numActors; i++) {
		a = _vm->_actors[i];
		const byte *name = _vm->getObjOrActorName(_vm->actorToObj(a->_number));
		if (!name)
			name = (const byte *)"(null)";
		if (a->_visible)
			debugPrintf("|%2d|%-24.24s|%4d|%4d|%3d|%3d|%4d|%3d|%3d|%3d|%3d|%3d|%3d|%3d|$%08x|\n",
						 a->_number, name, a->getRealPos().x, a->getRealPos().y, a->_width,  a->_bottom - a->_top,
						 a->getElevation(),
						 a->_costume, a->_walkbox, a->_moving, a->_forceClip, a->_frame,
						 a->_scalex, a->getFacing(), _vm->_classData[a->_number]);
	}
	debugPrintf("\n");
	return true;
}
}