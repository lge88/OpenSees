#include <tcl.h>
#include <TCP_Stream.h>
#include <string.h>
#include <unistd.h>
#include "jsonTCPStream.h"

static TCP_Stream * theTCPStream=0;
int jsonTCPStream(ClientData clientData, Tcl_Interp *interp, int argc,
                     TCL_Char **argv) {
    const char *inetAddr = 0;
    std::string socket_id;
    int inetPort;

    if (argc != 4) {
        return -1;
    } else {
        inetAddr = argv[1];
        socket_id = argv[3];
        if (Tcl_GetInt(interp, argv[2], &inetPort) != TCL_OK) {
            return -1;
        }
    }
    if (theTCPStream == 0) {
        theTCPStream = new TCP_Stream(inetPort, inetAddr);
        // TODO: use sleep() is a dirty way to send socket_id...
        theTCPStream->write(socket_id);
        sleep(1);
    } else {        
        // TODO: do something to change inetAddr, port or socket_id
    }
    return TCL_OK;
};

TCP_Stream * getTheTCPStream(void) {
    return theTCPStream;
}
