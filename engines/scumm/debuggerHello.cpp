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
#include "common/array.h"
#include "common/config-manager.h"
#include "common/debug.h"
#include "common/file.h"
#include "common/macresman.h"
#include "common/md5.h"
#include "common/tokenizer.h"
#include "common/translation.h"
#include "gui/integrity-dialog.h"

#include "common/array.h"
#include "common/config-manager.h"
#include "common/debug.h"
#include "common/file.h"
#include "common/macresman.h"
#include "common/md5.h"
#include "common/tokenizer.h"
#include "common/translation.h"

#include "gui/gui-manager.h"
#include "gui/launcher.h"
#include "gui/message.h"
#include "gui/widget.h"
#include "common/hashmap.h"
#include "common/config-manager.h"

namespace Scumm {
    Common::JSONValue* ScummDebugger::generateHelloJSONRequest() {
    	Common::JSONObject requestObject;
	    requestObject.setVal("message", new Common::JSONValue("Hello from ScummVM!"));
        debug(0, "JSON Request: %s", requestObject.getVal("message"));
       int i=0;
        ObjectData *o;

        for(i=0; i<_vm->getInventoryCount(_vm->_scummVars[_vm->VAR_EGO]); i++){
            debugPrintf("\n_inventory[%d]: %d", i, _vm->_inventory[i]);

            const byte *name = _vm->getObjOrActorName(_vm->_inventory[i]);
            if (!name)
                name = (const byte *)"(null)";
            debugPrintf("\n_inventory[%d]: %s", i, name);

        }
        Common::JSONValue* request = new Common::JSONValue(requestObject);
	    return request;
    }

    bool ScummDebugger::Cmd_Hello(int argc, const char **argv) {
        Common::JSONValue* request = generateHelloJSONRequest();
        Common::String url = Common::String::format("localhost:9090");
 
        auto conn = new Networking::PostRequest(url,nullptr,nullptr);

            // new Common::Callback<IntegrityDialog, const Common::JSONValue *>(this, &IntegrityDialog::checksumResponseCallback),
            // new Common::Callback<IntegrityDialog, const Networking::ErrorResponse &>(this, &IntegrityDialog::errorCallback));
            
        conn->setJSONData(request);
        conn->setContentType("application/json");
        conn->start();
        delete request;
 
        // Common::String url = Common::String::format("localhost:9090");
        // Networking::CurlJsonRequest *request = new Networking::CurlJsonRequest(nullptr,nullptr, url);
    	// ConnMan.addRequest(request);

        return true;
    }
}
