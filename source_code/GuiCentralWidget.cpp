#include "GuiCentralWidget.h"

GuiCentralWidget::GuiCentralWidget(QWidget *parent)
  : QWidget(parent)
{
  initAction();
  initTrayIcon();
//! do not delete yet, see if it causes problems.
//  QPalette palette;
//  palette.setColor(QPalette::Window, QColor(250,250,250));
//  this->setPalette(palette);

  this->setMinimumHeight(600);
  this->setMinimumWidth(900);
  this->setAttribute(Qt::WA_TranslucentBackground);
  this->setWindowTitle(QString("Hive! %1.%2.%3 alpha-test")
                       .arg(GlobalData::current_version[0])
                       .arg(GlobalData::current_version[1])
                       .arg(GlobalData::current_version[2]));

  ////Gui
  gui_tab_block = new GuiTabBlock(this);
  gui_main_block = new GuiMainBlock(this);

  QFrame *line = new QFrame(this);
  line->setFrameShape(QFrame::VLine);
  line->setFrameShadow(QFrame::Plain);
  line->setFixedWidth(1);
  line->setStyleSheet ("QFrame{  background: #CFCFCF; border: transparent;  }");

  //main_layout
  main_layout = new QHBoxLayout(this);
  main_layout->setMargin(0);
  main_layout->setSpacing(0);
  main_layout->addWidget(gui_tab_block);
  main_layout->addWidget(line);
  main_layout->addWidget(gui_main_block);

  //connect

  connect(gui_tab_block->gui_chat_tab->comb_scroll_widget, SIGNAL(combWidgetClicked(const QString&)), this, SLOT(onCombWidgetClicked(QString)));

  ///buttons~~
	connect(gui_tab_block->gui_home_tab->welcome_btn, SIGNAL(clicked(GUI::StaticStackType)), gui_main_block, SLOT(displayStaticStack( GUI::StaticStackType)));
	connect(gui_tab_block->gui_home_tab->list_btn, SIGNAL(clicked(GUI::StaticStackType)), gui_main_block, SLOT(displayStaticStack( GUI::StaticStackType)));
	connect(gui_tab_block->gui_home_tab->storage_btn, SIGNAL(clicked(GUI::StaticStackType)), gui_main_block, SLOT(displayStaticStack( GUI::StaticStackType)));


	connect(gui_tab_block->gui_settings_tab->messaging_btn, SIGNAL(clicked(GUI::StaticStackType)), gui_main_block, SLOT(displayStaticStack( GUI::StaticStackType)));
	connect(gui_tab_block->gui_settings_tab->profile_btn, SIGNAL(clicked(GUI::StaticStackType)), gui_main_block, SLOT(displayStaticStack( GUI::StaticStackType)));
	connect(gui_tab_block->gui_settings_tab->questions_btn, SIGNAL(clicked(GUI::StaticStackType)), gui_main_block, SLOT(displayStaticStack( GUI::StaticStackType)));
	connect(gui_tab_block->gui_settings_tab->update_btn, SIGNAL(clicked(GUI::StaticStackType)), gui_main_block, SLOT(displayStaticStack( GUI::StaticStackType)));



}

GuiCentralWidget::~GuiCentralWidget()
{
  qDebug()<<"\n@Hive UI is destructed";
}

void GuiCentralWidget::initAction()
{
  hide_action = new QAction(tr("&Hide"), this);
  connect(hide_action, &QAction::triggered, this, &QWidget::hide);

  show_action = new QAction(tr("&Show"), this);
  connect(show_action, &QAction::triggered, this, &QWidget::showNormal);

  quit_action = new QAction(tr("&Quit"), this);
  connect(quit_action, &QAction::triggered, qApp, &QCoreApplication::quit);
}

void GuiCentralWidget::initTrayIcon()
{
  tray_icon_menu = new QMenu(this);
  tray_icon_menu->addAction(hide_action);
  tray_icon_menu->addAction(show_action);
  tray_icon_menu->addSeparator();
  tray_icon_menu->addAction(quit_action);

  QIcon icon(":/img/img/tray_0.png");

  tray_icon = new QSystemTrayIcon(this);
  tray_icon->setIcon(icon);
  tray_icon->setToolTip("Hive!");
  tray_icon->setContextMenu(tray_icon_menu);
  tray_icon->setVisible(true);
	tray_icon->show();
}

void GuiCentralWidget::onMessageReceived(const Message::TextMessageStruct &messageStruct, const bool &fromMe)
{
  if(fromMe)
    {
      gui_main_block->gui_chat_stack->refreshMessage(messageStruct.reciever);
    }
  else
    {
      //if not displaying the usr
      if(!gui_main_block->gui_chat_stack->refreshMessage(messageStruct.sender))
        {
          UsrData *temp_usr_data = GlobalData::online_usr_data_map.value(messageStruct.sender);
          gui_tab_block->gui_chat_tab->comb_scroll_widget->refreshBadgeNumber(messageStruct.sender, temp_usr_data->unreadMessageNumber());
					if(GlobalData::settings_struct.notification.message_notification
						 && GlobalData::settings_struct.update.auto_check_update)
            {
              if(GlobalData::settings_struct.notification.message_detail_notification)
                {
                  tray_icon->showMessage(temp_usr_data->name(), messageStruct.message);
                }
              else
                {
                  tray_icon->showMessage(temp_usr_data->name(), "new message");
                }
            }
        }
    }
}

void GuiCentralWidget::onCombWidgetClicked(const QString &usrKey)
{
  gui_main_block->displayChatStack(usrKey);
  gui_tab_block->gui_chat_tab->comb_scroll_widget->refreshBadgeNumber(usrKey, 0);
}

void GuiCentralWidget::addUsr(UsrData *userData)
{
  gui_tab_block->gui_chat_tab->comb_scroll_widget->addComb(userData->usrProfileStruct());
  gui_main_block->gui_home_stack_list->addUsr(userData->usrProfileStruct());
}

void GuiCentralWidget::delUsr(UsrData *userData)
{
  userData;
}

void GuiCentralWidget::changeUsr(UsrData *userData)
{
  gui_tab_block->gui_chat_tab->comb_scroll_widget->refreshComb(userData->usrProfileStruct());
  gui_main_block->gui_chat_stack->refreshProfile(userData->key());
  gui_main_block->gui_home_stack_list->refreshUsrProfile(userData->usrProfileStruct());
}

void GuiCentralWidget::onUpdateAvailable()
{
	if(GlobalData::settings_struct.notification.update_notification)
		{
			QString message = QString("current version: %0.%1.%2\nnew version: %3.%4.%5")
					.arg(GlobalData::current_version[0])
					.arg(GlobalData::current_version[1])
					.arg(GlobalData::current_version[2])
					.arg(GlobalData::update_struct.version[0])
					.arg(GlobalData::update_struct.version[1])
					.arg(GlobalData::update_struct.version[2]);
			tray_icon->showMessage("Update Available", message);
		}
}


