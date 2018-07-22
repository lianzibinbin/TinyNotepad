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

#include "codeeditor.h"//��UI�����Widget������Ȼ������������������ͷ�ļ���Ȼ��ʹ�ú���ͨ�ؼ�һ��

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
//    //����Ĭ������
//    QFont font("Arial",16);
//    ui->widgetEditer->setFont(font);
//    //״̬��Ĭ��ֵ
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
    //�½�
    connect(ui->actionNew,SIGNAL(triggered(bool)),this,SLOT(onNewFile()));
    //��
    connect(ui->actionOpen,SIGNAL(triggered(bool)),this,SLOT(onOpenFile()));
    //����
    connect(ui->actionSave,SIGNAL(triggered(bool)),this,SLOT(onSaveFile()));
    //���Ϊ
    connect(ui->actionSaveAs,SIGNAL(triggered(bool)),this,SLOT(onSaveAsFile()));
    //��ӡ
    connect(ui->actionPrint,SIGNAL(triggered(bool)),this,SLOT(onPrint()));
    //�˳�
    connect(ui->actionExit,SIGNAL(triggered(bool)),this,SLOT(close()));

    //����
    connect(ui->actionUndo,SIGNAL(triggered(bool)),ui->widgetEditer,SLOT(undo()));
    //�ָ�
    connect(ui->actionRedo,SIGNAL(triggered(bool)),ui->widgetEditer,SLOT(redo()));
    //����
    connect(ui->actionCut,SIGNAL(triggered(bool)),ui->widgetEditer,SLOT(cut()));
    //����
    connect(ui->actionCopy,SIGNAL(triggered(bool)),ui->widgetEditer,SLOT(copy()));
    //ճ��
    connect(ui->actionPaste,SIGNAL(triggered(bool)),ui->widgetEditer,SLOT(paste()));
    //ɾ��
    connect(ui->actionDelete,SIGNAL(triggered(bool)),this,SLOT(onDelete()));
    //ȫѡ
    connect(ui->actionSelectAll,SIGNAL(triggered(bool)),ui->widgetEditer,SLOT(selectAll()));
    //����
    connect(ui->actionFont,SIGNAL(triggered(bool)),this,SLOT(onFont()));

    //����
    connect(ui->actionFind,SIGNAL(triggered(bool)),this,SLOT(onFind()));
    //����
    connect(ui->actionHelp,SIGNAL(triggered(bool)),this,SLOT(onHelp()));
    //����
    connect(ui->actionAbout,SIGNAL(triggered()),this,SLOT(onAbout()));

    //���λ�÷����仯
    connect(ui->widgetEditer,SIGNAL(cursorPositionChanged()),this,SLOT(onShowCurrentPos()));

}

void MainWindow::timerEvent(QTimerEvent *event)
{
    //����Ƿ�ѡ���ı�����
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
        event->ignore();//�����˳��ź�
        //event->accept();//�����˳��ź�
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
        editWidget->setPlainText(textStream.readAll());//һ��ȫ����ȡ
        file.close();

//        while(!textStream.atEnd())
//        {
//            editWidget->setPlainText(textStream.readLine());//���ж�ȡ
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
    //�ر�ʱ�����ĵ�δ����ֱ�ӹرգ����и�����δ���棬����ʾ�Ƿ񱣴棬���и����ұ��棬��ֱ�ӹر�
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
    //����Ĭ������
    QFont font("Arial",16);
    ui->widgetEditer->setFont(font);
    //״̬��Ĭ��ֵ
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
    if(ok)//�ĵ��н���Ϊ��ok=true,��ʾ�û������OK��ok=false��ʾ�û���ȡ���˴˶Ի���
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
    //textCursor.select(QTextCursor::Document);//�����ö�ٱ�������ʵ��ɾ�����У�ɾ�������ĵ����ݹ���
    textCursor.removeSelectedText();//ɾ�����ѡ�������
}

void MainWindow::onNewFile()
{
    //������windows���±��߼�������ǰ�༭���е������Ѿ�����(����û���κ�����)��ֱ���½��༭��������ѯ���û��Ƿ񱣴浱ǰ�ļ���Y:������½���N����������½���C:ȡ��ͣ���ڵ�ǰҳ��
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
    //������windows���±��߼�������ǰ�༭���е������Ѿ�����(����û���κ�����)��ֱ���½��༭��������ѯ���û��Ƿ񱣴浱ǰ�ļ���Y:������½���N����������½���C:ȡ��ͣ���ڵ�ǰҳ��
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
    //���ĵ�
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

    //����ƶ����ĵ���ͷ
}
