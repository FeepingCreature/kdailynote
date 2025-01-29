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

    // Add framed toolbar
    QFrame *toolbarFrame = new QFrame(this);
    toolbarFrame->setFrameStyle(QFrame::Panel | QFrame::Raised);
    QVBoxLayout *frameLayout = new QVBoxLayout(toolbarFrame);
    frameLayout->setContentsMargins(1, 1, 1, 1);
    
    QToolBar *toolbar = new QToolBar(this);
    frameLayout->addWidget(toolbar);
    layout->addWidget(toolbarFrame);

    // Add editor
    editor = new DiaryEditor(this);
    layout->addWidget(editor);

    resize(400, 600);
}

void DiaryWindow::createActions()
{
    QToolBar *toolbar = findChild<QToolBar*>();
    if (!toolbar) return;

    QAction *boldAction = toolbar->addAction(QIcon::fromTheme(QIcon::ThemeIcon::FormatTextBold), 
                                           tr("Bold"), editor, &DiaryEditor::toggleBold);
    boldAction->setShortcut(QKeySequence::Bold);  // Ctrl+B
    
    QAction *italicAction = toolbar->addAction(QIcon::fromTheme(QIcon::ThemeIcon::FormatTextItalic),
                                             tr("Italic"), editor, &DiaryEditor::toggleItalic);
    italicAction->setShortcut(QKeySequence::Italic);  // Ctrl+I
    
    QAction *underlineAction = toolbar->addAction(QIcon::fromTheme(QIcon::ThemeIcon::FormatTextUnderline),
                                                tr("Underline"), editor, &DiaryEditor::toggleUnderline);
    underlineAction->setShortcut(QKeySequence::Underline);  // Ctrl+U
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
            
            // Focus the latest day
            if (auto latestEditor = editor->getLatestEditor()) {
                latestEditor->setFocus();
                QTextCursor cursor = latestEditor->textCursor();
                cursor.movePosition(QTextCursor::End);
                latestEditor->setTextCursor(cursor);
            }
        }
    }
}

void DiaryWindow::positionWindow()
{
    QRect trayIconGeom = trayIcon->geometry();
    QPoint pos = trayIconGeom.topLeft();

    QScreen *screen = QGuiApplication::screenAt(pos);
    if (!screen) {
        screen = QGuiApplication::primaryScreen();
        pos = screen->geometry().topRight();
    }

    // Check if the tray icon is on a vertical panel
    bool isVertical = trayIconGeom.height() > trayIconGeom.width();

    if (isVertical) {
        // Position window to the right of the icon
        pos.setX(trayIconGeom.right() + 5);
        pos.setY(trayIconGeom.top());
    } else {
        // Position window above the icon
        pos.setY(pos.y() - height());
        pos.setX(pos.x() - width() + trayIconGeom.width());
    }

    move(pos);
}

void DiaryWindow::focusOutEvent(QFocusEvent *event)
{
    hide();
    QWidget::focusOutEvent(event);
}

void DiaryWindow::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape) {
        hide();
        return;
    }
    QWidget::keyPressEvent(event);
}
