#include "MainComponent.h"

MainContentComponent::MainContentComponent() {
  setSize(480, 245);

  settingsPage = std::unique_ptr<SettingsPageComponent>(new SettingsPageComponent());
  settingsPage->setBounds(getLocalBounds());
  addAndMakeVisible(settingsPage.get());
}

MainContentComponent::~MainContentComponent() {}

void MainContentComponent::paint(Graphics &g) {
  g.fillAll(Colour(0xff202020));

  g.setFont(Font(16.0f));
  g.setColour(Colours::white);
  g.drawText("Herro PokeCHIP", getLocalBounds(), Justification::centred, true);
}

void MainContentComponent::resized() {
}