#include "diarywindow.h"
#include <QVBoxLayout>
#include <QToolBar>
#include <QScreen>
#include <QAction>
#include <QApplication>
#include <QMenu>

DiaryWindow::DiaryWindow(QWidget *parent)
    : QWidget(parent, Qt::Tool | Qt::FramelessWindowHint)
{
    setAttribute(Qt::WA_DeleteOnClose, false);

    setupUI();
    createActions();

    // Set up system tray
    // Create context menu
    QMenu *trayMenu = new QMenu(this);
    QAction *quitAction = trayMenu->addAction(tr("Quit"));
    connect(quitAction, &QAction::triggered, qApp, &QApplication::quit);

    trayIcon = new QSystemTrayIcon(QIcon::fromTheme(QStringLiteral("accessories-text-editor")), this);
    trayIcon->setToolTip(tr("KDailyNote"));
    trayIcon->setContextMenu(trayMenu);
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
            
            // Check if we need a new day and create it
            editor->checkAndUpdateDate();
            
            // Focus and scroll to the latest day
            if (auto latestEditor = editor->getLatestEditor()) {
                latestEditor->setFocus();
                QTextCursor cursor = latestEditor->textCursor();
                cursor.movePosition(QTextCursor::End);
                latestEditor->setTextCursor(cursor);
                editor->ensureWidgetVisible(latestEditor);
            }
        }
    }
}

void DiaryWindow::positionWindow()
{
    QPoint cursorPos = QCursor::pos();
    QScreen *screen = QGuiApplication::screenAt(cursorPos);
    if (!screen) {
        screen = QGuiApplication::primaryScreen();
    }

    // Get screen geometry in global coordinates
    QRect screenGeom = screen->geometry();
    
    QPoint pos;
    
    // Center window on cursor
    pos = cursorPos;
    pos.setX(pos.x() - width()/2);   // Center horizontally on cursor
    pos.setY(pos.y() - height()/2);  // Center vertically on cursor
    
    // Ensure window stays within screen bounds
    if (pos.x() + width() > screenGeom.right()) {
        pos.setX(screenGeom.right() - width());
    }
    if (pos.x() < screenGeom.left()) {
        pos.setX(screenGeom.left());
    }
    if (pos.y() + height() > screenGeom.bottom()) {
        pos.setY(screenGeom.bottom() - height());
    }
    if (pos.y() < screenGeom.top()) {
        pos.setY(screenGeom.top());
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
