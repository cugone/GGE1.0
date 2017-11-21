#include "Engine/Window.hpp"

#include "Engine/Core/EngineBase.hpp"
#include "Engine/Math/Vector2.hpp"
#include "Engine/Config.hpp"

std::size_t Window::_refCount = 0;

constexpr const unsigned int ID_FILE_LOAD = 1;
constexpr const unsigned int ID_FILE_EXIT = 2;

void Window::RegisterWindowClass() {
    memset(&_windowClassDescription, 0, sizeof(_windowClassDescription));
    _windowClassDescription.cbSize = sizeof(_windowClassDescription);
    _windowClassDescription.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS; // Redraw on move, request own Display Context, Accept double-click events
    _windowClassDescription.lpfnWndProc = EngineMessageHandlingProcedure; // Assign a win32 message-handling function
    _windowClassDescription.hInstance = GetModuleHandle(NULL);
    _windowClassDescription.hIcon = NULL;
    _windowClassDescription.hCursor = NULL;
    _windowClassDescription.lpszClassName = TEXT("Simple Window Class");
    _windowClassDescription.lpszMenuName = NULL;
    RegisterClassEx(&_windowClassDescription);
}

void Window::UnregisterWindowClass() {
    UnregisterClass(_windowClassDescription.lpszClassName, nullptr);
}

Window::Window()
    : _hwnd(nullptr)
    , _msg(0)
    , _wparam(0)
    , _lparam(0)
    , _windowClassDescription{}
    , _windowTitle{}
    , _clientSize(DEFAULT_CLIENT_WIDTH, DEFAULT_CLIENT_HEIGHT)
    , _windowSize(DEFAULT_CLIENT_WIDTH, DEFAULT_CLIENT_HEIGHT)
    , _windowPosition(0, 0)
    , _styleFlags(WS_CAPTION | WS_BORDER | WS_SYSMENU | WS_OVERLAPPED)
    , _styleFlagsEx(WS_EX_APPWINDOW)
    , _hasMenu(false)
    , _hmenu(0)
    , _currentMode(RHIOutputMode::WINDOWED)
    , _initialClippingArea()
    , _isClippingCursor(false)
    , custom_message_handler(nullptr)
{
    if(_refCount == 0) {
        RegisterWindowClass();
        ++_refCount;
    }

    RECT desktopRect;
    HWND desktopWindowHandle = GetDesktopWindow();
    GetClientRect(desktopWindowHandle, &desktopRect);

    //RECT windowRect = { 0, 0, DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT };
    ::AdjustWindowRectEx(&desktopRect, _styleFlags, _hasMenu, _styleFlagsEx);
    ::ClipCursor(&desktopRect);
    ::GetClipCursor(&_initialClippingArea);
}

Window::~Window() {
    Close();
    if(_refCount != 0) {
        --_refCount;
        if(_refCount == 0) {
            UnregisterWindowClass();
        }
    }
}

void Window::Open() {
    _hwnd = CreateWindowEx(
        _styleFlagsEx,
        _windowClassDescription.lpszClassName,
        &_windowTitle[0],
        _styleFlags,
        _windowPosition.x,
        _windowPosition.y,
        _windowSize.x,
        _windowSize.y,
        NULL,
        _hmenu,
        GetModuleHandle(nullptr),
        this);

    Show();
    SetForegroundWindow();
    SetFocus();

    HCURSOR cursor = ::LoadCursor(nullptr, IDC_ARROW);
    ::SetCursor(cursor);

}

bool Window::IsOpen() {
    return ::IsWindow(_hwnd) != 0;
}

void Window::Close() {
    ::ClipCursor(&_initialClippingArea);
    ::DestroyWindow(_hwnd);
}

bool Window::IsClosed() {
    return !IsOpen();
}

void Window::Show() {
    ShowWindow(_hwnd, SW_SHOW);
}
void Window::Hide() {
    ShowWindow(_hwnd, SW_HIDE);
}
void Window::Unhide() {
    Show();
}
void Window::SetForegroundWindow() {
    ::SetForegroundWindow(_hwnd);
}
void Window::SetFocus() {
    ::SetFocus(_hwnd);
}

IntVector2 Window::GetClientPosition() {
    RECT r;
    ::GetClientRect(_hwnd, &r);

    return IntVector2(r.left, r.top);
}

IntVector2 Window::GetClientDimensions() {
    RECT r;
    ::GetClientRect(_hwnd, &r);

    return IntVector2(r.right - r.left, r.bottom - r.top);
}

IntVector2 Window::GetClientSize() const {
    return _clientSize;
}

void Window::SetClientSize(const IntVector2& clientPosition, const IntVector2& clientSize) {
    RECT r;
    r.top = clientPosition.y;
    r.left = clientPosition.x;
    r.bottom = r.top + clientSize.y;
    r.right = r.left + clientSize.x;
    ::AdjustWindowRectEx(&r, _styleFlags, _hasMenu, _styleFlagsEx);
    if(_isClippingCursor) {
        ConstrainMouseToWindow();
    }
    _clientSize = clientSize;
    SetWindowSize(IntVector2(r.right - r.left, r.bottom - r.top));

}

void Window::SetWindowSize(const IntVector2& windowSize) {
    _windowSize = windowSize;
}

void Window::SetWindowPosition(const IntVector2& windowPosition) {
    _windowPosition = windowPosition;
}
RHIOutputMode Window::GetDisplayMode() const {
    return _currentMode;
}

void Window::SetDisplayMode(const RHIOutputMode& mode) {
    _currentMode = mode;
    RECT r;
    r.top = _windowPosition.y;
    r.left = _windowPosition.x;
    r.bottom = _clientSize.y;
    r.right = _clientSize.x;
    switch(_currentMode) {
        case RHIOutputMode::BORDERLESS: {
            _styleFlags = WS_POPUP;
            _hasMenu = false;

            GetClientRect(_hwnd, &r);

            long width =  r.right -  r.left;
            long height = r.bottom - r.top;
            ::SetWindowLongPtr(_hwnd, GWL_STYLE, _styleFlags);
            ::SetWindowPos(_hwnd, nullptr, 0, 0, width, height, SWP_SHOWWINDOW);
            SetClientSize(IntVector2::ZERO, IntVector2(width, height));
            ::AdjustWindowRectEx(&r, _styleFlags, _hasMenu, _styleFlagsEx);
            if(_isClippingCursor) {
                ConstrainMouseToWindow();
            }
            return;

            break;
        } case RHIOutputMode::WINDOWED: {
            _styleFlags = WS_CAPTION | WS_BORDER | WS_SYSMENU | WS_OVERLAPPED;
            break;
        } case RHIOutputMode::FULLSCREEN_WINDOW: {

            _styleFlags = WS_POPUP;

            RECT desktopRect;
            HWND desktopWindowHandle = GetDesktopWindow();
            GetClientRect(desktopWindowHandle, &desktopRect);

            long width = desktopRect.right - desktopRect.left;
            long height = desktopRect.bottom - desktopRect.top;
            ::SetWindowLongPtr(_hwnd, GWL_STYLE, _styleFlags);
            ::SetWindowPos(_hwnd, nullptr, 0, 0, width, height, SWP_SHOWWINDOW);
            SetClientSize(IntVector2::ZERO, IntVector2(width, height));
            ::AdjustWindowRectEx(&r, _styleFlags, _hasMenu, _styleFlagsEx);
            if(_isClippingCursor) {
                ConstrainMouseToWindow();
            }
            return;
        } case RHIOutputMode::FULLSCREEN_DEDICATED:
            break;
        default:
            /* DO NOTHING */;
    }
    ::SetWindowLongPtr(_hwnd, GWL_STYLE, _styleFlags);
    ::AdjustWindowRectEx(&r, _styleFlags, _hasMenu, _styleFlagsEx);
    if(_isClippingCursor) {
        ConstrainMouseToWindow();
    }
}

void Window::ShowMenu(bool showMenu /*= true*/) {
    RECT r;
    r.top = _windowPosition.y;
    r.left = _windowPosition.x;
    r.bottom = _clientSize.y;
    r.right = _clientSize.x;
    _hasMenu = showMenu;
    ::AdjustWindowRectEx(&r, _styleFlags, _hasMenu, _styleFlagsEx);
    if(_isClippingCursor) {
        ConstrainMouseToWindow();
    }
}

HMENU Window::GetMenu() const {
    return _hmenu;
}
void Window::MakeMenu() {
    _hmenu = ::CreateMenu();
}
void Window::AppendStringToMenu(HMENU menu, unsigned int menuID, const char* str) {
    ::AppendMenuA(menu, MF_STRING, menuID, str);
}
void Window::SetFullscreen(bool fullscreen) {
    _currentMode = fullscreen ? RHIOutputMode::FULLSCREEN_DEDICATED : RHIOutputMode::WINDOWED;
}

bool Window::IsFullscreen() const {
    return _currentMode == RHIOutputMode::FULLSCREEN_DEDICATED;
}

bool Window::IsWindowed() const {
    return !IsFullscreen();
}

HWND Window::GetWindowHandle() {
    return _hwnd;
}

void Window::SetWindowHandle(HWND hwnd) {
    _hwnd = hwnd;
}

bool Window::IsMouseConstrained() const {
    return _isClippingCursor;
}

void Window::ConstrainMouseToWindow() {
    auto s = GetClientSize();
    auto p = GetClientPosition();
    RECT r;
    r.top = p.y;
    r.left = p.x;
    r.right = r.left + s.x;
    r.bottom = r.top + s.y;
    ::ClipCursor(&r);
    _isClippingCursor = true;
}
void Window::UnconstrainMouseToWindow() {
    ::ClipCursor(&_initialClippingArea);
    _isClippingCursor = false;
}

void Window::SetWindowStyle(unsigned long styleFlags /* = 0*/, unsigned long styleFlagsEx /* = 0*/) {
    _styleFlags = styleFlags;
    _styleFlagsEx = styleFlagsEx;
    RECT r;
    r.top = _windowPosition.y;
    r.left = _windowPosition.x;
    r.bottom = _clientSize.y;
    r.right = _clientSize.x;
    ::AdjustWindowRectEx(&r, _styleFlags, _hasMenu, _styleFlagsEx);
    if(_isClippingCursor) {
        ConstrainMouseToWindow();
    }
}

void Window::SetTitle(const std::string& title) {
    _windowTitle = title;
    ::SetWindowText(_hwnd, &_windowTitle[0]);
}

void Window::RegisterCustomMessageHandler(std::function<bool(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)> cb) {
    custom_message_handler = cb;
}

IntVector2 Window::ScreenToWindowCoords(const IntVector2& screenPos) const {
    POINT winScreenPos;
    winScreenPos.x = screenPos.x;
    winScreenPos.y = screenPos.y;
    if(::ScreenToClient(_hwnd, &winScreenPos) == 0) {
        return IntVector2(winScreenPos.x, winScreenPos.y);
    }
    return IntVector2::ZERO;
}