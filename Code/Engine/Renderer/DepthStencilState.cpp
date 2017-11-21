#include "Engine/Renderer/DepthStencilState.hpp"

#include "Engine/Core/ErrorWarningAssert.hpp"

#include "Engine/RHI/DX11.hpp"
#include "Engine/RHI/RHIDevice.hpp"

DepthStencilState::DepthStencilState(RHIDevice* device, const XMLElement& element)
    : _dx_depthstencilstate(nullptr)
{
    bool load_succeeded = LoadFromXml(device, element);
    if(!load_succeeded) {
        _dx_depthstencilstate->Release();
        _dx_depthstencilstate = nullptr;
        ERROR_AND_DIE("DepthStencilState failed to load from Xml.");
    }
}

DepthStencilState::DepthStencilState(
                RHIDevice* device
                , bool enableDepthTest /*= true*/
                , bool enableStencilTest /*= false*/
                , bool enableDepthWrite /*= true*/
                , bool enableStencilRead /*= true*/
                , bool enableStencilWrite /*= true*/
                , const ComparisonFunction& depthComparison /*= ComparisonFunction::LESS*/
                , std::pair<const StencilOperation&, const StencilOperation&> failFrontBackOp /*= std::make_pair(StencilOperation::KEEP, StencilOperation::KEEP)*/
                , std::pair<const StencilOperation&, const StencilOperation&> failDepthFrontBackOp /*= std::make_pair(StencilOperation::KEEP, StencilOperation::KEEP)*/
                , std::pair<const StencilOperation&, const StencilOperation&> passFrontBackOp /*= std::make_pair(StencilOperation::KEEP, StencilOperation::KEEP)*/
                , std::pair<const ComparisonFunction&, const ComparisonFunction&> stencilComparisonFrontBack /*= std::make_pair(ComparisonFunction::ALWAYS, ComparisonFunction::ALWAYS)*/
)
: _dx_depthstencilstate(nullptr)
{
    bool state_succeeded = CreateDepthStencilState(device,
                                                   enableDepthTest,
                                                   enableStencilTest,
                                                   enableDepthWrite,
                                                   enableStencilRead,
                                                   enableStencilWrite,
                                                   depthComparison,
                                                   failFrontBackOp,
                                                   failDepthFrontBackOp,
                                                   passFrontBackOp,
                                                   stencilComparisonFrontBack);
    if (!state_succeeded) {
        _dx_depthstencilstate->Release();
        _dx_depthstencilstate = nullptr;
        ERROR_AND_DIE("Depth Stencil failed to create.");
    }
}

DepthStencilState::~DepthStencilState() {
    if (_dx_depthstencilstate) {
        _dx_depthstencilstate->Release();
        _dx_depthstencilstate = nullptr;
    }
}
ID3D11DepthStencilState* DepthStencilState::GetDxDepthStencilState() {
    return _dx_depthstencilstate;
}

bool DepthStencilState::LoadFromXml(RHIDevice* device, const XMLElement& element) {

    //Default values if no depth stencil element exists.
    bool enableDepthTest = true;
    bool depthWritable = true;
    ComparisonFunction depthComparison = ComparisonFunction::LESS;

    auto xml_depth = element.FirstChildElement("depth");
    if(xml_depth) {
        DataUtils::ValidateXmlElement(*xml_depth, "", "", "", "enable,writable,test");
        enableDepthTest = DataUtils::ParseXmlAttribute(*xml_depth, "enable", enableDepthTest);
        depthWritable = DataUtils::ParseXmlAttribute(*xml_depth, "writable", depthWritable);
        std::string comp_func_str = "less";
        comp_func_str = DataUtils::ParseXmlAttribute(*xml_depth, "test", comp_func_str);
        depthComparison = ComparisonFunctionFromString(comp_func_str);
    }

    //Default values if no stencil element exists.
    bool enableStencilTest = false;
    bool enableStencilRead = true;
    bool enableStencilWrite = true;
    std::pair<StencilOperation, StencilOperation> failFBOp = std::make_pair(StencilOperation::KEEP, StencilOperation::KEEP);
    std::pair<StencilOperation, StencilOperation> failDepthFBOp = std::make_pair(StencilOperation::KEEP, StencilOperation::KEEP);
    std::pair<StencilOperation, StencilOperation> passFBOp = std::make_pair(StencilOperation::KEEP, StencilOperation::KEEP);
    std::pair<ComparisonFunction, ComparisonFunction> stencilComparisonFB = std::make_pair(ComparisonFunction::ALWAYS, ComparisonFunction::ALWAYS);

    auto xml_stencil = element.FirstChildElement("stencil");
    if(xml_stencil) {
        DataUtils::ValidateXmlElement(*xml_stencil, "", "", "front,back", "enable,writable,readable");

        enableStencilRead = DataUtils::ParseXmlAttribute(*xml_stencil, "readable", enableStencilRead);
        enableStencilWrite = DataUtils::ParseXmlAttribute(*xml_stencil, "writable", enableStencilWrite);
        enableStencilTest = DataUtils::ParseXmlAttribute(*xml_stencil, "enable", enableStencilTest);

        auto xml_stencilfront = xml_stencil->FirstChildElement("front");
        if(xml_stencilfront) {
            DataUtils::ValidateXmlElement(*xml_stencilfront, "", "fail,depthfail,pass,test");

            std::string failFront_str = "keep";
            failFront_str = DataUtils::ParseXmlAttribute(*xml_stencilfront, "fail", failFront_str);
            failFBOp.first = StencilOperationFromString(failFront_str);

            std::string depthfailFront_str = "keep";
            depthfailFront_str = DataUtils::ParseXmlAttribute(*xml_stencilfront, "depthfail", depthfailFront_str);
            failDepthFBOp.first = StencilOperationFromString(depthfailFront_str);

            std::string passFront_str = "keep";
            passFront_str = DataUtils::ParseXmlAttribute(*xml_stencilfront, "pass", passFront_str);
            passFBOp.first = StencilOperationFromString(passFront_str);

            std::string compareFront_str = "always";
            compareFront_str = DataUtils::ParseXmlAttribute(*xml_stencilfront, "test", compareFront_str);
            stencilComparisonFB.first = ComparisonFunctionFromString(compareFront_str);
        }

        auto xml_stencilback = xml_stencil->FirstChildElement("back");
        if(xml_stencilback) {
            DataUtils::ValidateXmlElement(*xml_stencilback, "", "fail,depthfail,pass,test");

            std::string failBack_str = "keep";
            failBack_str = DataUtils::ParseXmlAttribute(*xml_stencilback, "fail", failBack_str);
            failFBOp.second = StencilOperationFromString(failBack_str);

            std::string depthfailBack_str = "keep";
            depthfailBack_str = DataUtils::ParseXmlAttribute(*xml_stencilback, "depthfail", depthfailBack_str);
            failDepthFBOp.second = StencilOperationFromString(depthfailBack_str);

            std::string passBack_str = "keep";
            passBack_str = DataUtils::ParseXmlAttribute(*xml_stencilback, "pass", passBack_str);
            passFBOp.second = StencilOperationFromString(passBack_str);

            std::string compareBack_str = "always";
            compareBack_str = DataUtils::ParseXmlAttribute(*xml_stencilback, "test", compareBack_str);
            stencilComparisonFB.second = ComparisonFunctionFromString(compareBack_str);
        }
    }

    return CreateDepthStencilState(device,
                                   enableDepthTest,
                                   enableStencilTest,
                                   depthWritable,
                                   enableStencilRead,
                                   enableStencilWrite,
                                   depthComparison,
                                   failFBOp,
                                   failDepthFBOp,
                                   passFBOp,
                                   stencilComparisonFB);

}

bool DepthStencilState::CreateDepthStencilState(RHIDevice* device,
                                                bool enableDepthTest /*= true */,
                                                bool enableStencilTest /*= false */,
                                                bool enableDepthWrite /*= true */,
                                                bool enableStencilRead /*= true */,
                                                bool enableStencilWrite /*= true */,
                                                const ComparisonFunction& depthComparison /*= ComparisonFunction::LESS */,
                                                std::pair<const StencilOperation&, const StencilOperation&> failFrontBackOp /*= std::make_pair(StencilOperation::KEEP, StencilOperation::KEEP) */,
                                                std::pair<const StencilOperation&, const StencilOperation&> failDepthFrontBackOp /*= std::make_pair(StencilOperation::KEEP, StencilOperation::KEEP) */,
                                                std::pair<const StencilOperation&, const StencilOperation&> passFrontBackOp /*= std::make_pair(StencilOperation::KEEP, StencilOperation::KEEP) */,
                                                std::pair<const ComparisonFunction&, const ComparisonFunction&> stencilComparisonFrontBack /*= std::make_pair(ComparisonFunction::ALWAYS, ComparisonFunction::ALWAYS) */
) {
    D3D11_DEPTH_STENCIL_DESC dsDesc;

    // Depth test parameters
    dsDesc.DepthEnable = enableDepthTest;
    dsDesc.DepthWriteMask = enableDepthWrite ? D3D11_DEPTH_WRITE_MASK_ALL : D3D11_DEPTH_WRITE_MASK_ZERO;
    dsDesc.DepthFunc = ComparisonFunctionToD3DComparisonFunction(depthComparison);

    // Stencil test parameters
    dsDesc.StencilEnable = enableStencilTest;
    dsDesc.StencilReadMask = enableStencilRead ? 0xFF : 0x00;
    dsDesc.StencilWriteMask = enableStencilWrite ? 0xFF : 0x00;

    // Stencil operations if pixel is front-facing
    dsDesc.FrontFace.StencilFailOp = StencilOperationToD3DStencilOperation(failFrontBackOp.first);
    dsDesc.FrontFace.StencilDepthFailOp = StencilOperationToD3DStencilOperation(failDepthFrontBackOp.first);
    dsDesc.FrontFace.StencilPassOp = StencilOperationToD3DStencilOperation(passFrontBackOp.first);
    dsDesc.FrontFace.StencilFunc = ComparisonFunctionToD3DComparisonFunction(stencilComparisonFrontBack.first);

    // Stencil operations if pixel is back-facing
    dsDesc.BackFace.StencilFailOp = StencilOperationToD3DStencilOperation(failFrontBackOp.second);
    dsDesc.BackFace.StencilDepthFailOp = StencilOperationToD3DStencilOperation(failDepthFrontBackOp.second);
    dsDesc.BackFace.StencilPassOp = StencilOperationToD3DStencilOperation(passFrontBackOp.second);
    dsDesc.BackFace.StencilFunc = ComparisonFunctionToD3DComparisonFunction(stencilComparisonFrontBack.second);

    // Create depth stencil state
    HRESULT state_hr = device->GetDxDevice()->CreateDepthStencilState(&dsDesc, &_dx_depthstencilstate);
    return SUCCEEDED(state_hr);
}