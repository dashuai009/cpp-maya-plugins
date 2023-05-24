#ifndef associationsSerializerXML_h
#define associationsSerializerXML_h

#include <maya/adskDataAssociationsSerializer.h>    // for base class
#include <libxml/tree.h>                // for xmlNode
#include <maya/adskCommon.h>

namespace adsk {
  namespace Data {
    class Associations;
  }
}

// ****************************************************************************
using namespace adsk::Data;
class AssociationsSerializerXML : public AssociationsSerializer
{
    DeclareSerializerFormat(AssociationsSerializerXML, AssociationsSerializer);
public:
    ~AssociationsSerializerXML() override;

    // Mandatory implementation overrides
    adsk::Data::Associations*
                        read        (std::istream&       cSrc,
                                     std::string&        errors)    const override;
    int         write       (const Associations& dataToWrite,
                                     std::ostream&       cDst,
                                     std::string&        errors)    const override;
    void        getFormatDescription(std::ostream& info)    const override;

    // Partial interface to allow passing off parsing of a subsection of a
    // metadata DOM to the Stream subsection
    adsk::Data::Associations* parseDOM  (xmlDocPtr      doc,
                                         xmlNode&       streamNode,
                                         unsigned int&  errorCount,
                                         std::string&   errors )    const;

private:
    AssociationsSerializerXML();        
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
#endif // associationsSerializerXML_h
