
#ifndef __WINDOW_FORM_UNIT_H__
#define __WINDOW_FORM_UNIT_H__

#include "tjsCommHead.h"
#include "tvpinputdefs.h"
#include "WindowIntf.h"

#include "TMLWindow.h"
#include "MouseCursor.h"
#include "TouchPoint.h"

enum {
	crDefault = 0x0,
	crNone = -1,
	crArrow = -2,
	crCross = -3,
	crIBeam = -4,
	crSize = -5,
	crSizeNESW = -6,
	crSizeNS = -7,
	crSizeNWSE = -8,
	crSizeWE = -9,
	crUpArrow = -10,
	crHourGlass = -11,
	crDrag = -12,
	crNoDrop = -13,
	crHSplit = -14,
	crVSplit = -15,
	crMultiDrag = -16,
	crSQLWait = -17,
	crNo = -18,
	crAppStart = -19,
	crHelp = -20,
	crHandPoint = -21,
	crSizeAll = -22,
	crHBeam = 1,
};
/*
enum TImeMode {
	imDisable,
	imClose,
	imOpen,
	imDontCare,
	imSAlpha,
	imAlpha,
	imHira,
	imSKata,
	imKata,
	imChinese,
	imSHanguel,
	imHanguel
};
*/
typedef unsigned long TShiftState;
//---------------------------------------------------------------------------
// Options
//---------------------------------------------------------------------------
extern void TVPInitWindowOptions();
extern int TVPFullScreenBPP; // = 0; // 0 for no-change
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// VCL-based constants to TVP-based constants conversion (and vice versa)
//---------------------------------------------------------------------------
tTVPMouseButton TVP_TMouseButton_To_tTVPMouseButton(int button);
tjs_uint32 TVP_TShiftState_To_uint32(TShiftState state);
TShiftState TVP_TShiftState_From_uint32(tjs_uint32 state);
tjs_uint32 TVPGetCurrentShiftKeyState();
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// TTVPWindowForm
//---------------------------------------------------------------------------
#define TVP_WM_SHOWVISIBLE (WM_USER + 2)
#define TVP_WM_SHOWTOP     (WM_USER + 3)
#define TVP_WM_RETRIEVEFOCUS     (WM_USER + 4)
#define TVP_WM_ACQUIREIMECONTROL    (WM_USER + 5)
extern void TVPShowModalAtAppActivate();
extern void TVPHideModalAtAppDeactivate();
extern HDWP TVPShowModalAtTimer(HDWP);
class TTVPWindowForm;
extern TTVPWindowForm * TVPFullScreenedWindow;
//---------------------------------------------------------------------------
struct tTVPMessageReceiverRecord
{
	tTVPWindowMessageReceiver Proc;
	const void *UserData;
	bool Deliver(tTVPWindowMessage *Message)
	{ return Proc(const_cast<void*>(UserData), Message); }	
};
class tTJSNI_Window;
struct tTVPRect;
class tTVPBaseBitmap;
class tTVPWheelDirectInputDevice; // class for DirectInputDevice management
class tTVPPadDirectInputDevice; // class for DirectInputDevice management

class TTVPWindowForm : public TML::Window, public TouchHandler {
	static const int TVP_MOUSE_MAX_ACCEL = 30;
	static const int TVP_MOUSE_SHIFT_ACCEL = 40;
private:

	bool InMode;
	//bool Focusable;

	//-- drawdevice related
	bool NextSetWindowHandleToDrawDevice;
	tTVPRect LastSentDrawDeviceDestRect;
	
	//-- interface to plugin
	tObjectList<tTVPMessageReceiverRecord> WindowMessageReceivers;
	
	//-- DirectInput related
	tTVPWheelDirectInputDevice *DIWheelDevice;
	tTVPPadDirectInputDevice *DIPadDevice;
	bool ReloadDevice;
	DWORD ReloadDeviceTick;

	//-- TJS object related
	tTJSNI_Window * TJSNativeInstance;
	int LastMouseDownX, LastMouseDownY; // in Layer coodinates
	
	POINT LastMouseMovedPos;  // in Layer coodinates
	//-- full screen managemant related
	int InnerWidthSave;
	int InnerHeightSave;
	DWORD OrgStyle;
	DWORD OrgExStyle;
	int OrgLeft;
	int OrgTop;
	int OrgWidth;
	int OrgHeight;
	
	//-- keyboard input
	std::string PendingKeyCodes;
	
	tTVPImeMode LastSetImeMode;
	tTVPImeMode DefaultImeMode;

	bool TrapKeys;
	bool CanReceiveTrappedKeys;
	bool InReceivingTrappedKeys;
	bool UseMouseKey; // whether using mouse key emulation
	tjs_int MouseKeyXAccel;
	tjs_int MouseKeyYAccel;
	bool LastMouseMoved;
	bool MouseLeftButtonEmulatedPushed;
	bool MouseRightButtonEmulatedPushed;
	DWORD LastMouseKeyTick;
	
	bool AttentionPointEnabled;
	POINT AttentionPoint;
	TFont *AttentionFont;

	//-- mouse cursor
	tTVPMouseCursorState MouseCursorState;
	bool ForceMouseCursorVisible; // true in menu select
	MouseCursor CurrentMouseCursor;
	tjs_int LastMouseScreenX; // managed by RestoreMouseCursor
	tjs_int LastMouseScreenY;

	//-- layer position / size
	tjs_int LayerLeft;
	tjs_int LayerTop;
	tjs_int LayerWidth;
	tjs_int LayerHeight;
	tjs_int ZoomDenom; // Zooming factor denominator (setting)
	tjs_int ZoomNumer; // Zooming factor numerator (setting)
	tjs_int ActualZoomDenom; // Zooming factor denominator (actual)
	tjs_int ActualZoomNumer; // Zooming factor numerator (actual)
	
	DWORD LastRecheckInputStateSent;

	TouchPointList touch_points_;

private:
	void SetDrawDeviceDestRect();
	void TranslateWindowToPaintBox(int &x, int &y);

	void FirePopupHide();
	bool CanSendPopupHide() const { return /*!Focusable &&*/ GetVisible() && GetStayOnTop(); }
	
	void RestoreMouseCursor();
	void SetMouseCursorVisibleState(bool b);
	void SetForceMouseCursorVisible(bool s);
	
	void InternalSetPaintBoxSize();

	void CallWindowDetach(bool close);
	void CallWindowAttach();
	
	bool InternalDeliverMessageToReceiver(tTVPWindowMessage &msg);
	bool DeliverMessageToReceiver(tTVPWindowMessage &msg) {
		if( WindowMessageReceivers.GetCount() )
			return InternalDeliverMessageToReceiver(msg);
		else
			return false;
	}
	void GenerateMouseEvent(bool fl, bool fr, bool fu, bool fd);

	void UnacquireImeControl();
	void AcquireImeControl();
	
	TTVPWindowForm * GetKeyTrapperWindow();

	int ConvertImeMode( tTVPImeMode mode );
	void OffsetClientPoint( int &x, int &y );
	
	static bool FindKeyTrapper(LRESULT &result, UINT msg, WPARAM wparam, LPARAM lparam);
	bool ProcessTrappedKeyMessage(LRESULT &result, UINT msg, WPARAM wparam, LPARAM lparam);
protected:
	LRESULT WINAPI Proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );

public:
	TTVPWindowForm( class TApplication* app, tTJSNI_Window* ni );
	virtual ~TTVPWindowForm();
	
	static void DeliverPopupHide();

	//-- properties
	tstring GetCaption() const {
		tstring ret;
		TML::Window::GetCaption( ret );
		return ret;
	}

/*

	void SetShowScrollBars(bool b){}
	bool GetShowScrollBars() const{return false;}
*/

	void SetFullScreen(bool b){}
	bool GetFullScreen() const{return false;}

	void SetUseMouseKey(bool b);
	bool GetUseMouseKey() const;

	void SetTrapKey(bool b);
	bool GetTrapKey() const;

/*
	void SetMaskRegion(HRGN threshold){}
	void RemoveMaskRegion(){}
*/
	void SetMouseCursorToWindow( MouseCursor& cursor );

	void HideMouseCursor();
	void SetMouseCursorState(tTVPMouseCursorState mcs);
    tTVPMouseCursorState GetMouseCursorState() const { return MouseCursorState; }

	/*
	void SetFocusable(bool b){}
	bool GetFocusable() { return Focusable; }
	*/
	
	void AdjustNumerAndDenom(tjs_int &n, tjs_int &d);
	void SetZoom(tjs_int numer, tjs_int denom, bool set_logical = true);
	void SetZoomNumer( tjs_int n ) { SetZoom(n, ZoomDenom); }
	tjs_int GetZoomNumer() const { return ZoomNumer; }
	void SetZoomDenom(tjs_int d) { SetZoom(ZoomNumer, d); }
	tjs_int GetZoomDenom() const { return ZoomDenom; }
	
	//-- full screen management
	void SetFullScreenMode(bool b);
	bool GetFullScreenMode() const;

	//-- methods/properties
	//void BeginMove(){}
	void UpdateWindow(tTVPUpdateType type = utNormal);
	void ShowWindowAsModal();

	void RegisterWindowMessageReceiver(tTVPWMRRegMode mode, void * proc, const void *userdata);

	//-- close action related
	bool Closing;
	bool ProgramClosing;
	bool CanCloseWork;
	void Close();
	void InvalidateClose();
	void OnCloseQueryCalled(bool b);
	void SendCloseMessage();
	
	void ZoomRectangle( tjs_int & left, tjs_int & top, tjs_int & right, tjs_int & bottom);

	HWND GetSurfaceWindowHandle();
	HWND GetWindowHandle(tjs_int &ofsx, tjs_int &ofsy);
	HWND GetWindowHandleForPlugin();

	//-- form mode
	bool GetFormEnabled();
	void TickBeat(); // called every 50ms intervally
	bool GetWindowActive();

	//-- draw device
	void ResetDrawDevice();

	void InternalKeyUp(WORD key, tjs_uint32 shift);
	void InternalKeyDown(WORD key, tjs_uint32 shift);

	
	void SetPaintBoxSize(tjs_int w, tjs_int h);

	void SetDefaultMouseCursor();
	void SetMouseCursor(tjs_int handle);

	void GetCursorPos(tjs_int &x, tjs_int &y);
	void SetCursorPos(tjs_int x, tjs_int y);

	// void SetHintText(const ttstr &text){}

	void SetImeMode(tTVPImeMode mode);
	void SetDefaultImeMode(tTVPImeMode mode, bool reset);
	tTVPImeMode GetDefaultImeMode() const { return  DefaultImeMode; }
	void ResetImeMode();
	
	void SetAttentionPoint(tjs_int left, tjs_int top, class TFont *font);
	void DisableAttentionPoint();
	
	void InvokeShowVisible();
	void InvokeShowTop(bool activate = true);
	HDWP ShowTop(HDWP hdwp);

	//-- DirectInput related
	void CreateDirectInputDevice();
	void FreeDirectInputDevice();

	// message hander
	virtual void OnActive( HWND preactive );
	virtual void OnDeactive( HWND postactive );

	virtual void OnKeyDown( WORD vk, int shift, int repreat, bool prevkeystate );
	virtual void OnKeyUp( WORD vk, int shift );
	virtual void OnKeyPress( WORD vk, int repreat, bool prevkeystate, bool convertkey );

	virtual void OnPaint();
	virtual void OnClose( CloseAction& action );
	virtual void OnMouseDown( int button, int shift, int x, int y );
	virtual void OnMouseUp( int button, int shift, int x, int y );
	virtual void OnMouseMove( int shift, int x, int y );
	virtual void OnMouseDoubleClick( int button, int x, int y );
	virtual void OnMouseClick( int button, int shift, int x, int y );
	virtual void OnMouseWheel( int delta, int shift, int x, int y );

	virtual void OnMove( int x, int y );
	virtual void OnDropFile( HDROP hDrop );
	// virtual int OnMouseActivate( HWND hTopLevelParentWnd, WORD hitTestCode, WORD MouseMsg );
	virtual void OnEnable( bool enabled );
	//virtual void OnEnterMenuLoop( bool entered );
	//virtual void OnExitMenuLoop( bool isShortcutMenu );
	virtual void OnDeviceChange( int event, void *data );
	virtual void OnNonClientMouseDown( int button, int hittest, int x, int y );
	virtual void OnMouseEnter();
	virtual void OnMouseLeave();
	virtual void OnShow( int status );
	virtual void OnHide( int status );
	
	virtual void OnFocus(HWND hFocusLostWnd);
	virtual void OnFocusLost(HWND hFocusingWnd);
	
	virtual void OnTouchDown( double x, double y, double cx, double cy, DWORD id );
	virtual void OnTouchMove( double x, double y, double cx, double cy, DWORD id );
	virtual void OnTouchUp( double x, double y, double cx, double cy, DWORD id );

	virtual void OnTouchScaling( double startdist, double currentdist, double cx, double cy, int flag );
	virtual void OnTouchRotate( double startangle, double currentangle, double distance, double cx, double cy, int flag );
	virtual void OnMultiTouch();

	void WMShowVisible();
	void WMShowTop( WPARAM wParam );
	void WMRetrieveFocus();
	void WMAcquireImeControl();

	void SetTouchScaleThreshold( double threshold ) {
		touch_points_.SetScaleThreshold( threshold );
	}
	double GetTouchScaleThreshold() const {
		return touch_points_.GetScaleThreshold();
	}
	void SetTouchRotateThreshold( double threshold ) {
		touch_points_.SetRotateThreshold( threshold );
	}
	double GetTouchRotateThreshold() const {
		return touch_points_.GetRotateThreshold();
	}
	tjs_real GetTouchPointStartX( tjs_int index ) const { return touch_points_.GetStartX(index); }
	tjs_real GetTouchPointStartY( tjs_int index ) const { return touch_points_.GetStartY(index); }
	tjs_real GetTouchPointX( tjs_int index ) const { return touch_points_.GetX(index); }
	tjs_real GetTouchPointY( tjs_int index ) const { return touch_points_.GetY(index); }
	tjs_int GetTouchPointID( tjs_int index ) const { return touch_points_.GetID(index); }
	tjs_int GetTouchPointCount() const { return touch_points_.CountUsePoint(); }
};

#endif // __WINDOW_FORM_UNIT_H__
