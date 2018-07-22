#ifndef DIALOGHELP_H
#define DIALOGHELP_H

#include <QDialog>

namespace Ui {
class DialogHelp;
}

class DialogHelp : public QDialog
{
    Q_OBJECT

public:
    explicit DialogHelp(QWidget *parent = 0);
    ~DialogHelp();

private slots:
    void onHelpOK();

private:
    Ui::DialogHelp *ui;
};

#endif // DIALOGHELP_H
