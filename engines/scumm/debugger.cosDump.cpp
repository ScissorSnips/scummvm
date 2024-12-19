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
bool ScummDebugger::Cmd_Cosdump(int argc, const char **argv) {
	const byte *akos;
	const byte *aksq;
	uint32 curState;
	uint32 code;
	uint32 aend;
	int costume;
	int count;
	int i;

	if (argc < 2) {
		debugPrintf("Syntax: cosdump <num>\n");
		return true;
	}

	costume = atoi(argv[1]);
	if (costume >= _vm->_numCostumes) {
		debugPrintf("Costume %d is out of range (range: 1 - %d)\n", costume, _vm->_numCostumes);
		return true;
	}

	akos = _vm->getResourceAddress(rtCostume, costume);

	curState = 0;
	aksq = _vm->findResourceData(MKTAG('A','K','S','Q'), akos);
	if (aksq == nullptr) {
		debugPrintf("Costume %d does not have AKSQ block\n", costume);
		return true;
	}
	aend = READ_BE_UINT32(aksq - 4) - 8;
	debugPrintf("DUMP COSTUME SCRIPT %d (size %d)\n", costume, aend);
	while (curState < aend) {
		code = GB(0);
		if (code & 0x80)
			code = READ_BE_UINT16(aksq + curState);
		debugPrintf("[%04x] (%04x) ", curState, code);
		switch (code) {
		case AKC_EmptyCel:
			debugPrintf("RETURN\n");
			curState += 2;
			break;
		case AKC_SetVar:
			debugPrintf("VAR[%d] = %d\n", GB(4), GW(2));
			curState += 5;
			break;
		case AKC_StartSound:
			debugPrintf("START SOUND %d\n", GB(2));
			curState += 3;
			break;
		case AKC_IfSoundInVarRunningGoTo:
			debugPrintf("IF SOUND RUNNING VAR[%d] GOTO [%04x]\n", GB(4), GUW(2));
			curState += 5;
			break;
		case AKC_IfNotSoundInVarRunningGoTo:
			debugPrintf("IF NOT SOUND RUNNING VAR[%d] GOTO [%04x]\n", GB(4), GUW(2));
			curState += 5;
			break;
		case AKC_IfSoundRunningGoTo:
			debugPrintf("IF SOUND RUNNING %d GOTO [%04x]\n", GB(4), GUW(2));
			curState += 5;
			break;
		case AKC_IfNotSoundRunningGoTo:
			debugPrintf("IF NOT SOUND RUNNING %d GOTO [%04x]\n", GB(4), GUW(2));
			curState += 5;
			break;
		case AKC_DrawMany:
			debugPrintf("DRAW:\n");
			curState += 2;
			count = GB(0);
			curState++;
			for (i = 0; i < count; i++) {
				code = GB(4);
				if (code & 0x80) {
					code = READ_BE_UINT16(aksq + curState + 4);
					debugPrintf("\tEXTENDED OFFSET %d POS %d,%d\n", code, GW(0), GW(2));
					curState++;
				} else {
					debugPrintf("\tOFFSET %d POS %d,%d\n", code, GW(0), GW(2));
				}
				curState += 5;
			}
			break;
		case AKC_CondDrawMany:
			debugPrintf("CONDITION MASK DRAW [%04x] [", curState + GB(2));
			count = GB(3);
			for (i = 0; i < count; i++) {
				if (i)
					debugPrintf(", ");
				debugPrintf("%d", GB(4));
				curState++;
			}
			debugPrintf("]\n");
			curState += 4;
			count = GB(0);
			curState++;
			for (i = 0; i < count; i++) {
				code = GB(4);
				if (code & 0x80) {
					code = READ_BE_UINT16(aksq + curState + 4);
					debugPrintf("\tEXTENDED OFFSET %d POS %d,%d\n", code, GW(0), GW(2));
					curState++;
				} else {
					debugPrintf("\tOFFSET %d POS %d,%d\n", code, GW(0), GW(2));
				}
				curState += 5;
			}
			break;
		case AKC_CondRelativeOffsetDrawMany:
			debugPrintf("CONDITION MASK DRAW [%04x] [", curState + GB(2));
			count = GB(3);
			for (i = 0; i < count; i++) {
				if (i)
					debugPrintf(", ");
				debugPrintf("%d", GB(4));
				curState++;
			}
			debugPrintf("] AT OFFSET %d, %d:\n", GW(2), GW(4));
			curState += 6;
			count = GB(0);
			curState++;
			for (i = 0; i < count; i++) {
				code = GB(4);
				if (code & 0x80) {
					code = READ_BE_UINT16(aksq + curState + 4);
					debugPrintf("\tEXTENDED OFFSET %d POS %d,%d\n", code, GW(0), GW(2));
					curState++;
				} else {
					debugPrintf("\tOFFSET %d POS %d,%d\n", code, GW(0), GW(2));
				}
				curState += 5;
			}
			break;
		case AKC_RelativeOffsetDrawMany:
			debugPrintf("DRAW AT OFFSET %d, %d:\n", GW(2), GW(4));
			curState += 6;
			count = GB(0);
			curState++;
			for (i = 0; i < count; i++) {
				code = GB(4);
				if (code & 0x80) {
					code = READ_BE_UINT16(aksq + curState + 4);
					debugPrintf("\tEXTENDED OFFSET %d POS %d,%d\n", code, GW(0), GW(2));
					curState++;
				} else {
					debugPrintf("\tOFFSET %d POS %d,%d\n", code, GW(0), GW(2));
				}
				curState += 5;
			}
			break;
		case AKC_GoToState:
			debugPrintf("GOTO [%04x]\n", GUW(2));
			curState += 4;
			break;
		case AKC_IfVarGoTo:
			debugPrintf("IF VAR[%d] GOTO [%04x]\n", GB(4), GUW(2));
			curState += 5;
			break;
		case AKC_AddVar:
			debugPrintf("VAR[%d] += %d\n", GB(4), GW(2));
			curState += 5;
			break;
		case AKC_SoftSound:
			debugPrintf("START SOUND %d SOFT\n", GB(2));
			curState += 3;
			break;
		case AKC_SoftVarSound:
			debugPrintf("START SOUND VAR[%d] SOFT\n", GB(2));
			curState += 3;
			break;
		case AKC_SetUserCondition:
			debugPrintf("USER CONDITION %d = VAR[%d] GOTO [%04x] \n", GB(3), GB(4), GB(2));
			curState += 5;
			break;
		case AKC_SetVarToUserCondition:
			debugPrintf("VAR[%d] = USER CONDITION %d GOTO [%04x] \n", GB(4), GB(3), GB(2));
			curState += 5;
			break;
		case AKC_SetTalkCondition:
			debugPrintf("TALK CONDITION %d SET GOTO [%04x] \n", GB(3), GB(2));
			curState += 4;
			break;
		case AKC_SetVarToTalkCondition:
			debugPrintf("VAR[%d] = TALK CONDITION %d GOTO [%04x] \n", GB(4), GB(3), GB(2));
			curState += 5;
			break;
		case AKC_StartScript:
			debugPrintf("IGNORE %d\n", GB(2));
			curState += 3;
			break;
		case AKC_IncVar:
			debugPrintf("VAR[0]++\n");
			curState += 2;
			break;
		case AKC_StartSound_SpecialCase:
			debugPrintf("START SOUND QUICK\n");
			curState += 2;
			break;
		case AKC_IfVarEQJump:
			debugPrintf("IF VAR[%d] == %d GOTO [%04x]\n", GB(4), GW(5), GUW(2));
			curState += 7;
			break;
		case AKC_IfVarNEJump:
			debugPrintf("IF VAR[%d] != %d GOTO [%04x]\n", GB(4), GW(5), GUW(2));
			curState += 7;
			break;
		case AKC_IfVarLTJump:
			debugPrintf("IF VAR[%d] < %d GOTO [%04x]\n", GB(4), GW(5), GUW(2));
			curState += 7;
			break;
		case AKC_IfVarLEJump:
			debugPrintf("IF VAR[%d] <= %d GOTO [%04x]\n", GB(4), GW(5), GUW(2));
			curState += 7;
			break;
		case AKC_IfVarGTJump:
			debugPrintf("IF VAR[%d] > %d GOTO [%04x]\n", GB(4), GW(5), GUW(2));
			curState += 7;
			break;
		case AKC_IfVarGEJump:
			debugPrintf("IF VAR[%d] >= %d GOTO [%04x]\n", GB(4), GW(5), GUW(2));
			curState += 7;
			break;
		case AKC_StartAnim:
			debugPrintf("START ANIMATION %d\n", GB(2));
			curState += 3;
			break;
		case AKC_StartVarAnim:
			debugPrintf("START ANIMATION VAR[%d]\n", GB(2));
			curState += 3;
			break;
		case AKC_SetVarRandom:
			debugPrintf("VAR[%d] = RANDOM BETWEEN %d AND %d\n", GB(6), GW(2), GW(4));
			curState += 7;
			break;
		case AKC_SetActorZClipping:
			debugPrintf("ZCLIP %d\n", GB(2));
			curState += 3;
			break;
		case AKC_StartActorAnim:
			debugPrintf("START ANIMATION ACTOR VAR[%d] VAR[%d]\n", GB(2), GB(3));
			curState += 4;
			break;
		case AKC_SetActorVar:
			debugPrintf("ACTOR VAR[%d] VAR[%d] = %d\n", GB(2), GB(3), GW(4));
			curState += 6;
			break;
		case AKC_HideActor:
			debugPrintf("DESTROY ACTOR\n");
			curState += 2;
			break;
		case AKC_SetDrawOffs:
			debugPrintf("SET DRAW OFFSETS %d %d\n", GW(2), GW(4));
			curState += 6;
			break;
		case AKC_JumpToOffsetInVar:
			debugPrintf("GOTO OFFSET AT VAR[%d]\n", GB(2));
			curState += 3;
			break;
		// case AKC_SoundStuff:
		//	break;
		// case AKC_Flip:
		//	break;
		// case AKC_StartActionOn:
		//	break;
		// case AKC_StartScriptVar:
		//	break;
		case AKC_StartSoundVar:
			debugPrintf("START SOUND VAR[%d]\n", GB(2));
			curState += 3;
			break;
		// case AKC_DisplayAuxFrame:
		//	break;
		// case AKC_IfVarEQDo:
		//	break;
		// case AKC_SkipNE:
		//	break;
		// case AKC_IfVarLTDo:
		//	break;
		// case AKC_IfVarLEDo:
		//	break;
		// case AKC_IfVarGTDo:
		//	break;
		// case AKC_IfVarGEDo:
		//	break;
		// case AKC_EndOfIfDo:
		//	break;
		case AKC_StartActorTalkie:
			debugPrintf("START TALK %d {%d}\n", GB(2), GB(3));
			curState += 4;
			break;
		case AKC_IfTalkingGoTo:
			debugPrintf("IF ACTOR TALKING GOTO [%04x]\n", GUW(2));
			curState += 4;
			break;
		case AKC_IfNotTalkingGoTo:
			debugPrintf("IF NOT ACTOR TALKING GOTO [%04x]\n", GUW(2));
			curState += 4;
			break;
		case AKC_StartTalkieInVar:
			debugPrintf("START TALK VAR[%d]\n", GB(2));
			curState += 3;
			break;
		// case AKC_IfAnyTalkingGoTo:
		//	break;
		// case AKC_IfNotAnyTalkingGoTo:
		//	break;
		// case AKC_IfTalkingPickGoTo:
		//	break;
		// case AKC_IfNotTalkingPickGoTo:
		//	break;
		case AKC_EndSeq:
			debugPrintf("STOP\n");
			curState += 2;
			break;
		default:
			warning("DEFAULT OP, breaking...\n");
			return true;
			break;
		}
	}

	return true;
}
}