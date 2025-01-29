#include <QtTest>
#include <QSignalSpy>
#include <QTemporaryFile>
#include <QTextStream>
#include "../diaryeditor.h"
#include "../dayeditor.h"

class TestDiaryEditor : public DiaryEditor
{
    Q_OBJECT

public:
    TestDiaryEditor() : DiaryEditor(nullptr) {}

private Q_SLOTS:
    void testMarkdownConversion();
    void testRichTextConversion();
};

void TestDiaryEditor::testMarkdownConversion()
{
    DiaryEditor editor;
    
    // Create a temporary file with markdown content
    QTemporaryFile tempFile;
    tempFile.open();
    tempFile.write(QStringLiteral("# 2024-01-01\nThis is **bold** and *italic* and _underlined_ text").toUtf8());
    tempFile.close();
    
    // Point editor to our test file
    editor.setContentFile(tempFile.fileName());
    editor.setProperty("skipDateHeader", true);
    
    // Load and test conversion
    editor.loadContent();
    
    // Get the editor for our test date
    QDate testDate(2024, 1, 1);
    DayEditor *dayEditor = editor.findChild<DayEditor*>();
    QVERIFY2(dayEditor != nullptr, "Expected to find a DayEditor");
    
    // Check the formatting
    QTextDocument *doc = dayEditor->document();
    QTextCursor cursor(doc);
    cursor.movePosition(QTextCursor::Start);
    
    // Move to "bold" text position
    cursor.movePosition(QTextCursor::Right, QTextCursor::MoveAnchor, 8); // "This is "
    cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor, 4); // "bold"
    QVERIFY2(cursor.charFormat().fontWeight() == QFont::Bold,
             "Expected 'bold' text to be bold");
}

void TestDiaryEditor::testRichTextConversion()
{
    DiaryEditor editor;
    
    // Create a temporary file for output
    QTemporaryFile tempFile;
    tempFile.open();
    QString tempFileName = tempFile.fileName();
    tempFile.close();
    
    editor.setContentFile(tempFileName);
    editor.setProperty("skipDateHeader", true);
    
    // Create a day editor for testing
    QDate testDate(2024, 1, 1);
    editor.addDateHeader(testDate);
    DayEditor *dayEditor = editor.createDayEditor(testDate);
    QVERIFY2(dayEditor != nullptr, "Failed to create DayEditor");
    
    // Insert formatted text
    QTextCursor cursor = dayEditor->textCursor();
    cursor.movePosition(QTextCursor::Start);
    
    cursor.insertText(QStringLiteral("This is "));
    
    QTextCharFormat boldFormat;
    boldFormat.setFontWeight(QFont::Bold);
    cursor.setCharFormat(boldFormat);
    cursor.insertText(QStringLiteral("bold"));
    
    cursor.setCharFormat(QTextCharFormat());
    cursor.insertText(QStringLiteral(" and "));
    
    QTextCharFormat italicFormat;
    italicFormat.setFontItalic(true);
    cursor.setCharFormat(italicFormat);
    cursor.insertText(QStringLiteral("italic"));
    
    cursor.setCharFormat(QTextCharFormat());
    cursor.insertText(QStringLiteral(" and "));
    
    QTextCharFormat underlineFormat;
    underlineFormat.setFontUnderline(true);
    cursor.setCharFormat(underlineFormat);
    cursor.insertText(QStringLiteral("underlined"));
    
    cursor.setCharFormat(QTextCharFormat());
    cursor.insertText(QStringLiteral(" text"));
    
    // Save and verify content
    editor.saveContent();
    
    tempFile.open();
    QTextStream in(&tempFile);
    QString savedContent = in.readAll();
    tempFile.close();
    
    QVERIFY2(savedContent.contains(QStringLiteral("**bold**")),
             qPrintable(QStringLiteral("Expected to find '**bold**' in: '%1'")
                       .arg(savedContent)));
    QVERIFY2(savedContent.contains(QStringLiteral("*italic*")),
             qPrintable(QStringLiteral("Expected to find '*italic*' in: '%1'")
                       .arg(savedContent)));
    QVERIFY2(savedContent.contains(QStringLiteral("_underlined_")),
             qPrintable(QStringLiteral("Expected to find '_underlined_' in: '%1'")
                       .arg(savedContent)));
}

QTEST_MAIN(TestDiaryEditor)
#include "testdiaryeditor.moc"
