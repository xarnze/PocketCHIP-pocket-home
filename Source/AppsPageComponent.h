#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

#include "Grid.h"

class AppsPageComponent;
class LauncherComponent;

class AppCheckTimer : public Timer {
public:
  AppCheckTimer() {};
  virtual void timerCallback() override;
  // TODO: better pointer usage, weakref for cycle relationship?
  AppsPageComponent* appsPage;
};

class AppDebounceTimer : public Timer {
public:
  AppDebounceTimer() {};
  virtual void timerCallback() override;
  // TODO: better pointer usage, weakref for cycle relationship?
  AppsPageComponent* appsPage;
};

class AppIconButton : public DrawableButton {
public:
  AppIconButton(const String &label, const String &shell, const Drawable *image);
  
  const String shell;
  
  Rectangle<float> getImageBounds() const override;
};

class AppListComponent : public Component, public Button::Listener{
public:
  AppListComponent();
  ~AppListComponent();
  
  ScopedPointer<Grid> grid;
  
  OwnedArray<Component> gridIcons;
  OwnedArray<DrawableImage> iconDrawableImages;
  ScopedPointer<ImageButton> nextPageBtn;
  ScopedPointer<ImageButton> prevPageBtn;
  
  void resized() override;
  void checkShowPageNav();
  
  void next();
  void previous();
  
  void addAndOwnIcon(const String &name, Component *icon);
  DrawableButton *createAndOwnIcon(const String &name, const String &iconPath, const String &shell);
  
  void removeIcon(Component*);
  virtual Array<DrawableButton *> createIconsFromJsonArray(const var &json);
  
  void buttonStateChanged(Button*) override {};
  void buttonClicked(Button *button) override {};
  
  void mouseDrag(const MouseEvent&) override {};
  // FIXME: this is barsize from launcher component
  double btnHeight = 50;
  
private:
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AppListComponent)
};

class AppsPageComponent : public AppListComponent{
public:
  AppsPageComponent(LauncherComponent* launcherComponent);
  ~AppsPageComponent();
  
  Array<DrawableButton *> createIconsFromJsonArray(const var &json) override;
  
  OwnedArray<ChildProcess> runningApps;
  
  virtual void buttonStateChanged(Button*) override;
  void buttonClicked(Button *button) override;
  void mouseDrag(const MouseEvent&) override;
  void mouseUp(const MouseEvent&) override;
  
  void checkRunningApps();
  
  bool debounce = false;

private:
  using AppRunningMap = HashMap<AppIconButton*, int>;
  ScopedPointer<Drawable> cpy;

  DrawableButton* appsLibraryBtn;
  LauncherComponent* launcherComponent;
  
  AppRunningMap runningAppsByButton;
  AppCheckTimer runningCheckTimer;
  AppDebounceTimer debounceTimer;

  void startApp(AppIconButton* appButton);
  void focusApp(AppIconButton* appButton, const String& windowId);
  void startOrFocusApp(AppIconButton* appButton);
  void openAppsLibrary();
  
  //Trash Icon
  ScopedPointer<ImageButton> trashButton;
  
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AppsPageComponent)
  
  //Store mouse dragged event X and Y
  int x;
  int y;
  bool shouldMove;
};

class NavigationListener : public Button::Listener {
public:
    NavigationListener(Button*, AppListComponent*);
    void buttonClicked(Button *button) override;

private:
    Button* next;
    AppListComponent* page;
};
