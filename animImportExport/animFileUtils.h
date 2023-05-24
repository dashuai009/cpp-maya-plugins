//-
// ==========================================================================
// Copyright (C) 1995 - 2006 Autodesk, Inc. and/or its licensors.  All 
// rights reserved.
//
// The coded instructions, statements, computer programs, and/or related 
// material (collectively the "Data") in these files contain unpublished 
// information proprietary to Autodesk, Inc. ("Autodesk") and/or its 
// licensors, which is protected by U.S. and Canadian federal copyright 
// law and by international treaties.
//
// The Data is provided for use exclusively by You. You have the right 
// to use, modify, and incorporate this Data into other products for 
// purposes authorized by the Autodesk software license agreement, 
// without fee.
//
// The copyright notices in the Software and this entire statement, 
// including the above license grant, this restriction and the 
// following disclaimer, must be included in all copies of the 
// Software, in whole or in part, and all derivative works of 
// the Software, unless such copies or derivative works are solely 
// in the form of machine-executable object code generated by a 
// source language processor.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND. 
// AUTODESK DOES NOT MAKE AND HEREBY DISCLAIMS ANY EXPRESS OR IMPLIED 
// WARRANTIES INCLUDING, BUT NOT LIMITED TO, THE WARRANTIES OF 
// NON-INFRINGEMENT, MERCHANTABILITY OR FITNESS FOR A PARTICULAR 
// PURPOSE, OR ARISING FROM A COURSE OF DEALING, USAGE, OR 
// TRADE PRACTICE. IN NO EVENT WILL AUTODESK AND/OR ITS LICENSORS 
// BE LIABLE FOR ANY LOST REVENUES, DATA, OR PROFITS, OR SPECIAL, 
// DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES, EVEN IF AUTODESK 
// AND/OR ITS LICENSORS HAS BEEN ADVISED OF THE POSSIBILITY 
// OR PROBABILITY OF SUCH DAMAGES.
//
// ==========================================================================
//+

//
//  File Name:  animFileUitls.h
//
//  Description:
//
//

#ifndef _animFileUtils
#define _animFileUtils

#include <maya/MFnAnimCurve.h>
#include <maya/MAngle.h>
#include <maya/MTime.h>
#include <maya/MDistance.h>

#include <iosfwd>

// The base class for the translators.
//
class animBase {
public:
    animBase();
    virtual ~animBase();

    enum AnimBaseType           {kAnimBaseUnitless, kAnimBaseTime,
                                    kAnimBaseLinear, kAnimBaseAngular};

    const char *                tangentTypeAsWord(MFnAnimCurve::TangentType);
    MFnAnimCurve::TangentType   wordAsTangentType(char *);
    const char *                infinityTypeAsWord(MFnAnimCurve::InfinityType);
    MFnAnimCurve::InfinityType  wordAsInfinityType(const char *);
    const char *                outputTypeAsWord(MFnAnimCurve::AnimCurveType);
    MFnAnimCurve::AnimCurveType typeAsAnimCurveType(AnimBaseType,
                                                    AnimBaseType);

    AnimBaseType                wordAsOutputType(const char *);
    AnimBaseType                wordAsInputType(const char *);
    const char *                boolInputTypeAsWord(bool);

    double                      asDouble(std::ifstream &);
    char *                      asString(std::ifstream &);
    char *                      asWord(std::ifstream &, bool = false);
    char                        asChar(std::ifstream &);

    bool                        isNextNumeric(std::ifstream &);
    bool                        isEquivalent(double, double);
protected:
    void                        resetUnits();
    void                        advance(std::ifstream &);

    MTime::Unit                 timeUnit;
    MAngle::Unit                angularUnit;
    MDistance::Unit             linearUnit;
};

class animReader : public animBase {
public:
    animReader();
    ~animReader() override;

    MStatus readClipboard(std::ifstream &, MAnimCurveClipboard&);
protected:
    bool    readAnimCurve(std::ifstream&, MAnimCurveClipboardItem&);
    void    convertAnglesAndWeights2To3(MFnAnimCurve::AnimCurveType, bool,
                                        MAngle &, double &);
    void    convertAnglesAndWeights3To2(MFnAnimCurve::AnimCurveType, bool,
                                        MAngle &, double &);

    bool    convertAnglesFromV2To3;
    bool    convertAnglesFromV3To2;
    double  animVersion;
};

class animWriter : public animBase {
public:
    animWriter();
    ~animWriter() override;
    
    MStatus writeClipboard(std::ofstream&, const MAnimCurveClipboard&,
                            bool = false, bool = false);
protected:
    bool    writeHeader(std::ofstream&);
    bool    writeAnim(std::ofstream&, const MAnimCurveClipboardItem&,
                        bool = false, bool = false);
    bool    writeAnimCurve(std::ofstream&, const MObject *,
                            MFnAnimCurve::AnimCurveType,
                            bool = false);
};

class animUnitNames {
public:
    animUnitNames();
    virtual ~animUnitNames();

    static void setToLongName(const MAngle::Unit&, MString&);
    static void setToShortName(const MAngle::Unit&, MString&);

    static void setToLongName(const MDistance::Unit&, MString&);
    static void setToShortName(const MDistance::Unit&, MString&);

    static void setToLongName(const MTime::Unit&, MString&);
    static void setToShortName(const MTime::Unit&, MString&);

    static bool setFromName(const MString&, MAngle::Unit&);
    static bool setFromName(const MString&, MDistance::Unit&);
    static bool setFromName(const MString&, MTime::Unit&);
};
#endif /* _animFileUtils */
