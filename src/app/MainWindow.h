#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <Kpf/Kpf.h>
#include <QtWidgets/QtWidgets>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    KPF_SUBEVENT(TestEvent, testEvent)

public:
    explicit MainWindow(QWidget* parent = nullptr);
    virtual ~MainWindow();

    Q_SLOT bool init(const QDomElement& config);

    Q_SLOT QString testEvent(QString text);

protected:
    virtual void changeEvent(QEvent* event) override;

private:
    Ui::MainWindow* ui;
};
KPF_REGISTER(MainWindow)

#endif // MAINWINDOW_H
