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
                                                                        
// $Revision: 1.3 $
// $Date: 2001-07-04 01:11:52 $
// $Source: /usr/local/cvs/OpenSees/EXAMPLES/TclPlaneTruss/MyTruss.h,v $
                                                                        
                                                                        
#ifndef MyTruss_h
#define MyTruss_h

// Written: fmk 
// Created: 02/99
//
// Description: This file contains the interface for the MyTruss class.
// It defines the class interface and the class attributes. MyTruss 
// provides the abstraction of a simple truss element for 2-d problems.
// The stress-strain relationship for the truss being performed in the
// associated UniaxialMaterial object.
//
// What: "@(#) MyTruss.h, revA"

#include <Element.h>
#include <Matrix.h>
#include <Vector.h>

class Node;
class Channel;
class UniaxialMaterial;

#define ELE_TAG_MyTruss 4002

class MyTruss : public Element
{
  public:
    // constructors
    MyTruss(int tag, 
	    int Nd1, int Nd2, 
	    UniaxialMaterial &theMaterial,
	    double A, double rho=0.0); 

    MyTruss();    
    
    // destructor
    ~MyTruss();

    
    // public methods to obtain inforrmation about dof & connectivity
    int getNumExternalNodes(void) const;
    const ID &getExternalNodes(void);
    int getNumDOF(void);
    void setDomain(Domain *theDomain);

    // public methods to set the state of the element    
    int commitState(void);
    int revertToLastCommit(void);        
    int revertToStart(void);        
    int update(void);

    // public methods to obtain stiffness, mass, damping and residual information    
    const Matrix &getTangentStiff(void);
    const Matrix &getSecantStiff(void);    
    const Matrix &getDamp(void);    
    const Matrix &getMass(void);    

    void zeroLoad(void);	
    int addLoad(const Vector &load);	    
    const Vector &getResistingForce(void);
    const Vector &getResistingForceIncInertia(void);            

    // public methods for output    
    int sendSelf(int commitTag, Channel &theChannel);
    int recvSelf(int commitTag, Channel &theChannel, FEM_ObjectBroker &theBroker);
    int displaySelf(Renderer &theViewer, int displayMode, float fact);    
    void Print(ostream &s, int flag =0);    

    Response *setResponse(char **argv, int argc, Information &eleInfo);
    int getResponse(int responseID, Information &eleInformation);

  protected:
    
  private:
    // private member functions - only available to objects of the class
    double computeCurrentStrain(void) const;
    
    // private attributes - a copy for each object of the class
    UniaxialMaterial *theMaterial;       // pointer to a material
    ID  externalNodes;          	 // contains the id's of end nodes
    Matrix *t;          // hold the transformation matrix, could use a Vector
	                // if ever bother to change the Vector interface for
			// x-product.

    double L;		// length of truss (undeformed configuration) - set in setDomain()
    double A; 		// area of truss
    double M; 		// mass per unit volume of truss
    Node *end1Ptr, *end2Ptr; // two pointers to the nodes - these are set in setDomain()

    Vector load;

    // static data - single copy for all objects of the class
    static Matrix trussK;   // class wide matrix for returning stiffness
    static Matrix trussD;   // class wide matrix for returning damping
    static Matrix trussM;   // class wide matrix for returning mass 
    static Vector trussR;   // class wide vector for returning residual
};
#endif














