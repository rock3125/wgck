#ifndef _DIALOGS_CVDIALOG_H_
#define _DIALOGS_CVDIALOG_H_

//==========================================================================

class TEvent;

//==========================================================================

class _EXPORT CustomiseVehicle
{
public:
	CustomiseVehicle( TEvent*, HINSTANCE, HWND );
	~CustomiseVehicle( void );

	bool Execute( size_t maxPoints );

	bool Initialised( void ) const;
	void Initialised( bool i );

private:
	void UpdateInfo( void );

private:
#ifdef _USEGUI
	friend class HostDialog;
	friend class JoinDialog;
#else
	friend LRESULT CALLBACK HostDlgProc( HWND, UINT, WPARAM, LPARAM );
	friend LRESULT CALLBACK JoingDlgProc( HWND, UINT, WPARAM, LPARAM );

	friend LRESULT CALLBACK CustomiseVehicleDlgProc( HWND, UINT, WPARAM, LPARAM );
	static CustomiseVehicle* ptr;
#endif

	TEvent*		app;
	HINSTANCE	hInstance;
	HWND		parent;

	size_t	max;
	size_t	speed;
	size_t	range;
	size_t	strength;
	size_t	amount;
	bool	submerge;

	bool initialised;

#ifdef _USEGUI
	TGUI*	customisePage;
	TGUI*	info1Label;
	TGUI*	info2Label;
	TGUI*	info3Label;
	TGUI*	info4Label;
	TGUI*	info5Label;
	TGUI*	pointsLabel;
	TGUI*	submergeCheckBox;
	TGUI*	speedSlider;
	TGUI*	rangeSlider;
	TGUI*	strengthSlider;
	TGUI*	ammoSlider;
	TGUI*	okButton;
	TGUI*	cancelButton;
#else
	HWND hPoints;
	HWND hSubmerge;
	HWND hSpeed;
	HWND hRange;
	HWND hStrength;
	HWND hAmount;
	HWND hOk;

	HWND hInfo1;
	HWND hInfo2;
	HWND hInfo3;
	HWND hInfo4;
	HWND hInfo5;
#endif
};

//==========================================================================

#endif
