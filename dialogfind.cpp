#include "dialogfind.h"
#include "ui_dialogfind.h"
#include <QButtonGroup>
#include <QDebug>
#include <QTextDocument>
#include <QPlainTextEdit>
#include <QMessageBox>


#include "mainwindow.h"
#include "chinesedisplaytool.h"
#include "codeeditor.h"



DialogFind::DialogFind(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogFind)
{
    ui->setupUi(this);
    setWindowTitle("Find&Replace");

    mainDlg = (MainWindow *)parent;
    //pButtonGroup = new QButtonGroup;
    //pButtonGroup->addButton(ui->radioButtonUp);
    //pButtonGroup->addButton(ui->radioButtonDown);
    ui->pushButtonFindNext->setEnabled(false);
    ui->tabWidget->setCurrentIndex(0);

    ui->pushButtonReplaceFindNext->setEnabled(false);
    ui->pushButtonReplace->setEnabled(false);
    ui->pushButtonReplaceAll->setEnabled(false);
    connect(ui->lineEditFind,SIGNAL(textChanged(QString)),this,SLOT(onFindTabEnableBtnFindNext()));
    connect(ui->lineEditFindContent,SIGNAL(textChanged(QString)),this,SLOT(onReplacetabEnableBtn()));
    connect(ui->pushButtonFindNext,SIGNAL(clicked(bool)),this,SLOT(onFindTabNextBtn()));
    connect(ui->pushButtonFindCancel,SIGNAL(clicked(bool)),this,SLOT(onFindTabCancel()));
    connect(ui->pushButtonReplaceCancel,SIGNAL(clicked(bool)),this,SLOT(onReplaceTabCancel()));
    connect(ui->pushButtonReplaceFindNext,SIGNAL(clicked(bool)),this,SLOT(onReplaceTabFindNext()));
    connect(ui->pushButtonReplace,SIGNAL(clicked(bool)),this,SLOT(onReplaceTabReplace()));
    connect(ui->pushButtonReplaceAll,SIGNAL(clicked(bool)),this,SLOT(onReplaceTabAllReplace()));
}

DialogFind::~DialogFind()
{
    delete ui;
}

void DialogFind::onFindTabEnableBtnFindNext()
{
    //当lienEdit中没有内容时查找下一个按钮禁用，否则使能
    uint len = ui->lineEditFind->text().length();
    if(len > 0)
    {
        ui->pushButtonFindNext->setEnabled(true);
    }
    else
    {
        ui->pushButtonFindNext->setEnabled(false);
    }
}

void DialogFind::onReplacetabEnableBtn()
{
    uint len = ui->lineEditFindContent->text().length();
    if(len > 0)
    {
        ui->pushButtonReplaceFindNext->setEnabled(true);
        ui->pushButtonReplace->setEnabled(true);
        ui->pushButtonReplaceAll->setEnabled(true);
    }
    else
    {
        ui->pushButtonReplaceFindNext->setEnabled(false);
        ui->pushButtonReplace->setEnabled(false);
        ui->pushButtonReplaceAll->setEnabled(false);
    }
}

void DialogFind::onFindTabNextBtn()
{
    findContent(ui->lineEditFind->text(),mainDlg->getPlainTextEdit(),true);
}

void DialogFind::onFindTabCancel()
{
    this->hide();
}

void DialogFind::onReplaceTabFindNext()
{
    findContent(ui->lineEditFindContent->text(),mainDlg->getPlainTextEdit(),true);
}

void DialogFind::onReplaceTabCancel()
{
    this->hide();
}

void DialogFind::onReplaceTabReplace()
{
    //鼠标光标移到文件头部
    QTextCursor cursor = mainDlg->getPlainTextEdit()->textCursor();
    cursor.setPosition(0);
    mainDlg->getPlainTextEdit()->setTextCursor(cursor);
    if(!findContent(ui->lineEditFindContent->text(),mainDlg->getPlainTextEdit(),true))
    {
        return;
    }
    replaceContent(mainDlg->getPlainTextEdit(),ui->lineEditReplaceContent->text());
}

void DialogFind::onReplaceTabAllReplace()
{
    //鼠标光标移到文件头部
    QTextCursor cursor = mainDlg->getPlainTextEdit()->textCursor();
    cursor.setPosition(0);
    mainDlg->getPlainTextEdit()->setTextCursor(cursor);
    if(!findContent(ui->lineEditFindContent->text(),mainDlg->getPlainTextEdit(),true))
    {
        return;
    }
    replaceContent(mainDlg->getPlainTextEdit(),ui->lineEditReplaceContent->text());
    while(findContent(ui->lineEditFindContent->text(),mainDlg->getPlainTextEdit(),false))
    {
        replaceContent(mainDlg->getPlainTextEdit(),ui->lineEditReplaceContent->text());
    }
}

void DialogFind::highLightFindContent(QWidget *widget)
{
    QPalette palette = widget->palette();
    palette.setColor(QPalette::Highlight,palette.color(QPalette::Active,QPalette::Highlight));
    widget->setPalette(palette);
}

bool DialogFind::findContent(QString strContent,CodeEditor *textEdit,bool bShowMsg)
{
    FINDMODE nMode = this->getMode();
    switch(nMode)
    {
        case DOWN_NOCASESENSITIVE:
        case REPLACE_DOWN_NOCASESENSITIVE:
        {
            if(textEdit->find(strContent))//如果不指定该参数，默认的是向前查找、不区分大小写、包含该字符串的词也可以查找到
            {
                highLightFindContent(textEdit);
                return true;
            }
            else
            {
                if(bShowMsg)
                {
                    //ChineseDisplayTool tl;
                    QString msg;
                    msg = QString("%1 %2").arg("NOT find").arg(strContent);
                    QMessageBox::information(this,"Find",msg);
                }
                if(nMode == REPLACE_DOWN_NOCASESENSITIVE)
                {
                    //鼠标光标移到文件头部
                    QTextCursor cursor = textEdit->textCursor();
                    cursor.setPosition(0);
                    textEdit->setTextCursor(cursor);
                }
                return false;
            }
        }
        break;
        case DOWN_CASESENSITIVE:
        case REPLACE_DOWN_CASESENSITIVE:
        {
            if(textEdit->find(strContent,QTextDocument::FindCaseSensitively))
            {
                highLightFindContent(textEdit);
                return true;
            }
            else
            {
                if(bShowMsg)
                {
                    //ChineseDisplayTool tl;
                    QString msg;
                    msg = QString("%1 %2").arg("NOT find").arg(strContent);
                    QMessageBox::information(this,"Find",msg);
                }
                if(nMode == REPLACE_DOWN_CASESENSITIVE)
                {
                    //鼠标光标移到文件头部
                    QTextCursor cursor = textEdit->textCursor();
                    cursor.setPosition(0);
                    textEdit->setTextCursor(cursor);
                }
                return false;
            }
        }
        break;
        case UP_NOCASESENSITIVE:
        {
            if(textEdit->find(strContent,QTextDocument::FindBackward))
            {
                highLightFindContent(textEdit);
                return true;
            }
            else
            {
                if(bShowMsg)
                {
                    ChineseDisplayTool tl;
                    QString msg;
                    msg = QString("%1 %2").arg("NOT find").arg(strContent);
                    QMessageBox::information(this,"Find",msg);
                }
                return false;
            }
        }
        break;
        case UP_CASESENSITIVE:
        {
            if(textEdit->find(strContent,QTextDocument::FindBackward | QTextDocument::FindCaseSensitively))
            {
                highLightFindContent(textEdit);
                return true;
            }
            else
            {
                if(bShowMsg)
                {
                    ChineseDisplayTool tl;
                    QString msg;
                    msg = QString("%1 %2").arg("NOT find").arg(strContent);
                    QMessageBox::information(this,"Find",msg);
                }
                return false;
            }
        }
        break;
    }
}

FINDMODE DialogFind::getMode()
{
    int tabIndex = ui->tabWidget->currentIndex();
    if(tabIndex == 0)
    {
        if(ui->radioButtonDown->isChecked() && !ui->checkBox_CaseSensitive->isChecked())
        {
            return DOWN_NOCASESENSITIVE;
        }
        else if(ui->radioButtonDown->isChecked() && ui->checkBox_CaseSensitive->isChecked())
        {
            return DOWN_CASESENSITIVE;
        }
        else if(ui->radioButtonUp->isChecked() && !ui->checkBox_CaseSensitive->isChecked())
        {
            return UP_NOCASESENSITIVE;
        }
        else if(ui->radioButtonUp->isChecked() && ui->checkBox_CaseSensitive->isChecked())
        {
            return UP_CASESENSITIVE;
        }
    }
    else if(tabIndex == 1)
    {
        if(!ui->checkBox_CaseSensitiveReplace->isChecked())
        {
            return REPLACE_DOWN_NOCASESENSITIVE;
        }
        else if(ui->checkBox_CaseSensitiveReplace->isChecked())
        {
            return REPLACE_DOWN_CASESENSITIVE;
        }
    }
}

void DialogFind::replaceContent(CodeEditor *textEdit,QString replace)
{
    QTextCursor textCursor = textEdit->textCursor();
    textCursor.insertText(replace);
}



