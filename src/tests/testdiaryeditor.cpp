#include <QtTest>
#include <QSignalSpy>
#include <QTemporaryFile>
#include <QTextStream>
#include "../diaryeditor.h"

class TestDiaryEditor : public QObject
{
    Q_OBJECT

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
    tempFile.write(QStringLiteral("This is **bold** and *italic* and _underlined_ text").toUtf8());
    tempFile.close();
    
    // Point editor to our test file
    editor.setContentFile(tempFile.fileName());
    editor.document()->setModified(false);  // Reset modified state
    
    // Load and test conversion
    editor.loadContent();
    
    // Check the formatting directly
    QTextCursor cursor = editor.textCursor();
    cursor.movePosition(QTextCursor::Start);
    cursor.movePosition(QTextCursor::End, QTextCursor::KeepAnchor);
    QTextCharFormat format = cursor.charFormat();
    
    // Find and check "bold" text
    cursor.movePosition(QTextCursor::Start);
    if (cursor.movePosition(QTextCursor::Right, QTextCursor::MoveAnchor, 8)) {  // "This is "
        cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor, 4);    // "bold"
        QVERIFY2(cursor.charFormat().fontWeight() == QFont::Bold,
                "Expected 'bold' text to be bold");
    }
}

void TestDiaryEditor::testRichTextConversion()
{
    DiaryEditor editor;
    
    // Create a temporary file for output
    QTemporaryFile tempFile;
    tempFile.open();
    tempFile.close();
    editor.setProperty("contentFile", tempFile.fileName());
    
    // Insert text with formatting
    QTextCursor cursor = editor.textCursor();
    
    cursor.insertText(QStringLiteral("This is "));
    
    QTextCharFormat boldFormat;
    boldFormat.setFontWeight(QFont::Bold);
    cursor.insertText(QStringLiteral("bold"), boldFormat);
    
    cursor.insertText(QStringLiteral(" and "));
    
    QTextCharFormat italicFormat;
    italicFormat.setFontItalic(true);
    cursor.insertText(QStringLiteral("italic"), italicFormat);
    
    cursor.insertText(QStringLiteral(" and "));
    
    QTextCharFormat underlineFormat;
    underlineFormat.setFontUnderline(true);
    cursor.insertText(QStringLiteral("underlined"), underlineFormat);
    
    cursor.insertText(QStringLiteral(" text"));
    
    // Save content
    editor.saveContent();
    
    // Read the saved file directly
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
