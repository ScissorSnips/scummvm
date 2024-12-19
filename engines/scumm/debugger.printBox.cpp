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
bool ScummDebugger::Cmd_PrintBox(int argc, const char **argv) {
	int num, i = 0;

	if (argc > 1) {
		for (i = 1; i < argc; i++)
			printBox(atoi(argv[i]));
	} else {
		num = _vm->getNumBoxes();
		debugPrintf("\nWalk boxes:\n");
		for (i = 0; i < num; i++)
			printBox(i);
	}
	return true;
}
}