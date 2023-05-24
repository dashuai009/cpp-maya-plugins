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

/* 

    This example is based on the squareScaleManip example but uses
    a context and context command. Template classes are used
    for defining the context and context command below. If the
    plug-in, context is active, selecting geometry will show
    the manipulator.  Only the right and left sides of the
    square currently modify the geometry if moved.

    Loading and unloading:
    ----------------------

    The square scale manipulator context and tool button can be created with the 
    following MEL commands:

        loadPlugin squareScaleManipContext;
        squareScaleManipContext squareScaleManipContext1;
        setParent Shelf1;
        toolButton -cl toolCluster
                    -i1 "moveManip.xpm"
                    -t squareScaleManipContext1
                    squareManip1;

    If the preceding commands were used to create the manipulator context, 
    the following commands can destroy it:

        deleteUI squareScaleManipContext1;
        deleteUI squareManip1;

    If the plug-in is loaded and unloaded frequently (eg. during testing),
    it is useful to make these command sequences into shelf buttons.

    How to use:
    -----------

    Once the tool button has been created using the script above, select the
    tool button then click on an object. Move the right and left edges of the
    square to modify the selected object's scale.

    There is code duplication between this example and squareSclaeManip.  But
    the important additions here are the calls to addDoubleValue() and
    addDoubleValue() and the virtual connectToDependNode(). This functionality
    ties the updating of the manipulator into changing a node's attribute(s).
    
*/

#include <maya/MIOStream.h>

#include <maya/MPxNode.h>
#include <maya/MPxManipulatorNode.h>
#include <maya/MPxSelectionContext.h>
#include <maya/MPxContextCommand.h>
#include <maya/MModelMessage.h>
#include <maya/MFnPlugin.h>
#include <maya/MGlobal.h>
#include <maya/MItSelectionList.h>
#include <maya/MPoint.h>
#include <maya/MVector.h>
#include <maya/MDagPath.h>
#include <maya/MManipData.h>
#include <maya/MHardwareRenderer.h>
#include <maya/MFnDependencyNode.h>
#include <maya/MFnTransform.h>
#include <maya/MMatrix.h>
#include <maya/MFnCamera.h>
#include <maya/MTemplateCommand.h>
#include <maya/MTemplateManipulator.h>

#include "squareScaleManipContext.h"

// Statics
MTypeId squareScaleManipulator::id( 0x81048 );
const MPoint squareScaleManipulator::topLeft(-0.5f, 0.5f, 0.0f);
const MPoint squareScaleManipulator::topRight(0.5f, 0.5f, 0.0f);
const MPoint squareScaleManipulator::bottomLeft(-0.5f, -0.5f, 0.0f);
const MPoint squareScaleManipulator::bottomRight(0.5f, -0.5f, 0.0f);

//
// class implementation
//
squareScaleManipulator::squareScaleManipulator()
    : MPxManipulatorNode()
    , fActiveName(0)
    , fTopName(0)
    , fRightName(0)
    , fBottomName(0)
    , fLeftName(0)
{
    // Populate initial points
    topLeft.get(fTopLeft);
    topRight.get(fTopRight);
    bottomLeft.get(fBottomLeft);
    bottomRight.get(fBottomRight);
    // Setup the plane with a point on the
    // plane along with a normal
    MPoint pointOnPlane(topLeft);
    // Normal = cross product of two vectors on the plane
    MVector normalToPlane =
        (MVector(topLeft) - MVector(topRight)) ^
        (MVector(topRight) - MVector(bottomRight));
    // Necessary to normalize
    normalToPlane.normalize();
    // Plane defined by a point and a normal
    fPlane.setPlane(pointOnPlane, normalToPlane);
}

squareScaleManipulator::~squareScaleManipulator()
{
}

void squareScaleManipulator::postConstructor()
{
    // Get the starting value of the pickable items
    MGLuint glPickableItem;
    glFirstHandle(glPickableItem);

    fTopName = glPickableItem++;
    fBottomName = glPickableItem++;
    fLeftName = glPickableItem++;
    fRightName = glPickableItem++;
}

//virtual 
void squareScaleManipulator::preDrawUI(const M3dView &view)
{
    M3dView *viewPtr = const_cast<M3dView*>(&view);
    MDagPath dpath;
    viewPtr->getCamera(dpath);
    fDrawManip = shouldDraw(dpath);
}

//virtual 
void squareScaleManipulator::drawUI(MHWRender::MUIDrawManager& drawManager, const MHWRender::MFrameContext& frameContext) const
{
    if (!fDrawManip)
        return;

    const short defaultCol = mainColor();
    drawManager.beginDrawable(MHWRender::MUIDrawManager::kSelectable, fTopName);
    setHandleColor(drawManager, fTopName, defaultCol);
    drawManager.line(MPoint(fTopLeft), MPoint(fTopRight));
    drawManager.endDrawable();

    // Let's make the bottom line not selectable
    MPoint center = (MPoint(fBottomLeft) + MPoint(fBottomRight))*0.5;
    drawManager.beginDrawable();
    drawManager.setColor(MColor(1.0f, 0.0f, 0.0f, 1.0f));
    drawManager.text(center, MString("The bottom line is not selectable"));
    drawManager.endDrawable();

    drawManager.beginDrawable(MHWRender::MUIDrawManager::kNonSelectable);
    setHandleColor(drawManager, fBottomName, defaultCol);
    drawManager.line(MPoint(fBottomLeft), MPoint(fBottomRight));
    drawManager.endDrawable();

    drawManager.beginDrawable(MHWRender::MUIDrawManager::kSelectable, fLeftName);
    setHandleColor(drawManager, fLeftName, defaultCol);
    drawManager.line(MPoint(fTopLeft), MPoint(fBottomLeft));
    drawManager.endDrawable();

    drawManager.beginDrawable(MHWRender::MUIDrawManager::kSelectable, fRightName);
    setHandleColor(drawManager, fRightName, defaultCol);
    drawManager.line(MPoint(fTopRight), MPoint(fBottomRight));
    drawManager.endDrawable();
}

// virtual
MStatus squareScaleManipulator::doPress(M3dView& view)
{
    // Reset the mousePoint information on a new press
    fMousePoint = MPoint::origin;
    updateDragInformation();
    return MS::kSuccess;
}

// virtual
MStatus squareScaleManipulator::doDrag(M3dView& view)
{
    updateDragInformation();
    return MS::kSuccess;
}

// virtual
MStatus squareScaleManipulator::doRelease(M3dView& view)
{
    // Scale nodes on the selection list. Implementation
    // is very simple and will not support undo.
    MSelectionList list;
    MGlobal::getActiveSelectionList(list);
    for (MItSelectionList iter(list); !iter.isDone(); iter.next())
    {
        MObject node;
        MStatus status;
        iter.getDependNode(node);
        MFnTransform xform(node, &status);
        if (MS::kSuccess == status)
        {
            double newScale[3];
            newScale[0] = fMousePoint.x + 1;
            newScale[1] = fMousePoint.y + 1;
            newScale[2] = fMousePoint.z + 1;
            xform.setScale(newScale);
        }
    }
    return MS::kSuccess;
}

MStatus squareScaleManipulator::updateDragInformation()
{
    // Find the mouse point in local space
    MPoint localMousePoint;
    MVector localMouseDirection;
    if (MS::kFailure == mouseRay(localMousePoint, localMouseDirection))
        return MS::kFailure;

    // Find the intersection of the mouse point with the
    // manip plane
    MPoint mouseIntersectionWithManipPlane;
    if (!fPlane.intersect(localMousePoint, localMouseDirection, mouseIntersectionWithManipPlane))
        return MS::kFailure;

    fMousePoint = mouseIntersectionWithManipPlane;

    if (glActiveName(fActiveName))
    {
        // Reset draw points
        topLeft.get(fTopLeft);
        topRight.get(fTopRight);
        bottomLeft.get(fBottomLeft);
        bottomRight.get(fBottomRight);

        float* start = 0;
        float* end = 0;

        if (fActiveName == fTopName)
        {
            start = fTopLeft;
            end = fTopRight;
        }
        if (fActiveName == fBottomName)
        {
            start = fBottomLeft;
            end = fBottomRight;
        }
        if (fActiveName == fRightName)
        {
            start = fTopRight;
            end = fBottomRight;
        }
        if (fActiveName == fLeftName)
        {
            start = fTopLeft;
            end = fBottomLeft;
        }
        if (start && end)
        {
            // Find a vector on the plane
            lineMath line;
            MPoint a(start[0], start[1], start[2]);
            MPoint b(end[0], end[1], end[2]);
            MPoint vab = a - b;

            // Define line with a point and a vector on the plane
            line.setLine(start, vab);
            MPoint closestPoint;

            // Find the closest point so that we can get the
            // delta change of the mouse in local space
            if (line.closestPoint(fMousePoint, closestPoint))
            {
                fMousePoint.x -= closestPoint.x;
                fMousePoint.y -= closestPoint.y;
                fMousePoint.z -= closestPoint.z;
            }

            // Move draw points
            start[0] += (float)fMousePoint.x;
            start[1] += (float)fMousePoint.y;
            start[2] += (float)fMousePoint.z;
            end[0] += (float)fMousePoint.x;
            end[1] += (float)fMousePoint.y;
            end[2] += (float)fMousePoint.z;
        }
    }
    return MS::kSuccess;
}

bool squareScaleManipulator::shouldDraw(const MDagPath& cameraPath) const
{
    MStatus status;
    MFnCamera camera(cameraPath, &status);
    if (!status)
    {
        return false;
    }

    const char* nameBuffer = camera.name().asChar();
    if (0 == nameBuffer)
    {
        return false;
    }

    if ((0 == strstr(nameBuffer, "persp")) &&
        (0 == strstr(nameBuffer, "front")))
    {
        return false;
    }

    return true;
}

MStatus squareScaleManipulator::connectToDependNode(const MObject& node)
{
    // No-op
    return MS::kSuccess;
}

//
// Template command that creates and deletes the manipulator
//
class squareManipCmd;
char cmdName[] = "squareManipCmd";
char nodeName[] = "squareScaleManipulator";

class squareManipCmd : public MTemplateCreateNodeCommand<squareManipCmd, cmdName, nodeName>
{
public:
    //
    squareManipCmd()
    {}
};

static squareManipCmd _squareManipCmd;

//
// Static methods
//
void* squareScaleManipulator::creator()
{
    return new squareScaleManipulator();
}

MStatus squareScaleManipulator::initialize()
{
    // No-op
    return MS::kSuccess;
}

//
// squareScaleManipContext
//

class squareScaleManipContext;
char contextName[] = "squareScaleManipContext";
char manipulatorNodeName[] = "squareScaleContextManipulator";

class squareScaleManipContext : 
    public MTemplateSelectionContext<contextName, squareScaleManipContext, 
        MFn::kTransform, squareScaleManipulator, manipulatorNodeName >
{
public:
    squareScaleManipContext() {}
    ~squareScaleManipContext() override {}

    // Only work on scaleX
    void namesOfAttributes(MStringArray& namesOfAttributes) override
    {
        namesOfAttributes.append("scaleX");
    }
};

//
//  Setup the context command which makes the context
//

class squareScaleManipContextCommand;
char contextCommandName[] = "squareScaleManipContext";

class squareScaleManipContextCommand : 
    public MTemplateContextCommand<contextCommandName, squareScaleManipContextCommand, squareScaleManipContext >
{
public:
    squareScaleManipContextCommand() {}
    ~squareScaleManipContextCommand() override {}
};

static squareScaleManipContextCommand _squareScaleManipContextCommand;

//
// Entry points
//

MStatus initializePlugin(MObject obj)
{
    MStatus status;
    MFnPlugin plugin(obj, PLUGIN_COMPANY, "2009", "Any");

    status = _squareScaleManipContextCommand.registerContextCommand( obj );
    if (!status) 
    {
        MString errorInfo("Error: registering context command : ");
        errorInfo += contextCommandName;
        MGlobal::displayError(errorInfo);
        return status;
    }

    status = plugin.registerNode(manipulatorNodeName, squareScaleManipulator::id, 
                                 &squareScaleManipulator::creator, &squareScaleManipulator::initialize,
                                 MPxNode::kManipulatorNode);
    if (!status) 
    {
        MString str("Error registering node: ");
        str+= manipulatorNodeName;
        MGlobal::displayError(str);
        return status;
    }

    return status;
}

MStatus uninitializePlugin(MObject obj)
{
    MStatus status;
    MFnPlugin plugin(obj);

    status = _squareScaleManipContextCommand.deregisterContextCommand( obj );
    if (!status) 
    {
        MString errorInfo("Error: deregistering context command : ");
        errorInfo += contextCommandName;
        MGlobal::displayError(errorInfo);
        return status;
    }

    status = plugin.deregisterNode(squareScaleManipulator::id);
    if (!status) 
    {
        MString str("Error deregistering node: ");
        str+= manipulatorNodeName;
        MGlobal::displayError(str);
        return status;
    }

    return status;
}
