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
bool ScummDebugger::Cmd_Show(int argc, const char **argv) {

	if (argc != 2) {
		debugPrintf("Syntax: show <parameter>\n");
		return true;
	}

	if (!strcmp(argv[1], "hex")) {
		_vm->_hexdumpScripts = true;
		debugPrintf("Script hex dumping on\n");
	} else if (!strncmp(argv[1], "sta", 3)) {
		_vm->_showStack = 1;
		debugPrintf("Stack tracing on\n");
	} else {
		debugPrintf("Unknown show parameter '%s'\nParameters are 'hex' for hex dumping and 'sta' for stack tracing\n", argv[1]);
	}
	return true;
}
}