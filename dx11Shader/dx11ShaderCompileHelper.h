#ifndef _dx11ShaderCompileHelper_h_
#define _dx11ShaderCompileHelper_h_
//-
// ==========================================================================
// Copyright 1995,2006,2008,2011 Autodesk, Inc. All rights reserved.
//
// Use of this software is subject to the terms of the Autodesk
// license agreement provided at the time of installation or download,
// or which otherwise accompanies this software in either electronic
// or hard copy form.
// ==========================================================================
//+

#include <list>
#include <maya/MApiNamespace.h>

struct ID3D11Device;
struct ID3DX11Effect;
class dx11ShaderNode;


namespace CDX11EffectCompileHelper
{
    // Release the effect from the specified node
    void releaseEffect(dx11ShaderNode* node, ID3DX11Effect* effect, const MString& fileName);

    // Build an effect from an external file
    ID3DX11Effect* build(dx11ShaderNode* node, ID3D11Device* device, const MString& fileName, MString &errorLog, bool useStrictness = false);

    // Clone an effect - used for the duplicate operation
    ID3DX11Effect* build(dx11ShaderNode* node, ID3D11Device* device, const MString& fileName, ID3DX11Effect* effectSource, MString &errorLog);

    // Load a compiled effect
    ID3DX11Effect* build(dx11ShaderNode* node, ID3D11Device* device, const void* buffer, unsigned int dataSize, MString &errorLog, bool useStrictness = false);

    // Get the absolute path of an effect file
    MString resolveShaderFileName(const MString& shaderPath, bool* fileExists = NULL);

    // Get the list of the nodes that use the specified shader file
    typedef std::list< dx11ShaderNode* > ShaderNodeList;
    void getNodesUsingEffect(const MString& fileName, ShaderNodeList &nodes);
};

#endif //_dx11ShaderCompilHelper_h_
