#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

#include "OverlaySpinner.h"
#include "LauncherBarComponent.h"
#include "PageStackComponent.h"
#include "BatteryMonitor.h"
#include "SwitchComponent.h"
#include "ClockMonitor.hpp"
#include <sstream>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <net/if.h>
#include <arpa/inet.h>

class LauncherComponent;
class LibraryPageComponent;
class AppsPageComponent;

class BatteryIconTimer : public Timer {
public:
    BatteryIconTimer() {};
    void timerCallback();
    LauncherComponent* launcherComponent;
};

class WifiIconTimer : public Timer {
public:
  WifiIconTimer() {};
  void timerCallback();
  LauncherComponent* launcherComponent;
};

class LauncherComponent : public Component, private Button::Listener {
public:
    BatteryMonitor batteryMonitor;
    ScopedPointer<LauncherBarComponent> botButtons;
    ScopedPointer<LauncherBarComponent> topButtons;
    ScopedPointer<OverlaySpinner> launchSpinner;
    ScopedPointer<ImageComponent> focusButtonPopup;
  
    Array<Image> batteryIconImages;
    Array<Image> batteryIconChargingImages;
    Array<Image> wifiIconImages;
    
    ScopedPointer<Label> batteryLabel;
    ScopedPointer<Label> modeLabel;
  
    BatteryIconTimer batteryIconTimer;
    WifiIconTimer wifiIconTimer;
    Component* defaultPage;
  
    // FIXME: we have no need for the pages/pagesByName if we're using scoped pointers for each page.
    // All these variables do is add an extra string key the compiler can't see through.
    OwnedArray<Component> pages;
    ScopedPointer<PageStackComponent> pageStack;
    HashMap<String, Component *> pagesByName;
    
    bool resize = false;
    
    StretchableLayoutManager categoryButtonLayout;
    
    LauncherComponent(const var &configJson);
    ~LauncherComponent();
    
    void paint(Graphics &) override;
    void resized() override;
    void updateIp();
    void setIpVisible(bool);
  
    void showAppsLibrary();
    void showLaunchSpinner();
    void hideLaunchSpinner();

    void deleteIcon(String,String,Component*);
    void setClockAMPM(bool);
    void addIcon(const String&, const String&, const String&);
    void setColorBackground(const String&);
    void setImageBackground(const String&);
    void setClockVisible(bool);
    
private:
    Colour bgColor;
    Label labelip;
    Image bgImage;
    bool hasImg;
    ScopedPointer<ClockMonitor> clock;
  
    void buttonClicked(Button *) override;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LauncherComponent)
};
