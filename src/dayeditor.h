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

public Q_SLOTS:
    void toggleBold();
    void toggleItalic();
    void toggleUnderline();

Q_SIGNALS:
    void navigateToDate(const QDate &date, const QDate &fromDate);

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void focusOutEvent(QFocusEvent *event) override;

private:
    QDate m_date;
    void updateGeometry();
    
    bool checkListContext();
    void handleListContinuation();
};
