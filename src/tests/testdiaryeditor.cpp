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
    tempFile.write("This is **bold** and *italic* and _underlined_ text");
    tempFile.close();
    
    // Point editor to our test file
    editor.setProperty("contentFile", tempFile.fileName());
    
    // Load and test conversion
    editor.loadContent();
    
    // Get the HTML and compare
    QString actualHtml = editor.toHtml();
    QString expectedHtml = QStringLiteral("<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">This is <span style=\" font-weight:700;\">bold</span> and <span style=\" font-style:italic;\">italic</span> and <span style=\" text-decoration: underline;\">underlined</span> text</p>");
    
    QVERIFY2(actualHtml.contains(expectedHtml),
             qPrintable(QStringLiteral("Expected to find: '%1'\nActual content: '%2'")
                       .arg(expectedHtml, actualHtml)));
}

void TestDiaryEditor::testRichTextConversion()
{
    DiaryEditor editor;
    
    // Create a temporary file for output
    QTemporaryFile tempFile;
    tempFile.open();
    tempFile.close();
    editor.setProperty("contentFile", tempFile.fileName());
    
    // Set up rich text
    editor.setHtml(QStringLiteral("This is <span style=\" font-weight:700;\">bold</span> and <span style=\" font-style:italic;\">italic</span> and <span style=\" text-decoration: underline;\">underlined</span> text"));
    
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
