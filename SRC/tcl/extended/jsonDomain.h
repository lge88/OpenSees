#ifndef _JSONDOMAIN_H_
#define _JSONDOMAIN_H_

int jsonEchoDomain(ClientData clientData, Tcl_Interp *interp, int argc,
                TCL_Char **argv);

int jsonEchoNodes(ClientData clientData, Tcl_Interp *interp, int argc,
                TCL_Char **argv);

int jsonEchoElements(ClientData clientData, Tcl_Interp *interp, int argc,
                TCL_Char **argv);

int jsonEchoSPC(ClientData clientData, Tcl_Interp *interp, int argc,
                TCL_Char **argv);

int jsonEchoMPC(ClientData clientData, Tcl_Interp *interp, int argc,
                TCL_Char **argv);

int jsonEchoPatterns(ClientData clientData, Tcl_Interp *interp, int argc,
                TCL_Char **argv);

int jsonEchoDisp(ClientData clientData, Tcl_Interp *interp, int argc,
                TCL_Char **argv);

#endif /* _JSONDOMAIN_H_ */
