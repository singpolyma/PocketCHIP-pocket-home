#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

#include "PowerPageComponent.h"
#include "SwitchComponent.h"
#include "PageStackComponent.h"

class PowerPageComponent;
class LoginPage;

class PowerSpinnerTimer : public Timer {
public:
    PowerSpinnerTimer() {};
    void timerCallback() override;
    PowerPageComponent* powerComponent;
    int i = 0;
};

class PowerPageComponent : public Component, private Button::Listener {
public:

    StretchableLayoutManager verticalLayout;
    
    ScopedPointer<ImageButton> backButton;
    ScopedPointer<TextButton> powerOffButton;
    ScopedPointer<TextButton> rebootButton;
    ScopedPointer<TextButton> sleepButton;
    ScopedPointer<TextButton> felButton;
    ScopedPointer<Label> buildNameLabel;
    ScopedPointer<Label> rev;
    ScopedPointer<Component> mainPage;
    ScopedPointer<ImageComponent> powerSpinner;
    ScopedPointer<AlertWindow> updateWindow;
    
    PowerSpinnerTimer powerSpinnerTimer;
    Array<Image> launchSpinnerImages;
    HashMap<String, Component *> pagesByName;
  
  String buildName;
    ScopedPointer<PageStackComponent> pageStack;
    ScopedPointer<Component> felPage;
    

  PowerPageComponent();
  ~PowerPageComponent();
    
  void paint(Graphics &g) override;
  void resized() override;
  void showPowerSpinner();
  void buttonStateChanged(Button*) override;
  void buttonClicked(Button*) override;
  void setSleep();
  void hideLockscreen();
  
  static unsigned char rev_number;
  static unsigned char bug_number;
  
private:
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PowerPageComponent)
  Colour bgColor;
  Image bgImage;
  String bgImagePath;
  ChildProcess child;

#ifndef WITHOUT_LOGIN
  //Lockscreen to display when the screen goes sleeping
  ScopedPointer<LoginPage> lockscreen;
#endif
};

