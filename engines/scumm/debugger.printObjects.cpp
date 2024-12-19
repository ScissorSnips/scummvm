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
    bool ScummDebugger::Cmd_PrintObjects(int argc, const char **argv) {
        int i;
        ObjectData *o;
        debugPrintf("Objects in current room\n");
        debugPrintf("+-------------------------------------------------------------------------------+\n");
        debugPrintf("|num |    name                            |  x |  y |width|height|state|fl|   cls   | obimoff | obcdoff |\n");
        debugPrintf("+----+------------+----+----+-----+------+-----+--+---------+---------+---------+\n");

        for (i = 1; i < _vm->_numLocalObjects; i++) {
            o = &(_vm->_objs[i]);
            if (o->obj_nr == 0)
                continue;
            int classData = (_vm->_game.version != 0 ? _vm->_classData[o->obj_nr] : 0);
            const byte *name = _vm->getObjOrActorName(o->obj_nr);
            if (!name)
                name = (const byte *)"(null)";
            debugPrintf("|%4d|%-36.36s|%4d|%4d|%5d|%6d|%5d|%2d|$%08x|$%08x|$%08x|\n",
                    o->obj_nr, name, o->x_pos, o->y_pos, o->width, o->height, o->state,
                    o->fl_object_index, classData, o->OBIMoffset, o->OBCDoffset);
            drawRect(o->x_pos, o->y_pos, o->width, o->height, getNextColor());
        }
        debugPrintf("\n");

        return true;
    }
}