/* ****************************************************************** **
**    OpenSees - Open System for Earthquake Engineering Simulation    **
**          Pacific Earthquake Engineering Research Center            **
**                                                                    **
**                                                                    **
** (C) Copyright 1999, The Regents of the University of California    **
** All Rights Reserved.                                               **
**                                                                    **
** Commercial use of this program without express permission of the   **
** University of California, Berkeley, is strictly prohibited.  See   **
** file 'COPYRIGHT'  in main directory for information on usage and   **
** redistribution,  and for a DISCLAIMER OF ALL WARRANTIES.           **
**                                                                    **
** Developed by:                                                      **
**   Frank McKenna (fmckenna@ce.berkeley.edu)                         **
**   Gregory L. Fenves (fenves@ce.berkeley.edu)                       **
**   Filip C. Filippou (filippou@ce.berkeley.edu)                     **
**                                                                    **
** ****************************************************************** */
                                                                        
// $Revision: 1.13 $
// $Date: 2010-06-01 23:47:54 $
// $Source: /usr/local/cvs/OpenSees/SRC/analysis/integrator/DisplacementControl.cpp,v $
                                                                        
                                                                        
// File: ~/analysis/integrator/DisplacementControl.C
// 
// Written: fmk 
// Created: 07/98
// Revision: A
//
// Description: This file contains the class definition for DisplacementControl.
// DisplacementControl is an algorithmic class for perfroming a static analysis
// using the arc length scheme, that is within a load step the follwing
// constraint is enforced: dU^TdU + alpha^2*dLambda^2 = DisplacementControl^2
// where dU is change in nodal displacements for step, dLambda is
// change in applied load and DisplacementControl is a control parameter.
//
// What: "@(#) DisplacementControl.C, revA"


#include <DisplacementControl.h>
#include <AnalysisModel.h>
#include <LinearSOE.h>
#include <Vector.h>
#include <Channel.h>
#include <math.h>
#include <Domain.h>
#include <Node.h>
#include <DOF_Group.h>
#include <ID.h>
#include<stdlib.h>

DisplacementControl::DisplacementControl(int node, int dof, 
					 double increment, 
					 Domain *domain,
					 int numIncr,
					 double min, double max) 
:StaticIntegrator(INTEGRATOR_TAGS_DisplacementControl),
 theNode(node), theDof(dof), theIncrement(increment), theDomain(domain),
 theDofID(-1),
 deltaUhat(0), deltaUbar(0), deltaU(0), deltaUstep(0), 
 phat(0), deltaLambdaStep(0.0), currentLambda(0.0),
 specNumIncrStep(numIncr), numIncrLastStep(numIncr),
 minIncrement(min), maxIncrement(max)
{
  // to avoid divide-by-zero error on first update() ensure numIncr != 0
  if (numIncr == 0) {
    opserr << "WARNING DisplacementControl::DisplacementControl() -";
    opserr << " numIncr set to 0, 1 assumed\n";
    specNumIncrStep = 1.0;
    numIncrLastStep = 1.0;
  }
}

DisplacementControl::~DisplacementControl()
{
    // delete any vector object created
    if (deltaUhat != 0)
	delete deltaUhat;
    if (deltaU != 0)
	delete deltaU;
    if (deltaUstep != 0)
	delete deltaUstep;
    if (deltaUbar != 0)
	delete deltaUbar;
    if (phat != 0)
	delete phat;
}

int
DisplacementControl::newStep(void)
{
	if (theDofID == -1) {
		opserr << "DisplacementControl::newStep() - domainChanged has not been called\n";
		return -1;
	}

    // get pointers to AnalysisModel and LinearSOE
    AnalysisModel *theModel = this->getAnalysisModel();
    LinearSOE *theLinSOE = this->getLinearSOE();    
    if (theModel == 0 || theLinSOE == 0) {
	opserr << "WARNING DisplacementControl::newStep() ";
	opserr << "No AnalysisModel or LinearSOE has been set\n";
	return -1;
    }


    // determine increment for this iteration
    double factor = specNumIncrStep/numIncrLastStep;
    theIncrement *=factor;

    if (theIncrement < minIncrement)
      theIncrement = minIncrement;
    else if (theIncrement > maxIncrement)
      theIncrement = maxIncrement;


    // get the current load factor
    currentLambda = theModel->getCurrentDomainTime();

    // determine dUhat
    this->formTangent();
    theLinSOE->setB(*phat);

    if (theLinSOE->solve() < 0) {
      opserr << "DisplacementControl::newStep(void) - failed in solver\n";
      return -1;
    }

    (*deltaUhat) = theLinSOE->getX();
    Vector &dUhat = *deltaUhat;

    double dUahat = dUhat(theDofID);
    if (dUahat == 0.0) {
	opserr << "WARNING DisplacementControl::newStep() ";
	opserr << "dUahat is zero -- zero reference displacement at control node DOF\n";
	return -1;
    }
    
    // determine delta lambda(1) == dlambda    
    double dLambda = theIncrement/dUahat;

    deltaLambdaStep = dLambda;
    currentLambda += dLambda;
 //   opserr << "DisplacementControl: " << dUahat  << " " << theDofID << endln;
 //   opserr << "DisplacementControl::newStep() : " << deltaLambdaStep << endln;
    // determine delta U(1) == dU
    (*deltaU) = dUhat;
    (*deltaU) *= dLambda;
    (*deltaUstep) = (*deltaU);

    // update model with delta lambda and delta U
    theModel->incrDisp(*deltaU);    
    theModel->applyLoadDomain(currentLambda);    
    if (theModel->updateDomain() < 0) {
      opserr << "DisplacementControl::newStep - model failed to update for new dU\n";
      return -1;
    }

    numIncrLastStep = 0;

    return 0;
}

int
DisplacementControl::update(const Vector &dU)
{

  if (theDofID == -1) {
    opserr << "DisplacementControl::newStep() - domainChanged has not been called\n";
    return -1;
  } 
    AnalysisModel *theModel = this->getAnalysisModel();
    LinearSOE *theLinSOE = this->getLinearSOE();    
    if (theModel == 0 || theLinSOE == 0) {
	opserr << "WARNING DisplacementControl::update() ";
	opserr << "No AnalysisModel or LinearSOE has been set\n";
	return -1;
    }

    (*deltaUbar) = dU; // have to do this as the SOE is gonna change
    double dUabar = (*deltaUbar)(theDofID);
    
    // determine dUhat    
    theLinSOE->setB(*phat);
    theLinSOE->solve();
    (*deltaUhat) = theLinSOE->getX();    

    double dUahat = (*deltaUhat)(theDofID);
    if (dUahat == 0.0) {
	opserr << "WARNING DisplacementControl::update() ";
	opserr << "dUahat is zero -- zero reference displacement at control node DOF\n";
	return -1;
    }
    
    // determine delta lambda(1) == dlambda    
    double dLambda = -dUabar/dUahat;
    
    // determine delta U(i)
    (*deltaU) = (*deltaUbar);    
    deltaU->addVector(1.0, *deltaUhat,dLambda);
    
    // update dU and dlambda
    (*deltaUstep) += *deltaU;
    deltaLambdaStep += dLambda;
    currentLambda += dLambda;

    // update the model
    theModel->incrDisp(*deltaU);    
    theModel->applyLoadDomain(currentLambda);    
    if (theModel->updateDomain() < 0) {
      opserr << "DisplacementControl::update - model failed to update for new dU\n";
      return -1;
    }
	
    
    // set the X soln in linearSOE to be deltaU for convergence Test
    theLinSOE->setX(*deltaU);

    numIncrLastStep++;

    return 0;
}



int 
DisplacementControl::domainChanged(void)
{
    // we first create the Vectors needed
    AnalysisModel *theModel = this->getAnalysisModel();
    LinearSOE *theLinSOE = this->getLinearSOE();    
    if (theModel == 0 || theLinSOE == 0) {
	opserr << "WARNING DisplacementControl::update() ";
	opserr << "No AnalysisModel or LinearSOE has been set\n";
	return -1;
    }    
    int size = theModel->getNumEqn(); // ask model in case N+1 space

    if (deltaUhat == 0 || deltaUhat->Size() != size) { // create new Vector
	if (deltaUhat != 0)
	    delete deltaUhat;   // delete the old
	deltaUhat = new Vector(size);
	if (deltaUhat == 0 || deltaUhat->Size() != size) { // check got it
	    opserr << "FATAL DisplacementControl::domainChanged() - ran out of memory for";
	    opserr << " deltaUhat Vector of size " << size << endln;
	    exit(-1);
	}
    }

    if (deltaUbar == 0 || deltaUbar->Size() != size) { // create new Vector
	if (deltaUbar != 0)
	    delete deltaUbar;   // delete the old
	deltaUbar = new Vector(size);
	if (deltaUbar == 0 || deltaUbar->Size() != size) { // check got it
	    opserr << "FATAL DisplacementControl::domainChanged() - ran out of memory for";
	    opserr << " deltaUbar Vector of size " << size << endln;
	    exit(-1);
	}
    }
    
    if (deltaU == 0 || deltaU->Size() != size) { // create new Vector
	if (deltaU != 0)
	    delete deltaU;   // delete the old
	deltaU = new Vector(size);
	if (deltaU == 0 || deltaU->Size() != size) { // check got it
	    opserr << "FATAL DisplacementControl::domainChanged() - ran out of memory for";
	    opserr << " deltaU Vector of size " << size << endln;
	    exit(-1);
	}
    }

    if (deltaUstep == 0 || deltaUstep->Size() != size) { 
	if (deltaUstep != 0)
	    delete deltaUstep;  
	deltaUstep = new Vector(size);
	if (deltaUstep == 0 || deltaUstep->Size() != size) { 
	    opserr << "FATAL DisplacementControl::domainChanged() - ran out of memory for";
	    opserr << " deltaUstep Vector of size " << size << endln;
	    exit(-1);
	}
    }

    if (phat == 0 || phat->Size() != size) { 
	if (phat != 0)
	    delete phat;  
	phat = new Vector(size);
	if (phat == 0 || phat->Size() != size) { 
	    opserr << "FATAL DisplacementControl::domainChanged() - ran out of memory for";
	    opserr << " phat Vector of size " << size << endln;
	    exit(-1);
	}
    }    

    // now we have to determine phat
    // do this by incrementing lambda by 1, applying load
    // and getting phat from unbalance.
    currentLambda = theModel->getCurrentDomainTime();
    currentLambda += 1.0;
    theModel->applyLoadDomain(currentLambda);    
    this->formUnbalance(); // NOTE: this assumes unbalance at last was 0
    (*phat) = theLinSOE->getB();
    currentLambda -= 1.0;
    theModel->setCurrentDomainTime(currentLambda);    


    // check there is a reference load
    int haveLoad = 0;
    for (int i=0; i<size; i++)
      if ( (*phat)(i) != 0.0 ) {
	haveLoad = 1;
	i = size;
      }

    if (haveLoad == 0) {
      opserr << "WARNING DisplacementControl::domainChanged() - zero reference load";
      return -1;
    }

    // lastly we determine the id of the nodal dof
    // EXTRA CODE TO DO SOME ERROR CHECKING REQUIRED
    
    Node *theNodePtr = theDomain->getNode(theNode);
	if (theNodePtr == 0) {
		opserr << "DisplacementControl::domainChanged - no node\n";
		return -1;
	}

    DOF_Group *theGroup = theNodePtr->getDOF_GroupPtr();
	if (theGroup == 0) {
	   return 0;
	}
    const ID &theID = theGroup->getID();
    theDofID = theID(theDof);
    
    return 0;
}

int
DisplacementControl::sendSelf(int cTag,
		    Channel &theChannel)
{
    // TO FINISH
  return 0;
}


int
DisplacementControl::recvSelf(int cTag,
		    Channel &theChannel, FEM_ObjectBroker &theBroker)
{
    // TO FINISH
  return 0;
}

void
DisplacementControl::Print(OPS_Stream &s, int flag)
{
    // TO FINISH    
}
