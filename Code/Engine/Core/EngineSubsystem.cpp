#include "Engine/Core/EngineSubsystem.hpp"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

EngineSubsystem::EngineSubsystem() {
    /* DO NOTHING */
}

EngineSubsystem::~EngineSubsystem() {
    /* DO NOTHING */
}

void EngineSubsystem::Initialize() {
    /* DO NOTHING */
}

void EngineSubsystem::BeginFrame() {
    /* DO NOTHING */
}

void EngineSubsystem::Update(float /*deltaSeconds*/) {
    /* DO NOTHING */
}

void EngineSubsystem::Render() const {
    /* DO NOTHING */
}

void EngineSubsystem::EndFrame() {
    /* DO NOTHING */
}
WindowsSystemMessage EngineSubsystem::GetWindowsSystemMessageFromUintMessage(unsigned int wmMessage) {
    switch(wmMessage) {
        case WM_CLEAR:                  return WindowsSystemMessage::CLIPBOARD_CLEAR;
        case WM_COPY:                   return WindowsSystemMessage::CLIPBOARD_COPY;
        case WM_CUT:                    return WindowsSystemMessage::CLIPBOARD_CUT;
        case WM_PASTE:                  return WindowsSystemMessage::CLIPBOARD_PASTE;
        case WM_ASKCBFORMATNAME:        return WindowsSystemMessage::CLIPBOARD_ASKCBFORMATNAME;
        case WM_CHANGECBCHAIN:          return WindowsSystemMessage::CLIPBOARD_CHANGECBCHAIN;
        case WM_CLIPBOARDUPDATE:        return WindowsSystemMessage::CLIPBOARD_CLIPBOARDUPDATE;
        case WM_DESTROYCLIPBOARD:       return WindowsSystemMessage::CLIPBOARD_DESTROYCLIPBOARD;
        case WM_DRAWCLIPBOARD:          return WindowsSystemMessage::CLIPBOARD_DRAWCLIPBOARD;
        case WM_HSCROLLCLIPBOARD:       return WindowsSystemMessage::CLIPBOARD_HSCROLLCLIPBOARD;
        case WM_PAINTCLIPBOARD:         return WindowsSystemMessage::CLIPBOARD_PAINTCLIPBOARD;
        case WM_RENDERALLFORMATS:       return WindowsSystemMessage::CLIPBOARD_RENDERALLFORMATS;
        case WM_RENDERFORMAT:           return WindowsSystemMessage::CLIPBOARD_RENDERFORMAT;
        case WM_SIZECLIPBOARD:          return WindowsSystemMessage::CLIPBOARD_SIZECLIPBOARD;
        case WM_VSCROLLCLIPBOARD:       return WindowsSystemMessage::CLIPBOARD_VSCROLLCLIPBOARD;
        case WM_SETCURSOR:              return WindowsSystemMessage::CURSOR_SETCURSOR;
        case WM_ACTIVATE:               return WindowsSystemMessage::KEYBOARD_ACTIVATE;
        case WM_APPCOMMAND:             return WindowsSystemMessage::KEYBOARD_APPCOMMAND;
        case WM_CHAR:                   return WindowsSystemMessage::KEYBOARD_CHAR;
        case WM_DEADCHAR:               return WindowsSystemMessage::KEYBOARD_DEADCHAR;
        case WM_HOTKEY:                 return WindowsSystemMessage::KEYBOARD_HOTKEY;
        case WM_KEYDOWN:                return WindowsSystemMessage::KEYBOARD_KEYDOWN;
        case WM_KEYUP:                  return WindowsSystemMessage::KEYBOARD_KEYUP;
        case WM_KILLFOCUS:              return WindowsSystemMessage::KEYBOARD_KILLFOCUS;
        case WM_SETFOCUS:               return WindowsSystemMessage::KEYBOARD_SETFOCUS;
        case WM_SYSDEADCHAR:            return WindowsSystemMessage::KEYBOARD_SYSDEADCHAR;
        case WM_SYSKEYDOWN:             return WindowsSystemMessage::KEYBOARD_SYSKEYDOWN;
        case WM_SYSKEYUP:               return WindowsSystemMessage::KEYBOARD_SYSKEYUP;
        case WM_UNICHAR:                return WindowsSystemMessage::KEYBOARD_UNICHAR;
        case WM_CAPTURECHANGED:         return WindowsSystemMessage::MOUSE_CAPTURECHANGED;
        case WM_LBUTTONDBLCLK:          return WindowsSystemMessage::MOUSE_LBUTTONDBLCLK;
        case WM_LBUTTONDOWN:            return WindowsSystemMessage::MOUSE_LBUTTONDOWN;
        case WM_LBUTTONUP:              return WindowsSystemMessage::MOUSE_LBUTTONUP;
        case WM_MBUTTONDBLCLK:          return WindowsSystemMessage::MOUSE_MBUTTONDBLCLK;
        case WM_MBUTTONDOWN:            return WindowsSystemMessage::MOUSE_MBUTTONDOWN;
        case WM_MBUTTONUP:              return WindowsSystemMessage::MOUSE_MBUTTONUP;
        case WM_MOUSEACTIVATE:          return WindowsSystemMessage::MOUSE_MOUSEACTIVATE;
        case WM_MOUSEHOVER:             return WindowsSystemMessage::MOUSE_MOUSEHOVER;
        case WM_MOUSEHWHEEL:            return WindowsSystemMessage::MOUSE_MOUSEHWHEEL;
        case WM_MOUSELEAVE:             return WindowsSystemMessage::MOUSE_MOUSELEAVE;
        case WM_MOUSEMOVE:              return WindowsSystemMessage::MOUSE_MOUSEMOVE;
        case WM_MOUSEWHEEL:             return WindowsSystemMessage::MOUSE_MOUSEWHEEL;
        case WM_NCLBUTTONDBLCLK:        return WindowsSystemMessage::MOUSE_NCLBUTTONDBLCLK;
        case WM_NCLBUTTONDOWN:          return WindowsSystemMessage::MOUSE_NCLBUTTONDOWN;
        case WM_NCLBUTTONUP:            return WindowsSystemMessage::MOUSE_NCLBUTTONUP;
        case WM_NCMBUTTONDBLCLK:        return WindowsSystemMessage::MOUSE_NCMBUTTONDBLCLK;
        case WM_NCMBUTTONDOWN:          return WindowsSystemMessage::MOUSE_NCMBUTTONDOWN;
        case WM_NCMBUTTONUP:            return WindowsSystemMessage::MOUSE_NCMBUTTONUP;
        case WM_NCMOUSEHOVER:           return WindowsSystemMessage::MOUSE_NCMOUSEHOVER;
        case WM_NCMOUSELEAVE:           return WindowsSystemMessage::MOUSE_NCMOUSELEAVE;
        case WM_NCMOUSEMOVE:            return WindowsSystemMessage::MOUSE_NCMOUSEMOVE;
        case WM_NCRBUTTONDBLCLK:        return WindowsSystemMessage::MOUSE_NCRBUTTONDBLCLK;
        case WM_NCRBUTTONDOWN:          return WindowsSystemMessage::MOUSE_NCRBUTTONDOWN;
        case WM_NCRBUTTONUP:            return WindowsSystemMessage::MOUSE_NCRBUTTONUP;
        case WM_NCXBUTTONDBLCLK:        return WindowsSystemMessage::MOUSE_NCXBUTTONDBLCLK;
        case WM_NCXBUTTONDOWN:          return WindowsSystemMessage::MOUSE_NCXBUTTONDOWN;
        case WM_NCXBUTTONUP:            return WindowsSystemMessage::MOUSE_NCXBUTTONUP;
        case WM_RBUTTONDBLCLK:          return WindowsSystemMessage::MOUSE_RBUTTONDBLCLK;
        case WM_RBUTTONDOWN:            return WindowsSystemMessage::MOUSE_RBUTTONDOWN;
        case WM_RBUTTONUP:              return WindowsSystemMessage::MOUSE_RBUTTONUP;
        case WM_XBUTTONDBLCLK:          return WindowsSystemMessage::MOUSE_XBUTTONDBLCLK;
        case WM_XBUTTONDOWN:            return WindowsSystemMessage::MOUSE_XBUTTONDOWN;
        case WM_XBUTTONUP:              return WindowsSystemMessage::MOUSE_XBUTTONUP;
        case WM_ACTIVATEAPP:            return WindowsSystemMessage::WINDOW_ACTIVATEAPP;
        case WM_CANCELMODE:             return WindowsSystemMessage::WINDOW_CANCELMODE;
        case WM_CHILDACTIVATE:          return WindowsSystemMessage::WINDOW_CHILDACTIVATE;
        case WM_CLOSE:                  return WindowsSystemMessage::WINDOW_CLOSE;
        case WM_COMPACTING:             return WindowsSystemMessage::WINDOW_COMPACTING;
        case WM_CREATE:                 return WindowsSystemMessage::WINDOW_CREATE;
        case WM_DESTROY:                return WindowsSystemMessage::WINDOW_DESTROY;
        case WM_DPICHANGED:             return WindowsSystemMessage::WINDOW_DPICHANGED;
        case WM_ENABLE:                 return WindowsSystemMessage::WINDOW_ENABLE;
        case WM_ENTERSIZEMOVE:          return WindowsSystemMessage::WINDOW_ENTERSIZEMOVE;
        case WM_EXITSIZEMOVE:           return WindowsSystemMessage::WINDOW_EXITSIZEMOVE;
        case WM_GETICON:                return WindowsSystemMessage::WINDOW_GETICON;
        case WM_GETMINMAXINFO:          return WindowsSystemMessage::WINDOW_GETMINMAXINFO;
        case WM_INPUTLANGCHANGE:        return WindowsSystemMessage::WINDOW_INPUTLANGCHANGE;
        case WM_INPUTLANGCHANGEREQUEST: return WindowsSystemMessage::WINDOW_INPUTLANGCHANGEREQUEST;
        case WM_MOVE:                   return WindowsSystemMessage::WINDOW_MOVE;
        case WM_MOVING:                 return WindowsSystemMessage::WINDOW_MOVING;
        case WM_NCACTIVATE:             return WindowsSystemMessage::WINDOW_NCACTIVATE;
        case WM_NCCALCSIZE:             return WindowsSystemMessage::WINDOW_NCCALCSIZE;
        case WM_NCCREATE:               return WindowsSystemMessage::WINDOW_NCCREATE;
        case WM_NCDESTROY:              return WindowsSystemMessage::WINDOW_NCDESTROY;
        case WM_NULL:                   return WindowsSystemMessage::WINDOW_NULL;
        case WM_QUERYDRAGICON:          return WindowsSystemMessage::WINDOW_QUERYDRAGICON;
        case WM_QUERYOPEN:              return WindowsSystemMessage::WINDOW_QUERYOPEN;
        case WM_QUIT:                   return WindowsSystemMessage::WINDOW_QUIT;
        case WM_SHOWWINDOW:             return WindowsSystemMessage::WINDOW_SHOWWINDOW;
        case WM_SIZE:                   return WindowsSystemMessage::WINDOW_SIZE;
        case WM_SIZING:                 return WindowsSystemMessage::WINDOW_SIZING;
        case WM_STYLECHANGED:           return WindowsSystemMessage::WINDOW_STYLECHANGED;
        case WM_STYLECHANGING:          return WindowsSystemMessage::WINDOW_STYLECHANGING;
        case WM_THEMECHANGED:           return WindowsSystemMessage::WINDOW_THEMECHANGED;
        case WM_USERCHANGED:            return WindowsSystemMessage::WINDOW_USERCHANGED;
        case WM_WINDOWPOSCHANGED:       return WindowsSystemMessage::WINDOW_WINDOWPOSCHANGED;
        case WM_WINDOWPOSCHANGING:      return WindowsSystemMessage::WINDOW_WINDOWPOSCHANGING;
        case WM_COMMAND:                return WindowsSystemMessage::MENU_COMMAND;
        case WM_SYSCOMMAND:             return WindowsSystemMessage::MENU_SYSCOMMAND;
        default:
            return WindowsSystemMessage::MESSAGE_NOT_SUPPORTED;
    }
}