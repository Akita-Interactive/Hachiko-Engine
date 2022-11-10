#include "core/hepch.h"
#include "core/Scene.h"
#include "core/GameObject.h"

#include "modules/ModuleUserInterface.h"
#include "modules/ModuleSceneManager.h"
#include "modules/ModuleProgram.h"
#include "modules/ModuleCamera.h"
#include "modules/ModuleInput.h"
#include "modules/ModuleEvent.h"
#include "modules/ModuleWindow.h"

#include "components/ComponentCanvas.h"
#include "components/ComponentCanvasRenderer.h"
#include "components/ComponentCamera.h"
#include "components/ComponentTransform2D.h"
#include "components/ComponentButton.h"

bool Hachiko::ModuleUserInterface::Init()
{
    HE_LOG("INITIALIZING MODULE: USER INTERFACE");

    CreateSquare();

    std::function handle_mouse_action = [&](Event& evt) {
        const auto& payload = evt.GetEventData<MouseEventPayload>();
        const auto action = payload.GetAction();

        HandleMouseAction(action);
    };

    App->event->Subscribe(Event::Type::MOUSE_ACTION, handle_mouse_action);

    return true;
}

UpdateStatus Hachiko::ModuleUserInterface::Update(const float delta)
{
    // Returns OpenGL mouse position local to the program window if the engine
    // was built with PLAY_BUILD flag. Returns OpenGL mouse position local to
    // scene window otherwise:
    const float2 mouse_pos = Input::GetMouseOpenGLPosition();
    constexpr bool is_click = false;

    if(!App->input->IsGamepadModeOn())
    {
        RecursiveCheckMousePos(
            App->scene_manager->GetActiveScene()->GetRoot(), 
            mouse_pos, 
            is_click);
    }

    return UpdateStatus::UPDATE_CONTINUE;
}

bool Hachiko::ModuleUserInterface::CleanUp()
{
    RemoveSquare();
    return true;
}

void Hachiko::ModuleUserInterface::DrawUI(const Scene* scene) const
{
    Program* img_program = App->program->GetProgram(Program::Programs::UI_IMAGE);
    Program* txt_program = App->program->GetProgram(Program::Programs::UI_TEXT);

    glDepthFunc(GL_ALWAYS);

    unsigned width, height;
    App->camera->GetRenderingCamera()->GetResolution(width, height);

    ModuleProgram::CameraData camera_data;
    // position data is unused on the ui program
    camera_data.pos = float3::zero;
    camera_data.view = float4x4::identity;
    camera_data.proj = float4x4::D3DOrthoProjLH(
        -1, 
        1, 
        static_cast<float>(width), 
        static_cast<float>(height));

    App->program->UpdateCamera(camera_data);
    RecursiveDrawUI(nullptr, scene->GetRoot(), img_program, txt_program);
    UnbindSquare();
    glDepthFunc(GL_LESS);
}

void Hachiko::ModuleUserInterface::RecursiveDrawUI(
    ComponentCanvas* canvas,
    GameObject* __restrict game_object, 
    Program* img_program, 
    Program* txt_program)
{
    if (!game_object->IsActive())   return;
    ComponentCanvas* new_canvas = game_object->GetComponent<ComponentCanvas>();
    if (new_canvas)
    {
        canvas = new_canvas;
    }
    
    if (canvas)
    {
        const ComponentCanvasRenderer* renderer = 
            game_object->GetComponent<ComponentCanvasRenderer>();

        if (renderer && game_object->IsActive() && renderer->IsActive())
        {
            renderer->Render(canvas, img_program, txt_program);
        }
    }

    for (GameObject* child : game_object->children)
    {
        RecursiveDrawUI(canvas, child, img_program, txt_program);
    }
}

void Hachiko::ModuleUserInterface::RecursiveCheckMousePos(
    GameObject* game_object, 
    const float2& mouse_pos, 
    const bool is_click)
{
    // If it is not click it is considered a hover
    const ComponentTransform2D* transform = 
        game_object->GetComponent<ComponentTransform2D>();
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

void Hachiko::ModuleUserInterface::HandleMouseAction(
    const MouseEventPayload::Action mouse_action)
{
    if (mouse_action != MouseEventPayload::Action::CLICK)
    {
        return;
    }

    RecursiveCheckMousePos(
        App->scene_manager->GetActiveScene()->GetRoot(), 
        Input::GetMouseOpenGLPosition(), // Click position.
        true); // Is Click: true as we only handle Click action for now.
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

void Hachiko::ModuleUserInterface::BindSquare() const
{
    glBindVertexArray(vao);
}

void Hachiko::ModuleUserInterface::UnbindSquare() const
{
    glBindVertexArray(0);
}

void Hachiko::ModuleUserInterface::RemoveSquare()
{
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ebo);
}