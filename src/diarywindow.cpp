#include "diarywindow.h"
#include <QVBoxLayout>
#include <QToolBar>
#include <QScreen>
#include <QAction>
#include <QApplication>

DiaryWindow::DiaryWindow(QWidget *parent)
    : QWidget(parent, Qt::Tool | Qt::FramelessWindowHint)
{
    setAttribute(Qt::WA_DeleteOnClose, false);

    setupUI();
    createActions();

    // Set up system tray
    trayIcon = new QSystemTrayIcon(QIcon::fromTheme(QStringLiteral("accessories-text-editor")), this);
    trayIcon->setToolTip(tr("KDailyNote"));
    trayIcon->show();

    connect(trayIcon, &QSystemTrayIcon::activated,
            this, &DiaryWindow::trayIconActivated);
}

DiaryWindow::~DiaryWindow()
{
    // Save content before closing
    editor->saveContent();
}

void DiaryWindow::setupUI()
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(1, 1, 1, 1);

    // Add toolbar
    QToolBar *toolbar = new QToolBar(this);
    layout->addWidget(toolbar);

    // Add editor
    editor = new DiaryEditor(this);
    layout->addWidget(editor);

    resize(400, 600);
}

void DiaryWindow::createActions()
{
    QToolBar *toolbar = findChild<QToolBar*>();
    if (!toolbar) return;

    toolbar->addAction(QIcon::fromTheme(QIcon::ThemeIcon::FormatTextBold), tr("Bold"),
                      editor, &DiaryEditor::toggleBold);
    toolbar->addAction(QIcon::fromTheme(QIcon::ThemeIcon::FormatTextItalic), tr("Italic"),
                      editor, &DiaryEditor::toggleItalic);
    toolbar->addAction(QIcon::fromTheme(QIcon::ThemeIcon::FormatTextUnderline), tr("Underline"),
                      editor, &DiaryEditor::toggleUnderline);
}

void DiaryWindow::trayIconActivated(QSystemTrayIcon::ActivationReason reason)
{
    if (reason == QSystemTrayIcon::Trigger) {
        if (isVisible()) {
            hide();
        } else {
            positionWindow();
            show();
            raise();
            activateWindow();
        }
    }
}

void DiaryWindow::positionWindow()
{
    QRect trayIconGeom = trayIcon->geometry();
    QPoint pos = trayIconGeom.topRight();

    QScreen *screen = QGuiApplication::screenAt(pos);
    if (!screen) {
        screen = QGuiApplication::primaryScreen();
        pos = screen->geometry().topRight();
    }

    // Position window above tray icon
    pos.setY(pos.y() - height());
    pos.setX(pos.x() - width());

    move(pos);
}
