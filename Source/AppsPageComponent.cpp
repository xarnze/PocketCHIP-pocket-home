#include "AppsPageComponent.h"
#include "LauncherComponent.h"
#include "PokeLookAndFeel.h"
#include "Main.h"
#include "Utils.h"

void AppCheckTimer::timerCallback() {
  DBG("AppCheckTimer::timerCallback - check running apps");
  if (appsPage) {
    appsPage->checkRunningApps();
  }
}

void AppDebounceTimer::timerCallback() {
  DBG("AppDebounceTimer::timerCallback - check launch debounce");
  if (appsPage) {
    appsPage->debounce = false;
  }
  stopTimer();
}

AppIconButton::AppIconButton(const String &label, const String &shell, const Drawable *image)
: DrawableButton(label, DrawableButton::ImageAboveTextLabel),
  shell(shell) {
  // FIXME: supposedly setImages will "create internal copies of its drawables"
  // this relates to AppsPageComponent ownership of drawable icons ... docs are unclear
  setImages(image);
}

Rectangle<float> AppIconButton::getImageBounds() const {
  auto bounds = getLocalBounds();
  return bounds.withHeight(PokeLookAndFeel::getDrawableButtonImageHeightForBounds(bounds)).toFloat();
}

AppListComponent::AppListComponent(Component* parent) :
  grid(new Grid(3, 2)),
  nextPageBtn(createImageButton("NextAppsPage",
                                ImageFileFormat::loadFrom(assetFile("nextIcon.png")))),
  prevPageBtn(createImageButton("PrevAppsPage",
                                ImageFileFormat::loadFrom(assetFile("backIcon.png"))))
{
  if(!parent) parent = this;
  parent->addChildComponent(nextPageBtn);
  parent->addChildComponent(prevPageBtn, 500);
  NavigationListener* list = new NavigationListener(nextPageBtn, this);
  nextPageBtn->addListener(list);
  prevPageBtn->addListener(list);
  prevPageBtn->setAlwaysOnTop(true);
  
  addAndMakeVisible(grid);
}
AppListComponent::~AppListComponent() {}

void AppListComponent::next(){
    grid->showNextPage();
    checkShowPageNav();
}

void AppListComponent::previous(){
    grid->showPrevPage();
    checkShowPageNav();
}

DrawableButton *AppListComponent::createAndOwnIcon(const String &name, const String &iconPath, const String &shell) {
  File icon = assetFile(iconPath);
  Image image;
  if(iconPath == "" || !icon.exists())
    image = createImageFromFile(assetFile("appIcons/default.png"));
  else
    image = createImageFromFile(icon);
  auto drawable = new DrawableImage();
  drawable->setImage(image);
  // FIXME: is this OwnedArray for the drawables actually necessary?
  // won't the AppIconButton correctly own the drawable?
  // Further we don't actually use this list anywhere.
  iconDrawableImages.add(drawable);
  auto button = new AppIconButton(name, shell, drawable);
  addAndOwnIcon(name, button);
  return button;
}

void AppListComponent::resized() {
  auto b = getLocalBounds();
  
  prevPageBtn->setSize(btnHeight, btnHeight);
  nextPageBtn->setSize(btnHeight, btnHeight);
  prevPageBtn->setBoundsToFit(b.getX(), b.getY(), b.getWidth(), b.getHeight(), Justification::centredLeft, true);
  nextPageBtn->setBoundsToFit(b.getX(), b.getY(), 480, b.getHeight(), Justification::centredRight, true);
  
  // drop the page buttons from our available layout size
  auto gridWidth = b.getWidth();
  auto gridHeight = b.getHeight() - (2.0*btnHeight);
  grid->setSize(gridWidth, gridHeight);
  grid->setBoundsToFit(b.getX(), b.getY(), b.getWidth(), b.getHeight(), Justification::centred, true);
  
}

void AppListComponent::checkShowPageNav() {
  if (grid->hasNextPage()) {
    nextPageBtn->setVisible(true); nextPageBtn->setEnabled(true);
  }
  else {
    nextPageBtn->setVisible(false); nextPageBtn->setEnabled(false);
  }
  
  if (grid->hasPrevPage()) {
    prevPageBtn->setVisible(true); prevPageBtn->setEnabled(true);
  }
  else {
    prevPageBtn->setVisible(false); prevPageBtn->setEnabled(false);
  }
}

void AppListComponent::addAndOwnIcon(const String &name, Component *icon) {
  gridIcons.add(icon);
  grid->addItem(icon);
  ((Button*)icon)->addListener(this);
  ((Button*)icon)->addMouseListener(this, false);
}

void AppListComponent::removeIcon(Component* icon){
  gridIcons.removeObject(icon);
  grid->removeItem(icon);
}

Array<DrawableButton *> AppListComponent::createIconsFromJsonArray(const var &json) {
  Array<DrawableButton *> buttons;
  if (json.isArray()) {
    for (const auto &item : *json.getArray()) {
      auto name = item["name"];
      auto shell = item["shell"];
      auto iconPath = item["icon"];
      if (name.isString() && shell.isString() && iconPath.isString()) {
        auto icon = createAndOwnIcon(name, iconPath, shell);
        if (icon) {
          buttons.add(icon);
        }
      }
    }
  }
  
  checkShowPageNav();
  return buttons;
}

AppsPageComponent::AppsPageComponent(LauncherComponent* launcherComponent) :
  AppListComponent(launcherComponent),
  launcherComponent(launcherComponent),
  runningCheckTimer(),
  debounceTimer(),
  x(-1), y(-1), shouldMove(false)
{
  runningCheckTimer.appsPage = this;
  debounceTimer.appsPage = this;
  cpy = nullptr;
  
  //Trash Icon
  trashButton = createImageButton(
    "Trash", createImageFromFile(assetFile("trash.png")));
  trashButton->setName("Trash");
  trashButton->setAlwaysOnTop(true);
  addAndMakeVisible(trashButton, 100);
  trashButton->setBounds(170, 15, 40, 20);
  trashButton->setVisible(false);
}

AppsPageComponent::~AppsPageComponent() {}

Array<DrawableButton *> AppsPageComponent::createIconsFromJsonArray(const var &json) {
  auto buttons = AppListComponent::createIconsFromJsonArray(json);
  
  //// hard coded "virtual" application. Cannot be removed.
  //appsLibraryBtn = createAndOwnIcon("App Get", "appIcons/update.png", String::empty);
  //buttons.add(appsLibraryBtn);
  
  checkShowPageNav();
  return buttons;
}

void AppsPageComponent::startApp(AppIconButton* appButton) {
  DBG("AppsPageComponent::startApp - " << appButton->shell);
  auto launchApp = new ChildProcess();
  launchApp->start("xmodmap ${HOME}/.Xmodmap"); // Reload xmodmap to ensure it's running
  if (launchApp->start(appButton->shell)) {
    runningApps.add(launchApp);
    runningAppsByButton.set(appButton, runningApps.indexOf(launchApp));
    // FIXME: uncomment when process running check works
    // runningCheckTimer.startTimer(5 * 1000);
    
    debounce = true;
    debounceTimer.startTimer(2 * 1000);
    
    // TODO: should probably put app button clicking logic up into LauncherComponent
    // correct level for event handling needs more thought
    launcherComponent->showLaunchSpinner();
  }
};

void AppsPageComponent::focusApp(AppIconButton* appButton, const String& windowId) {
  DBG("AppsPageComponent::focusApp - " << appButton->shell);
  String focusShell = "echo 'focus_client_by_window_id("+windowId+")' | awesome-client";
  StringArray focusCmd{"sh", "-c", focusShell.toRawUTF8()};
  ChildProcess focusWindow;
  focusWindow.start(focusCmd);
};

void AppsPageComponent::startOrFocusApp(AppIconButton* appButton) {
  if (debounce) return;
  
  bool shouldStart = true;
  int appIdx = runningAppsByButton[appButton];
  bool hasLaunched = runningApps[appIdx] != nullptr;
  String windowId;
  
  if(hasLaunched) {
    const auto shellWords = split(appButton->shell, " ");
    const auto& cmdName = shellWords[0];
    StringArray findCmd{"xdotool", "search", "--all", "--limit", "1", "--class", cmdName.toRawUTF8()};
    ChildProcess findWindow;
    findWindow.start(findCmd);
    findWindow.waitForProcessToFinish(1000);
    windowId = findWindow.readAllProcessOutput().trimEnd();
    
    // does xdotool find a window id? if so, we shouldn't start a new one
    shouldStart = (windowId.length() > 0) ? false : true;
  }
  
  if (shouldStart) {
    startApp(appButton);
  }
  else {
    focusApp(appButton, windowId);
  }
  
};

void AppsPageComponent::openAppsLibrary() {
  launcherComponent->showAppsLibrary();
}

void AppsPageComponent::checkRunningApps() {
  Array<int> needsRemove{};
  
  // check list to mark any needing removal
  for (const auto& cp : runningApps) {
    if (!cp->isRunning()) {
      needsRemove.add(runningApps.indexOf(cp));
    }
  }
  
  // cleanup list
  for (const auto appIdx : needsRemove) {
    runningApps.remove(appIdx);
    runningAppsByButton.removeValue(appIdx);
  }
  
  if (!runningApps.size()) {
    // FIXME: uncomment when process running check works
    // runningCheckTimer.stopTimer();
    launcherComponent->hideLaunchSpinner();
  }
};

void AppsPageComponent::buttonStateChanged(Button* btn) {
  auto appBtn = (AppIconButton*)btn;
  auto appIcon = (DrawableImage*)appBtn->getCurrentImage();
  auto buttonPopup = launcherComponent->focusButtonPopup.get();
  constexpr auto scale = 1.3;

  // show floating button popup if we're holding downstate and not showing the popup
  if (btn->isMouseButtonDown() &&
      btn->isMouseOver() &&
      !buttonPopup->isVisible()) {
    // copy application icon bounds in screen space
    auto boundsNext = appIcon->getScreenBounds();
    auto boundsCentre = boundsNext.getCentre();
    
    // scale and recenter
    boundsNext.setSize(boundsNext.getWidth()*scale, boundsNext.getHeight()*scale);
    boundsNext.setCentre(boundsCentre);
    
    // translate back to space local to popup parent (local bounds)
    auto parentPos = launcherComponent->getScreenPosition();
    boundsNext.setPosition(boundsNext.getPosition() - parentPos);
    
    // show popup icon, hide real button beneath
    buttonPopup->setImage(appIcon->getImage());
    buttonPopup->setBounds(boundsNext);
    buttonPopup->setVisible(true);
    appIcon->setVisible(false);
    appBtn->setColour(DrawableButton::textColourId, Colours::transparentWhite);
  }
  // set UI back to default if we can see the popup, but aren't holding the button down
  else if (btn->isVisible()) {
    appIcon->setVisible(true);
    appBtn->setColour(DrawableButton::textColourId, getLookAndFeel().findColour(DrawableButton::textColourId));
    buttonPopup->setVisible(false);
  }
}

void AppsPageComponent::mouseDrag(const MouseEvent& me){
  if(me.originalComponent == this) return;
  
  //Get the position of the mouse relative to the Grid
  Point<int> pi = me.getPosition();
  Point<int> res = this->getLocalPoint(nullptr, me.getScreenPosition());
  if(cpy==nullptr){
    DrawableButton* dragging = (DrawableButton* const) me.originalComponent;
    Drawable* img = dragging->getNormalImage();
    cpy = img->createCopy();
    cpy->setOriginWithOriginalSize(Point<float>(0.f,0.f));
    addAndMakeVisible(cpy);
  }
  int drag_x = res.x - cpy->getWidth()/2;
  int drag_y = res.y - cpy->getHeight()/2;
  trashButton->setVisible(true);

  //x==-1 means this is the first first the function is called
  //So a new icon has just been pressed
  if(x==-1 && y==-1){
    x = drag_x;
    y = drag_y;
    //We draw the dragging icon at this coordinates
    cpy->setBounds(x, y, cpy->getWidth(), cpy->getHeight());
  }
  
  //The icon should move only if it has been dragged more than 5 pixels
  shouldMove = abs(x-drag_x)>5 || abs(y-drag_y)>5;

  //Should it move ?
  if(shouldMove){
    //Let's move it !
    cpy->setBounds(drag_x, drag_y, cpy->getWidth(), cpy->getHeight());
    x = drag_x;
    y = drag_y;
  }
  
  /* If the mouse is on the top of the screen (less than 10px)
   * The icon because more transparent
   */
  if(drag_y <= 10) cpy->setAlpha(0.3);
  else cpy->setAlpha(0.9);
}

void AppsPageComponent::mouseUp(const MouseEvent& me){
  if(!cpy) return;
  bool ontrash = cpy->getAlpha()>=0.25 && cpy->getAlpha()<=0.35;
  if(ontrash){
    //On Delete icon
    Button* button = (Button*) me.originalComponent;
    bool answer = AlertWindow::showOkCancelBox(AlertWindow::AlertIconType::WarningIcon,
                                  "Delete icon ?", 
                                  "Are you sure you want to delete "+button->getName()+" ?",
                                  "Yes",
                                  "No"
                                 );
    if(answer){
        auto appButton = (AppIconButton*) button;
        launcherComponent->deleteIcon(button->getName(), appButton->shell, appButton);
    }
  }
  trashButton->setVisible(false);
  removeChildComponent(cpy);
  cpy = nullptr;
  if(me.getLengthOfMousePress() < 1000 && !ontrash)
    buttonClicked((Button*) me.originalComponent);
  checkShowPageNav();
  //Setting back "old values" to -1
  x = -1;
  y = -1;
  shouldMove = false;
}

void AppsPageComponent::buttonClicked(Button *button) {
  if (button == prevPageBtn) {
    grid->showPrevPage();
    checkShowPageNav();
  }
  else if (button == nextPageBtn) {
    grid->showNextPage();
    checkShowPageNav();
  }
  else if (button == appsLibraryBtn) {
    openAppsLibrary();
  }
  else {
    /*ModifierKeys mk = ModifierKeys::getCurrentModifiers ();
    if(mk==ModifierKeys::ctrlModifier);*/
    auto appButton = (AppIconButton*)button;
    startOrFocusApp(appButton);
  }
}

NavigationListener::NavigationListener(Button* next, AppListComponent* p): next(next), page(p){}

void NavigationListener::buttonClicked(Button *button){
    if(button==next) page->next();
    else page->previous();
}
