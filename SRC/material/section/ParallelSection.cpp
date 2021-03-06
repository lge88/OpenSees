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
// $Date: 2008-08-26 16:48:13 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/section/ParallelSection.cpp,v $
                                                                        
                                                                        
// File: ~/section/ParallelSection.C
//
// Written: MHS
// Created: June 2000
// Revision: A
//
// Purpose: This file contains the implementation for the ParallelSection class. 

#include <stdlib.h>
#include <string.h>

#include <Channel.h>
#include <FEM_ObjectBroker.h>
#include <Vector.h>
#include <Matrix.h>
#include <MatrixUtil.h>
#include <classTags.h>
#include <ParallelSection.h>
#include <MaterialResponse.h>
#include <ID.h>

#include <classTags.h>

#define maxOrder 10

// Assumes section order is less than or equal to maxOrder.
// Can increase if needed!!!
double ParallelSection::workArea[2*maxOrder*(maxOrder+1)];
int    ParallelSection::codeArea[maxOrder];

// constructors:
ParallelSection::ParallelSection (int tag, SectionForceDeformation &theSec,
				      int numAdds, UniaxialMaterial **theAdds,
				      const ID &addCodes): 
  SectionForceDeformation(tag, SEC_TAG_Aggregator), 
  theSection(0), theAdditions(0), matCodes(0), numMats(numAdds),
  e(0), s(0), ks(0), fs(0), theCode(0),
  otherDbTag(0)
{
    theSection = theSec.getCopy();
    
    if (!theSection) {
      opserr << "ParallelSection::ParallelSection -- failed to get copy of section\n";
      exit(-1);
    }

    if (!theAdds) {
      opserr << "ParallelSection::ParallelSection -- null uniaxial material array passed\n";
      exit(-1);
    }
    theAdditions = new UniaxialMaterial *[numMats];

    if (!theAdditions) {
      opserr << "ParallelSection::ParallelSection -- failed to allocate pointers\n";
      exit(-1);
    }    
    int i;
    
    for (i = 0; i < numMats; i++) {
      if (!theAdds[i]) {
	opserr << "ParallelSection::ParallelSection -- null uniaxial material pointer passed\n";
	exit(-1);
      }	
      theAdditions[i] = theAdds[i]->getCopy();
      
      if (!theAdditions[i]) {
	opserr << "ParallelSection::ParallelSection -- failed to copy uniaxial material\n";
	exit(-1);
      }
    }

    int order = theSec.getOrder();

    if (order > maxOrder) {
      opserr << "ParallelSection::ParallelSection -- order too big, need to modify the #define in ParallelSection.cpp to " <<
	order << endln;
      exit(-1);
    }

    theCode = new ID(codeArea, order);
    e = new Vector(workArea, order);
    s = new Vector(&workArea[maxOrder], order);
    ks = new Matrix(&workArea[2*maxOrder], order, order);
    fs = new Matrix(&workArea[maxOrder*(maxOrder+2)], order, order);
    matCodes = new ID(addCodes);

    if (theCode == 0 || e == 0 || s == 0 || ks == 0 || fs == 0 || matCodes == 0) {
      opserr << "ParallelSection::ParallelSection -- out of memory\n";
      exit(-1);
    }	
}

ParallelSection::ParallelSection (int tag, SectionForceDeformation &theSec,
				      UniaxialMaterial &theAddition, int c) :
  SectionForceDeformation(tag, SEC_TAG_Aggregator),
  theSection(0), theAdditions(0), matCodes(0), numMats(1),
  e(0), s(0), ks(0), fs(0), theCode(0),
  otherDbTag(0)
{
  theSection = theSec.getCopy();
  
  if (!theSection) {
    opserr << "ParallelSection::ParallelSection -- failed to get copy of section\n";
    exit(-1);
  }

  theAdditions = new UniaxialMaterial *[1];
  
  theAdditions[0] = theAddition.getCopy();
  
  if (!theAdditions[0]) {
    opserr << "ParallelSection::ParallelSection -- failed to copy uniaxial material\n";
    exit(-1);
  }
    
  matCodes = new ID(1);
  (*matCodes)(0) = c;
  
  int order = theSec.getOrder();
  
  if (order > maxOrder) {
    opserr << "ParallelSection::ParallelSection -- order too big, need to modify the #define in ParallelSection.cpp to %d\n";
    exit(-1);
  }
  
  theCode = new ID(codeArea, order);
  e = new Vector(workArea, order);
  s = new Vector(&workArea[maxOrder], order);
  ks = new Matrix(&workArea[2*maxOrder], order, order);
  fs = new Matrix(&workArea[maxOrder*(maxOrder+2)], order, order);
  
  if (theCode == 0 || e == 0 || s == 0 || ks == 0 || fs == 0 || matCodes == 0) {
    opserr << "ParallelSection::ParallelSection -- out of memory\n";
    exit(-1);
  }
}

// constructor for blank object that recvSelf needs to be invoked upon
ParallelSection::ParallelSection():
  SectionForceDeformation(0, SEC_TAG_Aggregator),
  theSection(0), theAdditions(0), matCodes(0), numMats(0), 
  e(0), s(0), ks(0), fs(0), theCode(0),
  otherDbTag(0)
{

}

// destructor:
ParallelSection::~ParallelSection()
{
   int i;

   if (theSection)
       delete theSection;

   for (i = 0; i < numMats; i++)
       if (theAdditions[i])
	   delete theAdditions[i];

   if (theAdditions)
       delete [] theAdditions;
   
   if (e != 0)
     delete e;

   if (s != 0)
     delete s;

   if (ks != 0)
     delete ks;

   if (fs != 0)
     delete fs;

   if (theCode != 0)
     delete theCode;

   if (matCodes != 0)
     delete matCodes;
}

int ParallelSection::setTrialSectionDeformation (const Vector &def)
{
  int ret = 0;

  // Set deformation in section
  ret = theSection->setTrialSectionDeformation(def);

  int order = theSection->getOrder();
  const ID &type = theSection->getType();

  // Now send deformations to uniaxial materials with same code
  for (int j = 0; j < order; j++) {
    int code = type(j);
    for (int i = 0; i < numMats; i++) {
      if (code == (*matCodes)[i])
	ret += theAdditions[i]->setTrialStrain(def(j));
    }
  }

  return ret;
}

const Vector &
ParallelSection::getSectionDeformation(void)
{
  *e = theSection->getSectionDeformation();

  return *e;
}

const Matrix &
ParallelSection::getSectionTangent(void)
{
  *ks = theSection->getSectionTangent();

  int order = theSection->getOrder();
  const ID &type = theSection->getType();

  for (int j = 0; j < order; j++) {
    int code = type(j);
    for (int i = 0; i < numMats; i++) {
      if (code == (*matCodes)[i])
	(*ks)(j,j) += theAdditions[i]->getTangent();
    }
  }
  
  return *ks;
}

const Matrix &
ParallelSection::getInitialTangent(void)
{
  *ks = theSection->getInitialTangent();

  int order = theSection->getOrder();
  const ID &type = theSection->getType();

  for (int j = 0; j < order; j++) {
    int code = type(j);
    for (int i = 0; i < numMats; i++) {
      if (code == (*matCodes)[i])
	(*ks)(j,j) += theAdditions[i]->getInitialTangent();
    }
  }
  
  return *ks;
}

const Vector &
ParallelSection::getStressResultant(void)
{
  *s = theSection->getStressResultant();
    
  int order = theSection->getOrder();
  const ID &type = theSection->getType();

  for (int j = 0; j < order; j++) {
    int code = type(j);
    for (int i = 0; i < numMats; i++) {
      if (code == (*matCodes)[i])
	(*s)(j) += theAdditions[i]->getStress();
    }
  }

  return *s;
}

SectionForceDeformation *
ParallelSection::getCopy(void)
{
  ParallelSection *theCopy = 0;
    
  theCopy = new ParallelSection(this->getTag(), *theSection,
				numMats, theAdditions, *matCodes);
  
  if (theCopy == 0) {
    opserr << "ParallelSection::getCopy -- failed to allocate copy\n";
    exit(-1);
  }
			  
  
  return theCopy;
}

const ID&
ParallelSection::getType ()
{
  return theSection->getType();
}

int
ParallelSection::getOrder () const
{
  return theSection->getOrder();
}

int
ParallelSection::commitState(void)
{
  int err = 0;
    
  if (theSection)
    err += theSection->commitState();
  
  for (int i = 0; i < numMats; i++)
    err += theAdditions[i]->commitState();
  
  return err;
}

int
ParallelSection::revertToLastCommit(void)
{
  int err = 0;
  
  int i = 0;
  
  // Revert the section
  if (theSection)
    err += theSection->revertToLastCommit();
  
  // Do the same for the uniaxial materials
  for (i = 0; i < numMats; i++)
    err += theAdditions[i]->revertToLastCommit();
  
  return err;
}	

int
ParallelSection::revertToStart(void)
{
  int err = 0;
  
  // Revert the section
  if (theSection)
    err += theSection->revertToStart();
  
  // Do the same for the uniaxial materials
  for (int i = 0; i < numMats; i++)
    err += theAdditions[i]->revertToStart();
  
  return err;
}

int
ParallelSection::sendSelf(int cTag, Channel &theChannel)
{
  int res = 0;

  // Need otherDbTag since classTags ID and data ID may be the same size
  if (otherDbTag == 0) 
    otherDbTag = theChannel.getDbTag();
  
  // Create ID for tag and section order data
  static ID data(5);
  
  int order = this->getOrder();
  
  data(0) = this->getTag();
  data(1) = otherDbTag;
  data(2) = order;
  data(3) = (theSection != 0) ? theSection->getOrder() : 0;
  data(4) = numMats;

  // Send the tag and section order data
  res += theChannel.sendID(this->getDbTag(), cTag, data);
  if (res < 0) {
    opserr << "ParallelSection::sendSelf -- could not send data ID\n";
			    
    return res;
  }
  
  // Determine how many classTags there are and allocate ID vector
  // for the tags and section code
  int numTags = (theSection == 0) ? numMats : numMats + 1;
  ID classTags(2*numTags + numMats);
  
  // Loop over the UniaxialMaterials filling in class and db tags
  int i, dbTag;
  for (i = 0; i < numMats; i++) {
    classTags(i) = theAdditions[i]->getClassTag();
    
    dbTag = theAdditions[i]->getDbTag();
    
    if (dbTag == 0) {
      dbTag = theChannel.getDbTag();
      if (dbTag != 0)
	theAdditions[i]->setDbTag(dbTag);
    }
    
    classTags(i+numTags) = dbTag;
  }
  
  // Put the Section class and db tags into the ID vector
  if (theSection != 0) {
    classTags(numTags-1) = theSection->getClassTag();
    
    dbTag = theSection->getDbTag();
    
    if (dbTag == 0) {
      dbTag = theChannel.getDbTag();
      if (dbTag != 0)
	theSection->setDbTag(dbTag);
    }
    
    classTags(2*numTags-1) = dbTag;
  }
  
  // Put the UniaxialMaterial codes into the ID vector
  int j = 2*numTags;
  for (i = 0; i < numMats; i++, j++)
    classTags(j) = (*matCodes)(i);
  
  // Send the material class and db tags and section code
  res += theChannel.sendID(otherDbTag, cTag, classTags);
  if (res < 0) {
    opserr << "ParallelSection::sendSelf -- could not send classTags ID\n";
    return res;
  }

  // Ask the UniaxialMaterials to send themselves
  for (i = 0; i < numMats; i++) {
    res += theAdditions[i]->sendSelf(cTag, theChannel);
    if (res < 0) {
      opserr << "ParallelSection::sendSelf -- could not send UniaxialMaterial, i = " << i << endln;
      return res;
    }
  }
  
  // Ask the Section to send itself
  if (theSection != 0) {
    res += theSection->sendSelf(cTag, theChannel);
    if (res < 0) {
      opserr << "ParallelSection::sendSelf -- could not send SectionForceDeformation\n";
      return res;
    }
  }
  
  return res;
}


int
ParallelSection::recvSelf(int cTag, Channel &theChannel, FEM_ObjectBroker &theBroker)
{
  int res = 0;

  // Create an ID and receive tag and section order
  static ID data(5);
  res += theChannel.recvID(this->getDbTag(), cTag, data);
  if (res < 0) {
    opserr << "ParallelSection::recvSelf -- could not receive data ID\n";
    return res;
  }
  
  this->setTag(data(0));
  otherDbTag = data(1);
  int order = data(2);
  int theSectionOrder = data(3);
  numMats = data(4);

  if (order > 0) {
    if (e == 0 || e->Size() != order) {
      if (e != 0) {
	delete e;
	delete s;
	delete ks;
	delete fs;
	delete theCode;
      }
      e = new Vector(workArea, order);
      s = new Vector(&workArea[maxOrder], order);
      ks = new Matrix(&workArea[2*maxOrder], order, order);
      fs = new Matrix(&workArea[maxOrder*(maxOrder+2)], order, order);
      theCode = new ID(codeArea, order);
    }
  }

  if (numMats > 0) {
    if (matCodes == 0 || matCodes->Size() != numMats) {
      if (matCodes != 0)
	delete matCodes;

      matCodes = new ID(numMats);
    }
  }

  // Determine how many classTags there are and allocate ID vector
  int numTags = (theSectionOrder == 0) ? numMats : numMats + 1;
  ID classTags(numTags*2 + numMats);
  
  // Receive the material class and db tags
  res += theChannel.recvID(otherDbTag, cTag, classTags);
  if (res < 0) {
    opserr << "ParallelSection::recvSelf -- could not receive classTags ID\n";
    return res;
  }

  // Check if null pointer, allocate if so
  if (theAdditions == 0) {
    theAdditions = new UniaxialMaterial *[numMats];
    if (theAdditions == 0) {
      opserr << "ParallelSection::recvSelf -- could not allocate UniaxialMaterial array\n";
      return -1;
    }
    // Set pointers to null ... will get allocated by theBroker
    for (int j = 0; j < numMats; j++)
      theAdditions[j] = 0;
  }
  
  // Loop over the UniaxialMaterials
  int i, classTag;
  for (i = 0; i < numMats; i++) {
    classTag = classTags(i);
    
    // Check if the UniaxialMaterial is null; if so, get a new one
    if (theAdditions[i] == 0)
      theAdditions[i] = theBroker.getNewUniaxialMaterial(classTag);
    
    // Check that the UniaxialMaterial is of the right type; if not, delete
    // the current one and get a new one of the right type
    else if (theAdditions[i]->getClassTag() != classTag) {
      delete theAdditions[i];
      theAdditions[i] = theBroker.getNewUniaxialMaterial(classTag);
    }
    
    // Check if either allocation failed
    if (theAdditions[i] == 0) {
      opserr << "ParallelSection::recvSelf -- could not get UniaxialMaterial, i = " << i << endln;
      return -1;
    }
    
    // Now, receive the UniaxialMaterial
    theAdditions[i]->setDbTag(classTags(i+numTags));
    res += theAdditions[i]->recvSelf(cTag, theChannel, theBroker);
    if (res < 0) {
      opserr << "ParallelSection::recvSelf -- could not receive UniaxialMaterial, i = " << i << endln;
      return res;
    }
  }

  // If there is no Section to receive, return
  if (theSectionOrder == 0)
    return res;
  
  classTag = classTags(numTags-1);
  
  // Check if the Section is null; if so, get a new one
  if (theSection == 0)
    theSection = theBroker.getNewSection(classTag);
  
  // Check that the Section is of the right type; if not, delete
  // the current one and get a new one of the right type
  else if (theSection->getClassTag() != classTag) {
    delete theSection;
    theSection = theBroker.getNewSection(classTag);
  }
  
  // Check if either allocation failed
  if (theSection == 0) {
    opserr << "ParallelSection::recvSelf -- could not get a SectionForceDeformation\n";
    return -1;
  }

  // Now, receive the Section
  theSection->setDbTag(classTags(2*numTags-1));
  res += theSection->recvSelf(cTag, theChannel, theBroker);
  if (res < 0) {
    opserr << "ParallelSection::recvSelf -- could not receive SectionForceDeformation\n";
    return res;
  }
  
  // Fill in the section code
  int j = 2*numTags;
  for (i = 0; i < numMats; i++, j++)
    (*matCodes)(i) = classTags(j);

  return res;
}

void
ParallelSection::Print(OPS_Stream &s, int flag)
{
  if (flag == 2) {
      theSection->Print(s, flag);
  } else {
    s << "\nSection Aggregator, tag: " << this->getTag() << endln;
    if (theSection) {
      s << "\tSection, tag: " << theSection->getTag() << endln;
      theSection->Print(s, flag);
    }
    s << "\tUniaxial Additions" << endln;
    for (int i = 0; i < numMats; i++)
      s << "\t\tUniaxial Material, tag: " << theAdditions[i]->getTag() << endln;
    s << "\tUniaxial codes " << *matCodes << endln;
  }
}

Response*
ParallelSection::setResponse(const char **argv, int argc, OPS_Stream &output)
{
  // See if the response is one of the defaults
  Response *res = SectionForceDeformation::setResponse(argv, argc, output);
  if (res != 0)
    return res;
  
  // If not, forward the request to the section (need to do this to get fiber response)
  // CURRENTLY NOT SENDING ANYTHING OFF TO THE UniaxialMaterials ... Probably
  // don't need anything more from them than stress, strain, and stiffness, 
  // which are covered in base class method ... can change if need arises
  else if (theSection != 0)
    return theSection->setResponse(argv, argc, output);
  
  else
    return 0;
}

int
ParallelSection::getResponse(int responseID, Information &info)
{
  // Just call the base class method ... don't need to define
  // this function, but keeping it here just for clarity
  return SectionForceDeformation::getResponse(responseID, info);
}

int
ParallelSection::getVariable(const char *argv, Information &info)
{
  int i;

  info.theDouble = 0.0;

  int order = numMats;
  if (theSection != 0)
    order += theSection->getOrder();

  const Vector &e = this->getSectionDeformation();
  const ID &code  = this->getType();

  if (strcmp(argv,"axialStrain") == 0) {
    // Series model ... add all sources of deformation
    for (i = 0; i < order; i++)
      if (code(i) == SECTION_RESPONSE_P)
	info.theDouble += e(i);
  } else if (strcmp(argv,"curvatureZ") == 0) {
    for (i = 0; i < order; i++)
      if (code(i) == SECTION_RESPONSE_MZ)
	info.theDouble += e(i);
  }  else if (strcmp(argv,"curvatureY") == 0) {
    for (i = 0; i < order; i++)
      if (code(i) == SECTION_RESPONSE_MY)
	info.theDouble += e(i);
  } else
    return -1;

  return 0;
}

// AddingSensitivity:BEGIN ////////////////////////////////////
int
ParallelSection::setParameter(const char **argv, int argc, Parameter &param)
{
  if (argc < 1)
    return -1;

  // Check if the parameter belongs to the material (only option for now)
  if (strstr(argv[0],"material") != 0) {
    
    if (argc < 3)
      return -1;

    // Get the tag of the material
    int materialTag = atoi(argv[1]);
    
    // Loop to find the right material
    int ok = 0;
    for (int i = 0; i < numMats; i++)
      if (materialTag == theAdditions[i]->getTag())
	ok += theAdditions[i]->setParameter(&argv[2], argc-2, param);

    return ok;
  } 
  
  else if (theSection != 0)
    return theSection->setParameter(argv, argc, param);

  else {
    opserr << "ParallelSection::setParameter() - could not set parameter. " << endln;
    return -1;
  }
}

const Vector &
ParallelSection::getSectionDeformationSensitivity(int gradIndex)
{
  s->Zero();

  return *s;
}

const Vector &
ParallelSection::getStressResultantSensitivity(int gradIndex,
					       bool conditional)
{
  int i = 0;

  int theSectionOrder = 0;
    
  if (theSection) {
    const Vector &dsdh = theSection->getStressResultantSensitivity(gradIndex,
								   conditional);
    theSectionOrder = theSection->getOrder();
    
    for (i = 0; i < theSectionOrder; i++)
      (*s)(i) = dsdh(i);
  }
  
  int order = theSectionOrder + numMats;

  for ( ; i < order; i++)
    (*s)(i) = theAdditions[i-theSectionOrder]->getStressSensitivity(gradIndex,
								    conditional);
  
  return *s;
}

const Matrix &
ParallelSection::getSectionTangentSensitivity(int gradIndex)
{
  ks->Zero();
  
  return *ks;
}

int
ParallelSection::commitSensitivity(const Vector& defSens,
				     int gradIndex, int numGrads)
{
  int ret = 0;
  int i = 0;

  dedh = defSens;

  int theSectionOrder = 0;

  if (theSection) {
    theSectionOrder = theSection->getOrder();
    Vector dedh(workArea, theSectionOrder);
    
    for (i = 0; i < theSectionOrder; i++)
      dedh(i) = defSens(i);
    
    ret = theSection->commitSensitivity(dedh, gradIndex, numGrads);
  }

  int order = theSectionOrder + numMats;
  
  for ( ; i < order; i++)
    ret += theAdditions[i-theSectionOrder]->commitSensitivity(defSens(i),
							      gradIndex,
							      numGrads);
  
  return ret;
}

// AddingSensitivity:END ///////////////////////////////////

const Vector&
ParallelSection::getdedh(void)
{
  return dedh;
}
