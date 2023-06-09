//-
// ==========================================================================
// Copyright 1995,2006,2008 Autodesk, Inc. All rights reserved.
//
// Use of this software is subject to the terms of the Autodesk
// license agreement provided at the time of installation or download,
// or which otherwise accompanies this software in either electronic
// or hard copy form.
// ==========================================================================
//+

//
// DISCLAIMER: THIS PLUGIN IS PROVIDED AS IS.  IT IS NOT SUPPORTED BY
//            AUTODESK, SO PLEASE USE AND MODIFY AT YOUR OWN RISK.
//

// PLUGIN NAME: pointOnMeshInfo v1.0

// FILE: pointOnMeshInfoNode.cpp

// DESCRIPTION: - Defines the "pointOnMeshInfo" node.

// AUTHOR: QT

// REFERENCES: - This plug-in is based on the `pointOnSurface` MEL command and "pointOnSurfaceInfo" node.

//             - The pointOnSubdNode.cpp plug-in example from the Maya Developer Kit was also used for reference.

// LAST UPDATED: Oct. 11th, 2001.

// COMPILED AND TESTED ON: Maya 4.0 on Windows


// HEADER FILES:
#include "pointOnMeshInfoNode.h"
#include "getPointAndNormal.h"
#include <maya/MIOStream.h>


// DEFINE CLASS'S STATIC DATA MEMBERS:
MTypeId pointOnMeshInfoNode::id(0x00105480);
MObject pointOnMeshInfoNode::aInMesh;
MObject pointOnMeshInfoNode::aFaceIndex;
MObject pointOnMeshInfoNode::aRelative;
MObject pointOnMeshInfoNode::aParameterU;
MObject pointOnMeshInfoNode::aParameterV;
MObject pointOnMeshInfoNode::aPosition;
MObject pointOnMeshInfoNode::aPositionX;
MObject pointOnMeshInfoNode::aPositionY;
MObject pointOnMeshInfoNode::aPositionZ;
MObject pointOnMeshInfoNode::aNormal;
MObject pointOnMeshInfoNode::aNormalX;
MObject pointOnMeshInfoNode::aNormalY;
MObject pointOnMeshInfoNode::aNormalZ;


// CONSTRUCTOR DEFINITION:
pointOnMeshInfoNode::pointOnMeshInfoNode()
{
}


// DESTRUCTOR DEFINITION:
pointOnMeshInfoNode::~pointOnMeshInfoNode()
{
}


// FOR CREATING AN INSTANCE OF THIS NODE:
void *pointOnMeshInfoNode::creator()
{
   return new pointOnMeshInfoNode();
}


// INITIALIZES THE NODE BY CREATING ITS ATTRIBUTES:
MStatus pointOnMeshInfoNode::initialize()
{
   // CREATE AND ADD ".inMesh" ATTRIBUTE:
   MFnTypedAttribute inMeshAttrFn;
   aInMesh = inMeshAttrFn.create("inMesh", "im", MFnData::kMesh);
   CHECK_MSTATUS ( inMeshAttrFn.setStorable(true) );
   CHECK_MSTATUS ( inMeshAttrFn.setKeyable(false) );
   CHECK_MSTATUS ( inMeshAttrFn.setReadable(true) );
   CHECK_MSTATUS ( inMeshAttrFn.setWritable(true) );
   CHECK_MSTATUS ( inMeshAttrFn.setCached(false) );
   CHECK_MSTATUS ( addAttribute(aInMesh));

   // CREATE AND ADD ".faceIndex" ATTRIBUTE:
   MFnNumericAttribute faceIndexAttrFn;
   aFaceIndex = faceIndexAttrFn.create("faceIndex", "f", MFnNumericData::kLong, 0);
   CHECK_MSTATUS (faceIndexAttrFn.setStorable(true));
   CHECK_MSTATUS ( faceIndexAttrFn.setKeyable(true) );
   CHECK_MSTATUS ( faceIndexAttrFn.setReadable(true) );
   CHECK_MSTATUS ( faceIndexAttrFn.setWritable(true) );
   CHECK_MSTATUS ( faceIndexAttrFn.setMin(0) );
   CHECK_MSTATUS ( addAttribute(aFaceIndex) );

   // CREATE AND ADD ".relative" ATTRIBUTE:
   MFnNumericAttribute relativeAttrFn;
   aRelative = relativeAttrFn.create("relative", "r", MFnNumericData::kBoolean, 1);
   CHECK_MSTATUS ( relativeAttrFn.setStorable(true) );
   CHECK_MSTATUS ( relativeAttrFn.setKeyable(true));
   CHECK_MSTATUS ( relativeAttrFn.setReadable(true) );
   CHECK_MSTATUS ( relativeAttrFn.setWritable(true) );
   CHECK_MSTATUS ( addAttribute(aRelative) );

   // CREATE AND ADD ".parameterU" ATTRIBUTE:
   MFnNumericAttribute parameterUAttrFn;
   aParameterU = parameterUAttrFn.create("parameterU", "u", MFnNumericData::kDouble, 0.5);
   CHECK_MSTATUS ( parameterUAttrFn.setStorable(true) );
   CHECK_MSTATUS ( parameterUAttrFn.setKeyable(true) );
   CHECK_MSTATUS ( parameterUAttrFn.setReadable(true) ) ;
   CHECK_MSTATUS ( parameterUAttrFn.setWritable(true) ) ;
   CHECK_MSTATUS ( addAttribute(aParameterU) );

   // CREATE AND ADD ".parameterV" ATTRIBUTE:
   MFnNumericAttribute parameterVAttrFn;
   aParameterV = parameterVAttrFn.create("parameterV", "v", MFnNumericData::kDouble, 0.5);
   CHECK_MSTATUS ( parameterVAttrFn.setStorable(true) );
   CHECK_MSTATUS ( parameterVAttrFn.setKeyable(true) );
   CHECK_MSTATUS ( parameterVAttrFn.setReadable(true) );
   CHECK_MSTATUS ( parameterVAttrFn.setWritable(true) );
   CHECK_MSTATUS ( addAttribute(aParameterV) );

   // CREATE AND ADD ".positionX" ATTRIBUTE:
   MFnNumericAttribute pointXAttrFn;
   aPositionX = pointXAttrFn.create("positionX", "px", MFnNumericData::kDouble, 0.0);
   CHECK_MSTATUS ( pointXAttrFn.setStorable(false) );
   CHECK_MSTATUS ( pointXAttrFn.setKeyable(false) );
   CHECK_MSTATUS ( pointXAttrFn.setReadable(true) ) ;
   CHECK_MSTATUS ( pointXAttrFn.setWritable(false) );
   CHECK_MSTATUS ( addAttribute(aPositionX) );

   // CREATE AND ADD ".positionY" ATTRIBUTE:
   MFnNumericAttribute pointYAttrFn;
   aPositionY = pointYAttrFn.create("positionY", "py", MFnNumericData::kDouble, 0.0);
   CHECK_MSTATUS ( pointYAttrFn.setStorable(false) );
   CHECK_MSTATUS ( pointYAttrFn.setKeyable(false) );
   CHECK_MSTATUS ( pointYAttrFn.setReadable(true) );
   CHECK_MSTATUS ( pointYAttrFn.setWritable(false) );
   CHECK_MSTATUS ( addAttribute(aPositionY) );

   // CREATE AND ADD ".positionZ" ATTRIBUTE:
   MFnNumericAttribute pointZAttrFn;
   aPositionZ = pointZAttrFn.create("positionZ", "pz", MFnNumericData::kDouble, 0.0);
   CHECK_MSTATUS ( pointZAttrFn.setStorable(false) );
   CHECK_MSTATUS ( pointZAttrFn.setKeyable(false) );
   CHECK_MSTATUS ( pointZAttrFn.setReadable(true) );
   CHECK_MSTATUS ( pointZAttrFn.setWritable(false) );
   CHECK_MSTATUS ( addAttribute(aPositionZ) );

   // CREATE AND ADD ".position" ATTRIBUTE:
   MFnNumericAttribute pointAttrFn;
   aPosition = pointAttrFn.create("position", "p", aPositionX, aPositionY, aPositionZ);
   CHECK_MSTATUS ( pointAttrFn.setStorable(false) );
   CHECK_MSTATUS ( pointAttrFn.setKeyable(false) );
   CHECK_MSTATUS ( pointAttrFn.setReadable(true) );
   CHECK_MSTATUS ( pointAttrFn.setWritable(false) );
   CHECK_MSTATUS ( addAttribute(aPosition) );

   // CREATE AND ADD ".normalX" ATTRIBUTE:
   MFnNumericAttribute normalXAttrFn;
   aNormalX = normalXAttrFn.create("normalX", "nx", MFnNumericData::kDouble, 0.0);
   CHECK_MSTATUS ( normalXAttrFn.setStorable(false) );
   CHECK_MSTATUS ( normalXAttrFn.setKeyable(false) );
   CHECK_MSTATUS ( normalXAttrFn.setReadable(true) );
   CHECK_MSTATUS ( normalXAttrFn.setWritable(false) );
   CHECK_MSTATUS ( addAttribute(aNormalX) );

   // CREATE AND ADD ".normalY" ATTRIBUTE:
   MFnNumericAttribute normalYAttrFn;
   aNormalY = normalYAttrFn.create("normalY", "ny", MFnNumericData::kDouble, 0.0);
   CHECK_MSTATUS ( normalYAttrFn.setStorable(false) );
   CHECK_MSTATUS ( normalYAttrFn.setKeyable(false) );
   CHECK_MSTATUS ( normalYAttrFn.setReadable(true) );
   CHECK_MSTATUS ( normalYAttrFn.setWritable(false) );
   CHECK_MSTATUS (addAttribute(aNormalY));

   // CREATE AND ADD ".normalZ" ATTRIBUTE:
   MFnNumericAttribute normalZAttrFn;
   aNormalZ = normalZAttrFn.create("normalZ", "nz", MFnNumericData::kDouble, 0.0);
   CHECK_MSTATUS ( normalZAttrFn.setStorable(false));
   CHECK_MSTATUS ( normalZAttrFn.setKeyable(false));
   CHECK_MSTATUS ( normalZAttrFn.setReadable(true));
   CHECK_MSTATUS ( normalZAttrFn.setWritable(false));
   CHECK_MSTATUS ( addAttribute(aNormalZ));

   // CREATE AND ADD ".normal" ATTRIBUTE:
   MFnNumericAttribute normalAttrFn;
   aNormal = normalAttrFn.create("normal", "n", aNormalX, aNormalY, aNormalZ);
   CHECK_MSTATUS ( normalAttrFn.setStorable(false));
   CHECK_MSTATUS ( normalAttrFn.setKeyable(false));
   CHECK_MSTATUS ( normalAttrFn.setReadable(true));
   CHECK_MSTATUS ( normalAttrFn.setWritable(false));
   CHECK_MSTATUS ( addAttribute(aNormal) );

   // DEPENDENCY RELATIONS FOR ".inMesh":
   CHECK_MSTATUS ( attributeAffects(aInMesh, aPosition));
   CHECK_MSTATUS ( attributeAffects(aInMesh, aPositionX));
   CHECK_MSTATUS ( attributeAffects(aInMesh, aPositionY));
   CHECK_MSTATUS ( attributeAffects(aInMesh, aPositionZ));
   CHECK_MSTATUS ( attributeAffects(aInMesh, aNormal));
   CHECK_MSTATUS ( attributeAffects(aInMesh, aNormalX));
   CHECK_MSTATUS ( attributeAffects(aInMesh, aNormalY));
   CHECK_MSTATUS ( attributeAffects(aInMesh, aNormalZ));

   // DEPENDENCY RELATIONS FOR ".faceIndex":
   CHECK_MSTATUS ( attributeAffects(aFaceIndex, aPosition));
   CHECK_MSTATUS ( attributeAffects(aFaceIndex, aPositionX));
   CHECK_MSTATUS ( attributeAffects(aFaceIndex, aPositionY));
   CHECK_MSTATUS ( attributeAffects(aFaceIndex, aPositionZ));
   CHECK_MSTATUS ( attributeAffects(aFaceIndex, aNormal));
   CHECK_MSTATUS ( attributeAffects(aFaceIndex, aNormalX));
   CHECK_MSTATUS (attributeAffects(aFaceIndex, aNormalY));
   CHECK_MSTATUS ( attributeAffects(aFaceIndex, aNormalZ));

   // DEPENDENCY RELATIONS FOR ".relative":
   CHECK_MSTATUS ( attributeAffects(aRelative, aPosition));
   CHECK_MSTATUS ( attributeAffects(aRelative, aPositionX));
   CHECK_MSTATUS ( attributeAffects(aRelative, aPositionY));
   CHECK_MSTATUS (attributeAffects(aRelative, aPositionZ));
   CHECK_MSTATUS ( attributeAffects(aRelative, aNormal));
   CHECK_MSTATUS ( attributeAffects(aRelative, aNormalX));
   CHECK_MSTATUS ( attributeAffects(aRelative, aNormalY));
   CHECK_MSTATUS ( attributeAffects(aRelative, aNormalZ));

   // DEPENDENCY RELATIONS FOR ".parameterU":
   CHECK_MSTATUS (  attributeAffects(aParameterU, aPosition));
   CHECK_MSTATUS ( attributeAffects(aParameterU, aPositionX));
   CHECK_MSTATUS ( attributeAffects(aParameterU, aPositionY));
   CHECK_MSTATUS ( attributeAffects(aParameterU, aPositionZ));
   CHECK_MSTATUS ( attributeAffects(aParameterU, aNormal));
   CHECK_MSTATUS ( attributeAffects(aParameterU, aNormalX));
   CHECK_MSTATUS ( attributeAffects(aParameterU, aNormalY));
   CHECK_MSTATUS ( attributeAffects(aParameterU, aNormalZ));

   // DEPENDENCY RELATIONS FOR ".parameterV":
   CHECK_MSTATUS ( attributeAffects(aParameterV, aPosition));
   CHECK_MSTATUS ( attributeAffects(aParameterV, aPositionX));
   CHECK_MSTATUS ( attributeAffects(aParameterV, aPositionY));
   CHECK_MSTATUS ( attributeAffects(aParameterV, aPositionZ));
   CHECK_MSTATUS ( attributeAffects(aParameterV, aNormal));
   CHECK_MSTATUS ( attributeAffects(aParameterV, aNormalX));
   CHECK_MSTATUS ( attributeAffects(aParameterV, aNormalY));
   CHECK_MSTATUS ( attributeAffects(aParameterV, aNormalZ));

   return MS::kSuccess;
}


// COMPUTE METHOD'S DEFINITION:
MStatus pointOnMeshInfoNode::compute(const MPlug &plug, MDataBlock &data)
{

   // DO THE COMPUTE ONLY FOR THE *OUTPUT* PLUGS THAT ARE DIRTIED:
   if ((plug == aPosition)  || (plug == aPositionX)  || (plug == aPositionY)  || (plug == aPositionZ)
    || (plug == aNormal) || (plug == aNormalX) || (plug == aNormalY) || (plug == aNormalZ))
   {
      // READ IN ".inMesh" DATA:
      MDataHandle inMeshDataHandle = data.inputValue(aInMesh);
      MObject inMesh = inMeshDataHandle.asMesh();

      // READ IN ".faceIndex" DATA:
      MDataHandle faceIndexDataHandle = data.inputValue(aFaceIndex);
      int faceIndex = faceIndexDataHandle.asInt();

      // READ IN ".relative" DATA:
      MDataHandle relativeDataHandle = data.inputValue(aRelative);
      bool relative = relativeDataHandle.asBool();

      // READ IN ".parameterU" DATA:
      MDataHandle parameterUDataHandle = data.inputValue(aParameterU);
      double parameterU = parameterUDataHandle.asDouble();

      // READ IN ".parameterV" DATA:
      MDataHandle parameterVDataHandle = data.inputValue(aParameterV);
      double parameterV = parameterVDataHandle.asDouble();

      // GET THE POINT AND NORMAL:
      MPoint point;
      MVector normal;
      MDagPath dummyDagPath;
      getPointAndNormal(dummyDagPath, faceIndex, relative, parameterU, parameterV, point, normal, inMesh);

      // WRITE OUT ".position" DATA:
      MDataHandle pointDataHandle = data.outputValue(aPosition);
      pointDataHandle.set(point.x, point.y, point.z);
      data.setClean(plug);

      // WRITE OUT ".normal" DATA:
      MDataHandle normalDataHandle = data.outputValue(aNormal);
      normalDataHandle.set(normal.x, normal.y, normal.z);
      data.setClean(plug);
   }
   else
      return MS::kUnknownParameter;

   return MS::kSuccess;
}
