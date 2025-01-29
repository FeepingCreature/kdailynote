#pragma once

#include <QScrollArea>
#include <QVBoxLayout>
#include <QTimer>
#include <QMap>
#include <QDate>
#include "dayeditor.h"

class DiaryEditor : public QScrollArea
{
    Q_OBJECT

public:
    DiaryEditor(QWidget *parent = nullptr);
    void saveContent();
    void loadContent();
    void setContentFile(const QString &path) { contentFile = path; }

public Q_SLOTS:
    void toggleBold();
    void toggleItalic();
    void toggleUnderline();

private:
    QString contentFile;
    QTimer *autoSaveTimer;
    QWidget *containerWidget;
    QVBoxLayout *layout;
    QMap<QDate, DayEditor*> editors;
    
    void checkAndUpdateDate();
    bool skipDateHeader() const { return property("skipDateHeader").toBool(); }
    void parseContent(const QString &content);
    QString serializeContent() const;
    bool hasSection(const QDate &date) const;
    void setupAutoSave();
    DayEditor* createDayEditor(const QDate &date);
    DayEditor* getCurrentEditor();

private Q_SLOTS:
    void onEditorChanged();
    void addDateHeader(const QDate &date);
    DayEditor* createDayEditor(const QDate &date);
};
