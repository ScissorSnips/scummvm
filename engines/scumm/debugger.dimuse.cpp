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
#if defined(ENABLE_SCUMM_7_8)
bool ScummDebugger::Cmd_DiMuse(int argc, const char **argv) {
	if (!_vm->_imuseDigital || _vm->_imuseDigital->isEngineDisabled()) {
		debugPrintf("No Digital iMUSE engine is active.\n");
		return true;
	}

	if (argc > 1) {
		if (!strcmp(argv[1], "stop")) {
			if (argc > 2 && (!strcmp(argv[2], "all") || atoi(argv[2]) != 0)) {
				if (!strcmp(argv[2], "all")) {
					_vm->_imuseDigital->diMUSEStopAllSounds();
					debugPrintf("Stopping all sounds.\n");
				} else {
					_vm->_imuseDigital->diMUSEStopSound(atoi(argv[2]));
					debugPrintf("Attempted to stop sound %d.\n", atoi(argv[2]));
				}
			} else {
				debugPrintf("Specify a soundId or \"all\".\n");
			}
			return true;
		} else if (!strcmp(argv[1], "stopSpeech")) {
			debugPrintf("Attempting to stop the currently playing speech file, if any.\n");
			_vm->_imuseDigital->diMUSEStopSound(kTalkSoundID);
			return true;
		} else if (!strcmp(argv[1], "list") || !strcmp(argv[1], "tracks")) {
			_vm->_imuseDigital->listTracks();
			return true;
		} else if (!strcmp(argv[1], "playSfx")) {
			if (argc > 2 && atoi(argv[2]) != 0 && atoi(argv[2]) <= _vm->_numSounds) {
				debugPrintf("Attempting to play SFX %d...\n", atoi(argv[2]));
				_vm->_imuseDigital->diMUSEStartSound(atoi(argv[2]), 126);
			} else {
				debugPrintf("Specify a SFX soundId from 0-%d.\n", _vm->_numSounds - 1);
			}
			return true;
		} else if (!strcmp(argv[1], "playState") || !strcmp(argv[1], "setState")) {
			if (argc > 2 && atoi(argv[2]) >= 0) {
				debugPrintf("Attempting to play state %d...\n", atoi(argv[2]));
				_vm->_imuseDigital->diMUSESetState(atoi(argv[2]));
			} else {
				debugPrintf("Specify a valid stateId; available states for this game:\n");
				_vm->_imuseDigital->listStates();
			}
			return true;
		} else if (!strcmp(argv[1], "playSeq") || !strcmp(argv[1], "setSeq")) {
			if (argc > 2 && atoi(argv[2]) >= 0) {
				debugPrintf("Attempting to play sequence %d...\n", atoi(argv[2]));
				_vm->_imuseDigital->diMUSESetSequence(atoi(argv[2]));
			} else {
				debugPrintf("Specify a valid seqId; available sequences for this game:\n");
				_vm->_imuseDigital->listSeqs();
			}
			return true;
		} else if (!strcmp(argv[1], "playCue") || !strcmp(argv[1], "setCue")) {
			if (_vm->_game.id != GID_FT || (_vm->_game.features & GF_DEMO)) {
				debugPrintf("Cues are only available for Full Throttle (full version).\n");
			} else {
				if (argc > 2 && atoi(argv[2]) >= 0 && atoi(argv[2]) < 4) {
					debugPrintf("Attempting to play cue %d...\n", atoi(argv[2]));
					_vm->_imuseDigital->diMUSESetCuePoint(atoi(argv[2]));
				} else {
					debugPrintf("Specify a valid cueId; available sequences for this game:\n");
					_vm->_imuseDigital->listCues();
				}
			}
			return true;
		} else if (!strcmp(argv[1], "hook")) {
			if (argc > 3 && atoi(argv[3]) != 0) {
				debugPrintf("Attempting to set hookId %d for sound %d...\n", atoi(argv[2]), atoi(argv[3]));
				_vm->_imuseDigital->diMUSESetHook(atoi(argv[3]), atoi(argv[2]));
			} else {
				debugPrintf("Specify a hookId and a soundId;\nuse \"list\" to get a list of currently playing sounds.\n");
			}
			return true;
		} else if (!strcmp(argv[1], "states")) {
			debugPrintf("Available states for this game:\n");
			if (_vm->_imuseDigital->isFTSoundEngine() && _vm->_game.features & GF_DEMO) {
				debugPrintf("  No states available for demo game with id %s.\n", _vm->_game.gameid);
			} else {
				_vm->_imuseDigital->listStates();
			}
			return true;
		} else if (!strcmp(argv[1], "seqs")) {
			debugPrintf("Available sequences for this game:\n");
			if (_vm->_game.features & GF_DEMO) {
				debugPrintf("  No sequences available for demo game with id %s.\n", _vm->_game.gameid);
			} else {
				_vm->_imuseDigital->listSeqs();
			}
			return true;
		} else if (!strcmp(argv[1], "cues")) {
			debugPrintf("Available cues for this game:\n");
			if (_vm->_game.id == GID_FT && !(_vm->_game.features & GF_DEMO)) {
				_vm->_imuseDigital->listCues();
			} else {
				debugPrintf("  No cues available for game with id %s.\n", _vm->_game.gameid);
			}
			return true;
		} else if (!strcmp(argv[1], "groups") || !strcmp(argv[1], "vols")) {
			_vm->_imuseDigital->listGroups();
			return true;
		} else if (!strcmp(argv[1], "getParam")) {
			if (argc > 3) {
				int result = _vm->_imuseDigital->diMUSEGetParam(atoi(argv[2]), strtol(argv[3], NULL, 16));
				if (result != -5 && result != -4 && result != -1) {
					debugPrintf("Parameter value for sound %d: %d\n", atoi(argv[2]), result);
					return true;
				}
				debugPrintf("Invalid parameter id or soundId.\n");
			}
			debugPrintf("Usage: getParam <soundId> <param>.\nReadable params (use the hex id):\n");
			debugPrintf("\tP_SND_TRACK_NUM  0x100 \n");
			debugPrintf("\tP_MARKER         0x300 \n");
			debugPrintf("\tP_GROUP          0x400 \n");
			debugPrintf("\tP_PRIORITY       0x500 \n");
			debugPrintf("\tP_VOLUME         0x600 \n");
			debugPrintf("\tP_PAN            0x700 \n");
			debugPrintf("\tP_DETUNE         0x800 \n");
			debugPrintf("\tP_TRANSPOSE      0x900 \n");
			debugPrintf("\tP_MAILBOX        0xA00 \n");
			debugPrintf("\tP_SND_HAS_STREAM 0x1800\n");
			debugPrintf("\tP_STREAM_BUFID   0x1900\n");
			debugPrintf("\tP_SND_POS_IN_MS  0x1A00\n");
			return true;
		} else if (!strcmp(argv[1], "setParam")) {
			if (argc > 4) {
				int result = _vm->_imuseDigital->diMUSESetParam(atoi(argv[2]), strtol(argv[3], NULL, 16), atoi(argv[4]));
				if (result != -5)
					return true;

				debugPrintf("Invalid parameter id, value or soundId.\n");
			}
			debugPrintf("Usage: setParam <soundId> <param> <val>.\nWritable params (use the hex id):\n");
			debugPrintf("\tP_GROUP          0x400 \n");
			debugPrintf("\tP_PRIORITY       0x500 \n");
			debugPrintf("\tP_VOLUME         0x600 \n");
			debugPrintf("\tP_PAN            0x700 \n");
			debugPrintf("\tP_DETUNE         0x800 \n");
			debugPrintf("\tP_TRANSPOSE      0x900 \n");
			debugPrintf("\tP_MAILBOX        0xA00 \n");
			debugPrintf("Please note that editing values for some parameters might lead to unexpected behavior.\n\n");
			return true;
		}

		debugPrintf("Unknown command. ");
	}

	debugPrintf("Available Digital iMUSE commands:\n");
	debugPrintf("\tstates                           - Display music states available for the current game\n");
	debugPrintf("\tseqs                             - Display music sequences available for the current game\n");
	debugPrintf("\tcues                             - Display music cues available for the current sequence (FT only)\n");
	debugPrintf("\tplaySfx <soundId>                - Play a SFX resource by soundId\n");
	debugPrintf("\tplayState|setState <stateId>     - Play a music state resource by soundId\n");
	debugPrintf("\tplaySeq|setSeq <seqId>           - Play a music sequence resource by soundId\n");
	debugPrintf("\tplayCue|setCue <cueId>           - Play a music cue between the ones available (FT only)\n");
	debugPrintf("\tstop <soundId>|all               - Stop a SFX, speech or music resource by soundId\n");
	debugPrintf("\tstopSpeech                       - Stop the current speech file, if any\n");
	debugPrintf("\thook <soundId> <hookId>          - Set hookId for a sound\n");
	debugPrintf("\tlist|tracks                      - Display info for every virtual audio track\n");
	debugPrintf("\tgroups|vols                      - Show volume groups info\n");
	debugPrintf("\tgetParam <soundId> <param>       - Get parameter info from a sound\n");
	debugPrintf("\tsetParam <soundId> <param> <val> - Set parameter value for a sound (dangerous!)\n");
	debugPrintf("\n");

	return true;
}

#endif
}