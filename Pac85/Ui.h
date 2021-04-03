#ifndef PAC85_UI_H
#define PAC85_UI_H

enum EWindowID
{
	WINDOWID_Invalid,
	WINDOWID_Main,
	WINDOWID_Welcome,
	WINDOWID_DetectMirror,
};

struct SBasicComponent
{
	UINT nId;
	HWND hWnd;
};

struct SUi
{
	HINSTANCE hInstance;
	struct SBasicComponent* psActiveDlg;
	struct SBasicComponent* psBackgroundWin;
};

extern INT UiLoop(_In_ struct SUi* psUi);

extern HRESULT UiSwitchToDialog(_In_ struct SUi* psUi, _In_ UINT nId);
extern HRESULT UiDeleteActiveDialog(_In_ struct SUi* psUi);

extern HRESULT UiCreate(_In_ HINSTANCE hInstance, _Out_ struct SUi** pOut);
extern HRESULT UiDelete(_In_ struct SUi* psUi);

#endif /* PAC85_UI_H */
