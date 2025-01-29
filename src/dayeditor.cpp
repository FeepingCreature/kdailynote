#include "dayeditor.h"
#include <QKeyEvent>
#include <QTextBlock>
#include <QRegularExpression>

DayEditor::DayEditor(const QDate &date, QWidget *parent)
    : KTextEdit(parent)
    , m_date(date)
{
    setAcceptRichText(true);
    setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
    setLineWrapMode(QTextEdit::WidgetWidth);
    setMinimumHeight(100);
}

void DayEditor::setContent(const QString &content)
{
    // Convert markdown formatting to rich text
    QString html = QStringLiteral("<!DOCTYPE HTML><html><body>");
    
    // Split content into paragraphs on double newlines
    QStringList paragraphs = content.split(QRegularExpression(QStringLiteral("\n\\s*\n")));
    for (int i = 0; i < paragraphs.size(); ++i) {
        QString para = paragraphs[i].trimmed();
        if (!para.isEmpty()) {
            // Apply formatting
            para.replace(QRegularExpression(QStringLiteral("\\*\\*(.+?)\\*\\*")), 
                        QStringLiteral("<b>\\1</b>"));
            para.replace(QRegularExpression(QStringLiteral("\\*(.+?)\\*")), 
                        QStringLiteral("<i>\\1</i>"));
            para.replace(QRegularExpression(QStringLiteral("_(.+?)_")), 
                        QStringLiteral("<u>\\1</u>"));
            html += QStringLiteral("<p>") + para + QStringLiteral("</p>");
        }
    }
    
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
            
            // Handle formatting
            if (format.fontWeight() == QFont::Bold || 
                format.fontItalic() || 
                format.fontUnderline()) {
                
                // Output leading whitespace before format markers
                int i = 0;
                while (i < text.length() && text[i].isSpace()) {
                    markdown += text[i++];
                }
                
                // If there's any non-whitespace content, apply formatting
                if (i < text.length()) {
                    QString content = text.mid(i);
                    
                    // Check if remaining text is all whitespace
                    int j = content.length() - 1;
                    while (j >= 0 && content[j].isSpace()) {
                        j--;
                    }
                    
                    if (j >= 0) {  // Has non-whitespace content
                        QString middle = content.left(j + 1);
                        QString trailing = content.mid(j + 1);
                        
                        if (format.fontWeight() == QFont::Bold) {
                            middle = QStringLiteral("**") + middle + QStringLiteral("**");
                        }
                        if (format.fontItalic()) {
                            middle = QStringLiteral("*") + middle + QStringLiteral("*");
                        }
                        if (format.fontUnderline()) {
                            middle = QStringLiteral("_") + middle + QStringLiteral("_");
                        }
                        
                        markdown += middle + trailing;
                    } else {
                        markdown += content;  // All whitespace, no formatting needed
                    }
                }
            } else {
                markdown += formattedText;  // No formatting, output as-is
            }
        }
        
        block = block.next();
        if (block.isValid())
            markdown += QStringLiteral("\n\n");
    }
    
    return markdown;
}

void DayEditor::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Return && checkListContext()) {
        handleListContinuation();
        return;
    }

    if (event->key() == Qt::Key_Up && textCursor().atStart()) {
        Q_EMIT navigateToDate(m_date.addDays(-1));
        return;
    }

    if (event->key() == Qt::Key_Down && textCursor().atEnd()) {
        Q_EMIT navigateToDate(m_date.addDays(1));
        return;
    }

    if (event->key() == Qt::Key_Escape) {
        window()->hide();
        return;
    }

    KTextEdit::keyPressEvent(event);
}

void DayEditor::focusOutEvent(QFocusEvent *event)
{
    KTextEdit::focusOutEvent(event);
    window()->hide();
}

void DayEditor::resizeEvent(QResizeEvent *event)
{
    KTextEdit::resizeEvent(event);
    updateGeometry();
}

void DayEditor::updateGeometry()
{
    // Calculate required height based on content
    int docHeight = document()->size().height();
    setMinimumHeight(docHeight + 20); // Add some padding
    setMaximumHeight(docHeight + 20);
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
void DayEditor::toggleBold()
{
    QTextCursor cursor = textCursor();
    QTextCharFormat format = cursor.charFormat();
    bool wasBold = (format.fontWeight() == QFont::Bold);
    
    format.setFontWeight(wasBold ? QFont::Normal : QFont::Bold);
    cursor.mergeCharFormat(format);
    
    // Set the default format for future input
    setCurrentCharFormat(format);
}

void DayEditor::toggleItalic()
{
    QTextCursor cursor = textCursor();
    QTextCharFormat format = cursor.charFormat();
    format.setFontItalic(!format.fontItalic());
    cursor.mergeCharFormat(format);
    
    // Set the default format for future input
    setCurrentCharFormat(format);
}

void DayEditor::toggleUnderline()
{
    QTextCursor cursor = textCursor();
    QTextCharFormat format = cursor.charFormat();
    format.setFontUnderline(!format.fontUnderline());
    cursor.mergeCharFormat(format);
    
    // Set the default format for future input
    setCurrentCharFormat(format);
}
