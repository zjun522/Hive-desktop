#include "GuiSettingsStack_update.h"

GuiSettingsStack_update::GuiSettingsStack_update(QWidget *parent)
{
  this->setUpUI(LayoutStyle::Linear);

  setIcon(":/img/img/update.png");
  setTitle("Settings");
  setSubTitle("update");

  addTag("Check update");
  QCheckBox *auto_check_box = new QCheckBox(this);
  auto_check_box->setChecked(GlobalData::settings_struct.update.auto_check_update);
  connect(auto_check_box, &QCheckBox::toggled,
          [this](bool toggled){
            GlobalData::settings_struct.update.auto_check_update = toggled;
            GlobalData::settings_struct.modified_lock = true;
          });
  addItem("\tAuto check update\t\t\t          ", auto_check_box);

  QCheckBox *show_notification_box = new QCheckBox(this);
  show_notification_box->setChecked(GlobalData::settings_struct.notification.update_notification);
  connect(show_notification_box, &QCheckBox::toggled,
          [this](bool toggled){
            GlobalData::settings_struct.notification.update_notification = toggled;
            GlobalData::settings_struct.modified_lock = true;
          });
  addItem("\tShow notifications\t\t\t          ", show_notification_box);

  if(GlobalData::update_struct.version[0] == GlobalData::current_version[0]
     && GlobalData::update_struct.version[1] == GlobalData::current_version[1]
     && GlobalData::update_struct.version[2] == GlobalData::current_version[2])
    {
			addTag("Hive is new!");
			addItem("Current version: ",
							QString("%1.%2.%3")
							.arg(GlobalData::current_version[0])
							.arg(GlobalData::current_version[1])
							.arg(GlobalData::current_version[2]));
			addItem("features:\t"
							, "1. Added notifications for messaging\n"
								"2. Added notifications for updates\n"
								"3. Added update helper\n"
								"4. Added badge number for unread message count\n"
								"5. Added more settings\n"
								"6. Updated Icon\n"
								"7. Optimized typeface on Windows\n"
								"8. Optimized chat area switch algorithm to reduce RAM\n"
								"9. Fixed unexpected quit caused by unrecognized user\n"
							, false);
    }
  else
    {
			addTag("Available Update:");
      addItem("New version: version:\t", GlobalData::update_struct.title, false);
      addItem("Update version: version:\t",
              QString("%1.%2.%3")
              .arg(GlobalData::update_struct.version[0])
              .arg(GlobalData::update_struct.version[1])
              .arg(GlobalData::update_struct.version[2]),
              false);
      addItem("Features and Fixes: \t", GlobalData::update_struct.message);
    }


  this->setParent(parent);
}
