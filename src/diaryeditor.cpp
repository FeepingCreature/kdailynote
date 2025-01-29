#include "diaryeditor.h"
#include <QStandardPaths>
#include <QFile>
#include <QDir>
#include <QLabel>
#include <QApplication>
#include <QFontMetrics>

DiaryEditor::DiaryEditor(QWidget *parent)
    : QScrollArea(parent)
    , autoSaveTimer(new QTimer(this))
    , containerWidget(new QWidget(this))
    , layout(new QVBoxLayout(containerWidget))
{
    // Set up content file location
    QString dataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(dataPath);
    contentFile = dataPath + QStringLiteral("/diary.txt");

    // Setup scroll area
    setWidgetResizable(true);
    setWidget(containerWidget);
    
    // Setup layout
    layout->setSpacing(5);
    layout->setContentsMargins(10, 10, 10, 10);
    
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
    }
}

void DiaryEditor::parseContent(const QString &content)
{
    // Clear existing editors
    qDeleteAll(editors);
    editors.clear();
    
    // Clear layout
    QLayoutItem *item;
    while ((item = layout->takeAt(0)) != nullptr) {
        delete item->widget();
        delete item;
    }
    
    QStringList lines = content.split(QStringLiteral("\n"));
    QDate currentDate;
    QString currentContent;
    
    for (const QString &line : lines) {
        if (line.startsWith(QStringLiteral("# "))) {
            // If we were building a section, create its editor
            if (currentDate.isValid() && !currentContent.isEmpty()) {
                DayEditor *editor = createDayEditor(currentDate);
                editor->setContent(currentContent.trimmed());
            }
            
            // Parse new date header
            QString dateStr = line.mid(2).trimmed();
            currentDate = QDate::fromString(dateStr, Qt::ISODate);
            currentContent.clear();
            
            // Add date header label
            if (currentDate.isValid()) {
                addDateHeader(currentDate);
            }
        } else if (currentDate.isValid()) {
            currentContent += line + QStringLiteral("\n");
        }
    }
    
    // Handle the last section
    if (currentDate.isValid() && !currentContent.isEmpty()) {
        DayEditor *editor = createDayEditor(currentDate);
        editor->setContent(currentContent.trimmed());
    }
    
    // Add stretch at the end
    layout->addStretch();
}

void DiaryEditor::saveContent()
{
    QString content = serializeContent();
    
    QFile file(contentFile);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        file.write(content.toUtf8());
    }
}

QString DiaryEditor::serializeContent() const
{
    QString result;
    QMapIterator<QDate, DayEditor*> it(editors);
    while (it.hasNext()) {
        it.next();
        result += QStringLiteral("# %1\n\n").arg(it.key().toString(Qt::ISODate));
        result += it.value()->content();
        result += QStringLiteral("\n\n");
    }
    return result;
}

void DiaryEditor::checkAndUpdateDate()
{
    if (skipDateHeader()) {
        return;
    }
    
    QDate currentDate = QDate::currentDate();
    if (!hasSection(currentDate)) {
        addDateHeader(currentDate);
        createDayEditor(currentDate);
    }
}

bool DiaryEditor::hasSection(const QDate &date) const
{
    return editors.contains(date);
}

void DiaryEditor::addDateHeader(const QDate &date)
{
    QLabel *dateLabel = new QLabel(date.toString(Qt::ISODate), containerWidget);
    dateLabel->setAlignment(Qt::AlignCenter);
    
    // Style the date header
    QFont font = dateLabel->font();
    font.setBold(true);
    font.setPointSize(font.pointSize() + 2);
    dateLabel->setFont(font);
    
    // Add some vertical spacing
    layout->addSpacing(10);
    layout->addWidget(dateLabel);
}

DayEditor* DiaryEditor::createDayEditor(const QDate &date)
{
    DayEditor *editor = new DayEditor(date, containerWidget);
    editors[date] = editor;
    layout->addWidget(editor);
    
    connect(editor, &DayEditor::textChanged, this, &DiaryEditor::onEditorChanged);
    connect(editor, &DayEditor::navigateToDate, this, &DiaryEditor::onNavigateToDate);
    
    return editor;
}

DayEditor* DiaryEditor::getCurrentEditor()
{
    QWidget *focused = QApplication::focusWidget();
    if (DayEditor *editor = qobject_cast<DayEditor*>(focused)) {
        return editor;
    }
    return nullptr;
}


void DiaryEditor::toggleBold()
{
    if (DayEditor *editor = getCurrentEditor()) {
        editor->toggleBold();
    }
}

void DiaryEditor::toggleItalic()
{
    if (DayEditor *editor = getCurrentEditor()) {
        editor->toggleItalic();
    }
}

void DiaryEditor::toggleUnderline()
{
    if (DayEditor *editor = getCurrentEditor()) {
        editor->toggleUnderline();
    }
}

void DiaryEditor::setupAutoSave()
{
    // Save after 3 seconds of inactivity
    autoSaveTimer->setInterval(3000);
    autoSaveTimer->setSingleShot(true);
    connect(autoSaveTimer, &QTimer::timeout, this, &DiaryEditor::saveContent);
}

void DiaryEditor::onEditorChanged()
{
    // Restart inactivity timer
    autoSaveTimer->start();
}

void DiaryEditor::onNavigateToDate(const QDate &date)
{
    if (!editors.contains(date)) {
        addDateHeader(date);
        createDayEditor(date);
    }
    
    if (DayEditor *editor = editors.value(date)) {
        editor->setFocus();
        QTextCursor cursor = editor->textCursor();
        cursor.movePosition(date < QDate::currentDate() ? QTextCursor::End : QTextCursor::Start);
        editor->setTextCursor(cursor);
        ensureWidgetVisible(editor);
    }
}
