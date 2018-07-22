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
    DOWN_NOCASESENSITIVE,//���²��ң������ִ�Сд
    DOWN_CASESENSITIVE,//���²��ң����ִ�Сд
    UP_NOCASESENSITIVE,//���ϲ��ң������ִ�Сд
    UP_CASESENSITIVE,//���ϲ��ң����ִ�Сд
    REPLACE_DOWN_NOCASESENSITIVE,//�滻ҳ�����²��Ҳ����ִ�Сд
    REPLACE_DOWN_CASESENSITIVE,//�滻ҳ�����²������ִ�Сд
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
