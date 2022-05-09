#pragma once
#include <string>

namespace Hachiko
{
    struct EditorHistoryEntryRestore
    {
        explicit EditorHistoryEntryRestore(Scene* content) :
            content(content)
        {
        }

        [[nodiscard]] Scene* GetScene() const
        {
            return content;
        }

    private:
        Scene* content;
    };
}
