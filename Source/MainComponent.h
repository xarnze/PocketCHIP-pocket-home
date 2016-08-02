#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

#include "LauncherComponent.h"
#include "PageStackComponent.h"
#include "SettingsPageLogin.h"

class MainContentComponent;

class LoginPage : public Component, public Button::Listener{
public:
  LoginPage(MainContentComponent*);
  ~LoginPage();
  
  void resized() override;
  void paint(Graphics &) override;
  void buttonClicked(Button *button) override;
  
  bool hasPassword();
  
private:
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LoginPage)
  void displayError();
  
  MainContentComponent* main_page;
  ScopedPointer<TextButton> log;
  ScopedPointer<Label> label_password;
  String hashed_password;
  ScopedPointer<DrawableImage> bgImage;
  ScopedPointer<DrawableImage> ntcIcon;
  ScopedPointer<TextEditor> cur_password;
};

class MainContentComponent : public Component {
public:
  ScopedPointer<LauncherComponent> launcher;
  ScopedPointer<LookAndFeel> lookAndFeel;

  ScopedPointer<PageStackComponent> pageStack;

  MainContentComponent(const var &configJson);
  ~MainContentComponent();

  void paint(Graphics &) override;
  void resized() override;
  void loggedIn();
  
  void handleMainWindowInactive();

private:
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainContentComponent)
  ScopedPointer<LoginPage> lp;
};
