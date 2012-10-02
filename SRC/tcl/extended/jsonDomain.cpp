#include <json_spirit.h>
#include <iostream>
#include <sstream>
#include <string>
#include <cstring>
using namespace std;
using namespace json_spirit;
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
#include <TCP_Stream.h>
extern Domain theDomain;
#include "jsonDomain.h"
#include "jsonTCPStream.h"

mArray boundsToJSON(void);
mObject nodesToJSON(void);
mObject elementsToJSON(void);
mObject spcsToJSON(void);
mObject mpcsToJSON(void);
mObject patternsToJSON(void);
mObject domainToJSON(void);
mObject nodesDispToJSON(void);
mObject fieldsToJSON(void);

mArray boundsToJSON(void) {
    const Vector &theBounds = theDomain.getPhysicalBounds();
    mArray bounds;
    mValue tmp;
    int i;
    bounds.clear();
    for(i = 0; i < 6; i++) {
        tmp = theBounds(i);
        bounds.push_back(tmp);
    }
    return bounds;
}

mObject nodesToJSON(void) {
    NodeIter &theNodes = theDomain.getNodes();
    Node *theNode;
    const Vector *nodalCrds = NULL;
    int size;
    int i;

    mObject nodes;
    mArray coords;
    mValue tag, coord;
    char tag_str[15];

    nodes.clear();
    while ((theNode = theNodes()) != 0) {
        nodalCrds = &(theNode->getCrds());
        tag = theNode->getTag();
        size = nodalCrds->Size();
        coords.clear();
        for (i = 0; i < size; i++) {
            coord = (*nodalCrds)(i);
            coords.push_back(coord);
        }
        sprintf(tag_str, "%d", tag.get_int());
        nodes[tag_str] = coords;
    }
	
    return nodes; 
}

mObject elementsToJSON(void) {
    ElementIter &theEles = theDomain.getElements();
	Element *theEle;

    mObject eles;
    mValue tag, ele;
    char tag_str[15];

    eles.clear();
	while ((theEle = theEles()) != 0) {
		tag = theEle->getTag();
        ele = theEle->toJSON();
        sprintf(tag_str, "%d", tag.get_int());
        eles[tag_str] = ele;
	}
    return eles;
};

mObject spcsToJSON(void) {
    NodeIter &theNodes = theDomain.getNodes();
    Node *theNode;
    const Vector *nodalCrds = NULL;
    int size;
    int i;

    mObject nodes;
    mArray coords;
    mValue tag, coord;
    char tag_str[15];

    nodes.clear();
    while ((theNode = theNodes()) != 0) {
        nodalCrds = &(theNode->getCrds());
        tag = theNode->getTag();
        size = nodalCrds->Size();
        coords.clear();
        for (i = 0; i < size; i++) {
            coord = (*nodalCrds)(i);
            coords.push_back(coord);
        }
        sprintf(tag_str, "%d", tag.get_int());
        nodes[tag_str] = coords;
    }
	
    return nodes; 
}


mObject domainToJSON(void) {
    mObject domainJSON;
    domainJSON["theBounds"] = boundsToJSON();
    domainJSON["theNodes"] = nodesToJSON();
    domainJSON["theElements"] = elementsToJSON();    
    return domainJSON;
}

mObject nodesDispToJSON(void) {
    NodeIter &theNodes = theDomain.getNodes();
    Node *theNode;
    const Vector *nodalResponse;
    int size;
    int i;

    mObject disp_field, result;
    mArray nodal_disps;
    mValue tag, disp;
    char tag_str[15];

    // TODO: The field 'time' shouldn't be here, but old SketchIT depends on it, for compatibilty.
    mValue ctime = theDomain.getCurrentTime();
    result["time"] = ctime;

    disp_field.clear();
    while ((theNode = theNodes()) != 0) {
        nodalResponse = theNode->getResponse(Disp);
        size = nodalResponse->Size();

        nodal_disps.clear();
        for (i = 0; i < size; i++) {
            disp = (*nodalResponse)(i);
            nodal_disps.push_back(disp);
        }

        tag = theNode->getTag();
        sprintf(tag_str, "%d", tag.get_int());        
        disp_field[tag_str] = nodal_disps;
    }
    result["disp"] = disp_field;
    return result;
};


int jsonEchoDisp(ClientData clientData, Tcl_Interp *interp, int argc,
                 TCL_Char **argv) {

    std::ostringstream os; 
    mObject nodeDispJSON = nodesDispToJSON();
    os << "|(";
    write( nodeDispJSON, os, remove_trailing_zeros);
    os << ")|";
    string str = os.str();

    TCP_Stream * ts = getTheTCPStream();
    char * cstr;
    if(ts!=0) {
        ts->write(str);
    } 

    cstr = new char [str.size()+1];
    strcpy (cstr, str.c_str());
    Tcl_AppendResult(interp, cstr, NULL);

    return TCL_OK;
};


int jsonEchoNodes(ClientData clientData, Tcl_Interp *interp, int argc,
                  TCL_Char **argv) {
    // string str = nodesToJSON();
    // char * cstr;
    // cstr = new char [str.size()+1];
    // strcpy (cstr, str.c_str());
    // Tcl_AppendResult(interp, cstr, NULL);
    return TCL_OK;
};


int jsonEchoElements(ClientData clientData, Tcl_Interp *interp, int argc,
                     TCL_Char **argv) {
    // string str = elementsToJSON();
    // char * cstr;
    // cstr = new char [str.size()+1];
    // strcpy (cstr, str.c_str());
    // Tcl_AppendResult(interp, cstr, NULL);
    return TCL_OK;
};

int jsonEchoDomain(ClientData clientData, Tcl_Interp *interp, int argc,
                   TCL_Char **argv) {
    std::ostringstream os; 
    mObject domainJSON = domainToJSON();
    os << "|(";
    write( domainJSON, os, remove_trailing_zeros);
    os << ")|";
    string str = os.str();

    TCP_Stream * ts = getTheTCPStream();
    char * cstr;
    if(ts!=0) {
        ts->write(str);
    } 

    cstr = new char [str.size()+1];
    strcpy (cstr, str.c_str());
    Tcl_AppendResult(interp, cstr, NULL);

    return TCL_OK;
};

