// $Revision: 1.0 $
// Written: Li Ge
// Created: 03/12
// Description: This file contains the extended commands set


#include <string>
using std

#include <tcl.h>
#include "jsonModel.h"
//#include "../websockets/libwebsockets.h"
//#include <OPS_Globals.h>

#include <TclModelBuilder.h>
#include <commands.h>
#include <Domain.h>
#include <Node.h>
#include <Element.h>
#include <Truss.h>
#include <NodeIter.h>
#include <ElementIter.h>
#include <SP_ConstraintIter.h>
#include <SP_Constraint.h>
#include <MP_ConstraintIter.h>
#include <MP_Constraint.h>

#include <DirectIntegrationAnalysis.h>
extern Domain theDomain;

//extern ModelBuilder *theBuilder;
//extern StaticAnalysis *theStaticAnalysis;
//extern DirectIntegrationAnalysis *theTransientAnalysis;
//extern int ops_ringbuffer_head;
//extern struct a_message *ops_ringbuffer;
//static int ops_ringbuffer_head;
//#define MAX_MESSAGE_QUEUE 640

#define MAX_RESULT_BUFFER_LEN 10000000
// Add JSON interface: Li Ge, UCSD
#define RESULT_LEN 5000000
#define BUFFER_LEN 3000000

char *theElementsToJson(char *strptr) {
	ElementIter &theEles = theDomain.getElements();
	Element *theEle;
	const ID *nids = NULL;
	char buffer[40];
	int size;
	int ne = theDomain.getNumElements();
	int j, eid;

	sprintf(buffer, "\"theElements\":{");
	strcat(strptr, buffer);
	j = 0;
	while ((theEle = theEles()) != 0) {
		j++;
		nids = &(theEle->getExternalNodes());
		eid = theEle->getTag();
		size = nids->Size();
		sprintf(buffer, "\"%d\":", eid);
		strcat(strptr, buffer);
		strcat(strptr, theEle->toJSON());
		if (j < ne) {
			strcat(strptr, ",");
		}
	}
	strcat(strptr, "}");
	return strptr;
}

char *theNodesToJson(char *strptr) {
	NodeIter &theNodes = theDomain.getNodes();
	Node *theNode;
	const Vector *nodalCrds = NULL;
	char buffer[40];
//	double ctime = theDomain.getCurrentTime();
	int size;
	int nn = theDomain.getNumNodes();
	int nid;
	int i, j;

	sprintf(buffer, "\"theNodes\":{");
	strcat(strptr, buffer);
	j = 0;
	while ((theNode = theNodes()) != 0) {
		j++;
		nodalCrds = &(theNode->getCrds());
		nid = theNode->getTag();
		size = nodalCrds->Size();
		sprintf(buffer, "\"%d\":[", nid);
		strcat(strptr, buffer);

		for (i = 0; i < size; i++) {
			if (i == size - 1) {
				sprintf(buffer, "%15.15f", (*nodalCrds)(i));
			} else {
				sprintf(buffer, "%15.15f,", (*nodalCrds)(i));
			}
			strcat(strptr, buffer);
		}
		if (j < nn) {
			strcat(strptr, "],");
		} else {
			strcat(strptr, "]");
		}
	}
	strcat(strptr, "}");
	return strptr;
}

int modelToJSON(ClientData clientData, Tcl_Interp *interp, int argc,
		TCL_Char **argv) {
	char result[RESULT_LEN];
	char buffer[BUFFER_LEN];
	sprintf(result, "JSON:{");
	theNodesToJson(buffer);
	strcat(result, buffer);
	strcat(result, ",");
	memset(buffer, 0, BUFFER_LEN*sizeof(char));
	theElementsToJson(buffer);
	strcat(result, buffer);
	strcat(result, "}");
	Tcl_AppendResult(interp, result, NULL);
	return TCL_OK;
}

int jsonLoad(ClientData clientData, Tcl_Interp *interp, int argc,
		TCL_Char **argv) {
	char result[RESULT_LEN];
//	char buffer[BUFFER_LEN];
//	String result;
//	result.append("test");
//	intf(result, "JSON:{\"operation\":\"load\",\"parameters\":{");
//	theNodesToJson(buffer);
//	strcat(result, buffer);
//	strcat(result, ",");
//	memset(buffer, 0, BUFFER_LEN*sizeof(char));
//	theElementsToJson(buffer);
//	strcat(result, buffer);
//	strcat(result, "}}");
	Tcl_AppendResult(interp, result, NULL);
	return TCL_OK;
}
