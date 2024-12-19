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
void ScummDebugger::printBox(int box) {
	if (box < 0 || box >= _vm->getNumBoxes()) {
		debugPrintf("%d is not a valid box!\n", box);
		return;
	}
	BoxCoords coords;
	int flags = _vm->getBoxFlags(box);
	int mask = _vm->getMaskFromBox(box);
	int scale = _vm->getBoxScale(box);

	coords = _vm->getBoxCoordinates(box);

	// Print out coords, flags, zbuffer mask
	debugPrintf("%d: [%d x %d] [%d x %d] [%d x %d] [%d x %d], flags=0x%02x, mask=%d, scale=%d\n",
								box,
								coords.ul.x, coords.ul.y, coords.ll.x, coords.ll.y,
								coords.ur.x, coords.ur.y, coords.lr.x, coords.lr.y,
								flags, mask, scale);

	// Draw the box
	drawBox(box, getNextColor());
}
}