#ifndef DIALOGFIND_H
#define DIALOGFIND_H

#include <QDialog>

class QButtonGroup;
class QLineEdit;
class QPlainTextEdit;
class MainWindow;

class CodeEditor;


enum FINDMODE
{
    DOWN_NOCASESENSITIVE,//向下查找，不区分大小写
    DOWN_CASESENSITIVE,//向下查找，区分大小写
    UP_NOCASESENSITIVE,//向上查找，不区分大小写
    UP_CASESENSITIVE,//向上查找，区分大小写
    REPLACE_DOWN_NOCASESENSITIVE,//替换页面向下查找不区分大小写
    REPLACE_DOWN_CASESENSITIVE,//替换页面向下查找区分大小写
};


namespace Ui {
class DialogFind;
}

class DialogFind : public QDialog
{
    Q_OBJECT

public:
    explicit DialogFind(QWidget *parent = 0);
    ~DialogFind();

    MainWindow *mainDlg;

private slots:
    void onFindTabEnableBtnFindNext();
    void onReplacetabEnableBtn();
    void onFindTabNextBtn();
    void onFindTabCancel();
    void onReplaceTabFindNext();
    void onReplaceTabCancel();
    void onReplaceTabReplace();
    void onReplaceTabAllReplace();


private:
    Ui::DialogFind *ui;

    QButtonGroup *pButtonGroup;
    void highLightFindContent(QWidget *widget);
    bool findContent(QString strContent,CodeEditor *textEdit,bool bShowMsg);
    FINDMODE getMode();
    void replaceContent(CodeEditor *textEdit,QString replace);

};

#endif // DIALOGFIND_H
