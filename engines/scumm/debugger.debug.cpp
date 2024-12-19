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
bool ScummDebugger::Cmd_Debug(int argc, const char **argv) {
	const Common::DebugManager::DebugChannelList &lvls = DebugMan.getDebugChannels();

	// No parameters given: Print out a list of all channels and their status
	if (argc <= 1) {
		debugPrintf("Available debug channels:\n");
		for (Common::DebugManager::DebugChannelList::const_iterator i = lvls.begin(); i != lvls.end(); ++i) {
			bool enabled = DebugMan.isDebugChannelEnabled(i->channel);

			debugPrintf("%c%s - %s (%s)\n", enabled ? '+' : ' ',
					i->name.c_str(), i->description.c_str(),
					enabled ? "enabled" : "disabled");
		}
		return true;
	}

	// Enable or disable channel?
	bool result = false;
	if (argv[1][0] == '+') {
		result = DebugMan.enableDebugChannel(argv[1] + 1);
	} else if (argv[1][0] == '-') {
		result = DebugMan.disableDebugChannel(argv[1] + 1);
	}

	if (result) {
		debugPrintf("%s %s\n", (argv[1][0] == '+') ? "Enabled" : "Disabled", argv[1] + 1);
	} else {
		debugPrintf("Usage: debug [+CHANNEL|-CHANNEL]\n");
		debugPrintf("Enables or disables the given debug channel.\n");
		debugPrintf("When used without parameters, lists all available debug channels and their status.\n");
	}

	return true;
}
}