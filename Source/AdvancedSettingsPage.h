#ifndef ADVANCEDSETTINGS_H
#define ADVANCEDSETTINGS_H

#include "../JuceLibraryCode/JuceHeader.h"
#include "SettingsPageLogin.h"
#include "PersonalizePageComponent.h"
#include "Utils.h"
#include "Main.h"

using namespace juce;

class AdvancedSettingsPage : public Component, public Button::Listener{
public:
  AdvancedSettingsPage(LauncherComponent*);
  ~AdvancedSettingsPage();
  void buttonClicked(Button*) override;
  void paint(Graphics&) override;
  void resized() override;
  void deleteIcon(String, String);
  void displayNoPassword();

private:
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AdvancedSettingsPage)
  //Title of the pane
  Label title;
  //BackButton
  ScopedPointer<ImageButton> backButton;
  //Background color
  Colour bg_color;
  
  //Pages with associated buttons
  TextButton addLogin;
  TextButton removeLogin;
  ScopedPointer<SettingsPageLogin> spl;
  
  TextButton personalizeButton;
  ScopedPointer<PersonalizePageComponent> ppc;
};

#endif