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
bool ScummDebugger::Cmd_IMuse(int argc, const char **argv) {
	if (!_vm->_imuse && !_vm->_musicEngine) {
		debugPrintf("No iMuse engine is active.\n");
		return true;
	}

	if (argc > 1) {
		if (!strcmp(argv[1], "panic")) {
			_vm->_musicEngine->stopAllSounds();
			debugPrintf("AAAIIIEEEEEE!\n");
			debugPrintf("Shutting down all music tracks\n");
			return true;
		} else if (!strcmp(argv[1], "play")) {
			if (argc > 2 && (!strcmp(argv[2], "random") || atoi(argv[2]) != 0)) {
				int sound = atoi(argv[2]);
				if (!strcmp(argv[2], "random")) {
					debugPrintf("Selecting from %d songs...\n", _vm->_numSounds);
					sound = _vm->_rnd.getRandomNumber(_vm->_numSounds);
				}
				if (_vm->getResourceAddress(rtSound, sound))
					_vm->_musicEngine->startSound(sound);

				debugPrintf("Attempted to start music %d.\n", sound);
			} else {
				debugPrintf("Specify a music resource # from 1-255.\n");
			}
			return true;
		} else if (!strcmp(argv[1], "stop")) {
			if (argc > 2 && (!strcmp(argv[2], "all") || atoi(argv[2]) != 0)) {
				if (!strcmp(argv[2], "all")) {
					_vm->_musicEngine->stopAllSounds();
					debugPrintf("Shutting down all music tracks.\n");
				} else {
					_vm->_musicEngine->stopSound(atoi(argv[2]));
					debugPrintf("Attempted to stop music %d.\n", atoi(argv[2]));
				}
			} else {
				debugPrintf("Specify a music resource # or \"all\".\n");
			}
			return true;
		}
	}

	debugPrintf("Available iMuse commands:\n");
	debugPrintf("  panic - Stop all music tracks\n");
	debugPrintf("  play # - Play a music resource\n");
	debugPrintf("  stop # - Stop a music resource\n");
	return true;
}
}