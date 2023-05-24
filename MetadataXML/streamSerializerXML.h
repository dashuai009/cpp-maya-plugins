#ifndef streamSerializerXML_h
#define streamSerializerXML_h

#include <maya/adskDataStreamSerializer.h>  // for base class
#include <libxml/tree.h>                // for xmlNode
#include <maya/adskCommon.h>

namespace adsk {
  namespace Data {
    class Stream;
  }
}

// ****************************************************************************
using namespace adsk::Data;
class StreamSerializerXML : public adsk::Data::StreamSerializer
{
    DeclareSerializerFormat(StreamSerializerXML, adsk::Data::StreamSerializer);
public:
    ~StreamSerializerXML() override;

    // Mandatory implementation overrides
    adsk::Data::Stream*
                        read        (std::istream&      cSrc,
                                     std::string&       errors)     const override;
    int         write       (const adsk::Data::Stream&  dataToWrite,
                                     std::ostream&      cDst,
                                     std::string&       errors)     const override;
    void        getFormatDescription(std::ostream& info)    const override;

    // Partial interface to allow passing off parsing of a subsection of a
    // metadata DOM to the Stream subsection
    adsk::Data::Stream* parseDOM    (xmlDocPtr      doc,
                                     xmlNode&       streamNode,
                                     unsigned int&  errorCount,
                                     std::string&   errors )    const;

private:
    StreamSerializerXML();          
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
