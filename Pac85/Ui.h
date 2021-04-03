#ifndef PAC85_UI_H
#define PAC85_UI_H

/*!
 * IDs of the window used inside the program.
 */
enum EWindowID
{
	WINDOWID_Invalid,
	WINDOWID_Main,
	WINDOWID_Welcome,
	WINDOWID_DetectMirror,
};

/*!
 * Basic component of a Ui element, it must be implemented in every
 * component of a window/dialog.
 */
struct SBasicComponent
{
	UINT nId; /*! ID of the component @see EWindowID */
	HWND hWnd; /*! Handle of the component window */
};

/*!
 * Contains all the elements of the Ui.
 */
struct SUi
{
	HINSTANCE hInstance; /*! instance of this application */
	struct SBasicComponent* psActiveDlg; /*! Current active dialog */
	struct SBasicComponent* psBackgroundWin; /*! Main window/background component */
};

/*!
 * Ui window loop
 * @param psUi a pointer to an Ui structure
 * @return exit code of the application
 */
extern INT UiLoop(_In_ struct SUi* psUi);

/*!
 * Switches the current dialog to another (or creates one if no dialog is active)
 * @param psUi a pointer to an Ui structure
 * @param nId the id of the dialog @see EWindowID
 * @return result codes
 */
extern HRESULT UiSwitchToDialog(_In_ struct SUi* psUi, _In_ UINT nId);

/*!
 * Deletes the current active dialog
 * @param psUi a pointer to an Ui structure
 * @return result codes
 */
extern HRESULT UiDeleteActiveDialog(_In_ struct SUi* psUi);

/*!
 * Creates a new Ui structure
 * @param hInstance instance of the application
 * @param pOut output of the structure, if the function is not succedded pOut might be NULL or a garbage pointer
 * @return result codes
 */
extern HRESULT UiCreate(_In_ HINSTANCE hInstance, _Out_ struct SUi** pOut);

/*!
 * Deletes and frees the component of an Ui structure
 * @param psUi pointer to a structure to get deleted
 * @return result codes
 */
extern HRESULT UiDelete(_In_ struct SUi* psUi);

#endif /* PAC85_UI_H */
