#ifndef structureSerializerXML_h
#define structureSerializerXML_h

#include <maya/adskDataStructureSerializer.h>   // for base class
#include <maya/adskCommon.h>

namespace adsk {
  namespace Data {
    class Structure;
  }
}

// ****************************************************************************
using namespace adsk::Data;
class StructureSerializerXML : public StructureSerializer
{
    DeclareSerializerFormat(StructureSerializerXML, StructureSerializer);
public:
    ~StructureSerializerXML() override;

    // Mandatory implementation overrides
    Structure*  read    (std::istream&      cSrc,
                                 std::string&       errors)     const override;
    int         write   (const Structure&   dataToWrite,
                                 std::ostream&      cDst)       const override;
    void    getFormatDescription(std::ostream&  info)       const override;

private:
    StructureSerializerXML();       
};

//-
// ==================================================================
// Copyright 2015 Autodesk, Inc.  All rights reserved.
// 
// This computer source code  and related  instructions and comments are
// the unpublished confidential and proprietary information of Autodesk,
// Inc. and are  protected  under applicable  copyright and trade secret
// law. They may not  be disclosed to, copied or used by any third party
// without the prior written consent of Autodesk, Inc.
// ==================================================================
//+
#endif // structureSerializerXML_h
