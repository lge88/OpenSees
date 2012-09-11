#ifndef _JSONTCPSTREAM_H_
#define _JSONTCPSTREAM_H_
#include <TCP_Stream.h>

int jsonTCPStream(ClientData clientData, Tcl_Interp *interp, int argc,
                  TCL_Char **argv);

TCP_Stream * getTheTCPStream(void);

#endif /* _JSONTCPSTREAM_H_ */
