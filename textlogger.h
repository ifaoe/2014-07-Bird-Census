#ifndef TEXTLOGGER_H
#define TEXTLOGGER_H

#include <QObject>
#include <QPlainTextEdit>
#include <QStatusBar>
#include <QDateTime>

const QString STATUS_PROMPT  = "|OUT| ";
const QString STATUS_DEBUG   = "|DBG| ";
const QString STATUS_ERROR   = "|ERR| ";
const QString STATUS_TIME_FMT = "yyyy-MM-dd|hh:mm:ss";
const int STATUS_TIME_OUT = 5000;

class TextLogger : public QObject
{
    Q_OBJECT
public:
    explicit TextLogger(QObject *parent, QPlainTextEdit *aIoText, QStatusBar *aStatusBar);
    // ---------------------------------------------------------------------
    // Service Routinen
    // ---------------------------------------------------------------------
    void log(QString msg);
    void debug(QString msg);
    void error(QString msg);

signals:
    
public slots:
    
private:
    int level = -1;
    QPlainTextEdit *ioText = 0;
    QStatusBar *statusBar  = 0;
};

#endif // TEXTLOGGER_H
