#include "wizFramelessWebDialog.h"
#include <QVBoxLayout>
#include <QDesktopServices>
#include <QWebEnginePage>
#include "../share/wizwebengineview.h"
#include "../share/wizthreads.h"
#include <QTimer>

bool CWizFramelessWebDialog::m_bVisibling = false;

CWizFramelessWebDialog::CWizFramelessWebDialog(QWidget *parent) :
    QDialog(parent)
{
    setFixedSize(800, 600);
    setWindowFlags(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_DeleteOnClose);

    WizWebEngineView *view = new WizWebEngineView(this);
    //
    view->addToJavaScriptWindowObject("customObject", this);
    //
    m_frame = view->page();
    connect(view, SIGNAL(loadFinishedEx(bool)), SLOT(onPageLoadFinished(bool)));
    //
    view->setContextMenuPolicy(Qt::NoContextMenu);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(view);
}

void CWizFramelessWebDialog::loadAndShow(const QString& strUrl)
{
    m_url = strUrl;
    m_frame->load(QUrl(m_url));
}

void CWizFramelessWebDialog::Execute(const QString& strFunction, QVariant param1,
                                              QVariant param2, QVariant param3, QVariant param4)
{
    if (strFunction == "close")
    {
        ::WizExecuteOnThread(WIZ_THREAD_MAIN, [=]{
            hide();
            QTimer::singleShot(1000, Qt::PreciseTimer, [=]{
                deleteLater();
            });
        });
    }
    else if (strFunction == "openindefaultbrowser")
    {
        QString strUrl = param1.toString();
        QDesktopServices::openUrl(strUrl);
    }
    else if (strFunction == "setdonotprompt")
    {
        emit doNotShowThisAgain(param1.toBool());
    }
}


void CWizFramelessWebDialog::onPageLoadFinished(bool ok)
{
    if (ok)
    {
        while (!m_timerIDList.isEmpty())
        {
            int nTimerID = m_timerIDList.first();
            killTimer(nTimerID);
            m_timerIDList.removeFirst();
        }
        //avoid QDialog::exec: Recursive call
        disconnect(m_frame, SIGNAL(loadFinished(bool)), this, SLOT(onPageLoadFinished(bool)));
        //
        WizExecuteOnThread(WIZ_THREAD_MAIN, [=]{
            //
            if (m_bVisibling)
                return;
            //
            m_bVisibling = true;
            show();
            m_bVisibling = false;
            //
        });
    }
    else
    {
        QTimer::singleShot(2 * 60 * 1000, Qt::PreciseTimer, [=]{
            deleteLater();
        });
    }
}



