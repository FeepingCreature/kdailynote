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
        QString content = QString::fromUtf8(file.readAll());
        parseContent(content);
        setPlainText(content);
    }
}

void DiaryEditor::parseContent(const QString &content)
{
    sections.clear();
    QStringList lines = content.split(QStringLiteral("\n"));
    QDate currentDate;
    QString currentContent;
    
    for (const QString &line : lines) {
        if (line.startsWith(QStringLiteral("# "))) {
            // If we were building a section, save it
            if (currentDate.isValid()) {
                sections[currentDate] = DiarySection(currentContent.trimmed());
            }
            
            // Parse new date header
            QString dateStr = line.mid(2).trimmed();
            currentDate = QDate::fromString(dateStr, Qt::ISODate);
            currentContent.clear();
        } else if (currentDate.isValid()) {
            currentContent += line + QStringLiteral("\n");
        }
    }
    
    // Save the last section
    if (currentDate.isValid()) {
        sections[currentDate] = DiarySection(currentContent.trimmed());
    }
}

void DiaryEditor::saveContent()
{
    // Don't save if the content hasn't changed
    if (!document()->isModified()) {
        return;
    }

    // Update sections from current content
    parseContent(toPlainText());

    QFile file(contentFile);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        file.write(serializeContent().toUtf8());
        document()->setModified(false);
    }
}

QString DiaryEditor::serializeContent() const
{
    QString result;
    QMapIterator<QDate, DiarySection> it(sections);
    while (it.hasNext()) {
        it.next();
        result += QStringLiteral("# %1\n").arg(it.key().toString(Qt::ISODate));
        result += it.value().content;
        result += QStringLiteral("\n\n");
    }
    return result;
}

void DiaryEditor::checkAndUpdateDate()
{
    QDate currentDate = QDate::currentDate();
    if (!hasSection(currentDate)) {
        insertDateHeader(currentDate);
    }
}

bool DiaryEditor::hasSection(const QDate &date) const
{
    return sections.contains(date);
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
    // Save after 3 seconds of inactivity
    autoSaveTimer->setInterval(3000);
    autoSaveTimer->setSingleShot(true);
    connect(autoSaveTimer, &QTimer::timeout, this, &DiaryEditor::saveContent);
    
    // Reset timer on text changes
    connect(this, &DiaryEditor::textChanged, this, &DiaryEditor::onTextChanged);
}

void DiaryEditor::onTextChanged()
{
    // Mark document as modified and restart inactivity timer
    document()->setModified(true);
    autoSaveTimer->start();
}
