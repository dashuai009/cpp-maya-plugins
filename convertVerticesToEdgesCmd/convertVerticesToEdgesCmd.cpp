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

// DISCLAIMER: THIS PLUGIN IS PROVIDED AS IS.  IT IS NOT SUPPORTED BY

//            AUTODESK, SO PLEASE USE AND MODIFY AT YOUR OWN RISK.

//

// PLUGIN NAME: Convert Vertices To Contained Edges Command v1.0

// FILE: convertVerticesToEdgesCmd.cpp

// DESCRIPTION: o MEL command that converts a selection of vertices into a selection of edges that interconnect

//                the original vertices (i.e. only edges whose composite vertices are contained in the original

//                vertex selection).  The command's return value is a string array that contains the names of

//                all of the new contained edges.

//              o This MEL command has no flags, returns a string array, and operates on selected vertices.

//              o Example MEL usage:

//                   select -r pCube1.vtx[2:5];

//                   string $convertedEdges[] = `convertVerticesToEdges`;

//                   // Result: pCube1.e[1:2] pCube1.e[6:7] //

// AUTHOR: QT

// LAST UPDATED: Oct. 11th, 2001.

// TESTED ON: Maya 4.0 on Windows

// STEPS TO INSTALL: 1) Unzip the file convertVerticesToEdges_v1_0_Maya4_Windows.zip to any temporary directory.

//                   2) Copy the file convertVerticesToEdgesCmd.mll to: /AW/Maya4.0/bin/plug-ins/

//                   3) Launch Maya and load the plugin from Windows->Setting/Preferences->Plugin Manager.





// BEGIN PLUGIN:

// =============





// HEADER FILES:

#include <maya/MFnPlugin.h>

#include <maya/MPxCommand.h>

#include <maya/MGlobal.h>

#include <maya/MSelectionList.h>

#include <maya/MItSelectionList.h>

#include <maya/MObject.h>

#include <maya/MDagPath.h>

#include <maya/MItMeshVertex.h>

#include <maya/MItMeshEdge.h>





// MAIN CLASS FOR THE MEL COMMAND:

class convertVerticesToContainedEdgesCommand : public MPxCommand

{

   public:

      convertVerticesToContainedEdgesCommand();

      ~convertVerticesToContainedEdgesCommand() override;

      static void* creator();

      bool isUndoable() const override;



      MStatus doIt(const MArgList&) override;

      MStatus redoIt() override;

      MStatus undoIt() override;



   private:

      MSelectionList previousSelectionList;

};





// CONSTRUCTOR:

convertVerticesToContainedEdgesCommand::convertVerticesToContainedEdgesCommand()

{

}





// DESTRUCTOR CLEARS, AND FREES MEMORY OF THE PREVIOUS SELECTION LIST:

convertVerticesToContainedEdgesCommand::~convertVerticesToContainedEdgesCommand()

{

   previousSelectionList.clear();

}





// FOR CREATING AN INSTANCE OF THIS COMMAND:

void* convertVerticesToContainedEdgesCommand::creator()

{

   return new convertVerticesToContainedEdgesCommand;

}





// MAKE THIS COMMAND UNDOABLE:

bool convertVerticesToContainedEdgesCommand::isUndoable() const

{

   return true;

}





// SAVE THE PREVIOUS SELECTION, AND THEN CALL redoIt(), WHICH DOES MOST OF THE WORK:

MStatus convertVerticesToContainedEdgesCommand::doIt(const MArgList& args)

{

   MGlobal::getActiveSelectionList(previousSelectionList);

   return redoIt();

}





// CONVERTS THE SELECTED VERTICES TO "CONTAINED EDGES", AND RETURN FROM THE MEL COMMAND, A STRING ARRAY OF THE NEW EDGES:

MStatus convertVerticesToContainedEdgesCommand::redoIt()

{

   MSelectionList finalEdgesSelection;

   MDagPath meshDagPath;

   MObject multiVertexComponent, singleVertexComponent;

   int dummyIndex;



   // ITERATE THROUGH EACH "VERTEX COMPONENT" THAT IS CURRENTLY SELECTED:

   for (MItSelectionList vertexComponentIter(previousSelectionList, MFn::kMeshVertComponent); !vertexComponentIter.isDone(); vertexComponentIter.next())

   {

      // STORE THE DAGPATH, COMPONENT OBJECT AND MESH NAME OF THE CURRENT VERTEX COMPONENT:

      vertexComponentIter.getDagPath(meshDagPath, multiVertexComponent);

      MString meshName = meshDagPath.fullPathName();



      // VERTEX COMPONENT HAS TO CONTAIN AT LEAST ONE VERTEX:

      if (!multiVertexComponent.isNull())

      {

         // ITERATE THROUGH EACH "VERTEX" IN THE CURRENT VERTEX COMPONENT:

         for (MItMeshVertex vertexIter(meshDagPath, multiVertexComponent); !vertexIter.isDone(); vertexIter.next())

         {

            // FOR STORING THE EDGES CONNECTED TO EACH VERTEX:

            MIntArray connectedEdgesIndices;

            vertexIter.getConnectedEdges(connectedEdgesIndices);



            // ITERATE THROUGH EACH EDGE CONNECTED TO THE CURRENT VERTEX:

            MItMeshEdge edgeIter(meshDagPath);

            for (unsigned i=0; i<connectedEdgesIndices.length(); i++)

            {

               // FIND AND STORE THE *FIRST* "END VERTEX" OF THE CURRENT EDGE:

               edgeIter.setIndex(connectedEdgesIndices[i], dummyIndex);

               MSelectionList singleVertexList;

               MString vertexName = meshName;

               vertexName += ".vtx[";

               vertexName += edgeIter.index(0);

               vertexName += "]";

               singleVertexList.add(vertexName);

               singleVertexList.getDagPath(0, meshDagPath, singleVertexComponent);

               // SEE WHETHER THE VERTEX BELONGS TO THE ORIGINAL SELECTION, AND IF IT DOES PROCEED TO CHECK NEXT END VERTEX:

               if (!singleVertexComponent.isNull() && previousSelectionList.hasItem(meshDagPath, singleVertexComponent))

               {

                  // FIND AND STORE THE *SECOND* "END VERTEX" OF THE CURRENT EDGE:

                  singleVertexList.clear();

                  vertexName = meshName;

                  vertexName += ".vtx[";

                  vertexName += edgeIter.index(1);

                  vertexName += "]";

                  singleVertexList.add(vertexName);

                  singleVertexList.getDagPath(0, meshDagPath, singleVertexComponent);

                  // SEE WHETHER THE VERTEX BELONGS TO THE ORIGINAL SELECTION, AND IF IT DOES, ADD THE EDGE TO THE FINAL CONTAINED EDGES LIST:

                  if (!singleVertexComponent.isNull() && previousSelectionList.hasItem(meshDagPath, singleVertexComponent))

                  {

                     MString edgeName = meshName;

                     edgeName += ".e[";

                     edgeName += connectedEdgesIndices[i];

                     edgeName += "]";

                     finalEdgesSelection.add(edgeName);

                  }

               }

            }

         }

      }

   }



   // FINALLY, MAKE THE NEW "CONTAINED EDGES", THE CURRENT SELECTION:

   MGlobal::setActiveSelectionList(finalEdgesSelection, MGlobal::kReplaceList);



   // RETURN NEW CONTAINED EDGES LIST FROM THE MEL COMMAND, AS AN ARRAY OF STRINGS:

   MStringArray containedEdgesArray;

   finalEdgesSelection.getSelectionStrings(containedEdgesArray);

   MPxCommand::setResult(containedEdgesArray);



   return MS::kSuccess;

}





// TO UNDO THE COMMAND, SIMPLY RESTORE THE ORIGINAL SELECTION BEFORE THE COMMAND WAS INVOKED:

MStatus convertVerticesToContainedEdgesCommand::undoIt()

{

   MGlobal::setActiveSelectionList(previousSelectionList, MGlobal::kReplaceList);

   return MS::kSuccess;

}





// INITIALIZES THE PLUGIN:

MStatus initializePlugin(MObject obj)

{

   MStatus status;

   MFnPlugin plugin(obj, PLUGIN_COMPANY, "4.0", "Any");



   status = plugin.registerCommand("convertVerticesToEdges", convertVerticesToContainedEdgesCommand::creator);

   if (!status)

   {

      status.perror("registerCommand");

      return status;

   }



   return status;

}





// UNINITIALIZES THE PLUGIN:

MStatus uninitializePlugin(MObject obj)

{

   MStatus status;

   MFnPlugin plugin(obj);



   status = plugin.deregisterCommand("convertVerticesToEdges");

   if (!status)

   {

      status.perror("deregisterCommand");

      return status;

   }



   return status;

}





// ============================================================================================================== END PLUGIN.

