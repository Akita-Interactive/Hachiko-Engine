#include "core/hepch.h"
#include "ModuleUserInterface.h"
#include "ModuleSceneManager.h"
#include "ModuleProgram.h"
#include "ModuleCamera.h"
#include "ModuleEditor.h"
#include "ModuleInput.h"
#include "ModuleEvent.h"
#include "ModuleWindow.h"

#include "core/Scene.h"
#include "core/GameObject.h"
#include "components/ComponentCanvasRenderer.h"
#include "components/ComponentCamera.h"
#include "components/ComponentTransform2D.h"
#include "components/ComponentButton.h"

#include "ui/WindowScene.h"

Hachiko::ModuleUserInterface::ModuleUserInterface() = default;

Hachiko::ModuleUserInterface::~ModuleUserInterface() = default;

bool Hachiko::ModuleUserInterface::Init()
{
    CreateSquare();
    std::function handle_mouse_action = [&](Event& evt) { HandleMouseAction(evt); };
    App->event->Subscribe(Event::Type::MOUSE_ACTION, handle_mouse_action);
    return true;
}

UpdateStatus Hachiko::ModuleUserInterface::Update(float delta)
{
    // On playbuild mouse position is taken from the Window, not ImGUI
#ifdef PLAY_BUILD   
    int height, width;
    App->window->GetWindowSize(width, height);
    
    float2 mouse_pos = Input::GetMousePosition();
    // We center the mouse:
    mouse_pos = float2(mouse_pos.x - 0.5f, (mouse_pos.y - 0.5f) * -1); 
     // We scale it from normalizedL
    mouse_pos = float2(mouse_pos.x * width, mouse_pos.y * height);
    RecursiveCheckMousePos(App->scene_manager->GetActiveScene()->GetRoot(), 
        mouse_pos, false);
#else
    const WindowScene* w_scene = App->editor->GetSceneWindow();
    
    ImVec2 mouse_pos = ImGui::GetMousePos();
    float2 click_pos = w_scene->ImguiToScreenPos(
        float2(mouse_pos.x, mouse_pos.y));
    
    RecursiveCheckMousePos(App->scene_manager->GetActiveScene()->GetRoot(), 
        click_pos, false);
#endif

    return UpdateStatus::UPDATE_CONTINUE;
}

bool Hachiko::ModuleUserInterface::CleanUp()
{
    RemoveSquare();
    return true;
}

void Hachiko::ModuleUserInterface::DrawUI(const Scene* scene)
{
    Program* img_program = App->program->GetUserInterfaceImageProgram();
    Program* txt_program = App->program->GetUserInterfaceTextProgram();

    glDepthFunc(GL_ALWAYS);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    unsigned width, height;
    App->camera->GetMainCamera()->GetResolution(width, height);

    ModuleProgram::CameraData camera_data;
    // position data is unused on the ui program
    camera_data.pos = float3::zero;
    camera_data.view = float4x4::identity;
    camera_data.proj = float4x4::D3DOrthoProjLH(-1, 1, static_cast<float>(width), static_cast<float>(height));

    App->program->UpdateCamera(camera_data);
    RecursiveDrawUI(scene->GetRoot(), img_program, txt_program);
    UnbindSuare();
    glDepthFunc(GL_LESS);
}

void Hachiko::ModuleUserInterface::RecursiveDrawUI(GameObject* game_object, Program* img_program, Program* txt_program)
{
    ComponentCanvasRenderer* renderer = game_object->GetComponent<ComponentCanvasRenderer>();

    if (renderer && game_object->IsActive() && renderer->IsActive())
    {
        renderer->Render(img_program, txt_program);
    }

    for (GameObject* child : game_object->children)
    {
        RecursiveDrawUI(child, img_program, txt_program);
    }
}

void Hachiko::ModuleUserInterface::RecursiveCheckMousePos(GameObject* game_object, const float2& mouse_pos, bool is_click)
{
    // If it is not click it is considered a hover
    ComponentTransform2D* transform = game_object->GetComponent<ComponentTransform2D>();
    // Find selectables TODO: Improve how this is handled
    ComponentButton* selectable = game_object->GetComponent<ComponentButton>();
    if (transform && selectable)
    {
        selectable->OnUnSelect();

        if (transform->Intersects(mouse_pos))
        {
            selectable->OnPointerEnter();
            if (is_click)
            {
                selectable->Activate();
                selectable->OnSelect();
            }
        }
        else
        {
            selectable->OnPointerExit();
        }
    }
    for (GameObject* child : game_object->children)
    {
        RecursiveCheckMousePos(child, mouse_pos, is_click);
    }
}

void Hachiko::ModuleUserInterface::HandleMouseAction(Hachiko::Event& evt)
{
    const auto& payload = evt.GetEventData<MouseEventPayload>();
    MouseEventPayload::Action action = payload.GetAction();
    float2 coords = payload.GetCoords();

#ifdef PLAY_BUILD
    int height, width;
    App->window->GetWindowSize(width, height);
    
    float2 mouse_pos = Input::GetMousePosition();
    // We center the mouse:
    mouse_pos = float2(mouse_pos.x - 0.5f, (mouse_pos.y - 0.5f) * -1);
    // We scale it from normalized:
    mouse_pos = float2(mouse_pos.x * width, mouse_pos.y * height);
    
    constexpr bool is_click = true;
    RecursiveCheckMousePos(App->scene_manager->GetActiveScene()->GetRoot(), mouse_pos, is_click);
#else
    const WindowScene* w_scene = App->editor->GetSceneWindow();
    float2 click_pos = w_scene->ImguiToScreenPos(coords);

    constexpr bool is_click = true;
    RecursiveCheckMousePos(App->scene_manager->GetActiveScene()->GetRoot(), click_pos, is_click);
#endif
}

void Hachiko::ModuleUserInterface::CreateSquare()
{
    float positions[] = {
        0.5f,  0.5f,  0.0f, 1.0f, 1.0f, // top right
        0.5f,  -0.5f, 0.0f, 1.0f, 0.0f, // bottom right
        -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, // bottom left
        -0.5f, 0.5f,  0.0f, 0.0f, 1.0f // top left
    };

    unsigned int indices[] = {2, 1, 0, 0, 3, 2};

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);

    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Positions
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (void*)0);
    glEnableVertexAttribArray(0);

    // Texture coords
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    //HE_LOG("QUAD VAO %d %d %d", vao, vbo, ebo);
    glBindVertexArray(0);
}

void Hachiko::ModuleUserInterface::BindSquare()
{
    glBindVertexArray(vao);
}

void Hachiko::ModuleUserInterface::UnbindSuare()
{
    glBindVertexArray(0);
}

void Hachiko::ModuleUserInterface::RemoveSquare()
{
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ebo);
}