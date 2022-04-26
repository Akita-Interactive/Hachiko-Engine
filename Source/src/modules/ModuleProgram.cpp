#include "core/hepch.h"
#include "ModuleProgram.h"

#include "components/ComponentCamera.h"
#include "components/ComponentDirLight.h"
#include "components/ComponentPointLight.h"
#include "components/ComponentSpotLight.h"
#include "components/ComponentMaterial.h"
#include "resources/ResourceMaterial.h"

Hachiko::ModuleProgram::ModuleProgram() = default;

Hachiko::ModuleProgram::~ModuleProgram() = default;

bool Hachiko::ModuleProgram::Init()
{
    CreateMainProgram();
    CreateSkyboxProgram();
    CreateStencilProgram();
    CreateUserInterfaceImageProgram();
    CreateUserInterfaceTextProgram();
    if (!main_program || !skybox_program || !stencil_program || !ui_image_program || !ui_text_program)
    {
        return false;
    }

    CreateCameraUBO();
    CreateMaterialUBO();
    CreateLightsUBO();
    return true;
}

char* Hachiko::ModuleProgram::LoadShaderSource(const char* shader_file_name)
{
    char* data = nullptr;
    FILE* file = nullptr;
    fopen_s(&file, shader_file_name, "rb");
    if (file)
    {
        fseek(file, 0, SEEK_END);
        const int size = ftell(file);
        data = static_cast<char*>(malloc(size + 1));
        fseek(file, 0, SEEK_SET);
        fread(data, 1, size, file);
        data[size] = 0;
        fclose(file);
    }
    return data;
}

unsigned int Hachiko::ModuleProgram::CompileShader(unsigned type, const char* source) const
{
    const unsigned shader_id = glCreateShader(type);
    glShaderSource(shader_id, 1, &source, nullptr);
    glCompileShader(shader_id);
    int res = GL_FALSE;
    glGetShaderiv(shader_id, GL_COMPILE_STATUS, &res);
    if (res == GL_FALSE)
    {
        int len = 0;
        glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &len);
        if (len > 0)
        {
            int written = 0;
            const auto info = static_cast<char*>(malloc(len));
            glGetShaderInfoLog(shader_id, len, &written, info);
            HE_LOG("Log Info: %s", info);
            free(info);
        }
        return 0;
    }
    return shader_id;
}

void Hachiko::ModuleProgram::CompileShaders(const char* vtx_shader_path, const char* frg_shader_path, unsigned& vtx_shader, unsigned& frg_shader) const
{
    const char* vertex_source = LoadShaderSource(vtx_shader_path);
    const char* fragment_source = LoadShaderSource(frg_shader_path);

    vtx_shader = CompileShader(GL_VERTEX_SHADER, vertex_source);
    frg_shader = CompileShader(GL_FRAGMENT_SHADER, fragment_source);

    delete vertex_source;
    delete fragment_source;
}

Hachiko::Program* Hachiko::ModuleProgram::CreateProgram(const char* vtx_shader_path, const char* frg_shader_path)
{
    unsigned fragment_shader_id;
    unsigned vertex_shader_id;
    CompileShaders(vtx_shader_path, frg_shader_path, vertex_shader_id, fragment_shader_id);

    if (vertex_shader_id == 0 || fragment_shader_id == 0)
        return nullptr;

    const auto program = new Program(vertex_shader_id, fragment_shader_id);

    if (program->GetId() == 0)
    {
        delete program;
        return nullptr;
    }

    glDeleteShader(vertex_shader_id);
    glDeleteShader(fragment_shader_id);

    return program;
}

Hachiko::Program* Hachiko::ModuleProgram::CreateMainProgram()
{
    main_program = CreateProgram(ASSETS_FOLDER "/shaders/vertex.glsl", ASSETS_FOLDER "/shaders/fragment.glsl");
    return main_program;
}

Hachiko::Program* Hachiko::ModuleProgram::CreateSkyboxProgram()
{
    skybox_program = CreateProgram(ASSETS_FOLDER "/shaders/vertex_skybox.glsl", ASSETS_FOLDER "/shaders/fragment_skybox.glsl");
    return skybox_program;
}

Hachiko::Program* Hachiko::ModuleProgram::CreateStencilProgram()
{
    stencil_program = CreateProgram(ASSETS_FOLDER "/shaders/vertex_stencil.glsl", ASSETS_FOLDER "/shaders/fragment_stencil.glsl");
    return stencil_program;
}

Hachiko::Program* Hachiko::ModuleProgram::CreateUserInterfaceImageProgram()
{
    ui_image_program = CreateProgram(ASSETS_FOLDER "/shaders/vertex_ui.glsl", ASSETS_FOLDER "/shaders/fragment_ui.glsl");
    return ui_image_program;
}

Hachiko::Program* Hachiko::ModuleProgram::CreateUserInterfaceTextProgram()
{
    ui_text_program = CreateProgram(ASSETS_FOLDER "/Shaders/vertex_font.glsl", ASSETS_FOLDER "/Shaders/fragment_font.glsl");
    return ui_text_program;
}

void Hachiko::ModuleProgram::CreateUBO(UBOPoints binding_point, unsigned size)
{
    glGenBuffers(1, &ubos[static_cast<int>(binding_point)]);
    glBindBuffer(GL_UNIFORM_BUFFER, ubos[static_cast<int>(binding_point)]);
    glBufferData(GL_UNIFORM_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, static_cast<int>(binding_point), ubos[static_cast<int>(binding_point)]);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void Hachiko::ModuleProgram::UpdateUBO(UBOPoints binding_point, unsigned size, void* data, unsigned offset) const
{
    glBindBuffer(GL_UNIFORM_BUFFER, ubos[static_cast<int>(binding_point)]);
    glBufferSubData(GL_UNIFORM_BUFFER, offset, size, data);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void Hachiko::ModuleProgram::CreateCameraUBO()
{
    CreateUBO(UBOPoints::CAMERA, sizeof(CameraData));
}

void Hachiko::ModuleProgram::CreateMaterialUBO()
{
    CreateUBO(UBOPoints::MATERIAL, sizeof(MaterialData));
}

void Hachiko::ModuleProgram::CreateLightsUBO()
{
    CreateUBO(UBOPoints::LIGHTS, sizeof(Lights));
}

bool Hachiko::ModuleProgram::CleanUp()
{
    main_program->CleanUp();
    delete main_program;
    skybox_program->CleanUp();
    delete skybox_program;
    stencil_program->CleanUp();
    delete stencil_program;
    ui_image_program->CleanUp();
    delete ui_image_program;
    ui_text_program->CleanUp();
    delete ui_text_program;
    return true;
}

void Hachiko::ModuleProgram::UpdateCamera(const ComponentCamera* camera) const
{
    CameraData camera_data;
    camera_data.view = camera->GetViewMatrix();
    camera_data.proj = camera->GetProjectionMatrix();
    // TODO: Understand why camera_data.view.TranslatePart() does not give the position
    camera_data.pos = camera_data.view.RotatePart().Transposed().Transform(-camera_data.view.TranslatePart());

    UpdateUBO(UBOPoints::CAMERA, sizeof(CameraData), &camera_data);
}

void Hachiko::ModuleProgram::UpdateCamera(const CameraData& camera_data) const
{
    UpdateUBO(UBOPoints::CAMERA, sizeof(CameraData), (void*) &camera_data);
}

void Hachiko::ModuleProgram::UpdateMaterial(const ComponentMaterial* material_comp) const
{
    static int texture_slots[static_cast<int>(TextureSlots::COUNT)] = {static_cast<int>(TextureSlots::DIFFUSE), static_cast<int>(TextureSlots::SPECULAR), static_cast<int>(TextureSlots::NORMAL)};
    main_program->BindUniformInts("textures", static_cast<int>(TextureSlots::COUNT), &texture_slots[0]);

    const ResourceMaterial* material = material_comp->GetMaterial();

    if (material == nullptr)
    {
        return;
    }

    MaterialData material_data;
    material_data.diffuse_color = material->diffuse_color;
    material_data.diffuse_flag = material->HasDiffuse();
    material_data.specular_color = material->specular_color;
    material_data.specular_flag = material->HasSpecular();
    material_data.normal_flag = material->HasNormal();
    material_data.shininess = material->shininess;

    if (material_data.diffuse_flag)
    {
        ModuleTexture::Bind(material->GetDiffuseId(), static_cast<int>(TextureSlots::DIFFUSE));
    }
    if (material_data.specular_flag)
    {
        ModuleTexture::Bind(material->GetSpecularId(), static_cast<int>(TextureSlots::SPECULAR));
    }
    if (material_data.normal_flag)
    {
        ModuleTexture::Bind(material->GetNomalId(), static_cast<int>(TextureSlots::NORMAL));
    }

    UpdateUBO(UBOPoints::MATERIAL, sizeof(MaterialData), &material_data);
}

void Hachiko::ModuleProgram::UpdateLights(const ComponentDirLight* dir_light, const std::vector<ComponentPointLight*>& point_lights, const std::vector<ComponentSpotLight*>& spot_lights) const
{
    Lights lights_data;
    // Ambient
    lights_data.ambient = ambient_light;
    // DirectionalS
    if (dir_light && dir_light->IsActive())
    {
        lights_data.directional.direction = float4(dir_light->GetDirection(), 0.0f);
        lights_data.directional.color = dir_light->color;
        lights_data.directional.intensity = dir_light->intensity;
    }
    else
    {
        lights_data.directional.intensity = 0.0f;
    }

    // Point
    lights_data.n_points = 0;
    for (const auto point_light : point_lights)
    {
        if (point_light->IsActive())
        {
            lights_data.points[lights_data.n_points].position = float4(point_light->GetPosition(), 0.0f);
            lights_data.points[lights_data.n_points].color = point_light->color;
            lights_data.points[lights_data.n_points].intensity = point_light->intensity;
            lights_data.points[lights_data.n_points].radius = point_light->radius;
            ++lights_data.n_points;
            if (lights_data.n_points == MAX_POINT_LIGHTS)
            {
                break;
            }
        }
    }
    // Spot
    lights_data.n_spots = 0;
    for (const auto spot_light : spot_lights)
    {
        if (spot_light->IsActive())
        {
            lights_data.spots[lights_data.n_spots].position = float4(spot_light->GetPosition(), 0.0f);
            lights_data.spots[lights_data.n_spots].direction = float4(spot_light->GetDirection(), 0.0f);
            lights_data.spots[lights_data.n_spots].color = spot_light->color;
            lights_data.spots[lights_data.n_spots].inner = DegToRad(spot_light->inner);
            lights_data.spots[lights_data.n_spots].outer = DegToRad(spot_light->outer);
            lights_data.spots[lights_data.n_spots].intensity = spot_light->intensity;
            lights_data.spots[lights_data.n_spots].radius = spot_light->radius;
            ++lights_data.n_spots;
            if (lights_data.n_spots == MAX_SPOT_LIGHTS)
            {
                break;
            }
        }
    }
    UpdateUBO(UBOPoints::LIGHTS, sizeof(Lights), &lights_data);
}

void Hachiko::ModuleProgram::OptionsMenu()
{
    ImGui::PushItemWidth(100.0f);
    ImGui::Text("Ambient Light");
    ImGui::InputFloat("Intensity", &ambient_light.intensity);
    ImGuiUtils::CompactColorPicker("Color", &ambient_light.color[0]);
    ImGui::PopItemWidth();
}
