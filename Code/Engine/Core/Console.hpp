#pragma once

#include <functional>
#include <map>
#include <string>
#include <utility>
#include <vector>

#include "Engine/Core/EngineSubsystem.hpp"
#include "Engine/Core/Event.hpp"
#include "Engine/Core/Rgba.hpp"

#include "Engine/Math/Vector2.hpp"

class Arguments;
class SimpleRenderer;
class KerningFont;
class Texture2D;

class Console : public EngineSubsystem {
public:
    Console(SimpleRenderer* renderer, KerningFont* font, const Vector2& topleft);
    virtual ~Console() override;

    virtual void Initialize() override;

    virtual void BeginFrame() override;
    virtual void Update(float deltaSeconds) override;
    virtual void Render() const override;

    virtual void EndFrame() override;

    virtual bool ProcessSystemMessage(const SystemMessage& msg) override;

    bool RequestedQuit() const;
    bool IsConsoleOpen() const;
    void SetTextColor(const Rgba& color);
    const Rgba& GetTextColor();

    void RegisterCommand(const std::string& command_name, const std::function<void(const std::string& arguments)>& callback, const std::string& help_text);
    void UnregisterCommand(const std::string& command_name);
    void RunCommand(const std::string& command);

    KerningFont* GetFont() const;

    //Prints a message in non-threatening white text.
    void NotifyMsg(const std::string& msg);
    
    //Prints a message in warning-tone yellow text.
    void WarnMsg(const std::string& msg);

    //Prints a message in red text.
    void ErrorMsg(const std::string& msg);

    //Prints a message in the specified color.
    void OutputMsg(const std::string& msg, const Rgba& color);

    static Event<const std::string&> OnMessagePrint;

protected:
private:
    void Parse();
    void HistoryUp();
    void HistoryDown();
    void MoveCaretRight();
    void MoveCaretLeft();

    bool IsEntryLineEmpty() const;
    void Open();
    void Close();
    void ToggleOpenClose();
    void ClearConsoleBuffer();
    void PopConsoleBuffer();
    void RemoveText(std::string::const_iterator start, std::string::const_iterator end);
    void RemoveTextInFrontOfCaret();
    void RemoveTextBehindCaret();
    void ParseConsoleBuffer();
    void PushToConsoleString(unsigned char asKey);
    void PushToConsoleString(const std::string& str);
    void InsertToConsoleString(unsigned char asKey, std::string::const_iterator position);
    void InsertToConsoleString(const std::string& str, std::string::const_iterator position);
    void InsertToConsoleString(unsigned char asKey);
    void InsertToConsoleString(const std::string& str);
    void FlushConsoleBuffer();

    int UpdateSelectedRange(int direction);
    bool IncrementSelectedRange();
    bool DecrementSelectedRange();

    void RegisterCommands();

    void DrawEntryline() const;
    void DrawOutputBuffer() const;
    void DrawBackground() const;

    void PushEntrylineToOutputBuffer();
    void ResetCurrentCommand();
    void SaveEntrylineToBuffer();
    void SetEntryline(const std::string& str);
    void AutoCompleteEntryLine();

    std::vector<std::pair<std::string, Rgba>> output_buffer;
    std::vector<std::string> entryline_buffer;
    std::string entryline;
    std::string::const_iterator caretPos;
    std::string::const_iterator selectPos;
    std::vector<std::string>::iterator current_command;

    std::map<std::string, std::pair<std::function<void(const std::string& arguments)>, std::string>> _commands;
    Vector2 _topleft;
    SimpleRenderer* _renderer;
    KerningFont* _font;
    float _secondsPerBlink;
    float _defaultSecondsPerBlink;
    bool _requestQuit;
    bool _showCaret;
    bool _shiftModifier;
    Rgba _text_color;
    bool _isOpen;

    friend class Arguments;
};

class Vector3;
class Vector4;
class IntVector2;
class IntVector3;
class IntVector4;
class Matrix4;
class Rgba;

namespace Net {
class Address;
}

class Arguments {
public:
    Arguments(const std::string& args);
    bool GetNext(Rgba& value);
    bool GetNext(Vector2& value);
    bool GetNext(Vector3& value);
    bool GetNext(Vector4& value);
    bool GetNext(IntVector2& value);
    bool GetNext(IntVector3& value);
    bool GetNext(IntVector4& value);
    bool GetNext(Matrix4& value);
    bool GetNext(Net::Address& value);
    bool GetNext(std::string& value);
    bool GetNext(unsigned char& value);
    bool GetNext(signed char& value);
    bool GetNext(char& value);
    bool GetNext(unsigned short& value);
    bool GetNext(short& value);
    bool GetNext(unsigned int& value);
    bool GetNext(int& value);
    bool GetNext(unsigned long& value);
    bool GetNext(long& value);
    bool GetNext(unsigned long long& value);
    bool GetNext(long long& value);
    bool GetNext(float& value);
    bool GetNext(double& value);
    bool GetNext(long double& value);

protected:
private:
    std::string _args;
    std::string _current;
};