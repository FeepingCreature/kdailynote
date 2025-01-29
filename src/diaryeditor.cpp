#include "diaryeditor.h"
#include <QKeyEvent>
#include <QTextBlock>
#include <QStandardPaths>
#include <QFile>
#include <QDir>

DiaryEditor::DiaryEditor(QWidget *parent)
    : KTextEdit(parent)
    , autoSaveTimer(new QTimer(this))
{
    setAcceptRichText(true);

    // Set up content file location
    QString dataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(dataPath);
    contentFile = dataPath + QStringLiteral("/diary.txt");

    loadContent();
    checkAndUpdateDate();
    setupAutoSave();
}

void DiaryEditor::loadContent()
{
    QFile file(contentFile);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        setPlainText(QString::fromUtf8(file.readAll()));
    }
}

void DiaryEditor::saveContent()
{
    // Don't save if the content hasn't changed
    if (!document()->isModified()) {
        return;
    }

    QFile file(contentFile);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        file.write(toPlainText().toUtf8());
        document()->setModified(false);
    }
}

void DiaryEditor::checkAndUpdateDate()
{
    QDate currentDate = QDate::currentDate();
    if (currentDate != lastOpenedDate) {
        insertDateHeader(currentDate);
        lastOpenedDate = currentDate;
    }
}

void DiaryEditor::insertDateHeader(const QDate &date)
{
    QTextCursor cursor = textCursor();
    cursor.movePosition(QTextCursor::Start);

    QString header = QStringLiteral("\n# %1\n").arg(date.toString(Qt::ISODate));
    cursor.insertText(header);
}

void DiaryEditor::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Return && checkListContext()) {
        handleListContinuation();
        return;
    }

    KTextEdit::keyPressEvent(event);
}

bool DiaryEditor::checkListContext()
{
    QTextCursor cursor = textCursor();
    cursor.movePosition(QTextCursor::StartOfLine);
    cursor.movePosition(QTextCursor::EndOfLine, QTextCursor::KeepAnchor);
    QString line = cursor.selectedText();

    return line.startsWith(QStringLiteral(" - "));
}

void DiaryEditor::handleListContinuation()
{
    QTextCursor cursor = textCursor();
    QString currentLine = cursor.block().text();

    if (currentLine.trimmed().isEmpty()) {
        cursor.movePosition(QTextCursor::StartOfLine);
        cursor.movePosition(QTextCursor::EndOfLine, QTextCursor::KeepAnchor);
        cursor.removeSelectedText();
    } else {
        cursor.insertText(QStringLiteral("\n - "));
    }
}

void DiaryEditor::toggleBold()
{
    QTextCursor cursor = textCursor();
    QTextCharFormat format = cursor.charFormat();
    bool wasBold = (format.fontWeight() == QFont::Bold);
    
    format.setFontWeight(wasBold ? QFont::Normal : QFont::Bold);
    cursor.mergeCharFormat(format);
    
    // Set the default format for future input
    setCurrentCharFormat(format);
}

void DiaryEditor::toggleItalic()
{
    QTextCursor cursor = textCursor();
    QTextCharFormat format = cursor.charFormat();
    format.setFontItalic(!format.fontItalic());
    cursor.mergeCharFormat(format);
    
    // Set the default format for future input
    setCurrentCharFormat(format);
}

void DiaryEditor::toggleUnderline()
{
    QTextCursor cursor = textCursor();
    QTextCharFormat format = cursor.charFormat();
    format.setFontUnderline(!format.fontUnderline());
    cursor.mergeCharFormat(format);
    
    // Set the default format for future input
    setCurrentCharFormat(format);
}

void DiaryEditor::setupAutoSave()
{
    // Autosave every 30 seconds if there are changes
    autoSaveTimer->setInterval(30000);
    connect(autoSaveTimer, &QTimer::timeout, this, &DiaryEditor::saveContent);
    autoSaveTimer->start();

    // Also save when text changes after 3 seconds of inactivity
    connect(this, &DiaryEditor::textChanged, this, &DiaryEditor::onTextChanged);
}

void DiaryEditor::onTextChanged()
{
    // Reset and restart the timer
    autoSaveTimer->start();
}
