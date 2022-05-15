#include "core/hepch.h"
#include "WindowProject.h"
#include "modules/ModuleEditor.h"

Hachiko::WindowProject::WindowProject() :
    Window("Project", true) {}

Hachiko::WindowProject::~WindowProject() = default;

void Hachiko::WindowProject::Init()
{
    m_filter[0] = '\0';
    GetAssets();
}

void Hachiko::WindowProject::Update()
{
    ImGui::SetNextWindowDockID(App->editor->dock_down_id, ImGuiCond_FirstUseEver);
    if (!ImGui::Begin((std::string(ICON_FA_IMAGES " ") + name).c_str(), &active, ImGuiWindowFlags_NoNavInputs))
    {
        ImGui::End();
        return;
    }
    ImGui::PushItemWidth(100);
    if (ImGui::InputTextWithHint("##filter", "Filter", m_filter, sizeof(m_filter)))
    {
        DoFilter();
    }
    ImGui::PopItemWidth();
    ImGui::SameLine();
    if (ImGuiUtils::IconButton(ICON_FA_TIMES, "Reset filter"))
    {
        m_filter[0] = '\0';
        DoFilter();
    }

    ImGui::SameLine();
    CreateBreadCrumps();
    ImGui::Separator();

    const float content_w = ImGui::GetContentRegionAvail().x;
    ImVec2 left_size(m_left_column_width, 0);
    if (left_size.x < 10)
    {
        left_size.x = 10;
    }
    if (left_size.x > content_w - 10)
    {
        left_size.x = content_w - 10;
    }

    ShowDir(all_assets);

    ImGui::SameLine();
    ImGuiUtils::VSplitter("vsplit1", &left_size);
    if (left_size.x >= 120)
    {
        m_left_column_width = left_size.x;
    }
    ImGui::SameLine();

    ShowFilesOnFolder();
    ImGui::End();
}

void Hachiko::WindowProject::CleanUp() {}

//TODO
void Hachiko::WindowProject::DoFilter() {}

void Hachiko::WindowProject::CreateBreadCrumps()
{
    const char* c = all_assets.path.c_str();
    char tmp[260];

    while (*c)
    {
        char* c_out = tmp;
        while (*c && *c != '/')
        {
            *c_out = *c;
            ++c_out;
            ++c;
        }
        *c_out = '\0';
        if (*c == '/')
        {
            ++c;
        }
        if (ImGui::Button(tmp))
        {
            if (tmp != all_assets.localPath)
            {
                all_assets = FileSystem::GetAllFiles(tmp, nullptr, nullptr);
            }
        }
        ImGui::SameLine();
        ImGui::TextUnformatted("/");
        ImGui::SameLine();
    }
    ImGui::NewLine();
}

//TODO
void Hachiko::WindowProject::Thumbnail(PathNode& node, float size, bool selected)
{
    ImGui::BeginGroup();
    const ImVec2 img_size(size, size);

    ImGuiUtils::Rect(img_size.x, img_size.y, 0xffffFFFF);
    /* TODO */

    const ImVec2 text_size = ImGui::CalcTextSize(node.localPath.c_str());
    ImVec2 pos = ImGui::GetCursorPos();
    pos.x += (size - text_size.x) * 0.5f;
    ImGui::SetCursorPos(pos);
    ImGui::Text("%s", node.localPath.c_str());
    ImGui::EndGroup();
    if (selected)
    {
        ImDrawList* dl = ImGui::GetWindowDrawList();
        const unsigned int color = ImGui::ColorConvertFloat4ToU32(ImGui::GetStyle().Colors[ImGuiCol_ButtonActive]);
        dl->AddRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), color, 0, 0, 3.f);
    }
}

void Hachiko::WindowProject::ShowDir(PathNode& node)
{
    const ImVec2 size(std::max(120.f, m_left_column_width), 0);
    if (ImGui::BeginChild("left_col", size, false, ImGuiWindowFlags_NoNavInputs))
    {
        ImGui::PushItemWidth(120);

        PathNode new_root;
        bool selected_item = false;
        if ((node.path != ASSETS_FOLDER) && ImGui::Selectable("..", &selected_item))
        {
            const char* tmp = node.path.c_str();
            std::string previous_path;
            const std::string full(tmp);
            const size_t pos_last_separator = full.find_last_of("\\/");

            previous_path = full.substr(0, pos_last_separator);

            all_assets = FileSystem::GetAllFiles(previous_path.c_str(), nullptr, nullptr);

            ImGui::PopItemWidth();
            ImGui::EndChild();

            return;
        }

        for (auto& subdir : node.children)
        {
            if (!subdir.isFile)
            {
                if (ImGui::Selectable(subdir.localPath.c_str(), &selected_item))
                {
                    new_root = subdir;
                }
            }
        }

        if (selected_item)
        {
            all_assets = new_root;
        }

        ImGui::PopItemWidth();

        ImGui::EndChild();
    }
}

void Hachiko::WindowProject::ShowFilesOnFolder()
{
    if (ImGui::BeginChild("main_col",ImVec2(0,0), false, ImGuiWindowFlags_NoNavInputs))
    {
        const float w = ImGui::GetContentRegionAvail().x;
        int columns = static_cast<int>(w) / static_cast<int>(96 * 1.f);
        columns = std::max(columns, 1);
        int tile_count = all_assets.children.empty() ? all_assets.children.size() : 50;
        constexpr int row_count = 1;

        auto callbacks = [this](PathNode& node, int idx) {
            if (ImGui::IsItemHovered())
            {
                ImGui::SetTooltip("%s", node.path.c_str());
            }
            if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
            {
                ImGui::Text("%s", node.path.c_str());
                ImGui::SetDragDropPayload("path", node.path.c_str(), strlen(node.path.c_str()) + 1, ImGuiCond_Once);
                ImGui::EndDragDropSource();
            }
            else if (ImGui::IsItemHovered())
            {
                if (ImGui::IsMouseReleased(0))
                {
                    // selected_resource = find on list resource module resource
                }
                else if (ImGui::IsMouseReleased(1))
                {
                    ImGui::OpenPopup("item_ctx");
                }
                else if (ImGui::IsMouseDoubleClicked(0))
                {
                    // instanciate on current level;
                }
            }
        };

        ImGuiListClipper clipper;
        clipper.Begin(row_count);
        while (clipper.Step())
        {
            for (int j = clipper.DisplayStart; j < clipper.DisplayEnd; ++j)
            {
                for (int i = 0; i < columns; ++i)
                {
                    if (i > 0)
                    {
                        ImGui::SameLine();
                    }
                    const int idx = GetThumbnailIndex(i, j, columns);
                    if (idx < 0)
                    {
                        ImGui::NewLine();
                        break;
                    }
                    PathNode& node = all_assets.children[idx];
                    const bool is_resource = node.children.empty() && FileSystem::HasExtension(node.path.c_str());
                    if (is_resource)
                    {
                        // bool selected = find on resource and search match
                        Thumbnail(node, m_thumbnail_size * 70, false);
                        callbacks(node, idx);
                    }
                }
            }
        }
        clipper.End();

        bool open_delete_popup = false;
        static char tmp[260] = "";
        auto common_popup = [&]() {
            const char* base_path = nullptr;

            // TODO
            /*if (ImGui::BeginMenu("Create directory"))
        {
            /*ImGui::InputTextWithHint("##dirname", "New directory name", tmp, sizeof(tmp));
            ImGui::SameLine();
            if (ImGui::Button("Create"))
            {
                const char* new_dir = /* curent dir path + new dir path  nullptr;
                App->file_sys->ModuleFileSystem::CreateDir(new_dir);
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndMenu();
        }
        // TODO
        if (ImGui::MenuItem("Select all"))
        {
            
            ImGui::EndMenu();
        }*/
        };
        if (ImGui::BeginPopup("item_ctx"))
        {
            ImGui::Separator();

            open_delete_popup = ImGui::MenuItem("Delete");
            ImGui::Separator();
            common_popup();
            ImGui::EndPopup();
        }
        else if (ImGui::BeginPopupContextWindow("context"))
        {
            common_popup();
            ImGui::EndPopup();
        }

        if (open_delete_popup)
        {
            ImGui::OpenPopup("Delete file");
        }

        if (ImGui::BeginPopupModal("Delete file", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::Text("Are you sure? This can not be undone.");
            if (ImGui::Button("Yes, delete", ImVec2(100, 0)))
            {
                //TODO Delete resource no fucking idea how but i guess we need to destroy from library first
                ImGui::CloseCurrentPopup();
            }
            ImGui::SameLine(ImGui::GetWindowWidth() - 100 - ImGui::GetStyle().WindowPadding.x);
            if (ImGui::Button("Cancel", ImVec2(100, 0)))
            {
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }

        ImGui::EndChild();
    }
}

void Hachiko::WindowProject::GetAssets()
{
    std::vector<std::string> ignore_ext;
    ignore_ext.emplace_back("meta");
    all_assets = FileSystem::GetAllFiles(ASSETS_FOLDER, nullptr, &ignore_ext);
}

int Hachiko::WindowProject::GetThumbnailIndex(int i, int j, int columns) const
{
    const int idx = j * columns + i;
    if (!all_assets.children.empty())
    {
        if (idx >= all_assets.children.size())
        {
            return -1;
        }
        return idx;
    }
    if (idx >= all_assets.children.size())
    {
        return -1;
    }
    return idx;
}
