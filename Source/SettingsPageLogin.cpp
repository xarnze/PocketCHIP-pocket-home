#include "SettingsPageLogin.h"

SettingsPageLogin::SettingsPageLogin() :
root_label("RootLab", "Sudo password"), root_password("Root", 0x2022),
cur_label("CurLabel", "Current password"), cur_password("Current",0x2022),
new_label("NewLabel", "New password"), new_password("New", 0x2022),
ret_label("ConfLabel", "Retype password"), ret_password("Confirmation", 0x2022),
bg_color(0xffd23c6d), title("Title", "Change your password"),
deletemode(false)
{
  //Title
  title.setFont(Font(25.f));
  //Back button
  backButton = createImageButton("Back", createImageFromFile(assetFile("backIcon.png")));
  backButton->addListener(this);
  backButton->setAlwaysOnTop(true);
  addAndMakeVisible(backButton);
  apply.setButtonText("Apply");
  apply.addListener(this);
  //Setting the location in the view
  setLocations();
  //Adding components to the view
  addAndMakeVisible(title);
  addAndMakeVisible(root_label);
  addAndMakeVisible(root_password);
  addAndMakeVisible(cur_label);
  addAndMakeVisible(cur_password);
  addAndMakeVisible(new_label);
  addAndMakeVisible(new_password);
  addAndMakeVisible(ret_label);
  addAndMakeVisible(ret_password);
  addAndMakeVisible(apply);
}

SettingsPageLogin::~SettingsPageLogin(){ }

void SettingsPageLogin::paint(Graphics &g) {
    auto bounds = getLocalBounds();
    g.fillAll(bg_color);
    setLocations();
}

void SettingsPageLogin::setLocations(){
  auto bounds = getLocalBounds();
  //Setting the position of the back button
  backButton->setBounds(0, 0, 60, bounds.getHeight());
  
  int x = 90;
  int edit_x = 190;
  int edit_width = bounds.getWidth() - 210;
  int y = 50;
  int offset = 45;

  title.setBounds(130, 0, bounds.getWidth() - 100, 50);
  
  root_label.setBounds(x, y, 100, 30);
  root_password.setBounds(edit_x, y, edit_width , 30);
  y += offset;
  
  cur_label.setBounds(x, y, 100, 30);  
  cur_password.setBounds(edit_x, y, edit_width , 30);
  y += offset;
  
  new_label.setBounds(x, y, 100, 30);  
  new_password.setBounds(edit_x, y, edit_width , 30);
  y += offset;
  
  ret_label.setBounds(x, y, 100, 30);  
  ret_password.setBounds(edit_x, y, edit_width , 30);
  y += offset;  
  
  apply.setBounds(x, y, bounds.getWidth() - 110, 30);
}

void SettingsPageLogin::switchToModify(){
  root_label.setVisible(false);
  root_password.setVisible(false);
  if(new_password.isVisible()) return;
  new_label.setVisible(true);
  new_password.setVisible(true);
  ret_label.setVisible(true);
  ret_password.setVisible(true);
  title.setText("Change your password", dontSendNotification);
  deletemode = false;
}

void SettingsPageLogin::switchToRemove(){
  root_label.setVisible(true);
  root_password.setVisible(true);
  if(!new_password.isVisible()) return;
  new_label.setVisible(false);
  new_password.setVisible(false);
  ret_label.setVisible(false);
  ret_password.setVisible(false);
  title.setText("Remove your password", dontSendNotification);
  deletemode = true;  
}

bool SettingsPageLogin::passwordIdentical(){
  if(new_password.getText() != ret_password.getText()){
    AlertWindow::showMessageBoxAsync(AlertWindow::AlertIconType::WarningIcon,
                                  "Wrong password", 
                                  "Confirmation password is wrong, try again",
                                  "Ok");
    return false;
  }
  return true;
}

void SettingsPageLogin::deletePassword(){
  String typed_root = root_password.getText();
  String root_pass = root_password.getText();
  String cmd_passwd = "echo \""+root_pass+"\" | "; 
  //Changing the owner of the password file (to be able to write inside)
  String command1 = cmd_passwd+"sudo -kS passwd -d chip";
  int returned = system(command1.toRawUTF8());
  if(returned != 0){
    //An error occured !
    AlertWindow::showMessageBoxAsync(AlertWindow::AlertIconType::WarningIcon,
                                  "Wrong password", 
                                  "Impossible to modify the password, check your root password",
                                  "Ok");
    return;
  }
  AlertWindow::showMessageBoxAsync(AlertWindow::AlertIconType::InfoIcon,
                                  "Success", 
                                  "Password removed !",
                                  "Ok");
}

void SettingsPageLogin::savePassword(){
  if(!passwordIdentical()) return;
  String typed_current = cur_password.getText();
  String typed_new = new_password.getText();
  
  String user(getlogin());
  String cmd_passwd = "echo \""+typed_current+"\n"+typed_new+"\n"+typed_new+"\n\" | "; 
  //Changing the owner of the password file (to be able to write inside)
  String command1 = cmd_passwd+"passwd "+user;
  int returned = system(command1.toRawUTF8());
  if(returned != 0){
    //An error occured !
    AlertWindow::showMessageBoxAsync(AlertWindow::AlertIconType::WarningIcon,
                                  "Unable to change password",
				  command1,
                                  //"Please check your current password",
                                  "Ok");
    return;
  }
  AlertWindow::showMessageBoxAsync(AlertWindow::AlertIconType::InfoIcon,
                                  "Success", 
                                  "Password changed !",
                                  "Ok");
    getMainStack().popPage(PageStackComponent::kTransitionTranslateHorizontal);
    clearAllFields();
}

void SettingsPageLogin::clearAllFields(){
  root_password.clear();
  cur_password.clear();
  new_password.clear();
  ret_password.clear();
}

void SettingsPageLogin::buttonClicked(Button* button){
  if (button == backButton){
    getMainStack().popPage(PageStackComponent::kTransitionTranslateHorizontal);
    clearAllFields();
  }
  else if(button == &apply){
    if(!deletemode) savePassword();
    else deletePassword();
  }
}
