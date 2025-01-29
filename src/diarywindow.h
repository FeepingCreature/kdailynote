#pragma once

#include <QWidget>
#include <QSystemTrayIcon>
#include "diaryeditor.h"

class DiaryWindow : public QWidget
{
    Q_OBJECT

public:
    DiaryWindow(QWidget *parent = nullptr);
    ~DiaryWindow();

protected:
    void focusOutEvent(QFocusEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

private Q_SLOTS:
    void trayIconActivated(QSystemTrayIcon::ActivationReason reason);
    void positionWindow();

private:
    DiaryEditor *editor;
    QSystemTrayIcon *trayIcon;
    void createActions();
    void setupUI();
};
