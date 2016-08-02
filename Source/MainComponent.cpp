#include "MainComponent.h"
#include "PokeLookAndFeel.h"
#include "LauncherComponent.h"
#include "Utils.h"
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>

MainContentComponent::MainContentComponent(const var &configJson):
lp(new LoginPage(this))
{
  lookAndFeel = new PokeLookAndFeel();
  setLookAndFeel(lookAndFeel);

  LookAndFeel::setDefaultLookAndFeel(lookAndFeel);

  pageStack = new PageStackComponent();
  
  if(lp->hasPassword())
    addAndMakeVisible(lp);
  else
    addAndMakeVisible(pageStack);
    
  launcher = new LauncherComponent(configJson);
  pageStack->pushPage(launcher, PageStackComponent::kTransitionNone);

  setSize(480, 272);
}

void MainContentComponent::loggedIn(){
  removeChildComponent(lp);
  addAndMakeVisible(pageStack);
}

MainContentComponent::~MainContentComponent() {}

void MainContentComponent::paint(Graphics &g) {
  g.fillAll(Colours::white);
}

void MainContentComponent::resized() {
  auto bounds = getLocalBounds();
  pageStack->setBounds(bounds);
  lp->setBounds(bounds);
}

void MainContentComponent::handleMainWindowInactive() {
  launcher->hideLaunchSpinner();
}

bool LoginPage::hasPassword(){
  char* home_str = getenv("HOME");
  String home(home_str);
  File passwd(home+"/.pocket-home/.passwd/passwd");
  if(passwd.exists()){
    String content = passwd.loadFileAsString();
    content = content.removeCharacters("\n");
    if(content==String("none")) return false;
    else hashed_password = content;
    return true;
  }
  return false;
}

LoginPage::LoginPage(MainContentComponent* mcc):
main_page(mcc), bgImage(new DrawableImage),
ntcIcon(new DrawableImage),
cur_password(new TextEditor("field_password", 0x2022)),
label_password(new Label("pass", "Password :")),
log(new TextButton("login")), hashed_password("none")
{
  this->setBounds(0, 0, 480, 272);
  Image bg = createImageFromFile(assetFile("login/background.png"));
  bgImage->setImage(bg);
  Image ntc = createImageFromFile(assetFile("login/ntcbanner.png"));
  ntcIcon->setImage(ntc);
  log->setButtonText("Log In");
  
  label_password->setFont(Font(14.f, Font::FontStyleFlags::bold));
  label_password->setBounds(140, 160, 100, 30);
  cur_password->setBounds(140, 190, 200, 30);
  log->setBounds(140, 230, 200, 30);
  log->addListener(this);
  
  addAndMakeVisible(bgImage, 1);
  addAndMakeVisible(ntcIcon, 4);
  addAndMakeVisible(cur_password, 3);
  addAndMakeVisible(label_password, 2);
  addAndMakeVisible(log, 3);
}

LoginPage::~LoginPage(){}

void LoginPage::resized() {
  auto bounds = getLocalBounds();
  ntcIcon->setBounds(0, 0, 480, 130);
  bgImage->setBounds(0, 0, bgImage->getWidth(), bgImage->getHeight());
}

void LoginPage::displayError(){
  AlertWindow::showMessageBoxAsync(AlertWindow::AlertIconType::WarningIcon,
                "Wrong password", 
                "Wrong password, try again",
                "Ok");
}

void LoginPage::buttonClicked(Button *button){
  String pass_tmp = cur_password->getText();
  String hashed = SettingsPageLogin::hashString(pass_tmp);
  if(hashed == hashed_password)
    main_page->loggedIn();
  else displayError();
}

void LoginPage::paint(Graphics &g) {
  g.fillAll(Colours::white);
}
