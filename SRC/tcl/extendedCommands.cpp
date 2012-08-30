#include "extendedCommands.h"
#include "extended/jsonDomain.h"
/* #include "extended/jsonModelBuilder.h" */
/* #include "extended/jsonAnalyze.h" */

int createExtendedCommands(Tcl_Interp *interp){

	Tcl_CreateCommand(interp, "json-echo-nodes", jsonEchoNodes,
			      (ClientData)NULL, (Tcl_CmdDeleteProc *)NULL);

	Tcl_CreateCommand(interp, "json-echo-elements", jsonEchoElements,
			      (ClientData)NULL, (Tcl_CmdDeleteProc *)NULL);

	/* Tcl_CreateCommand(interp, "json-echo-single-point-constraints", jsonEchoSPC, */
	/* 		      (ClientData)NULL, (Tcl_CmdDeleteProc *)NULL); */

	/* Tcl_CreateCommand(interp, "json-echo-spc", jsonEchoSPC, */
	/* 		      (ClientData)NULL, (Tcl_CmdDeleteProc *)NULL); */

	/* Tcl_CreateCommand(interp, "json-echo-multiple-point-constraints", jsonEchoMPC, */
	/* 		      (ClientData)NULL, (Tcl_CmdDeleteProc *)NULL); */

	/* Tcl_CreateCommand(interp, "json-echo-patterns", jsonEchoPatterns, */
	/* 		      (ClientData)NULL, (Tcl_CmdDeleteProc *)NULL); */

	/* Tcl_CreateCommand(interp, "json-echo-mpc", jsonEchoMPC, */
	/* 		      (ClientData)NULL, (Tcl_CmdDeleteProc *)NULL); */

	/* Tcl_CreateCommand(interp, "json-echo-displacement-field", jsonEchoDisp, */
	/* 		      (ClientData)NULL, (Tcl_CmdDeleteProc *)NULL); */

	Tcl_CreateCommand(interp, "json-echo-disp", jsonEchoDisp,
			      (ClientData)NULL, (Tcl_CmdDeleteProc *)NULL);

	/* Tcl_CreateCommand(interp, "allNodeDisp", allNodeDisp, */
	/* 		      (ClientData)NULL, (Tcl_CmdDeleteProc *)NULL); */

	/* Tcl_CreateCommand(interp, "analyzeAndStreamNodeDisp", analyzeAndStreamNodeDisp, */
	/* 		      (ClientData)NULL, (Tcl_CmdDeleteProc *)NULL); */

	/* Tcl_CreateCommand(interp, "echoModel", echoModel, */
	/* 		      (ClientData)NULL, (Tcl_CmdDeleteProc *)NULL); */

	/* Tcl_CreateCommand(interp, "modelToJson", modelToJson, */
	/* 		      (ClientData)NULL, (Tcl_CmdDeleteProc *)NULL); */

	return 0;
}


