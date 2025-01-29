#include <QtTest>
#include <QSignalSpy>
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
    
    // Test loading markdown
    QString markdown = QStringLiteral("This is **bold** and *italic* and _underlined_ text");
    QString expectedHtml = QStringLiteral("This is <b>bold</b> and <i>italic</i> and <u>underlined</u> text");
    
    // Simulate loading markdown
    editor.setPlainText(markdown);
    editor.loadContent(); // This will convert markdown to rich text
    
    // Get the HTML and compare
    QString actualHtml = editor.toHtml();
    QVERIFY(actualHtml.contains(expectedHtml));
}

void TestDiaryEditor::testRichTextConversion()
{
    DiaryEditor editor;
    
    // Set up rich text
    editor.setHtml(QStringLiteral("This is <b>bold</b> and <i>italic</i> and <u>underlined</u> text"));
    
    // Save and verify markdown
    editor.saveContent();
    
    // Read the saved file and verify markdown format
    QString savedContent = editor.toPlainText();
    QVERIFY(savedContent.contains(QStringLiteral("**bold**")));
    QVERIFY(savedContent.contains(QStringLiteral("*italic*")));
    QVERIFY(savedContent.contains(QStringLiteral("_underlined_")));
}

QTEST_MAIN(TestDiaryEditor)
#include "testdiaryeditor.moc"
