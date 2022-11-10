#include "core/hepch.h"
#include "ModuleProgram.h"

#include "utils/FileSystem.h"
#include "components/ComponentCamera.h"
#include "components/ComponentDirLight.h"
#include "components/ComponentPointLight.h"
#include "components/ComponentSpotLight.h"
#include "components/ComponentMeshRenderer.h"
#include "resources/ResourceMaterial.h"
#include "Batching/GeometryBatch.h"
#include "Batching/TextureBatch.h"

//TODO centralize cache on module program
Hachiko::ModuleProgram::ModuleProgram() = default;

Hachiko::ModuleProgram::~ModuleProgram() = default;

bool Hachiko::ModuleProgram::Init()
{
    HE_LOG("INITIALIZING MODULE: PROGRAM");

    CreatePrograms();

    for (unsigned i = 0; i < static_cast<int>(Program::Programs::COUNT); ++i)
    {
        if (!programs[i])
        {
            return false;
        }
    }

    CreateUBO(UBOPoints::CAMERA, sizeof(CameraData));
    CreateUBO(UBOPoints::LIGHTS, sizeof(Lights));

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

void Hachiko::ModuleProgram::CreateGLSLIncludes() const 
{
    PathNode path_node = App->file_system.GetAllFiles(SHADERS_FOLDER, nullptr, nullptr);
    std::vector<PathNode*> files_in_shaders_folder;
    path_node.GetFilesRecursively(files_in_shaders_folder);

    const std::string shaders_folder_name = SHADERS_FOLDER;

    for (auto child : files_in_shaders_folder)
    {
        std::string local_path = child->path;

        assert(("File is not in shaders folder: ", 
            local_path.substr(0, shaders_folder_name.size()) == shaders_folder_name));
        
        // Get the file path local to shaders/ folder:
        local_path = local_path.substr(shaders_folder_name.size());
        // Get the absolute file path:
        std::string path = StringUtils::Concat(App->file_system.GetWorkingDirectory(), "/" , child->path);

        // Get the file content:
        std::ifstream file_stream(path);
        std::stringstream file_buffer;
        file_buffer << file_stream.rdbuf();
        file_stream.close();

        // Add the file to GLSL virtual filesystem:
        glNamedStringARB(GL_SHADER_INCLUDE_ARB, -1, &(local_path.c_str()[0]), -1, file_buffer.str().c_str());
    }
}

void Hachiko::ModuleProgram::CreateUBO(UBOPoints binding_point, unsigned size)
{
    glGenBuffers(1, &buffers[static_cast<int>(binding_point)]);
    glBindBuffer(GL_UNIFORM_BUFFER, buffers[static_cast<int>(binding_point)]);
    glBufferData(GL_UNIFORM_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, static_cast<int>(binding_point), buffers[static_cast<int>(binding_point)]);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void Hachiko::ModuleProgram::UpdateUBO(UBOPoints binding_point, unsigned size, void* data, unsigned offset) const
{
    glBindBuffer(GL_UNIFORM_BUFFER, buffers[static_cast<int>(binding_point)]);
    glBufferSubData(GL_UNIFORM_BUFFER, offset, size, data);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void Hachiko::ModuleProgram::CreateSSBO(UBOPoints binding_point, unsigned size)
{
    glGenBuffers(1, &buffers[static_cast<int>(binding_point)]);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, buffers[static_cast<int>(binding_point)]);
    glBufferData(GL_SHADER_STORAGE_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, static_cast<int>(binding_point), buffers[static_cast<int>(binding_point)]);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void Hachiko::ModuleProgram::UpdateSSBO(UBOPoints binding_point, unsigned size, void* data, unsigned offset) const
{
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, buffers[static_cast<int>(binding_point)]);
    glBufferData(GL_SHADER_STORAGE_BUFFER, size, data, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void* Hachiko::ModuleProgram::CreatePersistentBuffers(unsigned& buffer_id, int binding_point, unsigned size)
{
    const unsigned flags = GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT;

    glGenBuffers(1, &buffer_id);
    glBindBuffer(GL_ARRAY_BUFFER, buffer_id);
    glBufferStorage(GL_ARRAY_BUFFER, size, 0, flags);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, binding_point, buffer_id); // CHECK THIS
    return glMapBufferRange(GL_ARRAY_BUFFER, 0, size, flags);
}

void Hachiko::ModuleProgram::CreatePrograms()
{
    CreateGLSLIncludes();

    programs[static_cast<int>(Program::Programs::FORWARD)] = CreateProgram(SHADERS_FOLDER "vertex.glsl", SHADERS_FOLDER "fragment_forward.glsl");
    programs[static_cast<int>(Program::Programs::GAUSSIAN_FILTERING)] = CreateProgram(SHADERS_FOLDER "vertex_gaussian_filter.glsl", SHADERS_FOLDER "fragment_gaussian_filter.glsl");
    programs[static_cast<int>(Program::Programs::DEFERRED_GEOMETRY)] = CreateProgram(SHADERS_FOLDER "vertex.glsl", SHADERS_FOLDER "fragment_deferred_geometry.glsl");
    programs[static_cast<int>(Program::Programs::DEFERRED_LIGHTING)] = CreateProgram(SHADERS_FOLDER "vertex_deferred_lighting.glsl", SHADERS_FOLDER "fragment_deferred_lighting.glsl");
    programs[static_cast<int>(Program::Programs::SHADOW_MAPPING)] = CreateProgram(SHADERS_FOLDER "vertex_shadow_mapping.glsl", SHADERS_FOLDER "fragment_shadow_mapping.glsl");
    programs[static_cast<int>(Program::Programs::SKYBOX)] = CreateProgram(SHADERS_FOLDER "vertex_skybox.glsl", SHADERS_FOLDER "fragment_skybox.glsl");
    programs[static_cast<int>(Program::Programs::DIFFUSE_IBL)] = CreateProgram(SHADERS_FOLDER "vertex_diffuseIBL.glsl", SHADERS_FOLDER "fragment_diffuseIBL.glsl");
    programs[static_cast<int>(Program::Programs::PREFILTERED_IBL)] = CreateProgram(SHADERS_FOLDER "vertex_prefilteredIBL.glsl", SHADERS_FOLDER "fragment_prefilteredIBL.glsl");
    programs[static_cast<int>(Program::Programs::ENVIRONMENT_BRDF)] = CreateProgram(SHADERS_FOLDER "vertex_environmentBRDF.glsl", SHADERS_FOLDER "fragment_environmentBRDF.glsl");
    programs[static_cast<int>(Program::Programs::STENCIL)] = CreateProgram(SHADERS_FOLDER "vertex_stencil.glsl", SHADERS_FOLDER "fragment_stencil.glsl");
    programs[static_cast<int>(Program::Programs::UI_IMAGE)] = CreateProgram(SHADERS_FOLDER "vertex_ui.glsl", SHADERS_FOLDER "fragment_ui.glsl");
    programs[static_cast<int>(Program::Programs::UI_TEXT)] = CreateProgram(SHADERS_FOLDER "vertex_font.glsl", SHADERS_FOLDER "fragment_font.glsl");
    programs[static_cast<int>(Program::Programs::PARTICLE)] = CreateProgram(SHADERS_FOLDER "vertex_particle.glsl", SHADERS_FOLDER "fragment_particle.glsl");
    programs[static_cast<int>(Program::Programs::VIDEO)] = CreateProgram(SHADERS_FOLDER "vertex_video.glsl", SHADERS_FOLDER "fragment_video.glsl");
    programs[static_cast<int>(Program::Programs::FOG)] = CreateProgram(SHADERS_FOLDER "vertex_deferred_lighting.glsl", SHADERS_FOLDER "fragment_fog.glsl");
    programs[static_cast<int>(Program::Programs::TEXTURE_COPY)] = CreateProgram(SHADERS_FOLDER "vertex_texture_copy.glsl", SHADERS_FOLDER "fragment_texture_copy.glsl");
    programs[static_cast<int>(Program::Programs::SSAO)] = CreateProgram(SHADERS_FOLDER "vertex_ssao.glsl", SHADERS_FOLDER "fragment_ssao.glsl");
    programs[static_cast<int>(Program::Programs::OUTLINE)] = CreateProgram(SHADERS_FOLDER "vertex_outline.glsl", SHADERS_FOLDER "fragment_outline.glsl");
    programs[static_cast<int>(Program::Programs::FXAA)] = CreateProgram(SHADERS_FOLDER "vertex_fxaa_filter.glsl", SHADERS_FOLDER "fragment_fxaa_filter.glsl");
}

void Hachiko::ModuleProgram::DeletePrograms()
{
    for (unsigned i = 0; i < static_cast<int>(Program::Programs::COUNT); ++i)
    {
        programs[i]->CleanUp();
        delete programs[i];
    }
}

void Hachiko::ModuleProgram::RecompilePrograms()
{
    DeletePrograms();
    CreatePrograms();
}

bool Hachiko::ModuleProgram::CleanUp()
{
    DeletePrograms();

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

void Hachiko::ModuleProgram::UpdateCamera(const Frustum& frustum) const
{
    CameraData camera_data;
    camera_data.view = float4x4(frustum.ViewMatrix());
    camera_data.proj = frustum.ProjectionMatrix();
    // TODO: Understand why camera_data.view.TranslatePart() does not give the position
    camera_data.pos = camera_data.view.RotatePart().Transposed().Transform(-camera_data.view.TranslatePart());

    UpdateUBO(UBOPoints::CAMERA, sizeof(CameraData), &camera_data);
}

void Hachiko::ModuleProgram::UpdateCamera(const CameraData& camera_data) const
{
    UpdateUBO(UBOPoints::CAMERA, sizeof(CameraData), (void*)&camera_data);
}

void Hachiko::ModuleProgram::UpdateMaterial(
    const ComponentMeshRenderer* component_mesh_renderer, 
    const Program* program) const
{
    static int texture_slots[static_cast<int>(TextureSlots::COUNT)] = {static_cast<int>(TextureSlots::DIFFUSE),
                                                                       static_cast<int>(TextureSlots::SPECULAR),
                                                                       static_cast<int>(TextureSlots::NORMAL),
                                                                       static_cast<int>(TextureSlots::METALNESS),
                                                                       static_cast<int>(TextureSlots::EMISSIVE)};
    program->BindUniformInts("textures", static_cast<int>(TextureSlots::COUNT), 
        &texture_slots[0]);

    const ResourceMaterial* material = component_mesh_renderer->GetResourceMaterial();

    if (material == nullptr)
    {
        return;
    }

    MaterialData material_data;
    material_data.diffuse_color = material->diffuse_color;
    material_data.diffuse_flag = material->HasDiffuse();
    material_data.specular_color = material->specular_color;
    material_data.smoothness = material->smoothness;
    material_data.metalness_value = material->metalness_value;
    material_data.specular_flag = material->HasSpecular();
    material_data.normal_flag = material->HasNormal();
    material_data.metalness_flag = material->HasMetalness();
    material_data.emissive_flag = material->HasEmissive();
    material_data.emissive_color = material->emissive_color;
    material_data.is_metallic = material->is_metallic;
    material_data.smoothness_alpha = 1; // default value
    material_data.is_transparent = material->is_transparent;

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
    if (material_data.metalness_flag)
    {
        ModuleTexture::Bind(material->GetMetalnessId(), static_cast<int>(TextureSlots::METALNESS));
    }
    if (material_data.emissive_flag)
    {
        ModuleTexture::Bind(material->GetEmissiveId(), static_cast<int>(TextureSlots::EMISSIVE));
    }

    UpdateUBO(UBOPoints::MATERIAL, sizeof(MaterialData), &material_data);
}

void Hachiko::ModuleProgram::UpdateLights(float ambient_intensity,
                                          const float4& ambient_color,
                                          const ComponentDirLight* dir_light,
                                          const std::vector<ComponentPointLight*>& point_lights,
                                          const std::vector<ComponentSpotLight*>& spot_lights) const
{
    Lights lights_data;
    // Ambient
    lights_data.ambient.intensity = ambient_intensity;
    lights_data.ambient.color = ambient_color;
    // Directional Lights
    if (dir_light && dir_light->IsActive() && dir_light->GetGameObject()->active)
    {
        lights_data.directional.direction = float4(dir_light->GetDirection(), 0.0f);
        lights_data.directional.color = dir_light->color;
        lights_data.directional.intensity = dir_light->intensity;
    }
    else
    {
        lights_data.directional.intensity = 0.0f;
    }

    // Point Lights
    lights_data.n_points = 0;
    for (const auto point_light : point_lights)
    {
        if (point_light->IsActive() && point_light->GetGameObject()->active)
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
        if (spot_light->IsActive() && spot_light->GetGameObject()->active)
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