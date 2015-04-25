#ifndef __SIMPLEKEYGEN__
#define __SIMPLEKEYGEN__

#include "IPlug_include_in_plug_hdr.h"
#import "TextControls.h"

class SimpleKeyGen : public IPlug
{
public:
  SimpleKeyGen(IPlugInstanceInfo instanceInfo);
  ~SimpleKeyGen();

  void Reset();
  void OnParamChange(int paramIdx);
  void ProcessDoubleReplacing(double** inputs, double** outputs, int nFrames);
  string DoKeyGen(string uid, int type);

private:
  double mGain;
  ITextControlDisplay *textControl, *reverbOneControl, *shifterOneControl;
  IKnobMultiControl *gainControl;
  IEditabletextControl *editableControl;
  IKeyCatcher *mKeyCatcher;
};

#endif
