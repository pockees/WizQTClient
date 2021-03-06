#include "wizAccountManager.h"
#include "share/wizDatabase.h"
#include "share/wizMessageBox.h"

const QString g_strAccountSection = "Account";

CWizAccountManager::CWizAccountManager(CWizDatabaseManager& dbMgr)
    : m_dbMgr(dbMgr)
{

}

bool CWizAccountManager::isVip()
{
    CWizDatabase& personDb = m_dbMgr.db();

    CString strUserType = personDb.GetMetaDef(g_strAccountSection, "USERTYPE");
    if (strUserType.IsEmpty() || strUserType == "free")
        return false;

    return true;
}

bool CWizAccountManager::isPaidUser()
{
    if (isVip())
        return true;

    CWizDatabase& personDb = m_dbMgr.db();

    CWizBizDataArray arrayBiz;
    personDb.GetAllBizInfo(arrayBiz);

    for (WIZBIZDATA biz : arrayBiz)
    {
        if (biz.bizLevel > 0)
            return true;
    }

    return false;
}

bool CWizAccountManager::isPaidGroup(const QString& kbGUID)
{
    WIZGROUPDATA group;
    if (m_dbMgr.db().GetGroupData(kbGUID, group))
    {
        if (group.bizGUID.isEmpty())
            return false;

        WIZBIZDATA biz;
        if (m_dbMgr.db().GetBizData(group.bizGUID, biz))
        {
            return biz.bizLevel > 0;
        }
    }

    return false;
}

bool CWizAccountManager::isBizGroup(const QString& kbGUID)
{
    WIZGROUPDATA group;
    m_dbMgr.db().GetGroupData(kbGUID, group);
    return group.IsBiz();
}

