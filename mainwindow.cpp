#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "dialogabout.h"
#include "dialoghelp.h"
#include "dialogfind.h"
#include <QPlainTextEdit>
#include <QFontDialog>
#include <QtPrintSupport/QPrinter>
#include <QtPrintSupport/QPrintDialog>
#include <QPageSetupDialog>
#include <QMessageBox>
#include "chinesedisplaytool.h"
#include <QFile>
#include <QFileDialog>
#include <QTextStream>
#include <QLabel>
#include <QDebug>
#include <QTextBlock>
#include <QTextLine>

#include "codeeditor.h"//在UI中添加Widget容器，然后提升，输入类名和头文件，然后使用和普通控件一样

#include "highlighter.h"



MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    dlgFind = nullptr;
    dlgHelp = nullptr;

    bSaved = false;
    this->setTitleName("untitled");
//    //设置默认字体
//    QFont font("Arial",16);
//    ui->widgetEditer->setFont(font);
//    //状态栏默认值
//    pLabelRowColIndex = new QLabel;
//    ui->statusBar->addWidget(pLabelRowColIndex,1);
//    onShowCurrentPos();

    this->setUpEditer();

    ActionInit();

    startTimer(1);
}

MainWindow::~MainWindow()
{
    delete ui;
}

CodeEditor *MainWindow::getPlainTextEdit()
{
    pPlainTextEdit = ui->widgetEditer;
    return pPlainTextEdit;
}

void MainWindow::ActionInit()
{
    //新建
    connect(ui->actionNew,SIGNAL(triggered(bool)),this,SLOT(onNewFile()));
    //打开
    connect(ui->actionOpen,SIGNAL(triggered(bool)),this,SLOT(onOpenFile()));
    //保存
    connect(ui->actionSave,SIGNAL(triggered(bool)),this,SLOT(onSaveFile()));
    //另存为
    connect(ui->actionSaveAs,SIGNAL(triggered(bool)),this,SLOT(onSaveAsFile()));
    //打印
    connect(ui->actionPrint,SIGNAL(triggered(bool)),this,SLOT(onPrint()));
    //退出
    connect(ui->actionExit,SIGNAL(triggered(bool)),this,SLOT(close()));

    //撤销
    connect(ui->actionUndo,SIGNAL(triggered(bool)),ui->widgetEditer,SLOT(undo()));
    //恢复
    connect(ui->actionRedo,SIGNAL(triggered(bool)),ui->widgetEditer,SLOT(redo()));
    //剪切
    connect(ui->actionCut,SIGNAL(triggered(bool)),ui->widgetEditer,SLOT(cut()));
    //复制
    connect(ui->actionCopy,SIGNAL(triggered(bool)),ui->widgetEditer,SLOT(copy()));
    //粘贴
    connect(ui->actionPaste,SIGNAL(triggered(bool)),ui->widgetEditer,SLOT(paste()));
    //删除
    connect(ui->actionDelete,SIGNAL(triggered(bool)),this,SLOT(onDelete()));
    //全选
    connect(ui->actionSelectAll,SIGNAL(triggered(bool)),ui->widgetEditer,SLOT(selectAll()));
    //字体
    connect(ui->actionFont,SIGNAL(triggered(bool)),this,SLOT(onFont()));

    //查找
    connect(ui->actionFind,SIGNAL(triggered(bool)),this,SLOT(onFind()));
    //帮助
    connect(ui->actionHelp,SIGNAL(triggered(bool)),this,SLOT(onHelp()));
    //关于
    connect(ui->actionAbout,SIGNAL(triggered()),this,SLOT(onAbout()));

    //光标位置发生变化
    connect(ui->widgetEditer,SIGNAL(cursorPositionChanged()),this,SLOT(onShowCurrentPos()));

}

void MainWindow::timerEvent(QTimerEvent *event)
{
    //监测是否选中文本内容
    QTextCursor textCursor = ui->widgetEditer->textCursor();
    if(textCursor.anchor() == textCursor.position())
    {
        ui->actionDelete->setEnabled(false);
        ui->actionCut->setEnabled(false);
        ui->actionCopy->setEnabled(false);
    }
    else
    {
        ui->actionDelete->setEnabled(true);
        ui->actionCut->setEnabled(true);
        ui->actionCopy->setEnabled(true);
    }

}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if(!this->dealClose())
    {
        event->ignore();//忽略退出信号
        //event->accept();//接收退出信号
        return;
    }
    QMainWindow::closeEvent(event);
}

bool MainWindow::bFileModified(CodeEditor *editWidget)
{
    if(editWidget->document()->isModified())
    {
        return true;
    }
    return false;
}

QString MainWindow::getOpenFilePath()
{
    return this->fileName;
}

void MainWindow::loadFile(QString fileName,CodeEditor *editWidget)
{
    QFile file(fileName);
    if(file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QTextStream textStream(&file);
        editWidget->setPlainText(textStream.readAll());//一次全部读取
        file.close();

//        while(!textStream.atEnd())
//        {
//            editWidget->setPlainText(textStream.readLine());//按行读取
//        }
    }
}

bool MainWindow::bFileSaved()
{
    return bSaved;
}

void MainWindow::setTitleName(QString titleName)
{
    QString ss;
    ss = QString("%1 - TinyNotePad").arg(titleName);
    setWindowTitle(ss);
}

int MainWindow::getCurrentColIndex()
{
    int nCol = ui->widgetEditer->textCursor().columnNumber() + 1;
    return nCol;
}

int MainWindow::getCurrentRowIndex()
{
    int nRow = ui->widgetEditer->textCursor().blockNumber() + 1;
    return nRow;
}

bool MainWindow::dealClose()
{
    //关闭时，若文档未更改直接关闭，若有更改且未保存，则提示是否保存，若有更改且保存，则直接关闭
    if(ui->widgetEditer->document()->isModified())
    {
        if(!this->bFileSaved())
        {
            if(getOpenFilePath().isEmpty())
            {
                //ChineseDisplayTool tl;
                QString msg;
                QString filePath = this->getOpenFilePath();
                if(filePath.isEmpty())
                {
                    filePath = "untitled";
                }
                msg = QString("%1%2?").arg("Save changes to\n").arg(filePath);
                QMessageBox::StandardButton button = QMessageBox::information(this,"Save",msg,QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
                switch (button)
                {
                    case QMessageBox::Save:
                    {
                        QString fileName = QFileDialog::getSaveFileName(this,"Save",QString("untitled"),"plainText File(*.txt);;h File(*.h);;c File(*.c);;cpp File(*.cpp)");
                        if(fileName.isEmpty())
                        {
                            return false;
                        }
                        else
                        {
                            QFile file(fileName);
                            if(!file.open(QIODevice::WriteOnly))
                            {
                                QMessageBox::critical(this,"Error","Open file fail");
                                return false;
                            }
                            QTextStream outs(&file);
                            outs << ui->widgetEditer->toPlainText();
                            outs.flush();
                            file.close();
                            return true;
                        }
                    }
                    case QMessageBox::Discard:
                    {
                        return true;
                    }
                    case QMessageBox::Cancel:
                    {
                        return false;
                    }
                }
            }
            else
            {
                ChineseDisplayTool tl;
                QString msg;
                QString filePath = this->getOpenFilePath();
                msg = QString("%1%2?").arg("Save changes to\n").arg(filePath);
                QMessageBox::StandardButton button = QMessageBox::information(this,"Save",msg,QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
                switch (button)
                {
                    case QMessageBox::Save:
                    {
                        QFile file(fileName);
                        if(!file.open(QIODevice::WriteOnly))
                        {
                            QMessageBox::critical(this,"Error","Open file fail");
                            return false;
                        }
                        QTextStream outs(&file);
                        outs << ui->widgetEditer->toPlainText();
                        outs.flush();
                        file.close();
                        return true;
                    }
                    case QMessageBox::Discard:
                    {
                        return true;
                    }
                    case QMessageBox::Cancel:
                    {
                        return false;
                    }
                }
            }
        }
        else
        {
            return true;
        }
    }
    return true;
}

void MainWindow::setUpEditer()
{
    //设置默认字体
    QFont font("Arial",16);
    ui->widgetEditer->setFont(font);
    //状态栏默认值
    pLabelRowColIndex = new QLabel;
    ui->statusBar->addWidget(pLabelRowColIndex,1);
    onShowCurrentPos();

    highlighter = new Highlighter(ui->widgetEditer->document());
}

void MainWindow::onAbout()
{
    DialogAbout about;
    about.exec();
}

void MainWindow::onHelp()
{
    if(dlgHelp == nullptr)
    {
        dlgHelp = new DialogHelp(this);
    }
    if(dlgHelp)
    {
        dlgHelp->show();
    }
}

void MainWindow::onFind()
{
    if(dlgFind == nullptr)
    {
        dlgFind = new DialogFind(this);
    }
    if(dlgFind)
    {
        dlgFind->show();
    }
}

void MainWindow::onFont()
{
    QFontDialog fontDlg(this);
    bool ok;
    QFont font = fontDlg.getFont(&ok);
    if(ok)//文档中解释为：ok=true,表示用户点击了OK，ok=false表示用户点取消了此对话框
    {
        ui->widgetEditer->setFont(font);
    }

}

void MainWindow::onPrint()
{
    QPrinter printer;
    QPrintDialog printDlg(&printer,this);
    if(printDlg.exec())
    {
        QTextDocument *doc = ui->widgetEditer->document();
        doc->print(&printer);
    }
}

void MainWindow::onDelete()
{
    QTextCursor textCursor = ui->widgetEditer->textCursor();
    //textCursor.select(QTextCursor::Document);//后面的枚举变量可以实现删除整行，删除整个文档内容功能
    textCursor.removeSelectedText();//删除光标选择的内容
}

void MainWindow::onNewFile()
{
    //用依据windows记事本逻辑，若当前编辑框中的内容已经保存(或是没有任何内容)则直接新建编辑，否则先询问用户是否保存当前文件，Y:保存后新建，N：不保存后新建，C:取消停留在当前页面
    if(this->bFileModified(ui->widgetEditer))
    {
        if(!bFileSaved())
        {
            ChineseDisplayTool tl;
            QString msg;
            QString filePath = this->getOpenFilePath();
            if(filePath.isEmpty())
            {
                filePath = "untitled";
            }
            msg = QString("%1%2?").arg("Save changes to\n").arg(filePath);
            QMessageBox::StandardButton button = QMessageBox::information(this,"Save",msg,QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
            switch (button)
            {
                case QMessageBox::Save:
                {
                    if(getOpenFilePath().isEmpty())
                    {
                        QString fileName = QFileDialog::getSaveFileName(this,"Save",QString("untitled"),"plainText File(*.txt);;h File(*.h);;c File(*.c);;cpp File(*.cpp)");
                        if(fileName.isEmpty())
                        {
                            return;
                        }
                        else
                        {
                            QFile file(fileName);
                            if(!file.open(QIODevice::WriteOnly))
                            {
                                QMessageBox::critical(this,"Error","Open file fail");
                                return;
                            }
                            this->fileName = fileName;
                            QTextStream outs(&file);
                            outs << ui->widgetEditer->toPlainText();
                            outs.flush();
                            file.close();
                        }
                    }
                    else
                    {
                        QFile file(filePath);
                        if(!file.open(QIODevice::WriteOnly))
                        {
                            QMessageBox::critical(this,"Error","Open file fail");
                            return;
                        }
                        QTextStream outs(&file);
                        outs << ui->widgetEditer->toPlainText();
                        outs.flush();
                        file.close();
                    }
                    break;
                }
                case QMessageBox::Discard:
                {
                    break;
                }
                case QMessageBox::Cancel:
                {
                    return;
                }
            }
        }
    }
    ui->widgetEditer->clear();
    this->bSaved = false;
    this->fileName.clear();
    this->setTitleName("untitled");
}

void MainWindow::onOpenFile()
{
    //用依据windows记事本逻辑，若当前编辑框中的内容已经保存(或是没有任何内容)则直接新建编辑，否则先询问用户是否保存当前文件，Y:保存后新建，N：不保存后新建，C:取消停留在当前页面
    if(this->bFileModified(ui->widgetEditer))
    {
        if(!bFileSaved())
        {
            ChineseDisplayTool tl;
            QString msg;
            QString filePath = this->getOpenFilePath();
            if(filePath.isEmpty())
            {
                filePath = "untitled";
            }
            msg = QString("%1%2?").arg("Save changes to\n").arg(filePath);
            QMessageBox::StandardButton button = QMessageBox::information(this,"Save",msg,QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
            switch (button)
            {
                case QMessageBox::Save:
                {
                    if(getOpenFilePath().isEmpty())
                    {
                        QString fileName = QFileDialog::getSaveFileName(this,"Save",QString("untitled"),"plainText File(*.txt);;h File(*.h);;c File(*.c);;cpp File(*.cpp)");
                        if(fileName.isEmpty())
                        {
                            return;
                        }
                        else
                        {
                            QFile file(fileName);
                            if(!file.open(QIODevice::WriteOnly))
                            {
                                QMessageBox::critical(this,"Error","Open file fail");
                                return;
                            }
                            QTextStream outs(&file);
                            outs << ui->widgetEditer->toPlainText();
                            outs.flush();
                            file.close();
                        }
                    }
                    else
                    {
                        QFile file(fileName);
                        if(!file.open(QIODevice::WriteOnly))
                        {
                            QMessageBox::critical(this,"Error","Open file fail");
                            return;
                        }
                        QTextStream outs(&file);
                        outs << ui->widgetEditer->toPlainText();
                        outs.flush();
                        file.close();
                    }
                    this->fileName = fileName;
                    this->bSaved = true;
                    break;
                }
                case QMessageBox::Discard:
                {
                    break;
                }
                case QMessageBox::Cancel:
                {
                    return;
                }
            }
        }
    }
    //打开文档
    QString fileName = QFileDialog::getOpenFileName(this,"Open",QString(),"plainText File(*.txt);;h File(*.h);;c File(*.c);;cpp File(*.cpp)");
    if(fileName.isEmpty())
    {
        return;
    }
    this->fileName = fileName;
    ui->widgetEditer->clear();
    this->loadFile(fileName,ui->widgetEditer);
    this->setTitleName(getOpenFilePath());
}

void MainWindow::onSaveFile()
{
    QString fileName = getOpenFilePath();
    if(fileName.isEmpty())
    {
        QString fileName = QFileDialog::getSaveFileName(this,"Save",QString("untitled"),"plainText File(*.txt);;h File(*.h);;c File(*.c);;cpp File(*.cpp)");
        if(fileName.isEmpty())
        {
            return;
        }
        else
        {
            QFile file(fileName);
            if(!file.open(QIODevice::WriteOnly))
            {
                QMessageBox::critical(this,"Error","Open file fail");
                return;
            }
            this->fileName = fileName;
            QTextStream outs(&file);
            outs << ui->widgetEditer->toPlainText();
            outs.flush();
            file.close();
        }
    }
    else
    {
        QFile file(fileName);
        if(!file.open(QIODevice::WriteOnly))
        {
            QMessageBox::critical(this,"Error","Open file fail");
            return;
        }
        this->fileName = fileName;
        QTextStream outs(&file);
        outs << ui->widgetEditer->toPlainText();
        outs.flush();
        file.close();
    }
    bSaved = true;
    this->setTitleName(getOpenFilePath());
}

void MainWindow::onSaveAsFile()
{
    QString fileName = QFileDialog::getSaveFileName(this,"Save",QString("untitled"),"plainText File(*.txt);;h File(*.h);;c File(*.c);;cpp File(*.cpp)");
    if(!fileName.isEmpty())
    {
        QFile file(fileName);
        if(!file.open(QIODevice::WriteOnly))
        {
            QMessageBox::critical(this,"Error","Open file fail");
            return;
        }
        QTextStream outs(&file);
        outs << ui->widgetEditer->toPlainText();
        outs.flush();
        file.close();
    }
}

void MainWindow::onShowCurrentPos()
{
    QString ss;
    ss = QString("row: %1 col: %2").arg(QString::number(getCurrentRowIndex())).arg(QString::number(getCurrentColIndex()));
    pLabelRowColIndex->setText(ss);
    ui->statusBar->addWidget(pLabelRowColIndex,1);

    //光标移动到文档开头
}
