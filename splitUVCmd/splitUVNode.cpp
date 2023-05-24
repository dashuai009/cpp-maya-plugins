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
// File: splitUVNode.cpp
//
// Dependency Graph Node: splitUVNode
//
// Authors: Lonnie Li, Jeyprakash Michaelraj
//

#include "splitUVNode.h"

// Function Sets
//
#include <maya/MFnTypedAttribute.h>
#include <maya/MFnMeshData.h>
#include <maya/MFnComponentListData.h>
#include <maya/MFnSingleIndexedComponent.h>

// General Includes
//
#include <maya/MGlobal.h>
#include <maya/MPlug.h>
#include <maya/MDataBlock.h>
#include <maya/MDataHandle.h>
#include <maya/MIOStream.h>

// Macros
//
#define MCheckStatus(status,message)    \
    if( MStatus::kSuccess != status ) { \
        cerr << message << "\n";        \
        return status;                  \
    }


// Unique Node TypeId
//
MTypeId     splitUVNode::id( 0x34567 );

// Node attributes (in addition to inMesh and outMesh defined by polyModifierNode)
//
MObject     splitUVNode::uvList;

splitUVNode::splitUVNode()
{}

splitUVNode::~splitUVNode()
{}

MStatus splitUVNode::compute( const MPlug& plug, MDataBlock& data )
//
//  Description:
//      This method computes the value of the given output plug based
//      on the values of the input attributes.
//
//  Arguments:
//      plug - the plug to compute
//      data - object that provides access to the attributes for this node
//
{
    MStatus status = MS::kSuccess;
 
    MDataHandle stateData = data.outputValue( state, &status );
    MCheckStatus( status, "ERROR getting state" );

    // Check for the HasNoEffect/PassThrough flag on the node.
    //
    // (stateData is an enumeration standard in all depend nodes - stored as short)
    // 
    // (0 = Normal)
    // (1 = HasNoEffect/PassThrough)
    // (2 = Blocking)
    // ...
    //
    if( stateData.asShort() == 1 )
    {
        MDataHandle inputData = data.inputValue( inMesh, &status );
        MCheckStatus(status,"ERROR getting inMesh");

        MDataHandle outputData = data.outputValue( outMesh, &status );
        MCheckStatus(status,"ERROR getting outMesh");

        // Simply redirect the inMesh to the outMesh for the PassThrough effect
        //
        outputData.set(inputData.asMesh());
    }
    else
    {
        // Check which output attribute we have been asked to 
        // compute. If this node doesn't know how to compute it, 
        // we must return MS::kUnknownParameter
        // 
        if (plug == outMesh)
        {
            MDataHandle inputData = data.inputValue( inMesh, &status );
            MCheckStatus(status,"ERROR getting inMesh");

            MDataHandle outputData = data.outputValue( outMesh, &status );
            MCheckStatus(status,"ERROR getting outMesh"); 

            // Now, we get the value of the uvList and use it to perform
            // the operation on this mesh
            //
            MDataHandle inputUVs = data.inputValue( uvList, &status);
            MCheckStatus(status,"ERROR getting uvList"); 
            
            // Copy the inMesh to the outMesh, and now you can
            // perform operations in-place on the outMesh
            //
            outputData.set(inputData.asMesh());
            MObject mesh = outputData.asMesh();

            // Retrieve the UV list from the component list.
            //
            // Note, we use a component list to store the components
            // because it is more compact memory wise. (ie. comp[81:85]
            // is smaller than comp[81], comp[82],...,comp[85])
            //
            MObject compList = inputUVs.data();
            MFnComponentListData compListFn( compList );

            unsigned i;
            int j;
            MIntArray uvIds;

            for( i = 0; i < compListFn.length(); i++ )
            {
                MObject comp = compListFn[i];
                if( comp.apiType() == MFn::kMeshMapComponent )
                {
                    MFnSingleIndexedComponent uvComp( comp );
                    for( j = 0; j < uvComp.elementCount(); j++ )
                    {
                        int uvId = uvComp.element(j);
                        uvIds.append( uvId );
                    }
                }
            }

            // Set the mesh object and uvList on the factory
            //
            fSplitUVFactory.setMesh( mesh );
            fSplitUVFactory.setUVIds( uvIds );

            // Now, perform the splitUV
            //
            status = fSplitUVFactory.doIt();

            // Mark the output mesh as clean
            //
            outputData.setClean();
        }
        else
        {
            status = MS::kUnknownParameter;
        }
    }

    return status;
}

void* splitUVNode::creator()
//
//  Description:
//      this method exists to give Maya a way to create new objects
//      of this type. 
//
//  Return Value:
//      a new object of this type
//
{
    return new splitUVNode();
}

MStatus splitUVNode::initialize()
//
//  Description:
//      This method is called to create and initialize all of the attributes
//      and attribute dependencies for this node type.  This is only called 
//      once when the node type is registered with Maya.
//
//  Return Values:
//      MS::kSuccess
//      MS::kFailure
//      
{
    MStatus             status;

    MFnTypedAttribute attrFn;

    uvList = attrFn.create("inputComponents", "ics", MFnComponentListData::kComponentList);
    attrFn.setStorable(true);   // To be stored during file-save

    inMesh = attrFn.create("inMesh", "im", MFnMeshData::kMesh);
    attrFn.setStorable(true);   // To be stored during file-save

    // Attribute is read-only because it is an output attribute
    //
    outMesh = attrFn.create("outMesh", "om", MFnMeshData::kMesh);
    attrFn.setStorable(false);
    attrFn.setWritable(false);

    // Add the attributes we have created to the node
    //
    status = addAttribute( uvList );
        if (!status)
        {
            status.perror("addAttribute");
            return status;
        }
    status = addAttribute( inMesh );
        if (!status)
        {
            status.perror("addAttribute");
            return status;
        }
    status = addAttribute( outMesh);
        if (!status)
        {
            status.perror("addAttribute");
            return status;
        }

    // Set up a dependency between the input and the output.  This will cause
    // the output to be marked dirty when the input changes.  The output will
    // then be recomputed the next time the value of the output is requested.
    //
    status = attributeAffects( inMesh, outMesh );
        if (!status)
        {
            status.perror("attributeAffects");
            return status;
        }

    status = attributeAffects( uvList, outMesh );
        if (!status)
        {
            status.perror("attributeAffects");
            return status;
        }

    return MS::kSuccess;

}
