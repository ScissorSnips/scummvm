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
bool ScummDebugger::Cmd_ImportRes(int argc, const char** argv) {
	Common::File file;
	uint32 size;
	int resnum;

	if (argc != 4) {
		debugPrintf("Syntax: importres <restype> <filename> <resnum>\n");
		return true;
	}

	resnum = atoi(argv[3]);
	// FIXME add bounds check

	if (!strncmp(argv[1], "scr", 3)) {
		file.open(argv[2]);
		if (file.isOpen() == false) {
			debugPrintf("Could not open file %s\n", argv[2]);
			return true;
		}
		if (_vm->_game.features & GF_SMALL_HEADER) {
			size = file.readUint16LE();
			file.seek(-2, SEEK_CUR);
#if 0
		// FIXME: This never was executed due to duplicated if condition
		} else if (_vm->_game.features & GF_SMALL_HEADER) {
			if (_vm->_game.version == 4)
				file.seek(8, SEEK_CUR);
			size = file.readUint32LE();
			file.readUint16LE();
			file.seek(-6, SEEK_CUR);
#endif
		} else {
			file.readUint32BE();
			size = file.readUint32BE();
			file.seek(-8, SEEK_CUR);
		}

		file.read(_vm->_res->createResource(rtScript, resnum, size), size);

	} else
		debugPrintf("Unknown importres type '%s'\n", argv[1]);
	return true;
}
}