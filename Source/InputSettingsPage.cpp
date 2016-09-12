#include "InputSettingsPage.h"

InputSettingsPage::InputSettingsPage(LauncherComponent* lc):
bg_color(0xffd23c6d),
title("settings", "Input settings"),
choosemode("choosemode"),
calibrating("Calibrate the screen"),
cursorvisible("cursorvisible", "Select the visibility of the cursor:"),
lc(lc)
{
  //Title font
  title.setFont(Font(27.f));
  cursorvisible.setFont(Font(20.f));
  //Back button
  backButton = createImageButton("Back", createImageFromFile(assetFile("backIcon.png")));
  backButton->addListener(this);
  backButton->setAlwaysOnTop(true);
  //ComboBox
  choosemode.addItem("Not visible", 1);
  choosemode.addItem("Visible", 2);
  choosemode.addListener(this);
  
  //Let's check whether there is an option for time format in the config
  var json = getConfigJSON();
  if(json["cursor"].toString() == "visible")
    choosemode.setSelectedId(2);
  else choosemode.setSelectedId(1);
  
  calibrating.addListener(this);

  addAndMakeVisible(cursorvisible);  
  addAndMakeVisible(calibrating);
  addAndMakeVisible(choosemode);
  addAndMakeVisible(title);
  addAndMakeVisible(backButton);
}

InputSettingsPage::~InputSettingsPage(){ }

void InputSettingsPage::buttonClicked(Button* but){
  if(but == backButton)
    getMainStack().popPage(PageStackComponent::kTransitionTranslateHorizontal);
  else if(but == &calibrating){
    int ret = system("vala-terminal -fs 8 -g 20 20 -e 'xinput_calibrator && exit'");
    if(ret == -1)
      AlertWindow::showMessageBox(AlertWindow::WarningIcon, "Error", "Failed launching vala-terminal, is it installed ?");
  }
}

void InputSettingsPage::comboBoxChanged(ComboBox* c){
  if(c != &choosemode) return;
  var json = getConfigJSON();
  DynamicObject* mouse = json.getDynamicObject();
  String visibility = (c->getSelectedId()==1)?"notvisible":"visible";
  mouse->setProperty("cursor", visibility);
  //JSON config has been changed, let's write it to the file
  File configfile = getConfigFile();
  String jsonstring = JSON::toString(json);
  configfile.replaceWithText(jsonstring);
  
  LookAndFeel& laf = getLookAndFeel();
  PokeLookAndFeel* mc = (PokeLookAndFeel*) &laf;
  if(visibility == "visible")
    mc->setCursorVisible(true);
  else
    mc->setCursorVisible(false);
}

void InputSettingsPage::paint(Graphics& g){
  auto bounds = getLocalBounds();
  g.fillAll(bg_color);
}

void InputSettingsPage::resized(){
  auto bounds = getLocalBounds();
  int btn_height = 30;
  int btn_width = 345;
  
  int titlewidth = title.getFont().getStringWidth(title.getText());
  titlewidth /= 2;
  title.setBounds(bounds.getX()+240-titlewidth, bounds.getY()+10, btn_width, btn_height);

  backButton->setBounds(bounds.getX(), bounds.getY(), 60, bounds.getHeight());

  int datewidth = cursorvisible.getFont().getStringWidth(cursorvisible.getText());
  cursorvisible.setBounds(bounds.getX()+60, bounds.getY()+70, datewidth, btn_height);
  int combowidth = 360 - datewidth;
  choosemode.setBounds(bounds.getX()+60+datewidth, bounds.getY()+70, combowidth, btn_height);
  
  int middle = 240-btn_width/2;
  calibrating.setBounds(bounds.getX()+middle, bounds.getY()+150, btn_width, btn_height);
}
