// Copyright 2025 Wu Zhiwei. All Rights Reserved.

#include "Classes/PythonKey.h"

#include "InputCoreTypes.h"
#include "PyConversion.h"
#include "PythonUtilities.h"

#if defined(WITH_PYTHON) && WITH_PYTHON

namespace
{
	bool SetKeyConstant(PyObject* TypeObject, const char* AttributeName, const FKey& Key)
	{
		PyObject* OutPyObject = nullptr;
		if (!PyConversion::Internal::PythonizeStructInstance(
			TBaseStructure<FKey>::Get(),
			&Key,
			OutPyObject,
			PyConversion::ESetErrorState::Yes).Succeeded())
		{
			return false;
		}

		FPythonAutoRelease PyKeyObject(OutPyObject);
		return PyObject_SetAttrString(TypeObject, AttributeName, PyKeyObject.Get()) == 0;
	}

	PyObject* KeyTypeNew(PyTypeObject* /*Type*/, PyObject* /*Args*/, PyObject* /*Kwds*/)
	{
		PyErr_SetString(PyExc_TypeError, "widget_markup.Key is a namespace of constants and cannot be instantiated.");
		return nullptr;
	}

	bool PopulateKeyConstants(PyObject* TypeObject)
	{
#define WM_KEY_CONSTANT(Name) \
		if (!SetKeyConstant(TypeObject, #Name, EKeys::Name)) \
		{ \
			return false; \
		}

		WM_KEY_CONSTANT(AnyKey)

		WM_KEY_CONSTANT(MouseX)
		WM_KEY_CONSTANT(MouseY)
		WM_KEY_CONSTANT(Mouse2D)
		WM_KEY_CONSTANT(MouseScrollUp)
		WM_KEY_CONSTANT(MouseScrollDown)
		WM_KEY_CONSTANT(MouseWheelAxis)

		WM_KEY_CONSTANT(LeftMouseButton)
		WM_KEY_CONSTANT(RightMouseButton)
		WM_KEY_CONSTANT(MiddleMouseButton)
		WM_KEY_CONSTANT(ThumbMouseButton)
		WM_KEY_CONSTANT(ThumbMouseButton2)

		WM_KEY_CONSTANT(BackSpace)
		WM_KEY_CONSTANT(Tab)
		WM_KEY_CONSTANT(Enter)
		WM_KEY_CONSTANT(Pause)
		WM_KEY_CONSTANT(CapsLock)
		WM_KEY_CONSTANT(Escape)
		WM_KEY_CONSTANT(SpaceBar)
		WM_KEY_CONSTANT(PageUp)
		WM_KEY_CONSTANT(PageDown)
		WM_KEY_CONSTANT(End)
		WM_KEY_CONSTANT(Home)

		WM_KEY_CONSTANT(Left)
		WM_KEY_CONSTANT(Up)
		WM_KEY_CONSTANT(Right)
		WM_KEY_CONSTANT(Down)

		WM_KEY_CONSTANT(Insert)
		WM_KEY_CONSTANT(Delete)

		WM_KEY_CONSTANT(Zero)
		WM_KEY_CONSTANT(One)
		WM_KEY_CONSTANT(Two)
		WM_KEY_CONSTANT(Three)
		WM_KEY_CONSTANT(Four)
		WM_KEY_CONSTANT(Five)
		WM_KEY_CONSTANT(Six)
		WM_KEY_CONSTANT(Seven)
		WM_KEY_CONSTANT(Eight)
		WM_KEY_CONSTANT(Nine)

		WM_KEY_CONSTANT(A)
		WM_KEY_CONSTANT(B)
		WM_KEY_CONSTANT(C)
		WM_KEY_CONSTANT(D)
		WM_KEY_CONSTANT(E)
		WM_KEY_CONSTANT(F)
		WM_KEY_CONSTANT(G)
		WM_KEY_CONSTANT(H)
		WM_KEY_CONSTANT(I)
		WM_KEY_CONSTANT(J)
		WM_KEY_CONSTANT(K)
		WM_KEY_CONSTANT(L)
		WM_KEY_CONSTANT(M)
		WM_KEY_CONSTANT(N)
		WM_KEY_CONSTANT(O)
		WM_KEY_CONSTANT(P)
		WM_KEY_CONSTANT(Q)
		WM_KEY_CONSTANT(R)
		WM_KEY_CONSTANT(S)
		WM_KEY_CONSTANT(T)
		WM_KEY_CONSTANT(U)
		WM_KEY_CONSTANT(V)
		WM_KEY_CONSTANT(W)
		WM_KEY_CONSTANT(X)
		WM_KEY_CONSTANT(Y)
		WM_KEY_CONSTANT(Z)

		WM_KEY_CONSTANT(NumPadZero)
		WM_KEY_CONSTANT(NumPadOne)
		WM_KEY_CONSTANT(NumPadTwo)
		WM_KEY_CONSTANT(NumPadThree)
		WM_KEY_CONSTANT(NumPadFour)
		WM_KEY_CONSTANT(NumPadFive)
		WM_KEY_CONSTANT(NumPadSix)
		WM_KEY_CONSTANT(NumPadSeven)
		WM_KEY_CONSTANT(NumPadEight)
		WM_KEY_CONSTANT(NumPadNine)

		WM_KEY_CONSTANT(Multiply)
		WM_KEY_CONSTANT(Add)
		WM_KEY_CONSTANT(Subtract)
		WM_KEY_CONSTANT(Decimal)
		WM_KEY_CONSTANT(Divide)

		WM_KEY_CONSTANT(F1)
		WM_KEY_CONSTANT(F2)
		WM_KEY_CONSTANT(F3)
		WM_KEY_CONSTANT(F4)
		WM_KEY_CONSTANT(F5)
		WM_KEY_CONSTANT(F6)
		WM_KEY_CONSTANT(F7)
		WM_KEY_CONSTANT(F8)
		WM_KEY_CONSTANT(F9)
		WM_KEY_CONSTANT(F10)
		WM_KEY_CONSTANT(F11)
		WM_KEY_CONSTANT(F12)

		WM_KEY_CONSTANT(NumLock)
		WM_KEY_CONSTANT(ScrollLock)

		WM_KEY_CONSTANT(LeftShift)
		WM_KEY_CONSTANT(RightShift)
		WM_KEY_CONSTANT(LeftControl)
		WM_KEY_CONSTANT(RightControl)
		WM_KEY_CONSTANT(LeftAlt)
		WM_KEY_CONSTANT(RightAlt)
		WM_KEY_CONSTANT(LeftCommand)
		WM_KEY_CONSTANT(RightCommand)

		WM_KEY_CONSTANT(Semicolon)
		WM_KEY_CONSTANT(Equals)
		WM_KEY_CONSTANT(Comma)
		WM_KEY_CONSTANT(Underscore)
		WM_KEY_CONSTANT(Hyphen)
		WM_KEY_CONSTANT(Period)
		WM_KEY_CONSTANT(Slash)
		WM_KEY_CONSTANT(Tilde)
		WM_KEY_CONSTANT(LeftBracket)
		WM_KEY_CONSTANT(Backslash)
		WM_KEY_CONSTANT(RightBracket)
		WM_KEY_CONSTANT(Apostrophe)

		WM_KEY_CONSTANT(Ampersand)
		WM_KEY_CONSTANT(Asterix)
		WM_KEY_CONSTANT(Caret)
		WM_KEY_CONSTANT(Colon)
		WM_KEY_CONSTANT(Dollar)
		WM_KEY_CONSTANT(Exclamation)
		WM_KEY_CONSTANT(LeftParantheses)
		WM_KEY_CONSTANT(RightParantheses)
		WM_KEY_CONSTANT(Quote)

		WM_KEY_CONSTANT(A_AccentGrave)
		WM_KEY_CONSTANT(E_AccentGrave)
		WM_KEY_CONSTANT(E_AccentAigu)
		WM_KEY_CONSTANT(C_Cedille)
		WM_KEY_CONSTANT(Section)

		WM_KEY_CONSTANT(Platform_Delete)

		WM_KEY_CONSTANT(Gamepad_Left2D)
		WM_KEY_CONSTANT(Gamepad_LeftX)
		WM_KEY_CONSTANT(Gamepad_LeftY)
		WM_KEY_CONSTANT(Gamepad_Right2D)
		WM_KEY_CONSTANT(Gamepad_RightX)
		WM_KEY_CONSTANT(Gamepad_RightY)
		WM_KEY_CONSTANT(Gamepad_LeftTriggerAxis)
		WM_KEY_CONSTANT(Gamepad_RightTriggerAxis)
		WM_KEY_CONSTANT(Gamepad_LeftThumbstick)
		WM_KEY_CONSTANT(Gamepad_RightThumbstick)
		WM_KEY_CONSTANT(Gamepad_Special_Left)
		WM_KEY_CONSTANT(Gamepad_Special_Left_X)
		WM_KEY_CONSTANT(Gamepad_Special_Left_Y)
		WM_KEY_CONSTANT(Gamepad_Special_Right)
		WM_KEY_CONSTANT(Gamepad_FaceButton_Bottom)
		WM_KEY_CONSTANT(Gamepad_FaceButton_Right)
		WM_KEY_CONSTANT(Gamepad_FaceButton_Left)
		WM_KEY_CONSTANT(Gamepad_FaceButton_Top)
		WM_KEY_CONSTANT(Gamepad_LeftShoulder)
		WM_KEY_CONSTANT(Gamepad_RightShoulder)
		WM_KEY_CONSTANT(Gamepad_LeftTrigger)
		WM_KEY_CONSTANT(Gamepad_RightTrigger)
		WM_KEY_CONSTANT(Gamepad_DPad_Up)
		WM_KEY_CONSTANT(Gamepad_DPad_Down)
		WM_KEY_CONSTANT(Gamepad_DPad_Right)
		WM_KEY_CONSTANT(Gamepad_DPad_Left)
		WM_KEY_CONSTANT(Gamepad_LeftStick_Up)
		WM_KEY_CONSTANT(Gamepad_LeftStick_Down)
		WM_KEY_CONSTANT(Gamepad_LeftStick_Right)
		WM_KEY_CONSTANT(Gamepad_LeftStick_Left)
		WM_KEY_CONSTANT(Gamepad_RightStick_Up)
		WM_KEY_CONSTANT(Gamepad_RightStick_Down)
		WM_KEY_CONSTANT(Gamepad_RightStick_Right)
		WM_KEY_CONSTANT(Gamepad_RightStick_Left)

		WM_KEY_CONSTANT(Tilt)
		WM_KEY_CONSTANT(RotationRate)
		WM_KEY_CONSTANT(Gravity)
		WM_KEY_CONSTANT(Acceleration)

		WM_KEY_CONSTANT(Gesture_Pinch)
		WM_KEY_CONSTANT(Gesture_Flick)
		WM_KEY_CONSTANT(Gesture_Rotate)

		WM_KEY_CONSTANT(Steam_Touch_0)
		WM_KEY_CONSTANT(Steam_Touch_1)
		WM_KEY_CONSTANT(Steam_Touch_2)
		WM_KEY_CONSTANT(Steam_Touch_3)
		WM_KEY_CONSTANT(Steam_Back_Left)
		WM_KEY_CONSTANT(Steam_Back_Right)

		WM_KEY_CONSTANT(Global_Menu)
		WM_KEY_CONSTANT(Global_View)
		WM_KEY_CONSTANT(Global_Pause)
		WM_KEY_CONSTANT(Global_Play)
		WM_KEY_CONSTANT(Global_Back)

		WM_KEY_CONSTANT(Android_Back)
		WM_KEY_CONSTANT(Android_Volume_Up)
		WM_KEY_CONSTANT(Android_Volume_Down)
		WM_KEY_CONSTANT(Android_Menu)

#undef WM_KEY_CONSTANT
		return true;
	}
}

bool RegisterPythonKeyType(PyObject* Module)
{
	PyType_Slot TypeSlots[] = {
		{ Py_tp_new, reinterpret_cast<void*>(KeyTypeNew) },
		{ Py_tp_doc, const_cast<char*>("Well-known FKey constants from EKeys (keyboard, mouse, gamepad, platform).") },
		{ 0, nullptr },
	};

	PyType_Spec TypeSpec = {
		"widget_markup.Key",
		0,
		0,
		Py_TPFLAGS_DEFAULT,
		TypeSlots,
	};

	FPythonAutoRelease TypeObject(PyType_FromSpec(&TypeSpec));
	if (!TypeObject)
	{
		return false;
	}

	if (!PopulateKeyConstants(TypeObject.Get()))
	{
		return false;
	}

	return PyModule_AddObject(Module, "Key", TypeObject.Release()) == 0;
}

#else

bool RegisterPythonKeyType(PyObject* /*Module*/)
{
	return false;
}

#endif
