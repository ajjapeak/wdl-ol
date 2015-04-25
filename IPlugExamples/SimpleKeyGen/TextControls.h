#include "IPlugBase.h"
#include "IGraphics.h"

#include <string>
#include <stdio.h>

#include <ctime>

#include <fstream>

#include <iostream>

using namespace std;

enum EParamEditMsg
{
    kNone,
    kEditing,
    kUpdate,
    kCancel,
    kCommit
};

#define labelXOffset 16
#define labelYoffset 4


class ITextControlDisplay : public IControl
{
protected:
    WDL_String mStr, mStr2;
    IRECT mInputRect, mLabelRect;
    IText *pText = new IText(16, &COLOR_BLACK, "Verdana", IText::kStyleNormal, IText::kAlignCenter, 0, IText::kQualityAntiAliased);
public:
    ITextControlDisplay(IPlugBase* pPlug, IRECT pR, const char* str = "", const char* str2 = "")
    : IControl(pPlug, pR)
    {
        mDisablePrompt = true;
        mText = *pText;
        mStr.Set(str);
        mStr2.Set(str2);
        mInputRect = IRECT(pR.L, pR.T-10, pR.R, pR.B+10);
        mLabelRect = IRECT(pR.L - 100, pR.T, pR.L, pR.B);
    }
    
    ~ITextControlDisplay() {}
    
    bool Draw(IGraphics* pGraphics)
    {
        pGraphics->DrawIText(&mText, mStr2.Get(), &mLabelRect);
        return pGraphics->DrawIText(&mText, mStr.Get(), &mRECT);
    }
    
    void OnMouseDown(int x, int y, IMouseMod* pMod)
    {
        mPlug->GetGUI()->CreateTextEntry(this, &mText, &mInputRect, mStr.Get());
    }
    
    void TextFromTextEntry(const char* txt){
        mStr.Set(txt);
        SetDirty(true);
        Redraw();
        
    }
    WDL_String GetTextValue() { return mStr; }

};


class IEditabletextControl : public IControl
{
protected:
    WDL_String mStr;
    IRECT mInputRect;
    IText *pText = new IText(16, &COLOR_BLACK, "Verdana", IText::kStyleNormal, IText::kAlignCenter, 0, IText::kQualityAntiAliased);
public:
    IEditabletextControl(IPlugBase* pPlug, IRECT pR, const char* str = "")
    : IControl(pPlug, pR)
    {
        mDisablePrompt = true;
        mText = *pText;
        mStr.Set(str);
        mInputRect = IRECT(pR.L, pR.T-10, pR.R, pR.B+10);
    }
    
    ~IEditabletextControl() {}
    
    bool Draw(IGraphics* pGraphics)
    {
        return pGraphics->DrawIText(&mText, mStr.Get(), &mRECT);
    }
    
    void OnMouseDown(int x, int y, IMouseMod* pMod)
    {
        mPlug->GetGUI()->CreateTextEntry(this, &mText, &mInputRect, mStr.Get());
    }
    
    void TextFromTextEntry(const char* txt)
    {
        mStr.Set(txt);
        SetDirty(true);
        Redraw();
        mPlug->OnParamChange(0);
        
    }
    
    WDL_String GetTextValue() { return mStr; }
};

// Key catcher is an icontrol but only its OnKeyDown() is called... after all the other controls have been tested to see if they want keyboard input
class IKeyCatcher : public IControl
{
protected:
    WDL_String mStr, mStr2, mStr3, mLicencePath;
    
public:
    IKeyCatcher(IPlugBase* pPlug, IRECT pR, const char* str = "", const char* str2 = "", const char* str3 = "", const char* licencePath = "")
    : IControl(pPlug, pR) {
        mStr.Set(str);
        mStr2.Set(str2);
        mStr3.Set(str3);
        mLicencePath.Set(licencePath);
    }
    
    // this never gets called but is needed for an IControl
    bool Draw(IGraphics* pGraphics) { return false; }
    
    bool OnKeyDown(int x, int y, int key)
    {
        
        if (key == 33){
            ofstream writeFile(mLicencePath.Get(), ios::out);
            writeFile << mStr.Get() << endl;
            writeFile << mStr2.Get() << endl;
            writeFile << mStr3.Get() << endl;
            writeFile.close();
        
        }
        return true;
    }
    
    void SetStrings(const char* str = "", const char* str2 = "", const char* str3 = "")
    {
        mStr.Set(str);
        mStr2.Set(str2);
        mStr3.Set(str3);
        
    }
    
    
};
