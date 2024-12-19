/* ScummVM - Graphic Adventure Engine
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

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

#include "scumm/akos.h"

#include "scumm/debugger.actor.cpp"
#include "scumm/debugger.camera.cpp"
#include "scumm/debugger.cosDump.cpp"
#include "scumm/debugger.debug.cpp"
#include "scumm/debugger.dimuse.cpp"
#include "scumm/debugger.hide.cpp"
#include "scumm/debugger.importRes.cpp"
#include "scumm/debugger.imuse.cpp"
#include "scumm/debugger.loadGame.cpp"
#include "scumm/debugger.object.cpp"
#include "scumm/debugger.printActor.cpp"
#include "scumm/debugger.printBox.cpp"
#include "scumm/debugger.printBox2.cpp"
#include "scumm/debugger.printBoxMatrix.cpp"
#include "scumm/debugger.printObjects.cpp"
#include "scumm/debugger.printScript.cpp"
#include "scumm/debugger.restart.cpp"
#include "scumm/debugger.room.cpp"
#include "scumm/debugger.saveGame.cpp"
#include "scumm/debugger.script.cpp"
#include "scumm/debugger.show.cpp"
#include "scumm/debugger.ping.cpp"

namespace Scumm {

void debugC(int channel, const char *s, ...) {
	char buf[STRINGBUFLEN];
	va_list va;

	// FIXME: Still spew all debug at -d9, for crashes in startup etc.
	//	  Add setting from commandline ( / abstract channel interface)
	if (!DebugMan.isDebugChannelEnabled(channel) && (gDebugLevel < 9))
		return;

	va_start(va, s);
	vsnprintf(buf, STRINGBUFLEN, s, va);
	va_end(va);

	debug("%s", buf);
}

ScummDebugger::ScummDebugger(ScummEngine *s)
	: GUI::Debugger() {
	_vm = s;

	// Initialize the debug colors
	for (int i = 0; i < DEBUG_COLOR_COUNT; ++i) {
		_debugColors[i] = 0x01 + i;
	}

	// Register variables
	registerVar("scumm_speed", &_vm->_fastMode);
	registerVar("scumm_room", &_vm->_currentRoom);
	registerVar("scumm_roomresource", &_vm->_roomResource);
	registerVar("scumm_vars", &_vm->_scummVars, _vm->_numVariables);

	// Register commands
	registerCmd("continue",  WRAP_METHOD(ScummDebugger, cmdExit));
	registerCmd("restart",   WRAP_METHOD(ScummDebugger, Cmd_Restart));

	registerCmd("actor",     WRAP_METHOD(ScummDebugger, Cmd_Actor));
	registerCmd("actors",    WRAP_METHOD(ScummDebugger, Cmd_PrintActor));
	registerCmd("box",       WRAP_METHOD(ScummDebugger, Cmd_PrintBox));
	registerCmd("matrix",    WRAP_METHOD(ScummDebugger, Cmd_PrintBoxMatrix));
	registerCmd("camera",    WRAP_METHOD(ScummDebugger, Cmd_Camera));
	registerCmd("room",      WRAP_METHOD(ScummDebugger, Cmd_Room));
	registerCmd("objects",   WRAP_METHOD(ScummDebugger, Cmd_PrintObjects));
	registerCmd("object",    WRAP_METHOD(ScummDebugger, Cmd_Object));
	registerCmd("script",    WRAP_METHOD(ScummDebugger, Cmd_Script));
	registerCmd("scr",       WRAP_METHOD(ScummDebugger, Cmd_Script));
	registerCmd("cosdump",   WRAP_METHOD(ScummDebugger, Cmd_Cosdump));
	registerCmd("scripts",   WRAP_METHOD(ScummDebugger, Cmd_PrintScript));
	registerCmd("importres", WRAP_METHOD(ScummDebugger, Cmd_ImportRes));

	if (_vm->_game.id == GID_LOOM)
		registerCmd("drafts",  WRAP_METHOD(ScummDebugger, Cmd_PrintDraft));
	if (_vm->_game.id == GID_INDY3)
		registerCmd("grail",  WRAP_METHOD(ScummDebugger, Cmd_PrintGrail));
	if (_vm->_game.id == GID_MONKEY && _vm->_game.platform == Common::kPlatformSegaCD)
		registerCmd("passcode",  WRAP_METHOD(ScummDebugger, Cmd_Passcode));

	registerCmd("loadgame",  WRAP_METHOD(ScummDebugger, Cmd_LoadGame));
	registerCmd("savegame",  WRAP_METHOD(ScummDebugger, Cmd_SaveGame));

	registerCmd("debug",     WRAP_METHOD(ScummDebugger, Cmd_Debug));

	registerCmd("show",      WRAP_METHOD(ScummDebugger, Cmd_Show));
	registerCmd("hide",      WRAP_METHOD(ScummDebugger, Cmd_Hide));

	if (_vm->_game.version < 7)
		registerCmd("imuse", WRAP_METHOD(ScummDebugger, Cmd_IMuse));
#if defined(ENABLE_SCUMM_7_8)
	else
		registerCmd("imuse", WRAP_METHOD(ScummDebugger, Cmd_DiMuse));
#endif

	registerCmd("resetcursors",    WRAP_METHOD(ScummDebugger, Cmd_ResetCursors));

	registerCmd("ping",      WRAP_METHOD(ScummDebugger, Cmd_Ping));

}

void ScummDebugger::preEnter() {
}

void ScummDebugger::postEnter() {
	// Runtime debug level change is dealt with by the base class "debuglevel" command
	// but need to ensure that the _debugMode parameter is updated in sync.
	_vm->_debugMode = (gDebugLevel >= 0);
	// Boot params often need debugging switched on to work
	if (_vm->_bootParam)
		_vm->_debugMode = true;
}

void ScummDebugger::onFrame() {
	Debugger::onFrame();
#if defined(ENABLE_SCUMM_7_8)
	if (_vm->_imuseDigital && !_vm->_imuseDigital->isEngineDisabled() && !_vm->isSmushActive()) {
		_vm->_imuseDigital->diMUSEProcessStreams();
	}
#endif
}

static int gfxPrimitivesCompareInt(const void *a, const void *b);


static void hlineColor(ScummEngine *scumm, int x1, int x2, int y, byte color) {
	VirtScreen *vs = &scumm->_virtscr[kMainVirtScreen];
	byte *ptr;

	// Clip y
	y += scumm->_screenTop;
	if (y < 0 || y >= scumm->_screenHeight)
		return;

	if (x2 < x1)
		SWAP(x2, x1);

	// Clip x1 / x2
	const int left = scumm->_screenStartStrip * 8;
	const int right = scumm->_screenEndStrip * 8;
	if (x1 < left)
		x1 = left;
	if (x2 >= right)
		x2 = right - 1;


	ptr = (byte *)vs->getBasePtr(x1, y);

	while (x1++ <= x2) {
		*ptr++ = color;
	}
}

static int gfxPrimitivesCompareInt(const void *a, const void *b) {
	return (*(const int *)a) - (*(const int *)b);
}

static void fillQuad(ScummEngine *scumm, Common::Point v[4], int color) {
	const int N = 4;
	int i;
	int y;
	int miny, maxy;
	Common::Point pt1, pt2;

	int polyInts[N];


	// Determine Y maxima
	miny = maxy = v[0].y;
	for (i = 1; i < N; i++) {
		if (v[i].y < miny) {
			miny = v[i].y;
		} else if (v[i].y > maxy) {
			maxy = v[i].y;
		}
	}

	// Draw, scanning y
	for (y = miny; y <= maxy; y++) {
		int ints = 0;
		for (i = 0; i < N; i++) {
			int ind1 = i;
			int ind2 = (i + 1) % N;
			pt1 = v[ind1];
			pt2 = v[ind2];
			if (pt1.y > pt2.y) {
				SWAP(pt1, pt2);
			}

			if (pt1.y <= y && y <= pt2.y) {
				if (y == pt1.y && y == pt2.y) {
					hlineColor(scumm, pt1.x, pt2.x, y, color);
				} else if ((y >= pt1.y) && (y < pt2.y)) {
					polyInts[ints++] = (y - pt1.y) * (pt2.x - pt1.x) / (pt2.y - pt1.y) + pt1.x;
				} else if ((y == maxy) && (y > pt1.y) && (y <= pt2.y)) {
					polyInts[ints++] = (y - pt1.y) * (pt2.x - pt1.x) / (pt2.y - pt1.y) + pt1.x;
				}
			}
		}
		qsort(polyInts, ints, sizeof(int), gfxPrimitivesCompareInt);

		for (i = 0; i < ints; i += 2) {
			hlineColor(scumm, polyInts[i], polyInts[i + 1], y, color);
		}
	}

	return;
}

void ScummDebugger::drawBox(int box, int color) {
	BoxCoords coords;
	Common::Point r[4];

	coords = _vm->getBoxCoordinates(box);

	r[0] = coords.ul;
	r[1] = coords.ur;
	r[2] = coords.lr;
	r[3] = coords.ll;

	if (_vm->_game.version <= 2) {
		for (int i = 0; i < 4; ++i) {
			r[i].x *= V12_X_MULTIPLIER;
			r[i].y *= V12_Y_MULTIPLIER;
		}
	}

	// TODO - maybe also print the box number inside it?
	fillQuad(_vm, r, color);

	VirtScreen *vs = _vm->findVirtScreen(coords.ul.y);
	if (vs != nullptr)
		_vm->markRectAsDirty(vs->number, 0, vs->w, 0, vs->h);
	_vm->drawDirtyScreenParts();
	_vm->_system->updateScreen();
}

void ScummDebugger::drawRect(int x, int y, int width, int height, int color) {
	Common::Point r[4];
	r[0] = Common::Point(x, y);
	r[1] = Common::Point(x + width, y);
	r[2] = Common::Point(x + width, y + height);
	r[3] = Common::Point(x, y + height);

	fillQuad(_vm, r, color);

	VirtScreen *vs = _vm->findVirtScreen(y);
	if (vs != nullptr)
		_vm->markRectAsDirty(vs->number, 0, vs->w, 0, vs->h);
	_vm->drawDirtyScreenParts();
	_vm->_system->updateScreen();
}

int ScummDebugger::getNextColor() {
	int color = _debugColors[_nextColorIndex++];
	_nextColorIndex %= DEBUG_COLOR_COUNT;
	return color;
}

bool ScummDebugger::Cmd_PrintDraft(int argc, const char **argv) {
	const char *names[] = {
		"Opening",      "Straw Into Gold", "Dyeing",
		"Night Vision",	"Twisting",        "Sleep",
		"Emptying",     "Invisibility",    "Terror",
		"Sharpening",   "Reflection",      "Healing",
		"Silence",      "Shaping",         "Unmaking",
		"Transcendence"
	};

	const char *notes = "cdefgabC";
	int i, base, draft;

	if (_vm->_game.id != GID_LOOM) {
		debugPrintf("Command only works with Loom/LoomCD\n");
		return true;
	}

	// There are 16 drafts, stored from variable 50, 55 or 100 and upwards.
	// Each draft occupies two variables, the first of which contains the
	// notes for the draft and a number of flags.
	//
	// +---+---+---+---+-----+-----+-----+-----+
	// | A | B | C | D | 444 | 333 | 222 | 111 |
	// +---+---+---+---+-----+-----+-----+-----+
	//
	// A   Unknown
	// B   The player has used the draft successfully at least once
	// C   The player has knowledge of the draft
	// D   Unknown
	// 444 The fourth note
	// 333 The third note
	// 222 The second note
	// 111 The first note
	//
	// I don't yet know what the second variable is used for. Possibly to
	// store information on where and/or how the draft can be used. They
	// appear to remain constant throughout the game.

	if (_vm->_game.version == 4 || _vm->_game.platform == Common::kPlatformPCEngine) {
		// DOS CD version / PC-Engine version
		base = 100;
	} else if (_vm->_game.platform == Common::kPlatformMacintosh) {
		// Macintosh version
		base = 55;
	} else {
		// All (?) other versions
		base = 50;
	}

	if (argc == 2) {
		// We had to debug a problem at the end of the game that only
		// happened if you interrupted the intro at a specific point.
		// That made it useful with a command to learn all the drafts
		// and notes.

		if (strcmp(argv[1], "learn") == 0) {
			for (i = 0; i < 16; i++)
				_vm->_scummVars[base + 2 * i] |= 0x2000;
			_vm->_scummVars[base + 72] = 8;

			// In theory, we could run script 18 here to redraw
			// the distaff, but I don't know if that's a safe
			// thing to do.

			debugPrintf("Learned all drafts and notes.\n");
			return true;
		}
	}

	// Probably the most useful command for ordinary use: list the drafts.

	for (i = 0; i < 16; i++) {
		draft = _vm->_scummVars[base + i * 2];
		debugPrintf("%d %-15s %c%c%c%c %c%c\n",
			base + 2 * i,
			names[i],
			notes[draft & 0x0007],
			notes[(draft & 0x0038) >> 3],
			notes[(draft & 0x01c0) >> 6],
			notes[(draft & 0x0e00) >> 9],
			(draft & 0x2000) ? 'K' : ' ',
			(draft & 0x4000) ? 'U' : ' ');
	}

	return true;
}

bool ScummDebugger::Cmd_PrintGrail(int argc, const char **argv) {
	if (_vm->_game.id != GID_INDY3) {
		debugPrintf("Command only works with Indy3\n");
		return true;
	}

	if (_vm->_currentRoom != 86) {
		debugPrintf("Command only works in room 86\n");
		return true;
	}

	const int grailNumber = _vm->_scummVars[253];
	if (grailNumber < 1 || grailNumber > 10) {
		debugPrintf("Couldn't find the Grail number\n");
		return true;
	}

	debugPrintf("Real Grail is Grail #%d\n", grailNumber);

	return true;
}

bool ScummDebugger::Cmd_Passcode(int argc, const char **argv) {
	if (argc > 1) {
		_vm->_bootParam = atoi(argv[1]);
		int args[NUM_SCRIPT_LOCAL];
		memset(args, 0, sizeof(args));
		args[0] = _vm->_bootParam;

		_vm->runScript(61, 0, 0, args);

		if (_vm->_bootParam != _vm->_scummVars[411]){
			debugPrintf("Invalid Passcode\n");
			return true;
		}

		_vm->_bootParam = 0;
		detach();

	} else {
		debugPrintf("Current Passcode is %d \nUse 'passcode <SEGA CD Passcode>'\n",_vm->_scummVars[411]);
		return true;
	}
	return false;
}

bool ScummDebugger::Cmd_ResetCursors(int argc, const char **argv) {
	_vm->resetCursors();
	detach();
	return false;
}

} // End of namespace Scumm
