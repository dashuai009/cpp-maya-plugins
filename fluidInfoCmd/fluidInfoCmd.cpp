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
// fluidInfoCmd
//
// Demonstrates the use of the MFnFluid class for retrieving
// fluid information.  The resolution, size and data methods
// are printed to the script editor, followed by grid data for
// those fluid attributes with grids. If $maxVoxelsToPrint
// is not specified, it will print all of them. This might take
// a VERY long time.
//
//  Syntax:
//      fluidInfo $fluidNodeName $maxVoxelsToPrint;
//
//  Example:
//  fluidInfo fluidShape2 10;
//  fluidInfo fluidShape2;
//

#include <assert.h>

#include <maya/MGlobal.h>
#include <maya/MString.h>
#include <maya/MPoint.h>
#include <maya/MVector.h>
#include <maya/MVectorArray.h>
#include <maya/MFloatMatrix.h>
#include <maya/MArgList.h>
#include <maya/MPlug.h>
#include <maya/MFnDependencyNode.h>
#include <maya/MDagPath.h>
#include <maya/MItDag.h>
#include <maya/MSelectionList.h>
#include <maya/MPlug.h>
#include <maya/MPlugArray.h>
#include <maya/MPxCommand.h>
#include <maya/MFnDagNode.h>
#include <maya/MFnFluid.h>
#include <maya/MFnPlugin.h>


#define mCommandName "fluidInfo"    // Command name

class fluidInfoCmd : public MPxCommand
{
public:
                 fluidInfoCmd();
    virtual     ~fluidInfoCmd();

    virtual MStatus doIt ( const MArgList& args );

    static void* creator();

private:

    static MStatus nodeFromName(MString name, MObject & obj);
    MStatus fluidMethodName(MFnFluid::FluidMethod method, char *methodStr);
    MStatus colorMethodName(MFnFluid::ColorMethod method, char *methodStr);
    MStatus coordMethodName(MFnFluid::CoordinateMethod method, char *methodStr);
    MStatus falloffMethodName(MFnFluid::FalloffMethod method, char *methodStr);
    MStatus fluidGradientName(MFnFluid::FluidGradient gradient, char *gradientStr);

    MStatus parseArgs ( const MArgList& args );

    MString fluidName;
    MObject fluidNode;
    int     requestedVoxels;
};

fluidInfoCmd::fluidInfoCmd()
{
}

fluidInfoCmd::~fluidInfoCmd() 
{
}


//
//

MStatus fluidInfoCmd::nodeFromName(MString name, MObject & obj)
{
    MSelectionList tempList;
    tempList.add( name );
    if ( tempList.length() > 0 ) 
    {
        tempList.getDependNode( 0, obj );
        return MS::kSuccess;
    }
    return MS::kFailure;
}

MStatus fluidInfoCmd::fluidMethodName(MFnFluid::FluidMethod method, char *methodStr)
{
    MStatus stat = MS::kSuccess;

    if(!methodStr) {
        stat = MS::kFailure;
    } else {
        switch( method )
        {
        case MFnFluid::kZero:
            strcpy(methodStr, "Zero");
            break;
        case MFnFluid::kStaticGrid:
            strcpy(methodStr, "Static Grid");
            break;
        case MFnFluid::kDynamicGrid:
            strcpy(methodStr, "Dynamic Grid");
            break;
        case MFnFluid::kGradient:
            strcpy(methodStr, "Gradient");
            break;
        default:
            strcpy(methodStr, "Garbage");
            break;
        }
    }

    return stat;
}

MStatus fluidInfoCmd::falloffMethodName(MFnFluid::FalloffMethod method, char *methodStr)
{
    MStatus stat = MS::kSuccess;

    if(!methodStr) {
        stat = MS::kFailure;
    } else {
        switch( method )
        {
        case MFnFluid::kNoFalloffGrid:
            strcpy(methodStr, "No Falloff Grid");
            break;
        case MFnFluid::kStaticFalloffGrid:
            strcpy(methodStr, "Static Grid");
            break;
        default:
            strcpy(methodStr, "Garbage");
            break;
        }
    }

    return stat;
}


MStatus fluidInfoCmd::colorMethodName(MFnFluid::ColorMethod method, char *methodStr)
{
    MStatus stat = MS::kSuccess;

    if(!methodStr) {
        stat = MS::kFailure;
    } else {
        switch( method )
        {
        case MFnFluid::kUseShadingColor:
            strcpy(methodStr, "Shading Color");
            break;
        case MFnFluid::kStaticColorGrid:
            strcpy(methodStr, "Static Grid");
            break;
        case MFnFluid::kDynamicColorGrid:
            strcpy(methodStr, "Dynamic Grid");
            break;
        default:
            strcpy(methodStr, "Garbage");
            break;
        }
    }

    return stat;
}

MStatus fluidInfoCmd::coordMethodName(MFnFluid::CoordinateMethod method, char *methodStr)
{
    MStatus stat = MS::kSuccess;

    if(!methodStr) {
        stat = MS::kFailure;
    } else {
        switch( method )
        {
        case MFnFluid::kFixed:
            strcpy(methodStr, "Fixed");
            break;
        case MFnFluid::kGrid:
            strcpy(methodStr, "Grid");
            break;
        default:
            strcpy(methodStr, "Garbage");
            break;
        }
    }

    return stat;
}

MStatus fluidInfoCmd::fluidGradientName(MFnFluid::FluidGradient gradient, char *gradientStr)
{
    MStatus stat = MS::kSuccess;

    if(!gradientStr) {
        stat = MS::kFailure;
    } else {
        switch( gradient )
        {
        case MFnFluid::kConstant:
            strcpy(gradientStr, " Constant");
            break;
        case MFnFluid::kXGradient:
            strcpy(gradientStr, "X Gradient");
            break;
        case MFnFluid::kYGradient:
            strcpy(gradientStr, "Y Gradient");
            break;
        case MFnFluid::kZGradient:
            strcpy(gradientStr, "Z Gradient");
            break;
        case MFnFluid::kNegXGradient:
            strcpy(gradientStr, "Negative X Gradient");
            break;
        case MFnFluid::kNegYGradient:
            strcpy(gradientStr, "Negative Y Gradient");
            break;
        case MFnFluid::kNegZGradient:
            strcpy(gradientStr, "Negative Z Gradient");
            break;
        case MFnFluid::kCenterGradient:
            strcpy(gradientStr, "Center Gradient");
            break;
        default:
            strcpy(gradientStr, "Garbage");
            break;
        }
    }

    return stat;
}

MStatus fluidInfoCmd::parseArgs( const MArgList& args )
{
    // Parse the arguments.
    MStatus stat = MS::kSuccess;

    // some defaults for the number of voxels we might want to print
    requestedVoxels = -1;

    if( args.length() < 1 )
    {
        MGlobal::displayError( "Missing fluid node name argument." );
        return MS::kFailure;
    }
    else if( args.length() > 2 )
    {
        MGlobal::displayError( "Too many arguments." );
        return MS::kFailure;
    }

    fluidName = args.asString( 0, &stat );
    if (stat != MS::kSuccess)
    {
        MGlobal::displayError( "Failed to parse fluid node name argument." );
        return MS::kFailure;
    }

    if(args.length() == 1) {
        // assume that the user wants to print all the voxels
        // they probably won't do this more than once
        requestedVoxels = -1;
    } else {
        requestedVoxels = args.asInt( 1, &stat );
        if (stat != MS::kSuccess)
        {
            MGlobal::displayError( "Failed to parse num voxels to pribt argument." );
            return MS::kFailure;
        }
    }

    nodeFromName( fluidName, fluidNode );

    if( fluidNode.isNull() )
    {
        MGlobal::displayError( "There is no fluid node with the given name." );
        return MS::kFailure;
    }

    if( ! fluidNode.hasFn( MFn::kFluid ) )
    {
        MGlobal::displayError( "The named node is not a fluid." );
        return MS::kFailure;
    }

    return MS::kSuccess;
}


//
// Main routine
MStatus fluidInfoCmd::doIt( const MArgList& args )
{
    MStatus stat = parseArgs( args );
    if( stat != MS::kSuccess ) 
    {
        return stat;
    }

    MFnFluid fluffy( fluidNode );

    unsigned Xres, Yres, Zres;
    fluffy.getResolution(Xres, Yres, Zres);

    double Xdim, Ydim, Zdim;
    fluffy.getDimensions(Xdim, Ydim, Zdim);

    char buffer[256];

    // first, the basic facts about the fluid
    sprintf( buffer, "Fluid: %s", fluidName.asChar());
    MGlobal::displayInfo( buffer );

    sprintf( buffer, "resolution %u %u %u", Xres, Yres, Zres );
    MGlobal::displayInfo( buffer );

    sprintf( buffer, "dimensions %g %g %g\n", Xdim, Ydim, Zdim );
    MGlobal::displayInfo( buffer );

    // now lets see how many voxels we want to print
    // definitely not more than the number of voxels

    int numPrintVoxels = requestedVoxels;
    if(requestedVoxels < 0 || requestedVoxels > (int)fluffy.gridSize()) {
        numPrintVoxels = fluffy.gridSize();     
    }

    // get the object space position of the first voxel/
    MPoint centerPt;
    fluffy.voxelCenterPosition(0, 0, 0, centerPt);
    sprintf( buffer, "first voxel pos %g %g %g", centerPt.x, centerPt.y, centerPt.z );
    MGlobal::displayInfo( buffer );


    //
    // Print Density information
    //
    char methodName[32];
    char gradientName[32];
    MFnFluid::FluidMethod method = MFnFluid::kZero;
    MFnFluid::FluidGradient gradient = MFnFluid::kConstant;

    fluffy.getDensityMode(method, gradient);
    fluidMethodName(method, methodName);
    fluidGradientName(gradient, gradientName);
    if(method == MFnFluid::kGradient) 
        sprintf( buffer, "Density method: %s type: %s", methodName, gradientName);
    else 
        sprintf( buffer, "Density method: %s ", methodName);
    MGlobal::displayInfo( buffer );

    if(method == MFnFluid::kStaticGrid || method == MFnFluid::kDynamicGrid) {
        float *densityGrid = fluffy.density();
        if(densityGrid != NULL) {
            sprintf( buffer, "Density grid contents:");
            MGlobal::displayInfo( buffer );
            int xi, yi, zi;
            for(int i = 0; i < numPrintVoxels; i++) {
                fluffy.index(i, xi, yi, zi);
                sprintf(buffer, "index: %d voxel: [%d %d %d] density: %g", i, xi, yi, zi, densityGrid[i]);
                MGlobal::displayInfo( buffer );
            }
        }
    }

    //
    // Print Temperature information
    //
    fluffy.getTemperatureMode(method, gradient);
    fluidMethodName(method, methodName);
    fluidGradientName(gradient, gradientName);
    if(method == MFnFluid::kGradient) 
        sprintf( buffer, "Temperature method: %s type: %s", methodName, gradientName);
    else 
        sprintf( buffer, "Temperature method: %s ", methodName);
    MGlobal::displayInfo( buffer );

    if(method == MFnFluid::kStaticGrid || method == MFnFluid::kDynamicGrid) {
        float *tempGrid = fluffy.temperature();
        if(tempGrid != NULL) {
            sprintf( buffer, "Tempereature grid contents:");
            MGlobal::displayInfo( buffer );
            int xi, yi, zi;
            for(int i = 0; i < numPrintVoxels; i++) {
                fluffy.index(i, xi, yi, zi);
                sprintf(buffer, "index: %d voxel: [%d %d %d] temperature: %g", i, xi, yi, zi, tempGrid[i]);
                MGlobal::displayInfo( buffer );
            }
        }
    }

    //
    // Print Fuel information
    //
    fluffy.getFuelMode(method, gradient);
    fluidMethodName(method, methodName);
    fluidGradientName(gradient, gradientName);
    if(method == MFnFluid::kGradient) 
        sprintf( buffer, "Fuel method: %s type: %s", methodName, gradientName);
    else 
        sprintf( buffer, "Fuel method: %s ", methodName);
    MGlobal::displayInfo( buffer );

    if(method == MFnFluid::kStaticGrid || method == MFnFluid::kDynamicGrid) {
        float *fuelGrid = fluffy.fuel();
        if(fuelGrid != NULL) {
            sprintf( buffer, "Fuel grid contents:");
            MGlobal::displayInfo( buffer );
            int xi, yi, zi;
            for(int i = 0; i < numPrintVoxels; i++) {
                fluffy.index(i, xi, yi, zi);
                sprintf(buffer, "index: %d voxel: [%d %d %d] fuel: %g", i, xi, yi, zi, fuelGrid[i]);
                MGlobal::displayInfo( buffer );
            }
        }
    }

    //
    // Print Velocity information
    // Note the difference in the velocity grid sizes
    //
    fluffy.getVelocityMode(method, gradient);
    fluidMethodName(method, methodName);
    fluidGradientName(gradient, gradientName);
    if(method == MFnFluid::kGradient) 
        sprintf( buffer, "Velocity method: %s type: %s", methodName, gradientName);
    else 
        sprintf( buffer, "Velocity method: %s ", methodName);
    MGlobal::displayInfo( buffer );

    if(method == MFnFluid::kStaticGrid || method == MFnFluid::kDynamicGrid) {
        float *xvelGrid;
        float *yvelGrid;
        float *zvelGrid;
        fluffy.getVelocity(xvelGrid, yvelGrid, zvelGrid);

        if(xvelGrid && yvelGrid && zvelGrid) {
            int i;
            int xi, yi, zi;
            float xvel, yvel, zvel;
                
            // print out the X velocity grid, with its own grid indices
            int xvSize, yvSize, zvSize;
            fluffy.velocityGridSizes(xvSize, yvSize, zvSize);
            int numXvelVoxels = requestedVoxels;
            if(requestedVoxels < 0 || requestedVoxels > xvSize) {
                numXvelVoxels = xvSize;     
            }

            sprintf( buffer, "X velocity grid contents:");
            MGlobal::displayInfo( buffer );
            for(i = 0; i < numXvelVoxels; i++) {
                fluffy.index(i, Xres+1, Yres, Zres, xi, yi, zi);
                sprintf(buffer, "index: %d voxel face: [%d %d %d] x velocity: %g", i, xi, yi, zi, xvelGrid[i]);
                MGlobal::displayInfo( buffer );
            }

            // print out the average velocity at the voxel centers
            sprintf( buffer, "Voxel Center Velocity:");
            MGlobal::displayInfo( buffer );
            int viLow, viHigh;
            for(i = 0; i < numPrintVoxels; i++) {
                fluffy.index(i, xi, yi, zi);

                viLow = fluffy.index(xi, yi, zi, Xres+1, Yres, Zres);
                viHigh = fluffy.index(xi+1, yi, zi, Xres+1, Yres, Zres);
                xvel = (xvelGrid[viHigh] + xvelGrid[viLow])/2.0f;

                viLow = fluffy.index(xi, yi, zi, Xres, Yres+1, Zres);
                viHigh = fluffy.index(xi, yi+1, zi, Xres, Yres+1, Zres);
                yvel = (yvelGrid[viHigh] + yvelGrid[viLow])/2.0f;

                viLow = fluffy.index(xi, yi, zi, Xres, Yres, Zres+1);
                viHigh = fluffy.index(xi, yi, zi+1, Xres, Yres, Zres+1);
                zvel = (zvelGrid[viHigh] + zvelGrid[viLow])/2.0f;
                
                sprintf(buffer, "index: %d voxel: [%d %d %d] velocity: (%g, %g, %g)", i, xi, yi, zi, xvel, yvel, zvel);
                MGlobal::displayInfo( buffer );
            }
            
        }
        float *pressureGrid = fluffy.pressure();
        if(pressureGrid != NULL) {
            sprintf( buffer, "Pressure grid contents:");
            MGlobal::displayInfo( buffer );
            int xi, yi, zi;
            for(int i = 0; i < numPrintVoxels; i++) {
                fluffy.index(i, xi, yi, zi);
                sprintf(buffer, "index: %d voxel: [%d %d %d] pressure: %g", i, xi, yi, zi, pressureGrid[i]);
                MGlobal::displayInfo( buffer );
            }
        }

    }

    //
    // Print Texture Coordinate information
    //
    MFnFluid::CoordinateMethod coordMethod = MFnFluid::kFixed ;
    fluffy.getCoordinateMode(coordMethod);
    coordMethodName(coordMethod, methodName);
    sprintf( buffer, "Coordinate method: %s ", methodName);
    MGlobal::displayInfo( buffer );

    if(coordMethod == MFnFluid::kGrid) {
        float *uGrid;
        float *vGrid;
        float *wGrid;
        fluffy.getCoordinates(uGrid, vGrid, wGrid);

        if(uGrid && vGrid && wGrid) {
            int xi, yi, zi;
                
            // print out the 3D texture coordinates
            int i;
            sprintf( buffer, "Tex Coord grid contents:");
            MGlobal::displayInfo( buffer );
            for(i = 0; i < numPrintVoxels; i++) {
                fluffy.index(i, xi, yi, zi);
                sprintf(buffer, "index: %d voxel: [%d %d %d] uvw: (%g, %g, %g)", i, xi, yi, zi, uGrid[i], vGrid[i], wGrid[i]);
                MGlobal::displayInfo( buffer );
            }
            
        }
        else if(uGrid && vGrid && (Zres == 1)) {
            int xi, yi, zi;
                
            // this is a 2D fluid and there are only 2D texture coordinates
            int i;
            sprintf( buffer, "Tex Coord grid contents:");
            MGlobal::displayInfo( buffer );
            for(i = 0; i < numPrintVoxels; i++) {
                fluffy.index(i, xi, yi, zi);
                sprintf(buffer, "index: %d voxel: [%d %d %d] uv: (%g, %g)", i, xi, yi, zi, uGrid[i], vGrid[i]);
                MGlobal::displayInfo( buffer );
            }
            
        }
    }

    //
    // Print Color information
    //
    MFnFluid::ColorMethod colorMethod = MFnFluid::kUseShadingColor;
    fluffy.getColorMode(colorMethod);
    colorMethodName(colorMethod, methodName);
    sprintf( buffer, "Color method: %s ", methodName);
    MGlobal::displayInfo( buffer );

    if(colorMethod != MFnFluid::kUseShadingColor) {
        float *rGrid;
        float *gGrid;
        float *bGrid;
        fluffy.getColors(rGrid, gGrid, bGrid);

        if(rGrid && gGrid && bGrid) {
            int xi, yi, zi;
                
            int i;
            sprintf( buffer, "Color grid contents:");
            MGlobal::displayInfo( buffer );
            for(i = 0; i < numPrintVoxels; i++) {
                fluffy.index(i, xi, yi, zi);
                sprintf(buffer, "index: %d voxel: [%d %d %d] color: (%g, %g, %g)", i, xi, yi, zi, rGrid[i], gGrid[i], bGrid[i]);
                MGlobal::displayInfo( buffer );
            }
            
        }
    }

    //
    // Print Falloff information
    //
    MFnFluid::FalloffMethod falloffMethod;
    fluffy.getFalloffMode(falloffMethod);
    falloffMethodName(falloffMethod, methodName);
    sprintf( buffer, "Falloff method: %s ", methodName);
    MGlobal::displayInfo( buffer );

    if(falloffMethod == MFnFluid::kNoFalloffGrid ) {
        float *falloffGrid = fluffy.falloff();
        if(falloffGrid != NULL) {
            sprintf( buffer, "Falloff grid contents:");
            MGlobal::displayInfo( buffer );
            int xi, yi, zi;
            for(int i = 0; i < numPrintVoxels; i++) {
                fluffy.index(i, xi, yi, zi);
                sprintf(buffer, "index: %d voxel: [%d %d %d] falloff: %g", i, xi, yi, zi, falloffGrid[i]);
                MGlobal::displayInfo( buffer );
            }
        }
    }


    return MS::kSuccess;
}


//
//

void * fluidInfoCmd::creator() { return new fluidInfoCmd(); }


MStatus initializePlugin( MObject obj )
{
    MFnPlugin plugin( obj, PLUGIN_COMPANY, "6.0", "Any");

    MStatus status = plugin.registerCommand(mCommandName,
                                            fluidInfoCmd::creator );
    if (!status) 
        status.perror("registerCommand");

    return status;
}


MStatus uninitializePlugin( MObject obj )
{
    MFnPlugin plugin( obj );

    MStatus status = plugin.deregisterCommand(mCommandName);
    if (!status) 
        status.perror("deregisterCommand");

    return status;
}
