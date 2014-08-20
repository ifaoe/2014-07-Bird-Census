#include "textlogger.h"

TextLogger::TextLogger(QObject *parent, QPlainTextEdit *aIoText, QStatusBar *aStatusBar) :
    QObject(parent), ioText(aIoText), statusBar(aStatusBar)
{
}

// --------------------------------------------------------------------------
void TextLogger::error(const QString msg) {
    QDateTime dateTime = QDateTime::currentDateTime();
    statusBar->showMessage(tr("ERROR: ")+msg, STATUS_TIME_OUT);
    ioText->appendPlainText(dateTime.toString(STATUS_TIME_FMT)
                            +STATUS_ERROR+"\n"+msg+"\n");
}
// --------------------------------------------------------------------------
void TextLogger::debug(const QString msg) {
    QDateTime dateTime = QDateTime::currentDateTime();
    statusBar->showMessage(tr("DEBUG: ")+msg, STATUS_TIME_OUT);
    ioText->appendPlainText(dateTime.toString(STATUS_TIME_FMT)
                                   +STATUS_DEBUG+"\n"+msg+"\n");
}
// --------------------------------------------------------------------------
void TextLogger::log(const QString msg) {
    QDateTime dateTime = QDateTime::currentDateTime();
    statusBar->showMessage(tr("INFO: ")+msg, STATUS_TIME_OUT);
    ioText->appendPlainText(dateTime.toString(STATUS_TIME_FMT)
                                   +STATUS_PROMPT+"\n"+msg+"\n");
}

