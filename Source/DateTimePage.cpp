#include "DateTimePage.h"

DateTimePage::DateTimePage():
bg_color(0xffd23c6d),
title("settings", "Date and time settings"),
choosemode("choosemode"),
reconfigure("Reconfigure time"),
datemode("datemode", "Select the display mode for the clock:")
{
  //Title font
  title.setFont(Font(27.f));
  datemode.setFont(Font(20.f));
  //Back button
  backButton = createImageButton("Back", createImageFromFile(assetFile("backIcon.png")));
  backButton->addListener(this);
  backButton->setAlwaysOnTop(true);
  //ComboBox
  choosemode.addItem("24h mode", 1);
  choosemode.addItem("AM/PM mode", 2);
  choosemode.addListener(this);
  
  //Let's check whether there is an option for time format in the config
  var json = getConfigJSON();
  if(json["timeformat"].toString() == "ampm")
    choosemode.setSelectedId(2);
  else choosemode.setSelectedId(1);
  
  reconfigure.addListener(this);

  addAndMakeVisible(datemode);  
  addAndMakeVisible(reconfigure);
  addAndMakeVisible(choosemode);
  addAndMakeVisible(title);
  addAndMakeVisible(backButton);
}

DateTimePage::~DateTimePage(){ }

void DateTimePage::buttonClicked(Button* but){
  if(but == backButton)
    getMainStack().popPage(PageStackComponent::kTransitionTranslateHorizontal);
  else if(but == &reconfigure){
    int ret = system("vala-terminal -fs 8 -g 20 20 -e 'sudo dpkg-reconfigure tzdata && exit'");
    if(ret == -1)
      AlertWindow::showMessageBox(AlertWindow::WarningIcon, "Error", "Failed launching vala-terminal, is it installed ?");
  }
}

void DateTimePage::comboBoxChanged(ComboBox* c){
  if(c != &choosemode) return;
  var json = getConfigJSON();
  DynamicObject* hour = json.getDynamicObject();
  String format = (c->getSelectedId()==1)?"24h":"ampm";
  hour->setProperty("timeformat", format);
  //JSON config has been changed, let's write it to the file
  File configfile = getConfigFile();
  String jsonstring = JSON::toString(json);
  configfile.replaceWithText(jsonstring);
}

void DateTimePage::paint(Graphics& g){
  auto bounds = getLocalBounds();
  g.fillAll(bg_color);
}

void DateTimePage::resized(){
  auto bounds = getLocalBounds();
  int btn_height = 30;
  int btn_width = 345;
  
  int titlewidth = title.getFont().getStringWidth(title.getText());
  titlewidth /= 2;
  title.setBounds(bounds.getX()+240-titlewidth, bounds.getY()+10, btn_width, btn_height);

  backButton->setBounds(bounds.getX(), bounds.getY(), 60, bounds.getHeight());

  int datewidth = datemode.getFont().getStringWidth(datemode.getText());
  datemode.setBounds(bounds.getX()+60, bounds.getY()+70, datewidth, btn_height);
  int combowidth = 360 - datewidth;
  choosemode.setBounds(bounds.getX()+60+datewidth, bounds.getY()+70, combowidth, btn_height);
  
  int middle = 240-btn_width/2;
  reconfigure.setBounds(bounds.getX()+middle, bounds.getY()+150, btn_width, btn_height);
}
