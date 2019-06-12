/*
* Module:   TRTCMainViewController
*
* Function: ʹ��TRTC SDK��� 1v1 �� 1vn ����Ƶͨ������
*
*    1. ֧�־Ź���ƽ�̺�ǰ��������ֲ�ͬ����Ƶ���沼�ַ�ʽ���ò����� TRTCVideoViewLayout 
*       ������ÿ����Ƶ�����λ���Ų��ʹ�С�ߴ�(window��δʵ�־Ź��񲼾�)
*
*    2. ֧�ֶ���Ƶͨ���ķֱ��ʡ�֡�ʺ�����ģʽ���е������ò����� TRTCSettingViewController ��ʵ��
*
*    3. �������߼���ĳһ��ͨ�����䣬��Ҫ��ָ�� roomid �� userid���ⲿ���� TRTCNewViewController ��ʵ��
*/

#include "stdafx.h"
#include "afxdialogex.h"
#include "TRTCDemo.h"
#include "util/Base.h"
#include "StorageConfigMgr.h"
#include "TRTCMainViewController.h"
#include "TRTCSettingViewController.h"

#include <ctime>
#include <cstdio>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

ITRTCCloud* getTRTCCloud()
{
    if (TRTCMainViewController::g_cloud == nullptr)
    {
        TRTCMainViewController::g_cloud = getTRTCShareInstance();
    }
    return TRTCMainViewController::g_cloud;
}

void destroyTRTCCloud()
{
    if (TRTCMainViewController::g_cloud != nullptr)
        destroyTRTCShareInstance();
    TRTCMainViewController::g_cloud = nullptr;
}

ITRTCCloud* TRTCMainViewController::g_cloud = nullptr;

// CTRTCDemoDlg �Ի���

TRTCMainViewController::TRTCMainViewController(CWnd* pParent /*=NULL*/)
    : CDialogEx(IDD_TESTTRTCAPP_DIALOG, pParent)
{
    m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void TRTCMainViewController::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(TRTCMainViewController, CDialogEx)
    ON_WM_CLOSE(OnClose)
    ON_MESSAGE(WM_CUSTOM_CLOSE_SETTINGVIEW, OnMsgSettingViewClose)
    ON_BN_CLICKED(IDC_EXIT_ROOM, &TRTCMainViewController::OnBnClickedExitRoom)
    ON_BN_CLICKED(IDC_BTN_SETTING, &TRTCMainViewController::OnBnClickedSetting)
    ON_BN_CLICKED(IDC_BTN_LOG, &TRTCMainViewController::OnBnClickedLog)
    ON_WM_CTLCOLOR()
END_MESSAGE_MAP()
/*
* ��ʼ������ؼ���������Ҫ����Ƶ��ʾView���Լ��ײ���һ�Ź��ܰ�ť
*/
BOOL TRTCMainViewController::OnInitDialog()
{
    CDialogEx::OnInitDialog();
    newFont.CreatePointFont(120, L"΢���ź�");

    CWnd *pBtnSetting = GetDlgItem(IDC_BTN_SETTING);
    pBtnSetting->SetFont(&newFont);

    CWnd *pBtnExit = GetDlgItem(IDC_EXIT_ROOM);
    pBtnExit->SetFont(&newFont);

    // ���ô˶Ի����ͼ�ꡣ  ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
    // ִ�д˲���
    SetIcon(m_hIcon, TRUE);         // ���ô�ͼ��
    SetIcon(m_hIcon, FALSE);        // ����Сͼ��

    getTRTCCloud()->addCallback(this);

    // TODO: �ڴ���Ӷ���ĳ�ʼ������
    UpdateRemoteViewInfo(IDC_REMOTE_VIDEO_VIEW1, "");
    UpdateRemoteViewInfo(IDC_REMOTE_VIDEO_VIEW2, "");
    UpdateRemoteViewInfo(IDC_REMOTE_VIDEO_VIEW3, "");
    UpdateRemoteViewInfo(IDC_REMOTE_VIDEO_VIEW4, "");
    UpdateRemoteViewInfo(IDC_REMOTE_VIDEO_VIEW5, "");
    UpdateRemoteViewInfo(IDC_REMOTE_VIDEO_VIEW6, "");
    UpdateRemoteViewInfo(IDC_REMOTE_VIDEO_VIEW7, "");
    UpdateRemoteViewInfo(IDC_REMOTE_VIDEO_VIEW8, "");

    ShowWindow(SW_NORMAL);

    CRect rtDesk, rtDlg;
    ::GetWindowRect(::GetDesktopWindow(), &rtDesk);
    GetWindowRect(&rtDlg);
    int iXPos = rtDesk.Width() / 2 - rtDlg.Width() / 2;
    int iYPos = rtDesk.Height() / 2 - rtDlg.Height() / 2;
    SetWindowPos(NULL, iXPos, iYPos, 0, 0, SWP_NOOWNERZORDER | SWP_NOSIZE | SWP_NOZORDER);
    return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void TRTCMainViewController::onError(TXLiteAVError errCode, const char* errMsg, void* arg)
{

}

void TRTCMainViewController::onWarning(TXLiteAVWarning warningCode, const char* warningMsg, void* arg)
{

}

void TRTCMainViewController::onEnterRoom(uint64_t elapsed)
{
    CWnd *pLocalVideoView = GetDlgItem(IDC_LOCAL_VIDEO_VIEW);
    HWND hwnd = pLocalVideoView->GetSafeHwnd();
    getTRTCCloud()->setLocalViewFillMode(TRTCVideoFillMode_Fit);
    getTRTCCloud()->startLocalPreview(hwnd);
    getTRTCCloud()->startLocalAudio();

    CWnd *pStatic = GetDlgItem(IDC_STATIC_LOCAL_USERID);
    pStatic->SetWindowTextW(UTF82Wide(m_userId).c_str());
    pStatic->SetFont(&newFont);
}

void TRTCMainViewController::onExitRoom(int reason)
{
    getTRTCCloud()->removeCallback(this);
    getTRTCCloud()->stopLocalPreview();
    getTRTCCloud()->stopAllRemoteView();

    CWnd *pStatic = GetDlgItem(IDC_STATIC_LOCAL_USERID);
    pStatic->SetWindowTextW(L"");

    UpdateRemoteViewInfo(IDC_REMOTE_VIDEO_VIEW1, "");
    UpdateRemoteViewInfo(IDC_REMOTE_VIDEO_VIEW2, "");
    UpdateRemoteViewInfo(IDC_REMOTE_VIDEO_VIEW3, "");
    UpdateRemoteViewInfo(IDC_REMOTE_VIDEO_VIEW4, "");
    UpdateRemoteViewInfo(IDC_REMOTE_VIDEO_VIEW5, "");
    UpdateRemoteViewInfo(IDC_REMOTE_VIDEO_VIEW6, "");
    UpdateRemoteViewInfo(IDC_REMOTE_VIDEO_VIEW7, "");
    UpdateRemoteViewInfo(IDC_REMOTE_VIDEO_VIEW8, "");

    //�л��ص�¼����
    ShowWindow(SW_HIDE);
    CWnd* pWnd = GetParent();
    if (pWnd)
    {
        pWnd->ShowWindow(SW_NORMAL);
        ::PostMessage(pWnd->GetSafeHwnd(), WM_CUSTOM_CLOSE_MAINVIEW, 0, 0);
    }
}

void TRTCMainViewController::onUserEnter(const char* userId)
{
    int viewId = FindIdleRemoteVideoView();
    if (viewId != 0)
    {
        UpdateRemoteViewInfo(viewId, userId);
        CWnd *pRemoteVideoView = GetDlgItem(viewId);
        HWND hwnd = pRemoteVideoView->GetSafeHwnd();
        getTRTCCloud()->setRemoteViewFillMode(userId, TRTCVideoFillMode_Fit);
        getTRTCCloud()->startRemoteView(userId, hwnd);
    }
    else
    {
        // no find view to render remote video
    }
}

void TRTCMainViewController::onUserExit(const char* userId, int reason)
{
    int viewId = FindOccupyRemoteVideoView(userId);

    if (viewId != 0)
    {
        getTRTCCloud()->stopRemoteView(userId);
        UpdateRemoteViewInfo(viewId, "");
    }
}

void TRTCMainViewController::OnClose()
{
    getTRTCCloud()->exitRoom();
}

HBRUSH TRTCMainViewController::OnCtlColor(CDC * pDC, CWnd * pWnd, UINT nCtlColor)
{
    HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);
    if (nCtlColor == CTLCOLOR_STATIC)
    {
       
        if (pWnd->GetDlgCtrlID() == IDC_STATIC_LOCAL_BORD ||
            pWnd->GetDlgCtrlID() == IDC_STATIC_REMOTE_BORD1 ||
            pWnd->GetDlgCtrlID() == IDC_STATIC_REMOTE_BORD2 ||
            pWnd->GetDlgCtrlID() == IDC_STATIC_REMOTE_BORD3 ||
            pWnd->GetDlgCtrlID() == IDC_STATIC_REMOTE_BORD4 ||
            pWnd->GetDlgCtrlID() == IDC_STATIC_REMOTE_BORD5 ||
            pWnd->GetDlgCtrlID() == IDC_STATIC_REMOTE_BORD6 ||
            pWnd->GetDlgCtrlID() == IDC_STATIC_REMOTE_BORD7 ||
            pWnd->GetDlgCtrlID() == IDC_STATIC_REMOTE_BORD8)
        {
            static CBrush brh(RGB(210, 210, 210));//��̬��ˢ��Դ
            CRect rect;
            if (pWnd->GetDlgCtrlID() == IDC_STATIC_LOCAL_BORD)
                GetDlgItem(IDC_STATIC_LOCAL_BORD)->GetClientRect(rect);
            if (pWnd->GetDlgCtrlID() == IDC_STATIC_REMOTE_BORD1)
                GetDlgItem(IDC_STATIC_REMOTE_BORD1)->GetClientRect(rect);
            if (pWnd->GetDlgCtrlID() == IDC_STATIC_REMOTE_BORD2)
                GetDlgItem(IDC_STATIC_REMOTE_BORD2)->GetClientRect(rect);
            if (pWnd->GetDlgCtrlID() == IDC_STATIC_REMOTE_BORD3)
                GetDlgItem(IDC_STATIC_REMOTE_BORD3)->GetClientRect(rect);
            if (pWnd->GetDlgCtrlID() == IDC_STATIC_REMOTE_BORD4)
                GetDlgItem(IDC_STATIC_REMOTE_BORD4)->GetClientRect(rect);
            if (pWnd->GetDlgCtrlID() == IDC_STATIC_REMOTE_BORD5)
                GetDlgItem(IDC_STATIC_REMOTE_BORD5)->GetClientRect(rect);
            if (pWnd->GetDlgCtrlID() == IDC_STATIC_REMOTE_BORD6)
                GetDlgItem(IDC_STATIC_REMOTE_BORD6)->GetClientRect(rect);
            if (pWnd->GetDlgCtrlID() == IDC_STATIC_REMOTE_BORD7)
                GetDlgItem(IDC_STATIC_REMOTE_BORD7)->GetClientRect(rect);
            if (pWnd->GetDlgCtrlID() == IDC_STATIC_REMOTE_BORD8)
                GetDlgItem(IDC_STATIC_REMOTE_BORD8)->GetClientRect(rect);

            rect.InflateRect(-3, -7, -3, -3);
            pDC->FillRect(rect, &brh);//���Groupbox���α���ɫ
            return (HBRUSH)brh.m_hObject;//����Groupbox������ˢ���ƻ�ˢ
        }

        if (pWnd->GetDlgCtrlID() == IDC_STATIC_LOCAL_USERID ||
            pWnd->GetDlgCtrlID() == IDC_STATIC_REMOTE_USERID1 ||
            pWnd->GetDlgCtrlID() == IDC_STATIC_REMOTE_USERID2 ||
            pWnd->GetDlgCtrlID() == IDC_STATIC_REMOTE_USERID3 ||
            pWnd->GetDlgCtrlID() == IDC_STATIC_REMOTE_USERID4 ||
            pWnd->GetDlgCtrlID() == IDC_STATIC_REMOTE_USERID5 ||
            pWnd->GetDlgCtrlID() == IDC_STATIC_REMOTE_USERID6 ||
            pWnd->GetDlgCtrlID() == IDC_STATIC_REMOTE_USERID7 ||
            pWnd->GetDlgCtrlID() == IDC_STATIC_REMOTE_USERID8)
        {
            static CBrush brh(RGB(210, 210, 210));//��̬��ˢ��Դ
            CRect rect;
            if (pWnd->GetDlgCtrlID() == IDC_STATIC_LOCAL_USERID)
                GetDlgItem(IDC_STATIC_LOCAL_USERID)->GetClientRect(rect);
            if (pWnd->GetDlgCtrlID() == IDC_STATIC_REMOTE_USERID1)
                GetDlgItem(IDC_STATIC_REMOTE_USERID1)->GetClientRect(rect);
            if (pWnd->GetDlgCtrlID() == IDC_STATIC_REMOTE_USERID2)
                GetDlgItem(IDC_STATIC_REMOTE_USERID2)->GetClientRect(rect);
            if (pWnd->GetDlgCtrlID() == IDC_STATIC_REMOTE_USERID3)
                GetDlgItem(IDC_STATIC_REMOTE_USERID3)->GetClientRect(rect);
            if (pWnd->GetDlgCtrlID() == IDC_STATIC_REMOTE_USERID4)
                GetDlgItem(IDC_STATIC_REMOTE_USERID4)->GetClientRect(rect);
            if (pWnd->GetDlgCtrlID() == IDC_STATIC_REMOTE_USERID5)
                GetDlgItem(IDC_STATIC_REMOTE_USERID5)->GetClientRect(rect);
            if (pWnd->GetDlgCtrlID() == IDC_STATIC_REMOTE_USERID6)
                GetDlgItem(IDC_STATIC_REMOTE_USERID6)->GetClientRect(rect);
            if (pWnd->GetDlgCtrlID() == IDC_STATIC_REMOTE_USERID7)
                GetDlgItem(IDC_STATIC_REMOTE_USERID7)->GetClientRect(rect);
            if (pWnd->GetDlgCtrlID() == IDC_STATIC_REMOTE_USERID8)
                GetDlgItem(IDC_STATIC_REMOTE_USERID8)->GetClientRect(rect);

            pDC->FillRect(rect, &brh);//���Groupbox���α���ɫ
            pDC->SetBkMode(TRANSPARENT);
            return (HBRUSH)brh.m_hObject;//����Groupbox������ˢ���ƻ�ˢ
        }
    }
    return hbr;
}

int TRTCMainViewController::FindIdleRemoteVideoView()
{
    if (m_remoteUserInfo[IDC_REMOTE_VIDEO_VIEW1].compare("") == 0)
    {
        return IDC_REMOTE_VIDEO_VIEW1;
    }
    if (m_remoteUserInfo[IDC_REMOTE_VIDEO_VIEW2].compare("") == 0)
    {
        return IDC_REMOTE_VIDEO_VIEW2;
    }
    if (m_remoteUserInfo[IDC_REMOTE_VIDEO_VIEW3].compare("") == 0)
    {
        return IDC_REMOTE_VIDEO_VIEW3;
    }
    if (m_remoteUserInfo[IDC_REMOTE_VIDEO_VIEW4].compare("") == 0)
    {
        return IDC_REMOTE_VIDEO_VIEW4;
    }
    if (m_remoteUserInfo[IDC_REMOTE_VIDEO_VIEW5].compare("") == 0)
    {
        return IDC_REMOTE_VIDEO_VIEW5;
    }
    if (m_remoteUserInfo[IDC_REMOTE_VIDEO_VIEW6].compare("") == 0)
    {
        return IDC_REMOTE_VIDEO_VIEW6;
    }
    if (m_remoteUserInfo[IDC_REMOTE_VIDEO_VIEW7].compare("") == 0)
    {
        return IDC_REMOTE_VIDEO_VIEW7;
    }
    if (m_remoteUserInfo[IDC_REMOTE_VIDEO_VIEW8].compare("") == 0)
    {
        return IDC_REMOTE_VIDEO_VIEW8;
    }
    return 0;
}

int TRTCMainViewController::FindOccupyRemoteVideoView(std::string userId)
{
    if (m_remoteUserInfo[IDC_REMOTE_VIDEO_VIEW1].compare(userId.c_str()) == 0)
    {
        return IDC_REMOTE_VIDEO_VIEW1;
    }
    if (m_remoteUserInfo[IDC_REMOTE_VIDEO_VIEW2].compare(userId.c_str()) == 0)
    {
        return IDC_REMOTE_VIDEO_VIEW2;
    }
    if (m_remoteUserInfo[IDC_REMOTE_VIDEO_VIEW3].compare(userId.c_str()) == 0)
    {
        return IDC_REMOTE_VIDEO_VIEW3;
    }
    if (m_remoteUserInfo[IDC_REMOTE_VIDEO_VIEW4].compare(userId.c_str()) == 0)
    {
        return IDC_REMOTE_VIDEO_VIEW4;
    }
    if (m_remoteUserInfo[IDC_REMOTE_VIDEO_VIEW5].compare(userId.c_str()) == 0)
    {
        return IDC_REMOTE_VIDEO_VIEW5;
    }
    if (m_remoteUserInfo[IDC_REMOTE_VIDEO_VIEW6].compare(userId.c_str()) == 0)
    {
        return IDC_REMOTE_VIDEO_VIEW6;
    }
    if (m_remoteUserInfo[IDC_REMOTE_VIDEO_VIEW7].compare(userId.c_str()) == 0)
    {
        return IDC_REMOTE_VIDEO_VIEW7;
    }
    if (m_remoteUserInfo[IDC_REMOTE_VIDEO_VIEW8].compare(userId.c_str()) == 0)
    {
        return IDC_REMOTE_VIDEO_VIEW8;
    }
    return 0;
}

void TRTCMainViewController::UpdateRemoteViewInfo(int id, std::string userId)
{
    if (id == IDC_REMOTE_VIDEO_VIEW1)
    {
        CWnd *pStatic = GetDlgItem(IDC_STATIC_REMOTE_USERID1);
        pStatic->SetWindowTextW(UTF82Wide(userId).c_str());
        pStatic->SetFont(&newFont);
        m_remoteUserInfo[id] = userId;
    }
    if (id == IDC_REMOTE_VIDEO_VIEW2)
    {
        CWnd *pStatic = GetDlgItem(IDC_STATIC_REMOTE_USERID2);
        pStatic->SetWindowTextW(UTF82Wide(userId).c_str());
        pStatic->SetFont(&newFont);
        m_remoteUserInfo[id] = userId;
    }
    if (id == IDC_REMOTE_VIDEO_VIEW3)
    {
        CWnd *pStatic = GetDlgItem(IDC_STATIC_REMOTE_USERID3);
        pStatic->SetWindowTextW(UTF82Wide(userId).c_str());
        pStatic->SetFont(&newFont);
        m_remoteUserInfo[id] = userId;
    }
    if (id == IDC_REMOTE_VIDEO_VIEW4)
    {
        CWnd *pStatic = GetDlgItem(IDC_STATIC_REMOTE_USERID4);
        pStatic->SetWindowTextW(UTF82Wide(userId).c_str());
        pStatic->SetFont(&newFont);
        m_remoteUserInfo[id] = userId;
    }
    if (id == IDC_REMOTE_VIDEO_VIEW5)
    {
        CWnd *pStatic = GetDlgItem(IDC_STATIC_REMOTE_USERID5);
        pStatic->SetWindowTextW(UTF82Wide(userId).c_str());
        pStatic->SetFont(&newFont);
        m_remoteUserInfo[id] = userId;
    }
    if (id == IDC_REMOTE_VIDEO_VIEW6)
    {
        CWnd *pStatic = GetDlgItem(IDC_STATIC_REMOTE_USERID6);
        pStatic->SetWindowTextW(UTF82Wide(userId).c_str());
        pStatic->SetFont(&newFont);
        m_remoteUserInfo[id] = userId;
    }
    if (id == IDC_REMOTE_VIDEO_VIEW7)
    {
        CWnd *pStatic = GetDlgItem(IDC_STATIC_REMOTE_USERID7);
        pStatic->SetWindowTextW(UTF82Wide(userId).c_str());
        pStatic->SetFont(&newFont);
        m_remoteUserInfo[id] = userId;
    }
    if (id == IDC_REMOTE_VIDEO_VIEW8)
    {
        CWnd *pStatic = GetDlgItem(IDC_STATIC_REMOTE_USERID8);
        pStatic->SetWindowTextW(UTF82Wide(userId).c_str());
        pStatic->SetFont(&newFont);
        m_remoteUserInfo[id] = userId;
    }
}

LRESULT TRTCMainViewController::OnMsgSettingViewClose(WPARAM wParam, LPARAM lParam)
{
    if (m_pTRTCSettingViewController != nullptr)
    {
        delete m_pTRTCSettingViewController;
        m_pTRTCSettingViewController = nullptr;
    }
    SetForegroundWindow();
    return LRESULT();
}

/**
* ������Ƶ���䣺��Ҫ TRTCNewViewController �ṩ��  TRTCVideoEncParam ����
*/
void TRTCMainViewController::enterRoom(TRTCParams& params)
{
    // ����ı�������������
    // ������Ƶ��������������ֱ��ʡ�֡�ʡ����ʵȵȣ���Щ������������� TRTCSettingViewController ������
    // ע�⣨1������Ҫ�����ʺܵ͵���������úܸߵķֱ��ʣ�����ֽϴ��������
    // ע�⣨2������Ҫ���ó���25FPS���ϵ�֡�ʣ���Ϊ��Ӱ��ʹ��24FPS������һ���Ƽ�15FPS�������ܽ���������ʷ��������
    TRTCVideoEncParam& encParams = TRTCStorageConfigMgr::GetInstance()->videoEncParams;
    TRTCNetworkQosParam qosParams = TRTCStorageConfigMgr::GetInstance()->qosParams;
    getTRTCCloud()->setVideoEncoderParam(encParams);
    getTRTCCloud()->setNetworkQosParam(qosParams);
    
    bool m_bPushSmallVideo = TRTCStorageConfigMgr::GetInstance()->bPushSmallVideo;
    bool m_bPlaySmallVideo = TRTCStorageConfigMgr::GetInstance()->bPlaySmallVideo;


    if (m_bPushSmallVideo)
    {
        //С����ı�������������
        //TRTC SDK ֧�ִ�С��·�����ͬʱ����ʹ��䣬�������ٲ�������û�����ѡ��ۿ�С����
        //ע�⣺iPhone & Android ��Ҫ������С˫·���棬�ǳ��˷���������С·�����ʺ� Windows �� MAC �������������绷��
        TRTCVideoEncParam param;
        param.videoFps = 15;
        param.videoBitrate = 100;
        param.videoResolution = TRTCVideoResolution_320_240;
        getTRTCCloud()->enableSmallVideoStream(true, param);
    }
    if (m_bPlaySmallVideo)
    {
        getTRTCCloud()->setPriorRemoteVideoStreamType(TRTCVideoStreamTypeSmall);
    }

    getTRTCCloud()->enterRoom(params, TRTCStorageConfigMgr::GetInstance()->appScene);
    std::string userId(params.userId);
    m_userId = userId;
    std::wstring title = format(L"TRTCDemo������ID: %d, �û�ID: %s��", params.roomId, Ansi2Wide(userId.c_str()).c_str());

    SetWindowText(title.c_str());

}

void TRTCMainViewController::OnBnClickedExitRoom()
{
    getTRTCCloud()->exitRoom();
    CWnd *pExitRoomBtn = GetDlgItem(IDC_EXIT_ROOM);
    pExitRoomBtn->EnableWindow(FALSE);
}

void TRTCMainViewController::OnBnClickedSetting()
{
    if (m_pTRTCSettingViewController == nullptr)
    {
        m_pTRTCSettingViewController = new TRTCSettingViewController(this);
        m_pTRTCSettingViewController->Create(IDD_DIALOG_SETTING, this);
        m_pTRTCSettingViewController->ShowWindow(SW_SHOW);
    }
}

void TRTCMainViewController::OnBnClickedLog()
{
    m_showDebugView++;
    int style = m_showDebugView % 3;
    if (getTRTCCloud())
        getTRTCCloud()->showDebugView(style);
}
