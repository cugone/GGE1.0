#include "Engine/Core/Console.hpp"

#include <algorithm>
#include <cctype>
#include <filesystem>
#include <locale>
#include <stdexcept>
#include <string>
#include <sstream>
#include <thread>

#include "Engine/BuildConfig.cpp"

#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/KerningFont.hpp"
#include "Engine/Core/Memory.hpp"

#include "Engine/EngineConfig.hpp"

#include "Engine/Input/InputSystem.hpp"

#include "Engine/Renderer/SimpleRenderer.hpp"
#include "Engine/Renderer/Texture2D.hpp"

#include "Engine/Math/Vector3.hpp"
#include "Engine/Math/Vector4.hpp"
#include "Engine/Math/IntVector2.hpp"
#include "Engine/Math/IntVector3.hpp"
#include "Engine/Math/IntVector4.hpp"
#include "Engine/Math/Matrix4.hpp"

#include "Engine/Networking/Address.hpp"

Event<const std::string&> Console::OnMessagePrint;

Console::Console(SimpleRenderer* renderer, KerningFont* font, const Vector2& topleft)
    : output_buffer{}
    , entryline_buffer{}
    , entryline()
    , caretPos()
    , selectPos()
    , _renderer(renderer)
    , _font(font)
    , _topleft(topleft)
    , _secondsPerBlink(0.0f)
    , _defaultSecondsPerBlink(0.33f)
    , _requestQuit(false)
    , _showCaret(false)
    , _showProfiler(false)
    , _shiftModifier(false)
    , _text_color()
    , _isOpen(false)
{ /* DO NOTHING */ }

Console::~Console() {
    _font = nullptr; //Observing pointer
}

bool Console::ProcessSystemMessage(const SystemMessage& msg) {
    unsigned char asKey = (unsigned char)msg.wParam;
    KeyCode asKeyCode = InputSystem::ConvertScanCodeToKeyCode(asKey);
    switch(msg.wmMessageCode) {
        case WindowsSystemMessage::KEYBOARD_KEYDOWN:
        {
            switch(asKeyCode) {
                case KeyCode::TILDE:
                    ToggleOpenClose();
                    if(IsConsoleOpen()) {
                        ClearConsoleBuffer();
                        return true;
                    }
                    break;
                case KeyCode::BACKSPACE:
                    if(IsConsoleOpen()) {
                        if(caretPos != selectPos) {
                            RemoveText(caretPos, selectPos);
                        } else {
                            RemoveTextBehindCaret();
                        }
                        return true;
                    }
                    break;
                case KeyCode::DELETE_KEYCODE:
                    if(IsConsoleOpen()) {
                        if(caretPos != selectPos) {
                            RemoveText(caretPos, selectPos);
                        } else {
                            RemoveTextInFrontOfCaret();
                        }
                        return true;
                    }
                    break;
                case KeyCode::ENTER:
                    if(IsConsoleOpen()) {
                        ParseConsoleBuffer();
                        return true;
                    }
                    break;
                case KeyCode::ESCAPE:
                    if(IsConsoleOpen()) {
                        if(IsEntryLineEmpty()) {
                            ToggleOpenClose();
                        } else {
                            ClearConsoleBuffer();
                        }
                        return true;
                    }
                    break;
                case KeyCode::UP:
                    if(IsConsoleOpen()) {
                        HistoryUp();
                        return true;
                    }
                    break;
                case KeyCode::DOWN:
                    if(IsConsoleOpen()) {
                        HistoryDown();
                        return true;
                    }
                    break;
                case KeyCode::RIGHT:
                    if(IsConsoleOpen()) {
                        MoveCaretRight();
                        return true;
                    }
                    break;
                case KeyCode::LEFT:
                    if(IsConsoleOpen()) {
                        MoveCaretLeft();
                        return true;
                    }
                    break;
                case KeyCode::TAB:
                    if(IsConsoleOpen()) {
                        AutoCompleteEntryLine();
                        return true;
                    }
                break;
                case KeyCode::SHIFT:
                {
                    _shiftModifier = true;
                }
            }
            break;
        }
        case WindowsSystemMessage::KEYBOARD_CHAR:
        {
            if(IsConsoleOpen()) {
                bool skipChar = false;
                switch(asKeyCode) {
                    case KeyCode::SHIFT:
                    case KeyCode::BACKSPACE:
                    case KeyCode::ENTER:
                    case KeyCode::ESCAPE:
                    case KeyCode::TAB:
                        skipChar = true;
                        break;
                }
                switch(asKey) {
                    case '`':
                    case '~':
                        skipChar = true;
                        break;
                }
                if(!skipChar) {
                    RemoveText(caretPos, selectPos);
                    InsertToConsoleString(asKey);
                    return true;
                }
            }
            break;
        }
        case WindowsSystemMessage::KEYBOARD_KEYUP:
        {
            switch(asKeyCode) {
                case KeyCode::SHIFT:
                {
                    _shiftModifier = false;
                }
                break;
            }
        }
        break;
    }
    return false;
}

bool Console::RequestedQuit() const {
    return _requestQuit;
}

bool Console::IsConsoleOpen() const {
    return _isOpen;
}

void Console::Open() {
    _isOpen = true;
}

void Console::Close() {
    _isOpen = false;
}

void Console::ToggleOpenClose() {
    _isOpen = !_isOpen;
}

void Console::ClearConsoleBuffer() {
    entryline.clear();
    caretPos = entryline.end();
    selectPos = caretPos;
}

void Console::PopConsoleBuffer() {
    if(!entryline.empty()) {
        if(caretPos == entryline.end()) {
            entryline.pop_back();
            --caretPos;
        }
    }
}

void Console::RemoveTextInFrontOfCaret() {
    if(!entryline.empty()) {
        if(caretPos != entryline.end()) {
            caretPos = entryline.erase(caretPos);
            selectPos = caretPos;
        }
    }
}

void Console::RemoveTextBehindCaret() {
    if(!entryline.empty()) {
        if(caretPos != entryline.end()) {
            if(caretPos != entryline.begin()) {
                caretPos = entryline.erase(caretPos - 1);
            }
        } else {
            PopConsoleBuffer();
        }
        selectPos = caretPos;
    }
}

void Console::RemoveText(std::string::const_iterator start, std::string::const_iterator end) {
    if(end < start) {
        std::swap(start, end);
    }
    caretPos = entryline.erase(start, end);
    selectPos = caretPos;
}

void Console::ParseConsoleBuffer() {
    if(!entryline.empty()) {
        Parse();
        FlushConsoleBuffer();
    }
}

void Console::PushToConsoleString(unsigned char asKey) {
    entryline += asKey;
    caretPos = entryline.end();
    selectPos = caretPos;
}

void Console::PushToConsoleString(const std::string& str) {
    entryline += str;
    caretPos = entryline.end();
    selectPos = caretPos;
}

void Console::InsertToConsoleString(unsigned char asKey, std::string::const_iterator position) {
    caretPos = entryline.insert(position, asKey);
    selectPos = caretPos;
}

void Console::InsertToConsoleString(const std::string& str, std::string::const_iterator position) {
    caretPos = entryline.insert(position, str.begin(), str.end());
    selectPos = caretPos;
}

void Console::InsertToConsoleString(unsigned char asKey) {
    if(caretPos != entryline.end()) {
        caretPos = entryline.insert(caretPos, asKey);
        ++caretPos;
    } else {
        PushToConsoleString(asKey);
    }
    selectPos = caretPos;
}

void Console::InsertToConsoleString(const std::string& str) {
    if(caretPos != entryline.end()) {
        caretPos = entryline.insert(caretPos, str.begin(), str.end());
        ++caretPos;
    } else {
        PushToConsoleString(str);
    }
    selectPos = caretPos;
}

void Console::FlushConsoleBuffer() {
    PushEntrylineToOutputBuffer();
    SaveEntrylineToBuffer();
    ResetCurrentCommand();
    ClearConsoleBuffer();
}

KerningFont* Console::GetFont() const {
    return _font;
}

void Console::PushEntrylineToOutputBuffer() {
    if(!entryline.empty()) {
        OutputMsg(entryline, GetTextColor());
    }
}

void Console::ResetCurrentCommand() {
    current_command = entryline_buffer.end();
}

void Console::SaveEntrylineToBuffer() {
    if(!entryline.empty()) {
        entryline_buffer.push_back(entryline);
    }
}

void Console::AutoCompleteEntryLine() {
    for(auto& cmd_string : _commands) {
        if(cmd_string.first.find(entryline) == 0) {
            SetEntryline(cmd_string.first);
            break;
        }
    }
}

void Console::OutputMsg(const std::string& msg, const Rgba& color) {
    OnMessagePrint.Trigger(msg);
    output_buffer.push_back(std::make_pair(msg, color));
}

int Console::UpdateSelectedRange(int direction) {
    int moved_by = 0;
    while(0 > direction && direction < moved_by && DecrementSelectedRange()) {
        --moved_by;
    }
    while(0 < direction && direction > moved_by && IncrementSelectedRange()) {
        ++moved_by;
    }
    return moved_by;
}

bool Console::IncrementSelectedRange() {
    int direction = caretPos != entryline.end() ? 1 : 0;
    auto rangeStart = caretPos + direction;
    auto rangeEnd = selectPos;
    if(!_shiftModifier && selectPos < caretPos) {
        rangeStart = selectPos;
        rangeEnd = caretPos;
    }
    caretPos = rangeStart;
    selectPos = rangeEnd;
    return direction != 0;
}

bool Console::DecrementSelectedRange() {
    int direction = caretPos != entryline.begin() ? -1 : 0;
    auto rangeStart = caretPos + direction;
    auto rangeEnd = selectPos;
    if(!_shiftModifier && selectPos < caretPos) {
        rangeStart = selectPos;
        rangeEnd = caretPos;
    }
    caretPos = rangeStart;
    selectPos = rangeEnd;
    return direction != 0;
}

void Console::SetEntryline(const std::string& str) {
    entryline = str;
    caretPos = entryline.end();
    selectPos = caretPos;
}

void Console::HistoryUp() {
    if(!entryline_buffer.empty()) {
        if(current_command != entryline_buffer.begin()) {
            --current_command;
        }
        SetEntryline(*current_command);
    }
}

void Console::HistoryDown() {
    if(!entryline_buffer.empty()) {
        if(current_command != entryline_buffer.end()) {
            ++current_command;
            if(current_command == entryline_buffer.end()) {
                current_command = entryline_buffer.end() - 1;
            }
        }
        SetEntryline(*current_command);
    }
}

void Console::MoveCaretRight() {
    if(caretPos != entryline.end()) {
        if(!_shiftModifier) {
            ++caretPos;
            selectPos = caretPos;
        } else {
            UpdateSelectedRange(1);
        }
    }
}

void Console::MoveCaretLeft() {
    if(caretPos != entryline.begin()) {
        if(!_shiftModifier) {
            --caretPos;
            selectPos = caretPos;
        } else {
            UpdateSelectedRange(-1);
        }
    }
}

void Console::NotifyMsg(const std::string& msg) {
    OutputMsg(msg, Rgba::WHITE);
}

void Console::WarnMsg(const std::string& msg) {
    OutputMsg(msg, Rgba::YELLOW);
}

void Console::ErrorMsg(const std::string& msg) {
    OutputMsg(msg, Rgba::RED);
}

bool Console::IsEntryLineEmpty() const {
    return entryline.empty();
}

void Console::Initialize() {
    RegisterCommands();
    current_command = entryline_buffer.end();
}

void Console::RegisterCommands() {

    RegisterCommand("echo", [&](const std::string& args) { Arguments arg_set(args); for(std::string str; arg_set.GetNext(str); /* DO NOTHING */) this->NotifyMsg(str); }, "Echoes the input arguments.");

    RegisterCommand("clear",
    [&](const std::string& /*args*/) {
        output_buffer.clear(); entryline.clear();
    }, "Clears the screen.");
    
    RegisterCommand("quit",
    [&](const std::string& /*args*/) {
        _requestQuit = true;
    }, "Quits the game.");

    RegisterCommand("help",
    [&](const std::string& args) {
        Arguments arg_set(args);
        std::string start;
        if(arg_set.GetNext(start)) {
            RunCommand("clear");
            for(auto& command : _commands) {
                if(command.first.find(start) == 0) {
                    this->NotifyMsg(command.first + ": " + command.second.second);
                }
            }
        } else {
            RunCommand("clear");
            for(auto& command : _commands) {
                this->NotifyMsg(command.first + ": " + command.second.second);
            }
        }
    }
    , "Displays all available commands or commands starting with specific string.");

    RegisterCommand("memstat",
    [&](const std::string& /*args*/) {
        g_theProfiler->ToggleOpenClosed();
    }
    , "Shows/Hides Profile Visualizer");

    RegisterCommand("log_liveallocs",
    [&](const std::string& /*args*/) {
        std::thread t(&Memory::PrintLiveAllocations);
        t.detach();
    }
    , "Prints the callstack to the log.");


    RegisterCommand("launch",
    [&](const std::string& args) {
        Arguments arg_set(args);
        std::string arguments;
        arg_set.GetNext(arguments);

        std::array<char, 512> buf;
        ::GetModuleFileNameA(nullptr, buf.data(), buf.size() );

        STARTUPINFO si = { sizeof(STARTUPINFO) };
        si.cb = sizeof(si);
        si.dwFlags = STARTF_USESHOWWINDOW;
        si.wShowWindow = SW_SHOW;
        PROCESS_INFORMATION pi;
        ::CreateProcessA(buf.data(), nullptr, nullptr, nullptr, FALSE, CREATE_NO_WINDOW, nullptr, nullptr, &si, &pi);

    }
    , "Launches a separate process.");

}

void Console::BeginFrame() {
    Memory::TickMemoryProfiler();
}

void Console::Update(float deltaSeconds) {
    _secondsPerBlink += deltaSeconds;
    if (_defaultSecondsPerBlink < _secondsPerBlink) {
        _secondsPerBlink = 0.0f;
        _showCaret = !_showCaret;
    }
}

void Console::Render() const {

    if(!IsConsoleOpen()) {
        return;
    }

    DrawBackground();
    DrawOutputBuffer();
    DrawEntryline();

}

void Console::DrawBackground() const {
    const auto& window_dimensions = _renderer->_rhi_output->GetDimensions();
    float window_height = static_cast<float>(window_dimensions.y);
    float window_width  = static_cast<float>(window_dimensions.x);
    _renderer->SetOrthoProjection(Vector2(0.0f, window_height), Vector2(window_width, 0.0f), Vector2(0.0f, 1.0f));
    _renderer->SetMaterial(_renderer->GetMaterial("__console"));
    _renderer->SetViewMatrix(Matrix4::GetIdentity());
    _renderer->SetModelMatrix(Matrix4::GetIdentity());
    //_renderer->DrawDebugAABB2(AABB2(0.0f, 0.0f, window_width, window_height), Rgba::NOALPHA, Rgba(64, 64, 64, 128));
    _renderer->DrawQuad(0.0f, 0.0f, 0.0f, window_height, window_width, 1.0f, Rgba(64, 64, 64, 128));
}

void Console::DrawOutputBuffer() const {
    float draw_loc_y = _topleft.y - _font->GetLineHeight();
    for(auto riter = output_buffer.rbegin(); riter != output_buffer.rend(); ++riter) {
        std::istringstream ss;
        ss.str((*riter).first);
        std::string curline;
        while(std::getline(ss, curline)) {
            _renderer->DrawTextLine(_font, curline, riter->second, _topleft.x, draw_loc_y);
            draw_loc_y -= _font->GetLineHeight();
        }
    }
}

void Console::DrawEntryline() const {
    
    if(caretPos != selectPos) {
        _renderer->DrawTextLine(_font, std::string(entryline.begin(), caretPos), Rgba::WHITE, _topleft.x, _topleft.y);
        _renderer->DrawTextLine(_font, std::string(selectPos, entryline.end()), Rgba::WHITE, _topleft.x + _font->CalculateTextWidth(std::string(entryline.begin(), selectPos)), _topleft.y);

        _renderer->SetMaterial(_renderer->GetMaterial("__unlit"));

        float xPosOffsetToCaret = _font->CalculateTextWidth(std::string(entryline.begin(), caretPos));
        float xPosOffsetToSelect = _font->CalculateTextWidth(std::string(entryline.begin(), selectPos));
        auto rangeStart = caretPos;
        auto rangeEnd = selectPos;
        if(selectPos < caretPos) {
            std::swap(rangeStart, rangeEnd);
            std::swap(xPosOffsetToCaret, xPosOffsetToSelect);
        }

        float yPosOffset = static_cast<float>(-_font->GetLineHeight());

        _renderer->DrawQuad(_topleft.y,
                            _topleft.x + xPosOffsetToCaret,
                            0.0f,
                            _topleft.y + yPosOffset,
                            _topleft.x + xPosOffsetToSelect,
                            1.0f,
                            Rgba(64, 64, 0, 255)
        );

        float xPosOffsetToStart = _font->CalculateTextWidth(std::string(entryline.begin(), rangeStart));
        _renderer->DrawTextLine(_font, std::string(rangeStart, rangeEnd), Rgba::BLACK, _topleft.x + xPosOffsetToStart, _topleft.y);

    } else {
        _renderer->DrawTextLine(_font, entryline, Rgba::WHITE, _topleft.x, _topleft.y);
    }
    if(_showCaret) {
        //Recalculate caret position for rendering
        _renderer->DrawTextLine(_font, "|", Rgba::WHITE, _topleft.x + _font->CalculateTextWidth(std::string(entryline.begin(), caretPos)), _topleft.y);
    }
}

void Console::EndFrame() {
    /* DO NOTHING */
}

void Console::Parse() {
    RunCommand(entryline);
}

void Console::SetTextColor(const Rgba& color) {
    _text_color = color;
}

const Rgba& Console::GetTextColor() {
    return _text_color;
}

void Console::RegisterCommand(const std::string& command_name, const std::function<void(const std::string&)>& callback, const std::string& help_text) {
    _commands.insert_or_assign(command_name, std::make_pair(callback, help_text));
}

void Console::UnregisterCommand(const std::string& command_name) {
    _commands.erase(command_name);
}

void Console::RunCommand(const std::string& command) {
    if (command.empty()) {
        return;
    }
    std::string c = command.substr(0, command.find_first_of(' '));
    std::string args;
    if (command.find_first_of(' ') != std::string::npos) {
        args = command.substr(command.find_first_of(' '));
    }
    auto command_iter = _commands.find(c);
    if (command_iter == _commands.end()) {
        ErrorMsg("INVALID COMMAND");
        return;
    }
    command_iter->second.first(args);
}

Arguments::Arguments(const std::string& args)
    : _args(args)
    , _current(args)
{
    /* DO NOTHING */
}

bool Arguments::GetNext(Rgba& value) {
    std::string value_str;
    if(GetNext(value_str)) {
        value = Rgba(value_str);
        return true;
    }
    return false;
}

bool Arguments::GetNext(Vector2& value) {
    std::string value_str;
    if(GetNext(value_str)) {
        value = Vector2(value_str);
        return true;
    }
    return false;
}

bool Arguments::GetNext(Vector3& value) {
    std::string value_str;
    if(GetNext(value_str)) {
        value = Vector3(value_str);
        return true;
    }
    return false;
}

bool Arguments::GetNext(Vector4& value) {
    std::string value_str;
    if(GetNext(value_str)) {
        value = Vector4(value_str);
        return true;
    }
    return false;
}

bool Arguments::GetNext(IntVector2& value) {
    std::string value_str;
    if(GetNext(value_str)) {
        value = IntVector2(value_str);
        return true;
    }
    return false;
}

bool Arguments::GetNext(IntVector3& value) {
    std::string value_str;
    if(GetNext(value_str)) {
        value = IntVector3(value_str);
        return true;
    }
    return false;
}

bool Arguments::GetNext(IntVector4& value) {
    std::string value_str;
    if(GetNext(value_str)) {
        value = IntVector4(value_str);
        return true;
    }
    return false;
}

bool Arguments::GetNext(Matrix4& value) {
    std::string value_str;
    if(GetNext(value_str)) {
        value = Matrix4(value_str);
        return true;
    }
    return false;
}

bool Arguments::GetNext(Net::Address& value) {
    std::string value_str;
    if(GetNext(value_str)) {
        value = Net::NetAddressFromString(value_str);
        return true;
    }
    return false;
}

bool Arguments::GetNext(std::string& value) {
    std::istringstream ss;
    ss.str(_current);
    if (ss.str().empty()) {
        return false;
    }
    std::string arg;
    bool inQuote = false;
    if(ss >> arg) {
        if(arg.front() == '"') {
            inQuote = true;
        }
        if(arg.back() == '"') {
            inQuote = false;
            arg.erase(0, 1);
            arg.pop_back();
        }
        if(inQuote) {
            std::string next;
            while(ss >> next) {
                arg += " ";
                arg += next;
                if(next.back() == '"') {
                    arg.erase(0, 1);
                    arg.pop_back();
                    inQuote = false;
                    break;
                }
            }
        }
        if(!std::getline(ss, _current)) {
            _current = std::string("");
        }
        value = arg;
        return true;
    }
    return false;
}

bool Arguments::GetNext(unsigned char& value) {
    std::string value_str;
    if (GetNext(value_str)) {
        try {
            value = static_cast<unsigned char>(std::stoul(value_str));
        } catch(std::invalid_argument& /*e*/) {
            return false;
        }
        return true;
    }
    return false;
}

bool Arguments::GetNext(signed char& value) {
    std::string value_str;
    if (GetNext(value_str)) {
        try {
            value = static_cast<signed char>(std::stoi(value_str));
        } catch(std::invalid_argument& /*e*/) {
            return false;
        }
        return true;
    }
    return false;
}

bool Arguments::GetNext(char& value) {
    std::string value_str;
    if (GetNext(value_str)) {
        value = *value_str.begin();
        return true;
    }
    return false;
}

bool Arguments::GetNext(unsigned short& value) {
    std::string value_str;
    if (GetNext(value_str)) {
        try {
            value = static_cast<unsigned short>(std::stoul(value_str));
        } catch(std::invalid_argument& /*e*/) {
            return false;
        }
        return true;
    }
    return false;
}

bool Arguments::GetNext(short& value) {
    std::string value_str;
    if (GetNext(value_str)) {
        try {
            value = static_cast<short>(std::stoi(value_str));
        } catch(std::invalid_argument& /*e*/) {
            return false;
        }
        return true;
    }
    return false;
}

bool Arguments::GetNext(unsigned int& value) {
    std::string value_str;
    if (GetNext(value_str)) {
        try {
            value = static_cast<unsigned int>(std::stoul(value_str));
        } catch(std::invalid_argument& /*e*/) {
            return false;
        }
        return true;
    }
    return false;
}

bool Arguments::GetNext(int& value) {
    std::string value_str;
    if (GetNext(value_str)) {
        try {
            value = static_cast<unsigned short>(std::stoi(value_str));
        } catch(std::invalid_argument& /*e*/) {
            return false;
        }
        return true;
    }
    return false;
}

bool Arguments::GetNext(unsigned long& value) {
    std::string value_str;
    if (GetNext(value_str)) {
        try {
            value = std::stoul(value_str);
        } catch(std::invalid_argument& /*e*/) {
            return false;
        }
        return true;
    }
    return false;
}

bool Arguments::GetNext(long& value) {
    std::string value_str;
    if (GetNext(value_str)) {
        try {
            value = std::stol(value_str);
        } catch(std::invalid_argument& /*e*/) {
            return false;
        }
        return true;
    }
    return false;
}

bool Arguments::GetNext(unsigned long long& value) {
    std::string value_str;
    if (GetNext(value_str)) {
        try {
            value = std::stoull(value_str);
        } catch(std::invalid_argument& /*e*/) {
            return false;
        }
        return true;
    }
    return false;
}

bool Arguments::GetNext(long long& value) {
    std::string value_str;
    if (GetNext(value_str)) {
        try {
            value = std::stoll(value_str);
        } catch(std::invalid_argument& /*e*/) {
            return false;
        }
        return true;
    }
    return false;
}

bool Arguments::GetNext(float& value) {
    std::string value_str;
    if (GetNext(value_str)) {
        try {
            value = std::stof(value_str);
        } catch(std::invalid_argument& /*e*/) {
            return false;
        }
        return true;
    }
    return false;
}

bool Arguments::GetNext(double& value) {
    std::string value_str;
    if (GetNext(value_str)) {
        try {
            value = std::stod(value_str);
        } catch(std::invalid_argument& /*e*/) {
            return false;
        }
        return true;
    }
    return false;
}

bool Arguments::GetNext(long double& value) {
    std::string value_str;
    if (GetNext(value_str)) {
        try {
            value = std::stold(value_str);
        } catch(std::invalid_argument& /*e*/) {
            return false;
        }
        return true;
    }
    return false;
}