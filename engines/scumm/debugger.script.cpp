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
bool ScummDebugger::Cmd_Script(int argc, const char** argv) {
	int scriptnum;

	if (argc < 3) {
		debugPrintf("Syntax: script <scriptnum> <command>\n");
		return true;
	}

	scriptnum = atoi(argv[1]);

	// FIXME: what is the max range on these?
	// if (scriptnum >= _vm->_numScripts) {
	//	debugPrintf("Script number %d is out of range (range: 1 - %d)\n", scriptnum, _vm->_numScripts);
	//	return true;
	//}

	if ((!strcmp(argv[2], "kill")) || (!strcmp(argv[2], "stop"))) {
		_vm->stopScript(scriptnum);
	} else if ((!strcmp(argv[2], "run")) || (!strcmp(argv[2], "start"))) {
		_vm->runScript(scriptnum, 0, 0, nullptr);
		return false;
	} else {
		debugPrintf("Unknown script command '%s'\nUse <kill/stop | run/start> as command\n", argv[2]);
	}

	return true;
}
}