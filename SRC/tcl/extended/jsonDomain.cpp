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
#include <LoadPattern.h>
#include <LoadPatternIter.h>
#include <TimeSeries.h>
#include <NodalLoad.h>
#include <NodalLoadIter.h>
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
    SP_ConstraintIter &theSPs = theDomain.getSPs();
    SP_Constraint *theSP;

    mObject sps;
    mArray sp_val;
    mValue tag, ntag, dir, ref_val;
    char tag_str[15];

    sps.clear();
    while ((theSP = theSPs()) != 0) {
        sp_val.clear();
        tag = theSP->getTag();
        ntag = theSP->getNodeTag();
        dir = theSP->getDOF_Number();
        ref_val = theSP->getValue();
        sp_val.push_back(ntag);
        sp_val.push_back(dir);
        sp_val.push_back(ref_val);

        sprintf(tag_str, "%d", tag.get_int());
        sps[tag_str] = sp_val;
    }
	
    return sps; 
}


mObject patternsToJSON(void) {
    LoadPatternIter &thePatterns = theDomain.getLoadPatterns();
    LoadPattern *thePattern;
    TimeSeries *theSeries;
    NodalLoadIter *nli;
    NodalLoad *nload;
    const Vector *load_vec; 
    // TODO:
    // ElementalLoadIter *eli;
    // SP_ConstraintIter *spci;

    mObject patterns, pattern, nloads;
    mArray arr;
    mValue tmp, tmp2, tmp3;
    char tag_str[15];
    int i, size;

    patterns.clear();
    while ((thePattern = thePatterns()) != 0) {
        pattern.clear();
        // TODO:
        tmp = thePattern->getClassType();
        pattern["type"] = tmp;
        theSeries = thePattern->getTimeSeries();
        tmp2 = theSeries->getTag();
        sprintf(tag_str, "%d", tmp2.get_int());
        pattern["tsTag"] = tag_str;

        nli = &(thePattern->getNodalLoads());
        nloads.clear();
        while((nload = (*nli)()) != 0) {
            tmp2 = nload->getNodeTag();
            sprintf(tag_str, "%d", tmp2.get_int());
            load_vec = nload->getLoadValue();
            size = load_vec->Size();
            arr.clear();
            for (i = 0; i < size; i++) {
                tmp3 = (*load_vec)(i);
                arr.push_back(tmp3);
            }
            nloads[tag_str] = arr;
        }
        pattern["nodalLoads"] = nloads;

        tmp2 = thePattern->getTag();
        sprintf(tag_str, "%d", tmp2.get_int());
        patterns[tag_str] = pattern;
    }
	
    return patterns; 
}


mObject domainToJSON(void) {
    mObject domainJSON;
    domainJSON["theBounds"] = boundsToJSON();
    domainJSON["theNodes"] = nodesToJSON();
    domainJSON["theElements"] = elementsToJSON();
    domainJSON["theSPConstraints"] = spcsToJSON();
    domainJSON["thePatterns"] = patternsToJSON();
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

