#ifndef PERSONAlIZEPAGE_H
#define PERSONAlIZEPAGE_H

#include "../JuceLibraryCode/JuceHeader.h"
#include "Utils.h"
#include "Main.h"
class LauncherComponent;

/* Adding personnalization page */
class PersonalizePageComponent : public Component, private Button::Listener, private ComboBox::Listener {
public:
  PersonalizePageComponent(LauncherComponent*);
  ~PersonalizePageComponent();
  
  void paint(Graphics &g) override;
  void resized() override;
  void buttonClicked(Button*) override;
  void comboBoxChanged(ComboBox*) override;
  void showAddComponents(bool);
  void resetApplySuccess();
  void deleteIcon(String, String);
  
private:
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PersonalizePageComponent)
  void updateFile(bool);
  bool updateJSON();
  void updateComboBox();
  
  ScopedPointer<ImageButton> backButton;
  Colour bgColor;
  Image bgImage;
  File config;
  var json;
  
  /* Launcher component */
  LauncherComponent* lcomp;
  
  /* Labels for inputs */
  Label background;
  Label icons;
  
  /* Labels to show before the inputs */
  Label opt_back;
  Label opt_name;
  Label opt_img;
  Label opt_shell;
  Label success;
  
  /* Inputs */
  ComboBox choose_back;
  TextButton add_btn;
  TextButton browse;
  
  TextButton apply;
  TextEditor edit_back;
  TextEditor edit_name;
  TextEditor edit_icn;
  TextEditor edit_shell;
};

#endif