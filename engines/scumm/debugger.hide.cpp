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
bool ScummDebugger::Cmd_Hide(int argc, const char **argv) {

	if (argc != 2) {
		debugPrintf("Syntax: hide <parameter>\n");
		return true;
	}

	if (!strcmp(argv[1], "hex")) {
		_vm->_hexdumpScripts = false;
		debugPrintf("Script hex dumping off\n");
	} else if (!strncmp(argv[1], "sta", 3)) {
		_vm->_showStack = 0;
		debugPrintf("Stack tracing off\n");
	} else {
		debugPrintf("Unknown hide parameter '%s'\nParameters are 'hex' to turn off hex dumping and 'sta' to turn off stack tracing\n", argv[1]);
	}
	return true;
}
}