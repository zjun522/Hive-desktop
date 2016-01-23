#include "GuiMainBlock.h"
#include <QPalette>
#include <QPainter>

GuiMainBlock::GuiMainBlock(QWidget *parent) : QWidget(parent)
{
//  QPalette palette;
//  //255,204,0,255
//  palette.setColor(QPalette::Window, QColor(245,245,245));
//  this->setAutoFillBackground(true);
//  this->setPalette(palette);

  //gui_chat_stack = new GuiChatStack(this);
  gui_welcome_stack = new GuiWelcomeStack(this);

  main_stacked_widget = new QStackedWidget(this);
  main_stacked_widget->addWidget(gui_welcome_stack);
  //main_stacked_widget->addWidget(gui_chat_stack);
  main_stacked_widget->setCurrentWidget(gui_welcome_stack);

  main_layout = new QVBoxLayout(this);
  main_layout->setMargin(0);
  main_layout->setSpacing(0);
  main_layout->addWidget(main_stacked_widget);

  //connect(gui_chat_stack, SIGNAL(chosen(QWidget*)), this, SLOT(setCurrentStack(QWidget*)));
}

GuiMainBlock::~GuiMainBlock()
{

}

void GuiMainBlock::displayChatStack(QString usrKey)
{
  emit whoIs(usrKey);
}

void GuiMainBlock::addChatStack(QStringList usrInfoStrList)
{
  gui_chat_stack = new GuiChatStack(usrInfoStrList, this);
  main_stacked_widget->addWidget(gui_chat_stack);
  connect(this, SIGNAL(whoIs(QString)), gui_chat_stack, SLOT(checkIdentity(QString)));
  connect(gui_chat_stack, SIGNAL(chosen(QWidget*)), this, SLOT(setCurrentStack(QWidget*)));
}

void GuiMainBlock::setCurrentStack(QWidget *widget)
{
  main_stacked_widget->setCurrentWidget(widget);
}



