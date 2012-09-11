#include <tcl.h>
#include <TCP_Stream.h>
#include "jsonTCPStream.h"

static TCP_Stream * theTCPStream=0;
int jsonTCPStream(ClientData clientData, Tcl_Interp *interp, int argc,
                     TCL_Char **argv) {
    const char *inetAddr = 0;
    int inetPort;

    if (argc != 3) {
        return -1;
    } else {
        inetAddr = argv[1];
        if (Tcl_GetInt(interp, argv[2], &inetPort) != TCL_OK) {
            return -1;
        }
    }
    if (theTCPStream == 0) {
        theTCPStream = new TCP_Stream(inetPort, inetAddr);
    }
    return TCL_OK;
};

TCP_Stream * getTheTCPStream(void) {
    return theTCPStream;
}
