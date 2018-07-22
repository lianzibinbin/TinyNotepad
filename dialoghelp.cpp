#include "dialoghelp.h"
#include "ui_dialoghelp.h"

DialogHelp::DialogHelp(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogHelp)
{
    ui->setupUi(this);
    setWindowTitle("Help");
    connect(ui->pushButtonOK,SIGNAL(clicked(bool)),this,SLOT(onHelpOK()));
}

DialogHelp::~DialogHelp()
{
    delete ui;
}

void DialogHelp::onHelpOK()
{
    this->accept();
}
