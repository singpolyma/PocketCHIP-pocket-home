#include "SettingsPageWifiComponent.h"
#include "Main.h"
#include "Utils.h"

WifiAccessPointListItem::WifiAccessPointListItem(const WifiAccessPoint &ap, WifiIcons *icons)
: Button{ ap.ssid }, ap(ap), icons{ icons } {}

void WifiAccessPointListItem::paintButton(Graphics &g, bool isMouseOverButton, bool isButtonDown) {
  auto bounds = getLocalBounds();
  auto w = bounds.getWidth(), h = bounds.getHeight();

  auto iconBounds = Rectangle<float>(w - h, 0, h, h);

  if (ap.requiresAuth) {
    icons->lockIcon->drawWithin(g, iconBounds, RectanglePlacement::fillDestination, 1.0f);
  }

  iconBounds.translate(-h, 0);

  icons->wifiStrength[ap.signalStrength]->drawWithin(g, iconBounds,
                                                     RectanglePlacement::fillDestination, 1.0f);

  g.setFont(h);
  g.setColour(findColour(DrawableButton::textColourId));
  g.drawText(getName(), bounds, Justification::centredLeft);
}

SettingsPageWifiComponent::SettingsPageWifiComponent() {
  pageStack = new PageStackComponent();
  addAndMakeVisible(pageStack);

  wifiIcon = Drawable::createFromImageData(BinaryData::wifiIcon_png, BinaryData::wifiIcon_pngSize);
  addAndMakeVisible(wifiIcon);

  icons = new WifiIcons();

  icons->lockIcon = Drawable::createFromImageData(BinaryData::lock_png, BinaryData::lock_pngSize);

  icons->wifiStrength = OwnedArray<Drawable>();
  icons->wifiStrength.set(0, Drawable::createFromImageData(BinaryData::wifiStrength0_png,
                                                           BinaryData::wifiStrength0_pngSize));
  icons->wifiStrength.set(1, Drawable::createFromImageData(BinaryData::wifiStrength1_png,
                                                           BinaryData::wifiStrength1_pngSize));
  icons->wifiStrength.set(2, Drawable::createFromImageData(BinaryData::wifiStrength2_png,
                                                           BinaryData::wifiStrength2_pngSize));
  icons->wifiStrength.set(3, Drawable::createFromImageData(BinaryData::wifiStrength3_png,
                                                           BinaryData::wifiStrength3_pngSize));

  switchComponent = new SwitchComponent();
  switchComponent->addListener(this);
  switchComponent->toFront(false);
  addAndMakeVisible(switchComponent);

  // create back button
  ScopedPointer<Drawable> backButtonDrawable =
      Drawable::createFromImageData(BinaryData::backIcon_png, BinaryData::backIcon_pngSize);
  backButton = createImageButtonFromDrawable("Back", *backButtonDrawable);
  backButton->addListener(this);
  backButton->setAlwaysOnTop(true);
  addAndMakeVisible(backButton);

  // create ssid list "page"
  accessPointListPage = new TrainComponent();
  accessPointListPage->setOrientation(TrainComponent::kOrientationVertical);
  accessPointListPage->itemHeight = 32;
  accessPointListPage->itemScaleMin = 0.9f;

  auto wifiListJson = parseWifiListJson("../../assets/wifi.json");
  for (const auto &apJson : *wifiListJson.getArray()) {
    WifiAccessPoint ap;
    ap.ssid = apJson["name"];
    ap.signalStrength = apJson["strength"];
    ap.requiresAuth = apJson["auth"];
    auto item = new WifiAccessPointListItem(ap, icons);
    item->addListener(this);
    accessPointItems.add(item);
    accessPointListPage->addItem(item);
  }

  // create connection "page"
  connectionPage = new Component("Connection Page");

  connectionLabel = new Label("Connected", "Connection Label");
  connectionLabel->setJustificationType(juce::Justification::centred);
  connectionPage->addAndMakeVisible(connectionLabel);

  passwordEditor = new TextEditor("Password", (juce_wchar)0x2022);
  passwordEditor->setText("Password");
  connectionPage->addAndMakeVisible(passwordEditor);

  connectionButton = new TextButton("Connection Button");
  connectionButton->setButtonText("Connect");
  connectionButton->addListener(this);
  connectionPage->addAndMakeVisible(connectionButton);
}

SettingsPageWifiComponent::~SettingsPageWifiComponent() {}

void SettingsPageWifiComponent::paint(Graphics &g) {}

void SettingsPageWifiComponent::setWifiEnabled(bool enabled) {
  pageStack->setVisible(enabled);
  if (enabled) {
    Component *nextPage = wifiConnected ? connectionPage : accessPointListPage;
    pageStack->pushPage(nextPage, PageStackComponent::kTransitionNone);
  }
}

var SettingsPageWifiComponent::parseWifiListJson(const String &path) {
  auto ssidListFile = absoluteFileFromPath(path);
  auto ssidListJson = JSON::parse(ssidListFile);
  if (!ssidListJson) {
    std::cerr << "Could not read wifi.json file from " << ssidListFile.getFullPathName()
              << std::endl;
  }
  return ssidListJson;
}

void SettingsPageWifiComponent::resized() {
  auto bounds = getLocalBounds();
  auto pageBounds = Rectangle<int>(120, 0, bounds.getWidth() - 120, bounds.getHeight());

  backButton->setBounds(10, 10, 62, 62);

  pageStack->setBounds(pageBounds);

  connectionLabel->setBounds(10, 90, pageBounds.getWidth() - 20, 24);
  passwordEditor->setBounds(90, 120, pageBounds.getWidth() - 180, 24);
  connectionButton->setBounds(90, 160, pageBounds.getWidth() - 180, 24);

  wifiIcon->setTopLeftPosition(bounds.getX(), bounds.getHeight() / 2.0f - 20);

  {
    auto t = switchComponent->getTransform();
    t = AffineTransform::identity.rotated(-float_Pi / 2.0)
            .translated(bounds.getX() + 75, bounds.getHeight() / 2.0f + 40);
    switchComponent->setTransform(t);
  }
}

void SettingsPageWifiComponent::buttonClicked(Button *button) {
  if (button == connectionButton) {
    if (wifiConnected) {
      wifiConnected = false;
      passwordEditor->setVisible(true);
      connectionButton->setButtonText("Connect");
      pageStack->pushPage(accessPointListPage, PageStackComponent::kTransitionNone);
    } else {
      wifiConnected = true;
      passwordEditor->setVisible(false);
      connectionButton->setButtonText("Disconnect");
    }
  }

  if (button == backButton) {
    if (pageStack->getDepth() > 1) {
      pageStack->popPage(PageStackComponent::kTransitionTranslateHorizontal);
    } else {
      getMainStack().popPage(PageStackComponent::kTransitionTranslateHorizontal);
    }
  }

  auto apButton = dynamic_cast<WifiAccessPointListItem *>(button);
  if (apButton) {
    passwordEditor->setVisible(apButton->ap.requiresAuth);

    connectionLabel->setText(apButton->ap.ssid, juce::NotificationType::dontSendNotification);
    pageStack->pushPage(connectionPage, PageStackComponent::kTransitionTranslateHorizontal);
  }
}

void SettingsPageWifiComponent::buttonStateChanged(Button *button) {
  if (button == switchComponent && wifiEnabled != button->getToggleState()) {
    wifiEnabled = button->getToggleState();
    setWifiEnabled(wifiEnabled);
  }
}
