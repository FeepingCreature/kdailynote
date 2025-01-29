#pragma once

#include <KTextEdit>
#include <QDate>

class DayEditor : public KTextEdit
{
    Q_OBJECT

public:
    explicit DayEditor(const QDate &date, QWidget *parent = nullptr);
    
    QDate date() const { return m_date; }
    void setContent(const QString &content);
    QString content() const;

protected:
    void keyPressEvent(QKeyEvent *event) override;

private:
    QDate m_date;
    
    bool checkListContext();
    void handleListContinuation();
};
