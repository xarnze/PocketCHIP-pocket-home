#ifndef SETTINGSPAGELOGIN_H
#define SETTINGSPAGELOGIN_H

#include "../JuceLibraryCode/JuceHeader.h"
#include <stdlib.h>
#include "Utils.h"
#include "Main.h"
#include <openssl/sha.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>

using namespace juce;

class SettingsPageLogin : public Component, public Button::Listener{
public:
  SettingsPageLogin();
  ~SettingsPageLogin();
  void buttonClicked(Button*) override;
  void paint(Graphics&) override;
  static String hashString(const String&);
  void switchToRemove();
  void switchToModify();
  bool hasPassword();
  
private:
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SettingsPageLogin)
  //Title of the pane
  Label title;
  bool deletemode;
  //BackButton
  ScopedPointer<ImageButton> backButton;
  //Background color
  Colour bg_color;
  //Does the home have a password ?
  bool has_password;
  bool has_file;
  //Current hashed password
  String cur_hashed;
  
  void clearAllFields();
  void loadPassword();
  bool isPasswordCorrect(const String&);
  void savePassword();
  void deletePassword();
  void createIfNotExists();
  void setLocations();
  bool passwordIdentical();
  
  //Text field for the root password
  Label root_label;
  TextEditor root_password;
  //Text field for the current password (if there is)
  Label cur_label;
  TextEditor cur_password;
  //Text field for the new password
  Label new_label;
  TextEditor new_password;
  //Text field for the confirmation of the new password
  Label ret_label;
  TextEditor ret_password;
  //Button for applying the settings
  TextButton apply;
};

#endif