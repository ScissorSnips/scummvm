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
bool ScummDebugger::Cmd_PrintBoxMatrix(int argc, const char **argv) {
	byte *boxm = _vm->getBoxMatrixBaseAddr();
	int num = _vm->getNumBoxes();
	int i, j;

	debugPrintf("Walk matrix:\n");
	if (_vm->_game.version <= 2)
		boxm += num;
	for (i = 0; i < num; i++) {
		debugPrintf("%d: ", i);
		if (_vm->_game.version <= 2) {
			for (j = 0; j < num; j++)
				debugPrintf("[%d] ", *boxm++);
		} else {
			while (*boxm != 0xFF) {
				debugPrintf("[%d-%d=>%d] ", boxm[0], boxm[1], boxm[2]);
				boxm += 3;
			}
			boxm++;
		}
		debugPrintf("\n");
	}
	return true;
}
}