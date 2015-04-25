#include "SimpleKeyGen.h"
#include "IPlug_include_in_plug_src.h"
#include "IControl.h"
#include "resource.h"

#include <iostream>
#include <fstream>
#include <string>
#include "sha256.h"
using namespace std;

const int kNumPrograms = 1;

enum EParams
{
  kGain = 0,
  kNumParams
};

enum ETypes
{
  kDefault = 0,
  kShifterOne = 1,
  kReverbOne = 2,
  kNumTypes
};

enum ELayout
{
  kWidth = GUI_WIDTH,
  kHeight = GUI_HEIGHT,

  kGainX = 0,
  kGainY = 0,
  kKnobFrames = 60
};

SimpleKeyGen::SimpleKeyGen(IPlugInstanceInfo instanceInfo)
  :	IPLUG_CTOR(kNumParams, kNumPrograms, instanceInfo), mGain(1.)
{
  TRACE;

  //arguments are: name, defaultVal, minVal, maxVal, step, label
  GetParam(kGain)->InitDouble("Gain", 0., 0., 100.0, 0.01, "%");
  GetParam(kGain)->SetShape(2.);

  IGraphics* pGraphics = MakeGraphics(this, kWidth, kHeight);
  pGraphics->AttachPanelBackground(&COLOR_WHITE);

  IBitmap knob = pGraphics->LoadIBitmap(KNOB_ID, KNOB_FN, kKnobFrames);

  gainControl = new IKnobMultiControl(this, kGainX, kGainY, kGain, &knob);
  gainControl->Hide(true);
  pGraphics->AttachControl(gainControl);

  
  char userId = 0;
  FILE *checkFile = popen("ifconfig en0 | awk '/ether/{print $2}'", "r");
  fscanf(checkFile, "%s", &userId);
  pclose(checkFile);
  string uid(&userId);
  
  string label1 = "ShifterOne";
  string shifterOneHashed = DoKeyGen(uid, kShifterOne);
  
  string label2 = "ReverbOne";
  string reverbOneHashed = DoKeyGen(uid, kReverbOne);

  editableControl = new IEditabletextControl(this, IRECT(0, 0, GUI_WIDTH,  20), &userId);
  shifterOneControl = new ITextControlDisplay(this, IRECT(100, 100, GUI_WIDTH,  120), shifterOneHashed.data(), label1.data());
  reverbOneControl = new ITextControlDisplay(this, IRECT(100, 200, GUI_WIDTH,  220), reverbOneHashed.data(), label2.data());
  
  pGraphics->AttachControl(editableControl);
  pGraphics->AttachControl(shifterOneControl);
  pGraphics->AttachControl(reverbOneControl);
  
  mKeyCatcher = new IKeyCatcher(this, IRECT(0, 0, kWidth, kHeight), &userId, shifterOneHashed.data(), reverbOneHashed.data(), "/Users/ajja/Desktop/licenceFile.txt");
  pGraphics->AttachKeyCatcher(mKeyCatcher);
  AttachGraphics(pGraphics);

  //MakePreset("preset 1", ... );
  MakeDefaultPreset((char *) "-", kNumPrograms);
}

string SimpleKeyGen::DoKeyGen(string uid, int type){
  string salted;
  switch (type)
  {
    case kShifterOne:
      salted.append(uid);
      salted.append(uid.substr (0,10));
      salted.append(uid.substr (3,6));
      break;
      
    case kReverbOne:
      salted.append(uid);
      salted.append(uid.substr (0,11));
      salted.append(uid.substr (3,7));
      break;
      
    default:
      break;
  }
  return sha256(salted);
  
}


SimpleKeyGen::~SimpleKeyGen() {}

void SimpleKeyGen::ProcessDoubleReplacing(double** inputs, double** outputs, int nFrames)
{
  // Mutex is already locked for us.

  double* in1 = inputs[0];
  double* in2 = inputs[1];
  double* out1 = outputs[0];
  double* out2 = outputs[1];

  for (int s = 0; s < nFrames; ++s, ++in1, ++in2, ++out1, ++out2)
  {
    *out1 = *in1 * mGain;
    *out2 = *in2 * mGain;
  }
}

void SimpleKeyGen::Reset()
{
  TRACE;
  IMutexLock lock(this);
}

void SimpleKeyGen::OnParamChange(int paramIdx)
{
  IMutexLock lock(this);

  shifterOneControl->TextFromTextEntry(DoKeyGen(editableControl->GetTextValue().Get(), kShifterOne).data());
  reverbOneControl->TextFromTextEntry(DoKeyGen(editableControl->GetTextValue().Get(), kReverbOne).data());
  mKeyCatcher->SetStrings(editableControl->GetTextValue().Get(), shifterOneControl->GetTextValue().Get(), reverbOneControl->GetTextValue().Get());
  
  GetGUI()->SetAllControlsDirty();
}
