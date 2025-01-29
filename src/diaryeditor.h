#pragma once

#include <KTextEdit>
#include <QDate>
#include <QTimer>
#include <QMap>

class DiarySection {
public:
    DiarySection() = default;
    explicit DiarySection(const QString &content) : content(content) {}
    QString content;
};

class DiaryEditor : public KTextEdit
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

protected:
    void keyPressEvent(QKeyEvent *event) override;

private:
    QString contentFile;
    QTimer *autoSaveTimer;
    QMap<QDate, DiarySection> sections;
    
    void checkAndUpdateDate();
    void parseContent(const QString &content);
    QString serializeContent() const;
    bool hasSection(const QDate &date) const;
    void setupAutoSave();
private Q_SLOTS:
    void onTextChanged();
    void insertDateHeader(const QDate &date);
    bool checkListContext();
    void handleListContinuation();
};
