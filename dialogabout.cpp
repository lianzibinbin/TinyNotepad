#include "dialogabout.h"
#include "ui_dialogabout.h"

DialogAbout::DialogAbout(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogAbout)
{
    ui->setupUi(this);

    setWindowTitle("About");
    connect(ui->pushButtonOK,SIGNAL(clicked(bool)),this,SLOT(onAboutOK()));
}

DialogAbout::~DialogAbout()
{
    delete ui;
}

void DialogAbout::onAboutOK()
{
    this->accept();
}
