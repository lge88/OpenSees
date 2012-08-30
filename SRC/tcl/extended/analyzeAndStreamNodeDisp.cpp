#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <tcl.h>
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
#include "analyzeAndStreamNodeDisp.h"
#define MAX_RESULT_BUFFER_LEN 10000000
char *allNodeDispToJson(char *strptr);

int allNodeDisp(ClientData clientData, Tcl_Interp *interp, int argc,
		TCL_Char **argv) {

	char result[MAX_RESULT_BUFFER_LEN];
	allNodeDispToJson(result);
	Tcl_AppendResult(interp, result, NULL);

	return TCL_OK;
}

char *allNodeDispToJson(char *strptr) {
	NodeIter &theNodes = theDomain.getNodes();
	Node *theNode;
	const Vector *nodalResponse;
	char buffer[40];
	double ctime = theDomain.getCurrentTime();
	int size;
	int nn=theDomain.getNumNodes();
	int nid;
	int i,j;

//	memset(strptr,0,sizeof(strptr));
	sprintf(buffer, "\"%4.3f\":{", ctime);
	strcat(strptr,buffer);

	j=0;
	while ((theNode = theNodes()) != 0) {
		j++;
		nodalResponse=theNode->getResponse(Disp);
		nid = theNode->getTag();
		size = nodalResponse->Size();
		sprintf(buffer, "\"%d\":[", nid);
		strcat(strptr,buffer);

		for (i = 0; i < size; i++) {
			if (i==size-1) {
				sprintf(buffer, "%15.15f", (*nodalResponse)(i));
			} else {
				sprintf(buffer, "%15.15f,", (*nodalResponse)(i));
			}
			strcat(strptr,buffer);
		}
		if (j<nn) {
			strcat(strptr,"],");
		} else {
			strcat(strptr,"]");
		}
	}
	strcat(strptr,"}");
	return strptr;
}
//
//int analyzeAndStreamNodeDisp(ClientData clientData, Tcl_Interp *interp,
//                             int argc,TCL_Char **argv) {
//	int result = 0;
//	int i,j;
//	struct per_session_data__ops *pss = (per_session_data__ops *) clientData;
//	//TODO: do something to auto decide chunk sinze
//	int chunk = 1;
//
//	char resultStr[MAX_RESULT_BUFFER_LEN];
//	int result_length;
//	DirectIntegrationAnalysis *theTransientAnalysis = getDirectIntegrationAnalysis();
//	StaticAnalysis *theStaticAnalysis = getStaticAnalysis();
//	int numIncr;
//	double dT;
//	int analysisType=0;
//	char header[3];
//
//	header[0]='f';
//	header[1]='n';
//	header[2]='u';
//
//	if (theStaticAnalysis != 0) {
//		if (argc < 2) {
//			opserr << "WARNING static analysis: analysis numIncr?\n";
//			return TCL_ERROR;
//		}
//		if (Tcl_GetInt(interp, argv[1], &numIncr) != TCL_OK
//		)
//			return TCL_ERROR;
//		analysisType = 1;
//	} else if (theTransientAnalysis != 0) {
//		if (argc < 3) {
//			opserr << "WARNING transient analysis: analysis numIncr? deltaT?\n";
//			return TCL_ERROR;
//		}
//		if (Tcl_GetInt(interp, argv[1], &numIncr) != TCL_OK
//		)
//			return TCL_ERROR;
//		if (Tcl_GetDouble(interp, argv[2], &dT) != TCL_OK
//		)
//			return TCL_ERROR;
//		// Set global timestep variable
//		ops_Dt = dT;
//		analysisType = 2;
//	}
//
//	memset(resultStr,0,sizeof(resultStr));
//
//	//print header
//	strcpy(resultStr,header);
//	strcat(resultStr,"({");
//
//	j=1;
//	for (i=0;i<numIncr;i++) {
//		if (analysisType == 1) {
//			result = theStaticAnalysis->analyze(1);
//		} else if (analysisType == 2){
//			result = theTransientAnalysis->analyze(1, dT);
//		} else {
//			opserr << "WARNING No Analysis type has been specified \n";
//					return TCL_ERROR;
//		}
//		if (result < 0) {
//			opserr << "OpenSees > analyze failed, returned: " << result
//					<< " error flag\n";
//			return result;
//		}
//
//		allNodeDispToJson(resultStr);
//
//		j++;
//		if (j>chunk || i==numIncr-1){
//			strcat(resultStr,"})");
//			result_length=strlen(resultStr);
//			if (pss->result_buffer->payload) {
//				free(pss->result_buffer->payload);
//			}
//
//			pss->result_buffer->payload = malloc(LWS_SEND_BUFFER_PRE_PADDING+ result_length + LWS_SEND_BUFFER_POST_PADDING);
//			pss->result_buffer->len = result_length;
//			memcpy((char *)(pss->result_buffer->payload)+ LWS_SEND_BUFFER_PRE_PADDING,resultStr,result_length);
//			libwebsocket_write(
//												pss->wsi,
//												(unsigned char *) pss->result_buffer->payload
//														+ LWS_SEND_BUFFER_PRE_PADDING,
//														pss->result_buffer->len,
//												LWS_WRITE_TEXT);
//			j=0;
//			memset(resultStr,0,sizeof(resultStr));
//			strcpy(resultStr,header);
//			strcat(resultStr,"({");
//		} else {
//			strcat(resultStr,",");
//		}
//	}
//	return result;
//}

