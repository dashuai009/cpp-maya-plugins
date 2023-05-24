#include "metadataXML.h"
#include <string.h>

using namespace adsk;
using namespace adsk::Data;
using namespace adsk::Data::XML;

//----------------------------------------------------------------------
//
//
//
//
xmlNode*
Util::findNamedNode(
    xmlNode*    rootNode,
    const char* childName )
{
    // Firewall for safety
    if( ! rootNode ) return NULL;

    // Walk the children looking for one with the specific name
    for (xmlNode* curNode = rootNode; curNode; curNode = curNode->next)
    {
        // Skip anything that's not a node, for maximum flexibility
        if (curNode->type != XML_ELEMENT_NODE)
{
            continue;
        }

        // Check to see if the element has the desired name
        if( 0 == strcmp((const char*)curNode->name, childName) )
        {
            return curNode;
        }
    }
    return NULL;
}

//----------------------------------------------------------------------
//
//
//
//
xmlChar*
Util::findText(
    xmlDocPtr   doc,
    xmlNode*    node )
{
    return node ? xmlNodeListGetString(doc, node->xmlChildrenNode, 1) : NULL;
}

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
