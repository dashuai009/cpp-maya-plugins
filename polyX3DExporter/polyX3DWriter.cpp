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
//

//General Includes
//
#include <maya/MIOStream.h>
#include <maya/MGlobal.h>
#include <maya/MDagPath.h>
#include <maya/MIntArray.h>
#include <maya/MFnSet.h>
#include <maya/MItDependencyGraph.h>
#include <maya/MPlug.h>

//Iterator Includes
//
#include <maya/MItMeshPolygon.h>

//polyX3DWriter.cpp
#include "polyX3DWriter.h"


//Macros

//used as the default color for vertices which have no color assigned
//
#define DEFAULT_COLOR   "0.2 0.2 0.2"

//used as default values for the Material tag when outputting a texture
//
#define DIFFUSE_COLOR   "0 0 0"
#define SHININESS       "0.8"
#define SPECULAR_COLOR  "0.5 0.5 0.5"

//flags used to indicate which tags have been outputted already, so to
//minimize data duplication
//
#define COORDINATE_FLAG 0x0001
#define NORMAL_FLAG     0x0010
#define TEXTURE_FLAG    0x0100
#define COLOR_FLAG      0x1000

//number of spaces per tab
#define INITIAL_TAB_COUNT 2

polyX3DWriter::polyX3DWriter(const MDagPath& dagPath, MStatus& status):
    polyWriter(dagPath, status),
    fTagFlags(0),
    fInitialTabCount(INITIAL_TAB_COUNT)
//Summary:  creates and initializes an object of this class
//Args   :  dagPath - the DAG path of the current node
//          status - will be set to MStatus::kSuccess if the constructor was
//                   successful;  MStatus::kFailure otherwise
{
}


polyX3DWriter::~polyX3DWriter()
{
//Summary:  destructor method - does nothing
//
}


MStatus polyX3DWriter::extractGeometry() 
//Summary:  extracts main geometry as well as the current UV set and UV set 
//          coordinates
{
    if (MStatus::kFailure == polyWriter::extractGeometry()) {
        return MStatus::kFailure;
    }

    if (MStatus::kFailure == fMesh->getUVs(fUArray, fVArray, &fCurrentUVSetName)) {
        MGlobal::displayError("MFnMesh::getUVs"); 
        return MStatus::kFailure;
    }

    return MStatus::kSuccess;
}


MStatus polyX3DWriter::writeToFile(ostream& os) 
//Summary:  outputs the geometry of this polygonal mesh in X3D compliant format
//Args   :  os - an output stream to write to
//          tabCount - the initial number of tabs to print
//Returns:  MStatus::kSuccess if the method succeeds
//          MStatus::kFailure if the method fails
{
    MGlobal::displayInfo("Exporting " + fMesh->partialPathName());
    unsigned int setCount = fPolygonSets.length();

    if (0 == setCount) {
        return MStatus::kFailure;
    } else if (1 == setCount) {
        if (MStatus::kFailure == outputSets(os)) {
            return MStatus::kFailure;
        }
    } else {
        outputTabs(os, INITIAL_TAB_COUNT);
        fInitialTabCount++;
        os << "<Group DEF=\"" << fMesh->partialPathName() << "\">\n";
        if (MStatus::kFailure == outputSets(os)) {
            return MStatus::kFailure;
        }
        outputTabs(os, INITIAL_TAB_COUNT);
        os << "</Group>\n";
    }
    return MStatus::kSuccess;
}


MStatus polyX3DWriter::outputSingleSet (ostream& os, MString setName, MIntArray faces, MString textureName) {
//Summary:  outputs a single set of polygons grouped under a Shape tag
//Args   :  os - an output stream to write to
//          shapeName - the value for the DEF attribute
//          faces - an MIntArray containing the indices of the faces to include
//                  in this shape
//          textureName - the full path for the texture file
//Returns:  MStatus::kSuccess if the method successfully outputs this tag;
//          MStatus::kFailure otherwise
    if (0 == fPolygonSets.length()) {
        return MStatus::kFailure;
    } else if (1 == fPolygonSets.length()) {
        setName = fMesh->partialPathName();
    } else {
        setName = fMesh->partialPathName() + "_" + setName;
    }
    return outputX3DShapeTag (os, setName, faces, textureName, fInitialTabCount);
}


MStatus polyX3DWriter::outputX3DShapeTag(ostream & os,
                                     const MString shapeName,
                                     const MIntArray& faces,
                                     const MString textureName,
                                     const unsigned int tabCount) 
//Summary:  outputs the X3D Shape tag with the DEF attribute
//Args   :  os - an output stream to write to
//          shapeName - the value for the DEF attribute
//          faces - an MIntArray containing the indices of the faces to include
//                  in this shape
//          textureName - the full path for the texture file
//          tabCount - the number of tabs to preceed this tag
//Returns:  MStatus::kSuccess if the method successfully outputs this tag;
//          MStatus::kFailure otherwise
{
    outputTabs(os, tabCount);
    os << "<Shape DEF=\"" << shapeName << "\">\n";

    if (MStatus::kFailure == outputX3DAppearanceTag(os, textureName, tabCount + 1)) { 
        return MStatus::kFailure;
    }

    if (MStatus::kFailure == outputX3DIndexedFaceSetTag(os, faces, textureName, tabCount + 1)) {
        return MStatus::kFailure;
    }

    outputTabs(os, tabCount);
    os << "</Shape>\n";

    return MStatus::kSuccess;
}


MStatus polyX3DWriter::outputX3DAppearanceTag(ostream & os, 
                                          const MString textureName, 
                                          const unsigned int tabCount) 
//Summary:  outputs the X3D Appearance Tag.  If textureName is an empty string, 
//          outputs the default Material tag;  otherwise outputs a Material tag 
//          with diffuseColor, shininess, and specularColor values set, along 
//          with an ImageTexture tag with the url attribute value set to 
//          textureName
//Args   :  os - an output stream to write to
//          textureName - the full path of the texture file
//          tabCount - the number of tabs to preceed this tag
//Returns:  MStatus::kSuccess if the method successfully outputs this tag;
//          MStatus::kFailure otherwise
{
    outputTabs(os, tabCount);

    //No texture name given, output default apperance and material tags
    //
    if (textureName == MString("")) {
        os << "<Appearance><Material/></Appearance>\n";

    //Texture name was given, output an ImageTexture tag with the texture name 
    //as the value of the url attribute, and output default values for the 
    //diffuseColor, shininess, and specularColor of the material node
    //
    } else {
        os << "<Appearance>\n";
        outputTabs(os, tabCount + 1);
        os << "<ImageTexture url=\"" << textureName << "\"/>\n";
        outputTabs(os, tabCount + 1);
        os << "<Material diffuseColor=\"" << DIFFUSE_COLOR 
           << "\" shininess=\"" << SHININESS 
           << "\" specularColor=\"" << SPECULAR_COLOR 
           << "\"/>\n";
        outputTabs(os, tabCount);
        os << "</Appearance>\n";
    }
    return MStatus::kSuccess;
}


MStatus polyX3DWriter::outputX3DIndexedFaceSetTag(ostream & os, 
                                              const MIntArray& faces,
                                              const MString textureName, 
                                              const unsigned int tabCount) 
//Summary:  outputs the X3D IndexedFaceSet tag along with the Coordinate and
//          Normal tags.  If textureName is an empty string, also outputs the
//          Color tag and IndexedFaceSet's colorIndex and colorPerVertex 
//          attributes; otherwise outputs the TextureCoordinate tag and
//          IndexedFaceSet's texCoordIndex attribute
//Args   :  os - an output stream to write to
//          faces - an MIntArray containing the indices of the faces to include
//                  in this IndexedFaceSet
//          textureName - the full path for the texture file
//          tabCount - the number of tabs to preceed this tag
//Returns:  MStatus::kSuccess if the method successfully outputs this tag;
//          MStatus::kFailure otherwise
{
    outputTabs(os, tabCount);
    os << "<IndexedFaceSet coordIndex=\"";

    unsigned int faceCount = faces.length();

    //if there are no faces to output return an error status
    //
    if (0 == faceCount) {
        return MStatus::kFailure;
    }

    MStatus status;
    //general purpose counters
    //
    unsigned int i, j, indexCount;

    //general int array storage
    //
    MIntArray indexList;

    //For every face in the faces array, retrieve and output its vertices.
    //getPolygonVertices() returns the indices of the vertices of a given face,
    //and orders the vertex indices in the array in the manner required for the
    //X3D IndexedFaceSet tag.  Note, these indices refer to the array fVertexArray.
    //
    for (i = 0; i < faceCount; i++) {

        indexCount = fMesh->polygonVertexCount(faces[i], &status);
        if (MStatus::kFailure == status) {
            MGlobal::displayError("MFnMesh::polygonVertexCount");
            return MStatus::kFailure;
        }

        //store those vertices for face[i] and output them
        //
        status = fMesh->getPolygonVertices (faces[i], indexList);
        if (MStatus::kFailure == status) {
            MGlobal::displayError("MFnMesh::getPolygonVertices");
            return MStatus::kFailure;
        }

        for (j = 0; j < indexCount; j++) {
            os << indexList[j] << " ";
        }

        os << "-1 ";
    }

    //For every face in the faces array, retrieve and output its vertex normals.
    //getNormalIDs() returns the indices of the normals of a given face.  Note, 
    //these indices refer to the array fNormalArray.
    //
    os << "\" normalPerVertex=\"true\" normalIndex=\"";
    for (i = 0; i < faceCount; i++) {
        //store the vertex normals for face[i]
        //
        status = fMesh->getFaceNormalIds (faces[i], indexList);
        if (MStatus::kFailure == status) {
            MGlobal::displayError("MFnMesh::getFaceNormalIds");
            return MStatus::kFailure;
        }

        //determine the number of vertex normals for face[i] 
        //and output them
        indexCount = indexList.length();
        for (j = 0; j < indexCount; j++) {
            os << indexList[j] << " ";
        }
        os << "-1 ";
    }
    os << "\" ";

    //If a texture name is not given, output colorIndex attribute
    if (textureName == MString("")) {
        os << "colorPerVertex=\"true\" ";
        //output the color indices for each per polygon vertex.  These indices
        //refer to the array fColorArray
        //
        os << "colorIndex=\"";
        int colorIndex = 0;
        for (i = 0; i < faceCount; i++) {
            indexCount = fMesh->polygonVertexCount(faces[i], &status);
            if (MStatus::kFailure == status) {
                MGlobal::displayError("MFnMesh::polygonVertexCount");
                return MStatus::kFailure;
            }

            for (j = 0; j < indexCount; j++) {
                if (MStatus::kFailure == fMesh->getFaceVertexColorIndex(faces[i], j, colorIndex)) {
                    MGlobal::displayError("MFnMesh::getFaceVertexColorIndex");
                    return MStatus::kFailure;
                }
                os << colorIndex << " ";
            }
            os << "-1 ";
        }
        os << "\">\n";

        //output the X3D color tag to correspond with the indices
        //
        if (MStatus::kFailure == outputX3DColorTag(os, tabCount + 1)) {
            return MStatus::kFailure;
        }

    //A texture name was given, output the texCoordIndex attribute
    //
    } else {
        os << " texCoordIndex=\"";

        //output the uv indicies for each polygon per vertex.  These indices
        //refer to the arrays fUArray and fVArray
        //
        

        for (i = 0; i < faceCount; i++) {
            indexCount = fMesh->polygonVertexCount(faces[i], &status);
            if (MStatus::kFailure == status) {
                MGlobal::displayError("MFnMesh::polygonVertexCount");
                return MStatus::kFailure;
            }

            for (j = 0; j < indexCount; j++) {
                int uvID;
                status = fMesh->getPolygonUVid(faces[i], j, uvID, &fCurrentUVSetName);
                if (MStatus::kFailure == status) {
                    MGlobal::displayError("MFnMesh::getPolygonUVid");
                    return MStatus::kFailure;
                }
                os << uvID << " ";
            }
            os << "-1 ";
        }

        os << "\">\n";

        //output the X3D TextureCoordinateTag to correspond with the indices
        //
        if (MStatus::kFailure == outputX3DTextureCoordinateTag(os, tabCount + 1)) {
            return MStatus::kFailure;
        }
    }

    //output the vertex coordinates using the X3D Coordinate tag and
    //output the vertex normals using the X3D Normal tag
    //
    if (MStatus::kFailure == outputX3DCoordinateTag(os, tabCount + 1)
        || MStatus::kFailure == outputX3DNormalTag(os, tabCount + 1)) {
        return MStatus::kFailure;
    }
    
    outputTabs(os, tabCount);
    os << "</IndexedFaceSet>\n";

    return MStatus::kSuccess;
}


MStatus polyX3DWriter::outputX3DCoordinateTag(ostream & os, 
                                          const unsigned int tabCount) 
//Summary:  outputs the X3D Coordinate tag with the DEF attribute if the tag 
//          has not been previously output for this polygonal mesh; with the 
//          USE attribute otherwise
//Args   :  os - an output stream to write to
//          tabCount - the number of tabs to preceed this tag
//Returns:  MStatus::kSuccess if the method successfully outputs this tag;
//          MStatus::kFailure otherwise
{
    unsigned int vertexCount = fVertexArray.length();
    if (0 == vertexCount) {
        return MStatus::kFailure;
    }

    outputTabs(os, tabCount);
    //tag has already been output for this mesh, so just re-use that tag
    //
    if (fTagFlags & COORDINATE_FLAG) {
        os << "<Coordinate USE=\"" << fMesh->partialPathName() << "_coordinates";
    //tag has not been output for this mesh, so define a new tag with the
    //necessary data
    //
    } else {
        fTagFlags |= COORDINATE_FLAG;
        os << "<Coordinate DEF=\"" << fMesh->partialPathName() << "_coordinates\" point=\"";
        unsigned int i;
        for (i = 0; i < vertexCount; i++) {
            os << fVertexArray[i].x << " " 
               << fVertexArray[i].y << " " 
               << fVertexArray[i].z << ", ";
        }
    }

    os << "\"/>\n";

    return MStatus::kSuccess;
}


MStatus polyX3DWriter::outputX3DNormalTag(ostream & os, 
                                      const unsigned int tabCount) 
//Summary:  outputs the X3D Normal tag with the DEF attribute if the Normal tag
//          has not been previously output for this polygonal mesh; with the 
//          USE attribute otherwise
//Args   :  os - an output stream to write to
//          tabCount - the number of tabs to preceed this tag
//Returns:  MStatus::kSuccess if the method successfully outputs this tag;
//          MStatus::kFailure otherwise
{
    unsigned int normalCount = fNormalArray.length();
    if (0 == normalCount) {
        return MStatus::kFailure;
    }

    outputTabs(os, tabCount);
    //tag has already been output for this mesh, so just re-use that tag
    //
    if (fTagFlags & NORMAL_FLAG) {
        os << "<Normal USE=\"" << fMesh->partialPathName() << "_normals";
    //tag has not been output for this mesh, so define a new tag with the
    //necessary data
    //
    } else {
        fTagFlags |= NORMAL_FLAG;
        os << "<Normal DEF=\"" << fMesh->partialPathName() << "_normals\" vector=\"";
        unsigned int i;
        for (i = 0; i < normalCount; i++) {
            os << fNormalArray[i].x << " " 
               << fNormalArray[i].y << " " 
               << fNormalArray[i].z << ", ";
        }
    }
    os << "\"/>\n";

    return MStatus::kSuccess;
}


MStatus polyX3DWriter::outputX3DTextureCoordinateTag(ostream & os, 
                                                 const unsigned int tabCount) 
//Summary:  outputs the X3D TextureCoordinate tag with the DEF attribute if the 
//          tag has not been previously output for this polygonal mesh; with 
//          the USE attribute otherwise
//Args   :  os - an output stream to write to
//          tabCount - the number of tabs to preceed this tag
//Returns:  MStatus::kSuccess if the method successfully outputs this tag;
//          MStatus::kFailure otherwise
{
    unsigned int uvCount = fUArray.length();
    if (0 == uvCount) {
        return MStatus::kFailure;
    }

    outputTabs(os, tabCount);
    //tag has already been output for this mesh, so just re-use that tag
    //
    if (fTagFlags & TEXTURE_FLAG) {
        os << "<TextureCoordinate USE=\"" << fMesh->partialPathName() << "_texCoordinates";
    //tag has not been output for this mesh, so define a new tag with the
    //necessary data
    //
    } else {
        fTagFlags |= TEXTURE_FLAG;
        os << "<TextureCoordinate DEF=\"" << fMesh->partialPathName() << "_texCoordinates\" point=\"";
        unsigned int i;
        for (i = 0; i < uvCount; i++) {
            os << fUArray[i] << " " << fVArray[i] << ", ";
        }
    }
    os << "\"/>\n";

    return MStatus::kSuccess;
}


MStatus polyX3DWriter::outputX3DColorTag(ostream & os, 
                                     const unsigned int tabCount) 
//Summary:  outputs the X3D Color tag with the DEF attribute if the tag has not 
//          been previously output for this polygonal mesh; with the USE 
//          attribute otherwise
//Args   :  os - an output stream to write to
//          tabCount - the number of tabs to preceed this tag
//Returns:  MStatus::kSuccess if the method successfully outputs this tag;
//          MStatus::kFailure otherwise
{
    unsigned int colorCount = fColorArray.length();
    if (0 == colorCount) {
        return MStatus::kFailure;
    }
    
    outputTabs(os, tabCount);
    //tag has already been output for this mesh, so just re-use that tag
    //
    if (fTagFlags & COLOR_FLAG) {
        os << "<Color USE=\"" << fMesh->partialPathName() << "_colors";
    //tag has not been output for this mesh, so define a new tag with the
    //necessary data
    //
    } else {
        fTagFlags |= COLOR_FLAG;
        os << "<Color DEF=\"" << fMesh->partialPathName() << "_colors\" color=\"";
        unsigned int i;
        for (i = 0; i < colorCount; i++) {
            //an rgb value of -1 -1 -1 in Maya indicates no color, so if any
            //component is not -1, color info is present.  Note: only need to check
            //r, g, or b; r was chosen arbitrarily
            //
            if (-1 != fColorArray[i].r) {
                os << fColorArray[i].r << " " 
                   << fColorArray[i].g << " " 
                   << fColorArray[i].b << ", ";
            //use default coloring
            //
            } else {
                os << DEFAULT_COLOR << ", ";
            }
        }
    }
    os << "\"/>\n";

    return MStatus::kSuccess;
}
