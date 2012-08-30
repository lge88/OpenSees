#include <iostream>
#include <sstream>
#include <string>
#include <cstring>
using namespace std;
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
#include "jsonDomain.h"

string nodesToJSON(void);
string elementsToJSON(void);
string spcsToJSON(void);
string mpcsToJSON(void);
string patternsToJSON(void);
string nodesDispToJSON(void);

string nodesToJSON(void) {
    NodeIter &theNodes = theDomain.getNodes();
    Node *theNode;
    const Vector *nodalCrds = NULL;
    int size;
    int nn=theDomain.getNumNodes();
    int nid;
    int i, j;
    std::ostringstream s;
    s << "JSON:({\"theNodes\":{";

    j=0;
    while ((theNode = theNodes()) != 0) {
        j++;
        nodalCrds = &(theNode->getCrds());
        nid = theNode->getTag();
        size = nodalCrds->Size();
        s << "\"" << nid << "\":[";
        for (i = 0; i < size; i++) {
            if (i==size-1) {
                s << (*nodalCrds)(i);
            } else {
                s << (*nodalCrds)(i) << ",";
            }
        }
        if (j<nn) {
            s << "],";
        } else {
            s << "]";
        }
    }
    s << "}})";
    return s.str();
};

string elementsToJSON(void) {
    ElementIter &theEles = theDomain.getElements();
	Element *theEle;
	const ID *nids = NULL;
    char *eleJSON;
	int size;
	int ne = theDomain.getNumElements();
	int j, eid;

    std::ostringstream s;
    string str;
    s << "JSON:({\"theElements\":{";
	j = 0;
	while ((theEle = theEles()) != 0) {
		j++;
		nids = &(theEle->getExternalNodes());
		eid = theEle->getTag();
		size = nids->Size();
        str = theEle->toJSON();
        s << "\"" << eid << "\":" << str;
		if (j < ne) {
            s << ",";
		}
	}
    s << "}})";
    return s.str();
};

string nodesDispToJSON(void) {
    NodeIter &theNodes = theDomain.getNodes();
    Node *theNode;
    const Vector *nodalResponse;
    double ctime = theDomain.getCurrentTime();
    int size;
    int nn=theDomain.getNumNodes();
    int nid;
    int i, j;
    std::ostringstream s;
    s << "JSON:({\"time\":" << ctime << ",\"disp\":{";

    j=0;
    while ((theNode = theNodes()) != 0) {
        j++;
        nodalResponse = theNode->getResponse(Disp);
        nid = theNode->getTag();
        size = nodalResponse->Size();
        s << "\"" << nid << "\":[";
        
        for (i = 0; i < size; i++) {
            if (i==size-1) {
                s << (*nodalResponse)(i);
            } else {
                s << (*nodalResponse)(i) << ",";
            }
        }
        if (j<nn) {
            s << "],";
        } else {
            s << "]";
        }
    }
    s << "}})";
    return s.str();
};



int jsonEchoDisp(ClientData clientData, Tcl_Interp *interp, int argc,
                 TCL_Char **argv) {
    string str = nodesDispToJSON();
    char * cstr;
    cstr = new char [str.size()+1];
    strcpy (cstr, str.c_str());
    Tcl_AppendResult(interp, cstr, NULL);
    return TCL_OK;
};


int jsonEchoNodes(ClientData clientData, Tcl_Interp *interp, int argc,
                  TCL_Char **argv) {
    string str = nodesToJSON();
    char * cstr;
    cstr = new char [str.size()+1];
    strcpy (cstr, str.c_str());
    Tcl_AppendResult(interp, cstr, NULL);
    return TCL_OK;
};


int jsonEchoElements(ClientData clientData, Tcl_Interp *interp, int argc,
                     TCL_Char **argv) {
    string str = elementsToJSON();
    char * cstr;
    cstr = new char [str.size()+1];
    strcpy (cstr, str.c_str());
    Tcl_AppendResult(interp, cstr, NULL);
    return TCL_OK;
};

