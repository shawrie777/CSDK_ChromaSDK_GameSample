#include <iostream>

#include "HandleInput.h"
#include "Razer\ChromaSDKPluginTypes.h"
#include "Razer\ChromaAnimationAPI.h"

using namespace std;
using namespace ChromaSDK;

int _gSelection = 1;
const int MAX_SELECTION = 15;
char _gManualInput[] = {
	' ',
	' ',
};

static void PrintLegend()
{
	cout << "\033[15;1H\033[0J";
	
	cout << "C++ CHROMA GAME SAMPLE APP" << endl << endl;
	cout << "Use UP and DOWN arrows to select animation and press ENTER." << endl;
	cout << "Use ESCAPE to QUIT." << endl;

	for (int effect = 1; effect <= MAX_SELECTION; ++effect)
	{
		cout << "[" << (effect == _gSelection ? "*" : to_string(effect)) << "] Effect " << effect << (effect % 5 ? "\t\t" : "\r\n");
	}
	cout << endl << endl << endl;
	cout << "[" << (_gSelection > 0 ? to_string(_gSelection) : " ") << "] Press ENTER to play selection." << endl;

	if (_gManualInput[0] != ' ')
	{
		cout << _gManualInput[0];
		if (_gManualInput[1] != ' ')
		{
			cout << _gManualInput[1];
		}
		cout << endl;
	}
	else {
		cout << "\033[2K";
	}
}

static void ClearManualInput()
{
	_gManualInput[0] = ' ';
	_gManualInput[1] = ' ';
}

void ExecuteEffect();

static void Cleanup()
{
	if (ChromaAnimationAPI::IsInitialized())
	{
		ChromaAnimationAPI::UseIdleAnimations(false);
		ChromaAnimationAPI::StopAll();
		ChromaAnimationAPI::CloseAll();
		RZRESULT result = ChromaAnimationAPI::Uninit();
		ChromaAnimationAPI::UninitAPI();
		if (result != RZRESULT_SUCCESS)
		{
			cerr << "Failed to uninitialize Chroma! Result=" << result << endl;
			exit(1);
		}
	}
}

int main()
{
	ChromaSDK::APPINFOTYPE appInfo = {};

	wcscpy_s(appInfo.Title, 256, L"Game Sample: Application");
	wcscpy_s(appInfo.Description, 1024, L"A sample application using Razer Chroma SDK");
	wcscpy_s(appInfo.Author.Name, 256, L"Razer");
	wcscpy_s(appInfo.Author.Contact, 256, L"https://wyvrn.com");

	//appInfo.SupportedDevice = 
	//    0x01 | // Keyboards
	//    0x02 | // Mice
	//    0x04 | // Headset
	//    0x08 | // Mousepads
	//    0x10 | // Keypads
	//    0x20   // ChromaLink devices
	appInfo.SupportedDevice = (0x01 | 0x02 | 0x04 | 0x08 | 0x10 | 0x20);
	//    0x01 | // Utility. (To specifiy this is an utility application)
	//    0x02   // Game. (To specifiy this is a game);
	appInfo.Category = 1;

	RZRESULT result = ChromaAnimationAPI::InitSDK(&appInfo);
	if (result != RZRESULT_SUCCESS)
	{
		cerr << "Failed to initialize Chroma!" << endl;
		ChromaAnimationAPI::UninitAPI();
		exit(1);
	}
	Sleep(100); //wait for init

	// Manually set event names
	ChromaAnimationAPI::UseForwardChromaEvents(false);

	HandleInput numKeys[] =
	{
		HandleInput('0'),
		HandleInput('1'),
		HandleInput('2'),
		HandleInput('3'),
		HandleInput('4'),
		HandleInput('5'),
		HandleInput('6'),
		HandleInput('7'),
		HandleInput('8'),
		HandleInput('9'),
	};

	HandleInput numpadKeys[] =
	{
		HandleInput(VK_NUMPAD0),
		HandleInput(VK_NUMPAD1),
		HandleInput(VK_NUMPAD2),
		HandleInput(VK_NUMPAD3),
		HandleInput(VK_NUMPAD4),
		HandleInput(VK_NUMPAD5),
		HandleInput(VK_NUMPAD6),
		HandleInput(VK_NUMPAD7),
		HandleInput(VK_NUMPAD8),
		HandleInput(VK_NUMPAD9),
	};

	PrintLegend();
	HandleInput inputUp = HandleInput(VK_UP);
	HandleInput inputDown = HandleInput(VK_DOWN);
	HandleInput inputBackspace = HandleInput(VK_BACK);
	HandleInput inputEnter = HandleInput(VK_RETURN);
	HandleInput inputEscape = HandleInput(VK_ESCAPE);

	while (true)
	{
		if (inputEscape.WasReleased(true))
		{
			ChromaAnimationAPI::StopAll();
			ChromaAnimationAPI::CloseAll();
			ChromaAnimationAPI::ClearAll();
			ChromaAnimationAPI::Uninit();
			break;
		}
		else if (inputUp.WasReleased(true))
		{
			ClearManualInput();
			if (_gSelection > 1)
			{
				--_gSelection;
			}
			PrintLegend();
			if (_gSelection >= 1)
			{
				ExecuteEffect();
			}
		}


		else if (inputDown.WasReleased(true))
		{
			ClearManualInput();
			if (_gSelection < MAX_SELECTION)
			{
				_gSelection++;
			}
			PrintLegend();
			if (_gSelection > 0)
			{
				ExecuteEffect();
			}
		}

		bool hasManualInput = false;
		if (inputBackspace.WasReleased(true))
		{
			if (_gManualInput[1] != ' ')
			{
				_gManualInput[1] = ' ';
				hasManualInput = true;

			}
			else if (_gManualInput[0] != ' ')
			{
				_gManualInput[0] = ' ';
				hasManualInput = true;
			}
		}
		for (int i = 0; i < (int)size(numKeys); ++i)
		{
			if (numKeys[i].WasReleased(true) ||
				numpadKeys[i].WasReleased(true))
			{
				if (_gManualInput[0] != ' ')
				{
					_gManualInput[1] = numKeys[i].GetKey();
					hasManualInput = true;
				}
				else
				{
					_gManualInput[0] = numKeys[i].GetKey();
					hasManualInput = true;
				}
			}
		}
		if (hasManualInput)
		{
			int val = 0;
			if (_gManualInput[1] != ' ')
			{
				if (_gManualInput[0] >= '0' && _gManualInput[0] <= '9')
				{
					val = 10 * (_gManualInput[0] - '0');
				}
				if (_gManualInput[1] >= '0' && _gManualInput[1] <= '9')
				{
					val += _gManualInput[1] - '0';
				}
			}
			else if (_gManualInput[0] != ' ')
			{
				val = _gManualInput[0] - '0';
			}
			if (val <= MAX_SELECTION)
			{
				_gSelection = val;
			}
			PrintLegend();
		}

		if (inputEnter.WasReleased(true))
		{
			PrintLegend();
			ClearManualInput();

			ExecuteEffect();

			if (_gSelection < 1)
			{
				PrintLegend();
			}
		}
		Sleep(1);
	}

	Cleanup();

	return 0;
}

void ExecuteEffect()
{
	wchar_t buffer[9];
	swprintf(buffer, 30, L"Effect%d", _gSelection);
	ChromaAnimationAPI::CoreSetEventName(buffer);
}