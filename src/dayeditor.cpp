#include "dayeditor.h"
#include <QKeyEvent>
#include <QTextBlock>

DayEditor::DayEditor(const QDate &date, QWidget *parent)
    : KTextEdit(parent)
    , m_date(date)
{
    setAcceptRichText(true);
    setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
    setLineWrapMode(QTextEdit::WidgetWidth);
}

void DayEditor::setContent(const QString &content)
{
    // Convert markdown formatting to rich text
    QString html = QStringLiteral("<!DOCTYPE HTML><html><body>");
    html += content;
    html.replace(QRegularExpression(QStringLiteral("\\*\\*(.+?)\\*\\*")), 
                QStringLiteral("<b>\\1</b>"));
    html.replace(QRegularExpression(QStringLiteral("\\*(.+?)\\*")), 
                QStringLiteral("<i>\\1</i>"));
    html.replace(QRegularExpression(QStringLiteral("_(.+?)_")), 
                QStringLiteral("<u>\\1</u>"));
    html += QStringLiteral("</body></html>");
    
    setHtml(html);
    document()->setModified(false);
}

QString DayEditor::content() const
{
    QString markdown;
    QTextBlock block = document()->firstBlock();
    
    while (block.isValid()) {
        QTextBlock::iterator it;
        for (it = block.begin(); !it.atEnd(); ++it) {
            QTextFragment fragment = it.fragment();
            if (!fragment.isValid())
                continue;

            QTextCharFormat format = fragment.charFormat();
            QString text = fragment.text();
            
            // Build markdown with all applicable formats
            QString formattedText = text;
            
            if (format.fontWeight() == QFont::Bold) {
                formattedText = QStringLiteral("**") + formattedText + QStringLiteral("**");
            }
            if (format.fontItalic()) {
                formattedText = QStringLiteral("*") + formattedText + QStringLiteral("*");
            }
            if (format.fontUnderline()) {
                formattedText = QStringLiteral("_") + formattedText + QStringLiteral("_");
            }
            
            markdown += formattedText;
        }
        
        block = block.next();
        if (block.isValid())
            markdown += QStringLiteral("\n");
    }
    
    return markdown;
}

void DayEditor::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Return && checkListContext()) {
        handleListContinuation();
        return;
    }

    KTextEdit::keyPressEvent(event);
}

bool DayEditor::checkListContext()
{
    QTextCursor cursor = textCursor();
    cursor.movePosition(QTextCursor::StartOfLine);
    cursor.movePosition(QTextCursor::EndOfLine, QTextCursor::KeepAnchor);
    QString line = cursor.selectedText();

    return line.startsWith(QStringLiteral(" - "));
}

void DayEditor::handleListContinuation()
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
