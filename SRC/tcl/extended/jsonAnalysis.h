#ifndef _JSONANALYZE_H_
#define _JSONANALYZE_H_


int jsonAnalyze(ClientData clientData, Tcl_Interp *interp, int argc,
                TCL_Char **argv);

int jsonDispacementField(ClientData clientData, Tcl_Interp *interp, int argc,
                TCL_Char **argv);

int jsonNodes(ClientData clientData, Tcl_Interp *interp, int argc,
                TCL_Char **argv);

int jsonPatterns(ClientData clientData, Tcl_Interp *interp, int argc,
                TCL_Char **argv);

int jsonSPs(ClientData clientData, Tcl_Interp *interp, int argc,
                TCL_Char **argv);

int jsonMPs(ClientData clientData, Tcl_Interp *interp, int argc,
                TCL_Char **argv);


#endif /* _JSONANALYZE_H_ */
