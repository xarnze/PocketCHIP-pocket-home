#include "PersonalizePageComponent.h"

/* Personalize class */
PersonalizePageComponent::PersonalizePageComponent(LauncherComponent* lc):
lcomp(lc),
background("lab_back","Background"), icons("lab_icons","Icons management"), opt_back("opt_back",""),
opt_name("opt_name", "Name:"), opt_img("opt_img", "Icon path:"), opt_shell("opt_shell", "Command:"),
add_btn("Add"), apply("Apply"), choose_back("back_box"), edit_back("back_field"),
edit_name("name"), edit_icn("icn"), edit_shell("shell"), config(assetConfigFile("config.json")),
json(JSON::parse(config)), success("suc", "Success !"), browse("..."),
browseicon("...")
{
  bgColor = Colour(0xffd23c6d);
  bgImage = createImageFromFile(assetFile("settingsBackground.png"));
  Font big_font(Font::getDefaultMonospacedFontName(), 25.f, 0);
  background.setFont(big_font);
  icons.setFont(big_font);
  
  Font small_font(Font::getDefaultMonospacedFontName(), 17.5f, 0);
  opt_back.setFont(small_font);
  opt_name.setFont(small_font);
  opt_img.setFont(small_font);
  opt_shell.setFont(small_font);
  success.setFont(big_font);
  
  addAndMakeVisible(background);
  addAndMakeVisible(icons);
  addAndMakeVisible(add_btn);
  addAndMakeVisible(apply);
  addAndMakeVisible(success);
  apply.addListener(this);
  add_btn.addListener(this);
  /* ComboBox */
  choose_back.addItem("Default",1);
  choose_back.addItem("Color",2);
  choose_back.addItem("Image",3);
  choose_back.setSelectedId(1);
  choose_back.addListener(this);
  addAndMakeVisible(choose_back);
  //////////////
  /* + */
  edit_back.setColour(TextEditor::ColourIds::textColourId, Colour::greyLevel(0.f));
  addAndMakeVisible(edit_back);
  addAndMakeVisible(opt_back);
  addAndMakeVisible(opt_name);
  addAndMakeVisible(opt_img);
  addAndMakeVisible(opt_shell);
  addAndMakeVisible(edit_name);
  addAndMakeVisible(edit_icn);
  addAndMakeVisible(edit_shell);
  addAndMakeVisible(browse);
  addAndMakeVisible(browseicon);
  
  browse.addListener(this);
  browseicon.addListener(this);
  showAddComponents(false);
  success.setVisible(false);
  browse.setVisible(false);
  browseicon.setVisible(false);
  
  /* Create back button */
  backButton = createImageButton("Back", createImageFromFile(assetFile("backIcon.png")));
  backButton->addListener(this);
  backButton->setAlwaysOnTop(true);
  addAndMakeVisible(backButton);

  updateComboBox();
}

PersonalizePageComponent::~PersonalizePageComponent(){
}

void PersonalizePageComponent::updateComboBox(){
  /* Checking the current configuration */
  String background = (json["background"]).toString();
  bool display = false;
  if(background.length()==0);
  else if(background.length()==6 && 
          background.containsOnly("0123456789ABCDEF")){
    choose_back.setSelectedItemIndex(1, sendNotificationSync);
    display = true;
    edit_back.setText(background);
  }
  else{
    choose_back.setSelectedItemIndex(2, sendNotificationSync);
    display = true;
    edit_back.setText(background);
  }
  
  edit_back.setVisible(display);
  opt_back.setVisible(display);
}

void PersonalizePageComponent::paint(Graphics &g){
  auto bounds = getLocalBounds();
  g.fillAll(bgColor);
  g.drawImage(bgImage,bounds.getX(), bounds.getY(), bounds.getWidth(), bounds.getHeight(), 0, 0, bgImage.getWidth(), bgImage.getHeight(), false);
}

void PersonalizePageComponent::resized(){
  auto bounds = getLocalBounds();
  backButton->setBounds(bounds.getX(), bounds.getY(), 60, bounds.getHeight());
  
  background.setBounds(bounds.getX()+70, bounds.getY()+20, 150, 30);
  icons.setBounds(bounds.getX()+70, bounds.getY()+25+bounds.getHeight()/3, 200, 30);
  
  int btn_width  = 85;
  int btn_height = 30;
  choose_back.setBounds(bounds.getX()+260, bounds.getY()+20, 2*btn_width+20, btn_height);

  add_btn.setBounds(bounds.getX()+260, bounds.getY()+25+bounds.getHeight()/3,
		    btn_width, btn_height);
  
  opt_back.setBounds(bounds.getX()+70, bounds.getY()+70, 150, btn_height);
  edit_back.setBounds(bounds.getX()+155, bounds.getY()+70, 265, btn_height);
  browse.setBounds(bounds.getX()+422, bounds.getY()+70, 30, btn_height);

  int gap = 40;
  int x = bounds.getX()+70;
  int y = bounds.getY()+65+bounds.getHeight()/3;
  Label* labels[3] = {&opt_name, &opt_img, &opt_shell};
  TextEditor* fields[3] = {&edit_name, &edit_icn, &edit_shell};
  
  for(int i = 0; i < 3; i++){
    int size = 107;
    int t_width = 75;
    if(i==2) size = 304;
    labels[i]->setBounds(x, y, t_width, btn_height);
    fields[i]->setBounds(x+t_width+5, y, size, btn_height);
    //If i==1, we are putting the icon path field
    if(i==1)
      browseicon.setBounds(x+t_width+size-25, y, 30, btn_height);
    
    if(i==0) x += t_width+size+15;
    else{
      x = bounds.getX()+70;
      y+=gap;
    }
  }
  
  apply.setBounds(x, y, 385, btn_height);
  success.setBounds(x+130, y, 385, btn_height);
  
}

void PersonalizePageComponent::buttonClicked(Button* button){
  if (button == backButton) {
    getMainStack().popPage(PageStackComponent::kTransitionTranslateHorizontal);
    resetApplySuccess();
    updateComboBox();
  }
  else if(button == &apply){
    bool ok = updateJSON();
    updateFile(ok);
  }
  else if(button == &add_btn){
    showAddComponents(true);
  }
  else if(button == &browse){
    WildcardFileFilter wildcardFilter ("*.png", 
                                       String::empty,
                                       "Image files");

    FileBrowserComponent browser (FileBrowserComponent::canSelectFiles |
                                  FileBrowserComponent::openMode,
                                  File::nonexistent,
                                  &wildcardFilter,
                                  nullptr);

    FileChooserDialogBox dialogBox ("Choose the new background",
                                    "Please choose your new background (png) image",
                                    browser,
                                    false,
                                    Colours::lightgrey);
    if(dialogBox.show(480,272)){
      File selectedFile = browser.getSelectedFile(0);
      String path = selectedFile.getFullPathName();
      edit_back.setText(path);
    }
  }
  else if(button == &browseicon){
    WildcardFileFilter wildcardFilter ("*.png", 
                                       String::empty,
                                       "Image files");

    FileBrowserComponent browser (FileBrowserComponent::canSelectFiles |
                                  FileBrowserComponent::openMode,
                                  File::nonexistent,
                                  &wildcardFilter,
                                  nullptr);

    FileChooserDialogBox dialogBox ("Choose the icon",
                                    "Please choose your png icon (ideal size : 90x70 px)",
                                    browser,
                                    false,
                                    Colours::lightgrey);
    if(dialogBox.show(480,272)){
      File selectedFile = browser.getSelectedFile(0);
      String path = selectedFile.getFullPathName();
      edit_icn.setText(path);
    }
  }
}

void PersonalizePageComponent::showAddComponents(bool show){
  opt_name.setVisible(show);
  opt_img.setVisible(show);
  opt_shell.setVisible(show);
  edit_name.setVisible(show);
  edit_icn.setVisible(show);
  edit_shell.setVisible(show);
  browseicon.setVisible(show);
}

void PersonalizePageComponent::comboBoxChanged(ComboBox* box){
  if(box == &choose_back){
    edit_back.setText("");
    browse.setVisible(false);
    if(box->getSelectedId()==1){
      edit_back.setVisible(false);
      opt_back.setVisible(false);
      return;
    }
    else if(box->getSelectedId()==2)
      opt_back.setText("Hex value:", dontSendNotification);
    else if(box->getSelectedId()==3){
      browse.setVisible(true);
      opt_back.setText("Image path:", dontSendNotification);
    }
    edit_back.setVisible(true);
    opt_back.setVisible(true);
  }
}

void PersonalizePageComponent::updateFile(bool ok){
  DynamicObject* obj = json.getDynamicObject();
  String s = JSON::toString(json);
  bool write = config.replaceWithText(s);
  if(write && ok){
    apply.setVisible(false);
    success.setVisible(true);
  }
  else if(!write){
    String message = "Error writing the configuration into the file\nCheck the permissions";
    AlertWindow::showMessageBox(
            AlertWindow::AlertIconType::WarningIcon,
            "Error",
            message);
  }
}

bool PersonalizePageComponent::updateJSON(){
  Array<var>* pages_arr = (json["pages"].getArray());
  const var& pages = ((*pages_arr)[0]);
  Array<var>* items_arr = pages["items"].getArray();
  bool name_b = false;
  bool color_b = false;
  if(edit_name.isVisible()){
    String name = edit_name.getText();
    String path = edit_icn.getText();
    String cmmd = edit_shell.getText();
    DynamicObject* new_icn = new DynamicObject;
    new_icn->setProperty("name",name);
    new_icn->setProperty("icon",path);
    new_icn->setProperty("shell",cmmd);
    var icn_var(new_icn);
    items_arr->add(icn_var);
    name_b = true;
    
    /* Adding to the grid */
    lcomp->addIcon(name, path, cmmd);
  }
  if(choose_back.getSelectedId()==1){
    DynamicObject* new_back = json.getDynamicObject();
    new_back->setProperty("background","4D4D4D");
    color_b = true;
    lcomp->setColorBackground("4D4D4D");
  }
  if(choose_back.getSelectedId()==2){
    String value = edit_back.getText().toUpperCase();
    if(value.length()!=6 || !value.containsOnly("0123456789ABCDEF"))
      edit_back.setText("Invalid color");
    else{
      DynamicObject* new_back = json.getDynamicObject();
      new_back->setProperty("background",value);
      color_b = true;

      /* Change background in LauncherComponent */
      lcomp->setColorBackground(value);
    }
  }
  if(choose_back.getSelectedId()==3){
    String value = edit_back.getText();
    DynamicObject* new_back = json.getDynamicObject();
    new_back->setProperty("background",value);
    color_b = true;

    /* Change background in LauncherComponent */
    lcomp->setImageBackground(value);
  }
  
  return name_b || color_b;
}

void PersonalizePageComponent::resetApplySuccess(){
  showAddComponents(false);
  success.setVisible(false);
  edit_back.setVisible(false);
  edit_back.setText("");
  edit_name.setText("");
  edit_icn.setText("");
  edit_shell.setText("");
  apply.setVisible(true);
}

void PersonalizePageComponent::deleteIcon(String name, String shell){
  Array<var>* pages_arr = (json["pages"].getArray());
  const var& pages = ((*pages_arr)[0]);
  Array<var>* items_arr = pages["items"].getArray();
  
  //Searching for the element in the Array
  for(int i = 0; i < items_arr->size(); i++){
    const var& elt = (*items_arr)[i];
    if(elt["name"] == name && elt["shell"] == shell){
      items_arr->remove(i);
      break;
    }
  }
  /*False because we don't want to change the icons in the persoPage
    as it's even not shown */
  updateFile(false);
}