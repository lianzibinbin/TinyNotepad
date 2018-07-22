#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class DialogFind;
class DialogHelp;
class QPlainTextEdit;
class QLabel;
class CodeEditor;
class Highlighter;


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    CodeEditor/*QPlainTextEdit*/ *getPlainTextEdit();


private slots:
    void onAbout();
    void onHelp();
    void onFind();
    void onFont();
    void onPrint();
    void onDelete();
    void onNewFile();
    void onOpenFile();
    void onSaveFile();
    void onSaveAsFile();
    void onShowCurrentPos();

private:
    Ui::MainWindow *ui;

    void ActionInit();

    DialogFind *dlgFind;
    DialogHelp *dlgHelp;
    CodeEditor *pPlainTextEdit;
    QString fileName;
    bool bSaved;
    QLabel *pLabelRowColIndex;//当前所处行列
    Highlighter *highlighter;



    void timerEvent(QTimerEvent *event);
    void closeEvent(QCloseEvent *event);
    bool bFileModified(CodeEditor *editWidget);//文档是否被修改
    QString getOpenFilePath();
    void loadFile(QString fileName,CodeEditor *editWidget);
    bool bFileSaved();
    void setTitleName(QString titleName);
    int getCurrentColIndex();//当前光标所处位置
    int getCurrentRowIndex();
    bool dealClose();
    void setUpEditer();


};

#endif // MAINWINDOW_H
