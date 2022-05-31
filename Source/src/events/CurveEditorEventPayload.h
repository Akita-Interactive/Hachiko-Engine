#pragma once
namespace Hachiko
{
    struct CurveEditorEventPayload
    {
        CurveEditorEventPayload(const std::string& title, float2* curve):
            title(title),
            curve(curve)
        {
        }

        [[nodiscard]] const std::string& GetTitle() const
        {
            return title;
        }

        [[nodiscard]] float2* GetCurve() const
        {
            return curve;
        }

    private:
        std::string title;
        float2* curve;
    };
}
