#pragma once

#include <KTextEdit>
#include <QDate>
#include <QTimer>

class DiaryEditor : public KTextEdit
{
    Q_OBJECT

public:
    DiaryEditor(QWidget *parent = nullptr);
    void saveContent();
    void loadContent();

public Q_SLOTS:
    void toggleBold();
    void toggleItalic();
    void toggleUnderline();

protected:
    void keyPressEvent(QKeyEvent *event) override;

private:
    QString contentFile;
    QDate lastOpenedDate;
    QTimer *autoSaveTimer;

    void checkAndUpdateDate();
    void setupAutoSave();
private Q_SLOTS:
    void onTextChanged();
    void insertDateHeader(const QDate &date);
    bool checkListContext();
    void handleListContinuation();
};
