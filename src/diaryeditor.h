#pragma once

#include <KTextEdit>
#include <QDate>

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
    enum class FormatState {
        Normal,
        Bold,
        Italic,
        Underline
    };

    FormatState currentFormatState;
    QString contentFile;
    QDate lastOpenedDate;

    void checkAndUpdateDate();
    void insertDateHeader(const QDate &date);
    bool checkListContext();
    void handleListContinuation();
};
