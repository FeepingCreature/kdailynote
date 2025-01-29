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
    QRect trayIconGeom = trayIcon->geometry();
    QScreen *screen = QGuiApplication::screenAt(trayIconGeom.center());
    if (!screen) {
        screen = QGuiApplication::primaryScreen();
    }

    // Get screen geometry in global coordinates
    QRect screenGeom = screen->geometry();
    
    // Find which edge the tray is closest to
    int distToRight = screenGeom.right() - trayIconGeom.right();
    int distToLeft = trayIconGeom.left() - screenGeom.left();
    int distToTop = trayIconGeom.top() - screenGeom.top();
    int distToBottom = screenGeom.bottom() - trayIconGeom.bottom();
    
    QPoint pos;
    
    // Position based on which screen edge is closest
    if (distToRight < distToLeft && distToRight < distToTop && distToRight < distToBottom) {
        // Tray is on right edge
        pos.setX(trayIconGeom.left() - width() - 5);
        pos.setY(trayIconGeom.top());
    } else if (distToLeft < distToTop && distToLeft < distToBottom) {
        // Tray is on left edge
        pos.setX(trayIconGeom.right() + 5);
        pos.setY(trayIconGeom.top());
    } else if (distToTop < distToBottom) {
        // Tray is on top edge
        pos.setX(trayIconGeom.left());
        pos.setY(trayIconGeom.bottom() + 5);
    } else {
        // Tray is on bottom edge (or default)
        pos.setX(trayIconGeom.left());
        pos.setY(trayIconGeom.top() - height() - 5);
    }
    
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
