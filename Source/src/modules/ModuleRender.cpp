#include "core/hepch.h"
#include "core/rendering/Uniforms.h"
#include "core/preferences/src/EditorPreferences.h"

#include "ModuleRender.h"
#include "ModuleWindow.h"
#include "ModuleProgram.h"
#include "ModuleCamera.h"
#include "ModuleDebugDraw.h"
#include "ModuleSceneManager.h"
#include "ModuleEditor.h"
#include "ModuleUserInterface.h"
#include "ModuleNavigation.h"
#include "ModuleInput.h"

#include "Batching/BatchManager.h"

#include "core/GameObject.h"
#include "components/ComponentCamera.h"
#include "components/ComponentTransform2D.h"
#include "components/ComponentDirLight.h"
#include "components/ComponentImage.h"
#include "components/ComponentTransform.h"
#include "components/ComponentVideo.h"
#include "core/rendering/StandaloneGLTexture.h"

#ifdef _DEBUG
#include "core/ErrorHandler.h"
#endif

Hachiko::ModuleRender::ModuleRender() = default;

Hachiko::ModuleRender::~ModuleRender() = default;

bool Hachiko::ModuleRender::Init()
{
    HE_LOG("INITIALIZING MODULE: RENDER");

    CreateContext();

    RetrieveGpuInfo();
    RetrieveLibVersions();

    SetGLOptions();

    GenerateFullScreenQuad();
    GenerateFrameBuffer();

    shadow_manager.GenerateShadowMap();

    bloom_manager.Initialize();

    ssao_manager.SetupSSAO(fb_width, fb_height);

    outline_texture = new StandaloneGLTexture(
        fb_width, 
        fb_height, 
        GL_RGBA, 
        0, 
        GL_RGBA, 
        GL_FLOAT, 
        GL_NEAREST, 
        GL_NEAREST, 
        true);
    outline_texture_temp = new StandaloneGLTexture(
        fb_width, 
        fb_height, 
        GL_RGBA, 
        0, 
        GL_RGBA, 
        GL_FLOAT, 
        GL_NEAREST, 
        GL_NEAREST, 
        true);
    pre_post_process_frame_buffer = new StandaloneGLTexture(
        fb_width, 
        fb_height, 
        GL_RGBA, 
        0, 
        GL_RGBA, 
        GL_FLOAT, 
        GL_LINEAR, 
        GL_LINEAR, 
        true);

#ifdef _DEBUG
    glEnable(GL_DEBUG_OUTPUT); // Enable output callback
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(&ErrorHandler::HandleOpenGlError, nullptr); // Set the callback
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, true); // Filter notifications
#endif

    fps_log = std::vector<float>(n_bins);
    ms_log = std::vector<float>(n_bins);

    EditorPreferences* __restrict editor_preferences = 
        App->preferences->GetEditorPreference();

    draw_skybox = editor_preferences->GetDrawSkybox();
    draw_navmesh = editor_preferences->GetDrawNavmesh();
    shadow_pass_enabled = editor_preferences->GetShadowPassEnabled();

    shadow_manager.SetGaussianBlurringEnabled(
        editor_preferences->GetShadowMapGaussianBlurringEnabled());

    ssao_enabled = editor_preferences->GetSSAOEnabled();

    // Create noise texture for general use (dissolve effect)
    CreateNoiseTexture();

    return true;
}

float2 Hachiko::ModuleRender::GetFrameBufferSize() const
{
    return float2(fb_width, fb_height);
}

void Hachiko::ModuleRender::GenerateFrameBuffer()
{
    fb_width = 800;
    fb_height = 600;
    fb_width_inverse = 1.0f / static_cast<float>(fb_width);
    fb_height_inverse = 1.0f / static_cast<float>(fb_height);

    glGenFramebuffers(1, &frame_buffer);
    glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer);

    glGenTextures(1, &fb_texture);
    glBindTexture(GL_TEXTURE_2D, fb_texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, fb_width, fb_height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    glBindTexture(GL_TEXTURE_2D, 0);

    //Depth and stencil buffer
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fb_texture, 0);

    glGenRenderbuffers(1, &depth_stencil_buffer);
    glBindRenderbuffer(GL_RENDERBUFFER, depth_stencil_buffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, fb_width, fb_height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, depth_stencil_buffer);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        HE_LOG("Error creating frame buffer");
    }

    if (draw_deferred)
    {
        // Generate G-Buffer and associated textures:
        g_buffer.Generate();
    }

    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Hachiko::ModuleRender::ResizeFrameBuffer(const int width, const int height) const
{
    // Frame buffer texture:
    glBindTexture(GL_TEXTURE_2D, fb_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

    // Unbind:
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindRenderbuffer(GL_RENDERBUFFER, depth_stencil_buffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

void Hachiko::ModuleRender::ManageResolution(const ComponentCamera* camera)
{
    unsigned res_x, res_y;
    camera->GetResolution(res_x, res_y);

    if (res_x == fb_height && res_y == fb_width)
    {
        return;
    }

    // Resize frame buffer:
    ResizeFrameBuffer(res_x, res_y);
    glViewport(0, 0, res_x, res_y);

    // Cache width and height:
    fb_width = res_x;
    fb_height = res_y;

    // Cache inverses of width and height:
    fb_width_inverse = 1.0f / static_cast<float>(fb_width);
    fb_height_inverse = 1.0f / static_cast<float>(fb_height);

    // Resize textures of g-buffer:
    g_buffer.Resize(fb_width, fb_height);

    // Resize textures of bloom:
    bloom_manager.Resize(fb_width, fb_height);

    // Resize SSAO texture:
    ssao_manager.ResizeSSAO(fb_width, fb_height);

    // Resize outline texture:
    outline_texture->Resize(fb_width, fb_height);
    outline_texture_temp->Resize(fb_width, fb_height);

    // Resize pre post process frame buffer texture:
    pre_post_process_frame_buffer->Resize(fb_width, fb_height);
}

void Hachiko::ModuleRender::CreateContext()
{
    HE_LOG("Creating Renderer context");

    context = SDL_GL_CreateContext(App->window->GetWindow());
    GLenum err = glewInit();

    int value = 0;
    SDL_GL_GetAttribute(SDL_GL_DEPTH_SIZE, &value);
}

void Hachiko::ModuleRender::SetGLOptions()
{
    glEnable(GL_DEPTH_TEST); // Enable depth test
    glEnable(GL_CULL_FACE); // Enable cull backward faces
    glFrontFace(GL_CCW); // Front faces will be counter clockwise
    glEnable(GL_STENCIL_TEST); // Enable stencil test
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE); // Only replace stencil value if stencil and depth tests pass
}

UpdateStatus Hachiko::ModuleRender::PreUpdate(const float delta)
{
    render_list.PreUpdate();

    return UpdateStatus::UPDATE_CONTINUE;
}

UpdateStatus Hachiko::ModuleRender::Update(const float delta)
{
    ComponentCamera* camera = App->camera->GetRenderingCamera();
    Scene* active_scene = App->scene_manager->GetActiveScene();

#ifdef PLAY_BUILD
    int width = 0; 
    int height = 0;
    App->window->GetWindowSize(width, height);
    
    glViewport(0, 0, width, height);
    
    App->camera->OnResize(width, height);
#endif

    ManageResolution(camera);
    glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer);
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    if (active_scene == nullptr)
    {
        return UpdateStatus::UPDATE_CONTINUE;
    }

    active_scene->RebuildBatching();
    App->program->UpdateCamera(camera);

#ifdef PLAY_BUILD
    ComponentCamera* culling = App->camera->GetRenderingCamera();
#else
    ComponentCamera* culling = active_scene->GetCullingCamera();
#endif

    const ComponentDirLight* dir_light = nullptr;
    if (!active_scene->dir_lights.empty())
        dir_light = active_scene->dir_lights[0];
    const Scene::AmbientLightConfig& ambient_light = active_scene->GetAmbientLightConfig();
    App->program->UpdateLights(ambient_light.intensity,
                               ambient_light.color,
                               dir_light,
                               active_scene->point_lights,
                               active_scene->spot_lights);

    Draw(App->scene_manager->GetActiveScene(), camera, culling);

    EnableBlending();

    GLint polygonMode[2];
    glGetIntegerv(GL_POLYGON_MODE, polygonMode);
    if (polygonMode[0] == GL_LINE)
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        App->ui->DrawUI(active_scene);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
    else
    {
        App->ui->DrawUI(active_scene);
    }

    DisableBlending();

    // If in play build, blit frame_buffer to the default frame buffer and render to the whole 
    // screen, if not, bind default frame buffer:
#ifndef PLAY_BUILD
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
#else
    glBindFramebuffer(GL_READ_FRAMEBUFFER, frame_buffer);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glBlitFramebuffer(0, 0, fb_width, fb_height, 0, 0, fb_width, fb_height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
#endif

    return UpdateStatus::UPDATE_CONTINUE;
}

void Hachiko::ModuleRender::Draw(Scene* scene,
                                 ComponentCamera* camera,
                                 ComponentCamera* culling)
{
    OPTICK_CATEGORY("Draw", Optick::Category::Rendering);

    BatchManager* batch_manager = scene->GetBatchManager();

    DrawDeferred(scene, camera, batch_manager);
}

void Hachiko::ModuleRender::RunFXAA()
{
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, frame_buffer);

    Program* program;
    if (anti_aliasing_enabled)
    {
        program = App->program->GetProgram(Program::Programs::FXAA);
        program->Activate();

        program->BindUniformFloat(
            Uniforms::FXAA::TEXTURE_WIDTH_INVERSE, 
            &fb_width_inverse);
        program->BindUniformFloat(
            Uniforms::FXAA::TEXTURE_HEIGHT_INVERSE, 
            &fb_height_inverse);
    }
    else
    {
        program = App->program->GetProgram(Program::Programs::TEXTURE_COPY);
        program->Activate();
    }

    EnableBlending();
    pre_post_process_frame_buffer->BindForReading(0);
    RenderFullScreenQuad();
    glBindVertexArray(0);
    DisableBlending();

    Program::Deactivate();
}

void Hachiko::ModuleRender::DrawDeferred(Scene* scene,
                                         ComponentCamera* camera,
                                         BatchManager* batch_manager)
{
    Program* program = nullptr;
    glStencilMask(0x00);

#ifdef _DEBUG
    if (App->input->GetKey(SDL_SCANCODE_F5) == KeyState::KEY_DOWN)
    {
        deferred_mode = (deferred_mode + 1) % 7;
    }

    if (App->input->GetKey(SDL_SCANCODE_F4) == KeyState::KEY_DOWN)
    {
        render_forward_pass = !render_forward_pass;
    }
#endif

    if (shadow_pass_enabled)
    {
        // Generate shadow map from the scene:
        DrawToShadowMap(scene, batch_manager, DRAW_CONFIG_OPAQUE | DRAW_CONFIG_TRANSPARENT);
    }

    render_list.Update(scene->GetCullingCamera()->GetFrustum(), scene->GetQuadtree());

    // ----------------------------- GEOMETRY PASS ----------------------------

    glViewport(0, 0, fb_width, fb_height);
    g_buffer.BindForDrawing();
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Disable blending for deferred rendering as the meshes with transparent
    // materials are gonna be rendered with forward rendering after the
    // deferred lighting pass:
    glDisable(GL_BLEND);

    // Clear Opaque Batches List:
    batch_manager->ClearOpaqueBatchesDrawList();

    // Send mesh renderers with opaque materials to batch manager draw list:
    for (const RenderTarget& target : render_list.GetOpaqueTargets())
    {
        batch_manager->AddDrawComponent(target.mesh_renderer);
    }

    // Draw collected meshes with geometry pass program:
    program = App->program->GetProgram(Program::Programs::DEFERRED_GEOMETRY);

    program->Activate();

    // Binding the noise texture
    BindNoiseTexture(program);

    batch_manager->DrawOpaqueBatches(program);
    Program::Deactivate();

    // --------------------------- PRE LIGHT PASS -----------------------------

    if (ssao_enabled)
    {
        ssao_manager.DrawSSAO(
            g_buffer, 
            scene->GetCullingCamera()->GetFrustum().ViewMatrix(), 
            scene->GetCullingCamera()->GetFrustum().ProjectionMatrix(), 
            fb_width, 
            fb_height);
    }

    // Read the emissive texture, copy it and blur it band write to another
    // texture:
    bloom_manager.ApplyBloom(g_buffer.GetEmissiveTexture());

    const bool drew_outlines = DrawOutlines(batch_manager);

    // ------------------------------ LIGHT PASS ------------------------------

    glBindFramebuffer(
        GL_DRAW_FRAMEBUFFER, 
        pre_post_process_frame_buffer->GetFrameBufferId());
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Use Deferred rendering lighting pass program:
    program = App->program->GetProgram(Program::Programs::DEFERRED_LIGHTING);
    program->Activate();

    // Bind shadow specific necessary uniforms for lighting pass:
    shadow_manager.BindLightingPassUniforms(program);

    // Bind ImageBasedLighting uniforms
    scene->GetSkybox()->BindImageBasedLightingUniforms(program);

    // Bind SSAO flag:
    program->BindUniformBool("ssao_enabled", ssao_enabled);

    // Bind all g-buffer textures:
    g_buffer.BindTextures();
    if (shadow_pass_enabled)
    {
        // Bind Shadow map texture to texture slot 5:
        shadow_manager.BindShadowMapTexture(5);
    }

    // Bind blurred out emissive texture from bloom_manager:
    bloom_manager.BindForReading();

    if (ssao_enabled)
    {
        // Bind SSAO texture to texture slot 10:
        ssao_manager.BindSSAOTexture(10);
    }

    // Bind deferred rendering mode. This can be configured from the editor,
    // and shader sets the fragment color according to this mode:
    const int render_shadows = static_cast<int>(shadow_pass_enabled);
    program->BindUniformInts(Uniforms::ShadowMap::RENDER_MODE, 1, &render_shadows);
    program->BindUniformInts(Uniforms::DeferredRendering::MODE, 1, &deferred_mode);

    // Render the final texture from deferred rendering on a quad that is,
    // 1x1 on NDC:
    RenderFullScreenQuad();
    glBindVertexArray(0);

    g_buffer.UnbindTextures();
    Program::Deactivate();

    // Blit g_buffer depth buffer to frame_buffer to be used for forward
    // rendering pass:
    g_buffer.BlitDepth(
        pre_post_process_frame_buffer->GetFrameBufferId(), 
        fb_width, 
        fb_height);

    // ------------------------------ PRE FORWARD -----------------------------

    if (draw_skybox)
    {
        scene->GetSkybox()->Draw(camera);
    }

    // ----------------------------- FOG -----------------------------

    // Fog is drawn before forward pass because it doesn't apply its values to
    // g-buffer and we want to ensure that all the forward rendered objects are
    // properly visible.

    const Scene::FogConfig& fog = scene->GetFogConfig();
    if (fog.enabled)
    {
        glDepthMask(GL_FALSE);
        // Bind all g-buffer textures:
        g_buffer.BindForReading();
        g_buffer.BindFogTextures();

        program = App->program->GetProgram(Program::Programs::FOG);
        program->Activate();

        EnableBlending();
        const Scene::FogConfig& fog = scene->GetFogConfig();
        program->BindUniformFloat3(Uniforms::Fog::COLOR, fog.color.ptr());
        program->BindUniformFloat(Uniforms::Fog::GLOBAL_DENSITY, &fog.global_density);
        program->BindUniformFloat(Uniforms::Fog::HEIGHT_FALLOFF, &fog.height_falloff);

        RenderFullScreenQuad();
        glBindVertexArray(0);

        g_buffer.UnbindFogTextures();
        DisableBlending();
        Program::Deactivate();
        glDepthMask(GL_TRUE);
    }

    // ----------------------------- FORWARD PASS -----------------------------

    // Clear Transparent Batches:
    batch_manager->ClearTransparentBatchesDrawList();

     // If forward pass is disabled on the settings, skip:
    if (render_forward_pass)
    {
        // Get transparent meshes:
        const std::vector<RenderTarget>& transparent_targets = 
            render_list.GetTransparentTargets();

        if (transparent_targets.size() > 0)
        {
            // Get the targets that has transparent materials. These targets will be
            // rendered with regular forward rendering pass:
            for (const RenderTarget& target : transparent_targets)
            {
                batch_manager->AddDrawComponent(target.mesh_renderer);
            }

            // Forward rendering pass for transparent game objects:
            program = App->program->GetProgram(Program::Programs::FORWARD);
            program->Activate();

            EnableBlending();
            batch_manager->DrawTransparentBatches(program);
            DisableBlending();
        }
    }

    DrawParticles(scene, camera);

    if (draw_navmesh)
    {
        EnableBlending();
        App->navigation->DebugDraw();
        DisableBlending();
    }
      
    // ----------------------------- POST PROCCESS ----------------------------

    if (draw_outlines && drew_outlines)
    {
        glDisable(GL_DEPTH_TEST);
        glDepthMask(GL_FALSE);
        program = App->program->GetProgram(Program::Programs::OUTLINE);
        program->Activate();

        EnableBlending();
        outline_texture->BindForReading(11);
        RenderFullScreenQuad();
        glBindVertexArray(0);
        DisableBlending();

        Program::Deactivate();
        glDepthMask(GL_TRUE);
        glEnable(GL_DEPTH_TEST);
    }

    RunFXAA();
}

void Hachiko::ModuleRender::DrawParticles(Scene* scene, ComponentCamera* camera) const
{
    EnableBlending();

    // Draw debug draw stuff:
    ModuleDebugDraw::Draw(camera->GetViewMatrix(), camera->GetProjectionMatrix(), fb_height, fb_width);

    const auto& scene_particles = scene->GetSceneParticles();
    if (!scene_particles.empty())
    {
        Program* particle_program = App->program->GetProgram(Program::Programs::PARTICLE);
        particle_program->Activate();
        for (auto& particle : scene_particles)
        {
            particle->Draw(camera, particle_program);
        }

        Program::Deactivate();

        /*
        g_buffer.BlitDepth(frame_buffer, fb_width, fb_height);
        g_buffer.BindForDrawing();

        // Forward depth (Used for fog)
        particle_program = App->program->GetProgram(Program::PROGRAMS::PARTICLE_DEPTH);
        particle_program->Activate();

        for (auto& particle : scene_particles)
        {
            particle->Draw(camera, particle_program);
        }
    
        Program::Deactivate();
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, frame_buffer);
        */
    }
    

    DisableBlending();
}

bool Hachiko::ModuleRender::DrawOutlines(BatchManager* batch_manager) 
{
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, outline_texture->GetFrameBufferId());
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClear(GL_STENCIL_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    const bool drew_secondary = 
        ExecuteFullOutlinePass(Outline::Type::SECONDARY, batch_manager);
    const bool drew_primary = 
        ExecuteFullOutlinePass(Outline::Type::PRIMARY, batch_manager);

    if (!drew_primary && !drew_secondary)
    {
        return false;
    }

    // Blur outlines for softer look:
    ApplyGaussianFilter(
        outline_texture->GetFrameBufferId(), 
        outline_texture->GetTextureId(),
        outline_texture_temp->GetFrameBufferId(),
        outline_texture_temp->GetTextureId(),
        outline_blur_config.intensity, 
        outline_blur_config.sigma, 
        BlurPixelSize::ToInt(outline_blur_config.size), 
        fb_width, 
        fb_height, 
        App->program->GetProgram(Program::Programs::GAUSSIAN_FILTERING));

    return true;
}

void Hachiko::ModuleRender::ExecuteSingleOutlinePass(
    Outline::Config& outline_config,
    BatchManager* batch_manager, 
    const bool should_clear_draw_lists_after) const
{
    const Program* program = 
        App->program->GetProgram(Program::Programs::STENCIL);

    program->Activate();

        program->BindUniformFloat4("in_color", outline_config.color.ptr());
        program->BindUniformFloat(
            "outline_thickness", 
            &outline_config.thickness);

        batch_manager->DrawOpaqueBatches(
            program, 
            !should_clear_draw_lists_after);
        EnableBlending();
        batch_manager->DrawTransparentBatches(
            program, 
            !should_clear_draw_lists_after);
        DisableBlending();

    Program::Deactivate();

    if (should_clear_draw_lists_after)
    {
        batch_manager->ClearOpaqueBatchesDrawList();
        batch_manager->ClearTransparentBatchesDrawList();       
    }
}

bool Hachiko::ModuleRender::ExecuteFullOutlinePass(
    Outline::Type outline_type, 
    BatchManager* batch_manager)
{
    const auto& targets = render_list.GetOutlineTargets(outline_type);

    if (targets.empty())
    {
        return false;
    }

    Outline::Config outline = GetOutlineConfigFromType(outline_type);

    batch_manager->ClearOpaqueBatchesDrawList();
    batch_manager->ClearTransparentBatchesDrawList();

    for (const RenderTarget& target : targets)
    {
        batch_manager->AddDrawComponent(target.mesh_renderer);
    }

    glDisable(GL_DEPTH_TEST);
    ExecuteSingleOutlinePass(outline, batch_manager, false);
    outline.color.w = 0.0f;
    outline.thickness = 0.0f;
    ExecuteSingleOutlinePass(outline, batch_manager, true);
    glEnable(GL_DEPTH_TEST);

    return true;
}

Hachiko::Outline::Config Hachiko::ModuleRender::GetOutlineConfigFromType(
    Outline::Type type) const
{
    // TODO: Make these (PRIMARY and SECONDARY) configurable or decide on hardcoded values.

    switch (type)
    {
        case Outline::Type::PRIMARY:
            return Outline::Config {0.03f, float4(0.8f, 0.8f, 0.8f, 0.9f)};
        case Outline::Type::SECONDARY:
            return Outline::Config {0.03f, float4(1.0f, 0.11f, 0.11f, 1.0f)};
        default:
        case Outline::Type::NONE:
            return Outline::Config {0.0f, float4(0.0f, 0.0f, 0.0f, 0.0f)};
    }
}

bool Hachiko::ModuleRender::DrawToShadowMap(
    Scene* scene,
    BatchManager* batch_manager,
    DrawConfig draw_config)
{
    if (scene->dir_lights.empty())
    {
        return false;
    }

    // Update directional light frustum if there are any changes:
    shadow_manager.LazyCalculateLightFrustum();

    // Cull the scene with directional light frustum:
    render_list.Update(
        shadow_manager.GetDirectionalLightFrustum(), 
        scene->GetQuadtree());

    // Draw collected meshes with shadow mapping program:
    const Program* program = 
        App->program->GetProgram(Program::Programs::SHADOW_MAPPING);
    program->Activate();

    // Bind shadow map generation specific necessary uniforms:
    shadow_manager.BindShadowMapGenerationPassUniforms(program);
    // Bind shadow map fbo for drawing and adjust viewport size etc.:
    shadow_manager.BindBufferForDrawing();

    //Disable to cull for both faces
    glDisable(GL_CULL_FACE);

    if ((draw_config & DRAW_CONFIG_OPAQUE) != 0)
    {
        // Collect and draw opaque meshes to shadow map:

        batch_manager->ClearOpaqueBatchesDrawList();

        for (const RenderTarget& target : render_list.GetOpaqueTargets())
        {
            if (!target.mesh_renderer->IsCastingShadow())
            {
                continue;
            }

            batch_manager->AddDrawComponent(target.mesh_renderer);
        }

        batch_manager->DrawOpaqueBatches(program);
    }

    if ((draw_config & DRAW_CONFIG_TRANSPARENT) != 0)
    {
        // Collect and draw transparent meshes to shadow map:

        batch_manager->ClearTransparentBatchesDrawList();

        for (const RenderTarget& target : render_list.GetTransparentTargets())
        {
            if (!target.mesh_renderer->IsCastingShadow())
            {
                continue;
            }

            batch_manager->AddDrawComponent(target.mesh_renderer);
        }

        batch_manager->DrawTransparentBatches(program);
    }

    //Enable back to face culling
    glEnable(GL_CULL_FACE);

    // Unbind shadow map fbo:
    ShadowManager::UnbindBuffer();

    Program::Deactivate();

    // Smooth out the shadow map by applying gaussian filtering:
    shadow_manager.ApplyGaussianBlur(
        App->program->GetProgram(Program::Programs::GAUSSIAN_FILTERING));

    return true;
}

void Hachiko::ModuleRender::ApplyGaussianFilter(
    unsigned source_fbo,
    unsigned source_texture,
    unsigned temp_fbo,
    unsigned temp_texture,
    float blur_scale_amount,
    float blur_sigma,
    int blur_size,
    unsigned width,
    unsigned height,
    const Program* program) const
{
    // Calculate blur scales:
    float blur_scale_x = blur_scale_amount / static_cast<float>(width);
    float blur_scale_y = blur_scale_amount / static_cast<float>(height);
    float3 blur_scale_width(blur_scale_x, 0.0f, 0.0f);
    float3 blur_scale_height(0.0f, blur_scale_y, 0.0f);

    // X Axis Blur Pass:
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, temp_fbo);
    glViewport(0, 0, width, height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    program->Activate();

    program->BindUniformFloat(Uniforms::Filtering::GAUSSIAN_BLUR_SIGMA, &blur_sigma);
    program->BindUniformInt(Uniforms::Filtering::GAUSSIAN_BLUR_PIXELS, blur_size);
    program->BindUniformFloat3(Uniforms::Filtering::GAUSSIAN_BLUR_SCALE, blur_scale_width.ptr());

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, source_texture);

    RenderFullScreenQuad();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    Program::Deactivate();

    // Y Axis Blur Pass:
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, source_fbo);
    glViewport(0, 0, width, height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    program->Activate();
    program->BindUniformFloat3(Uniforms::Filtering::GAUSSIAN_BLUR_SCALE, blur_scale_height.ptr());
    program->BindUniformFloat(Uniforms::Filtering::GAUSSIAN_BLUR_SIGMA, &blur_sigma);
    program->BindUniformInt(Uniforms::Filtering::GAUSSIAN_BLUR_PIXELS, blur_size);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, temp_texture);

    RenderFullScreenQuad();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    Program::Deactivate();
}

void Hachiko::ModuleRender::SetRenderMode(bool is_deferred)
{
    if (is_deferred == draw_deferred)
    {
        return;
    }

    draw_deferred = is_deferred;

    if (!draw_deferred)
    {
        g_buffer.Free();
    }
    else
    {
        g_buffer.Generate();
    }
}

UpdateStatus Hachiko::ModuleRender::PostUpdate(const float delta)
{
    SDL_GL_SwapWindow(App->window->GetWindow());
    AddFrame(delta);

    return UpdateStatus::UPDATE_CONTINUE;
}

void GLOptionCheck(GLenum option, bool enable)
{
    if (enable)
    {
        glEnable(option);
    }
    else
    {
        glDisable(option);
    }
}

void Hachiko::ModuleRender::OptionsMenu()
{
    ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Draw Options");
    Widgets::Checkbox("Debug Draw", &App->debug_draw->debug_draw);
    Widgets::Checkbox("Quadtree", &App->debug_draw->draw_quadtree);
    Widgets::Checkbox("Skybox", &draw_skybox);
    Widgets::Checkbox("Navmesh", &draw_navmesh);

    ImGui::NewLine();
    ImGui::TextWrapped("Rendering mode");
    ImGui::Separator();
    ImGui::TextWrapped("Select whether the engine should use Deferred or Forward rendering.");
    ImGui::PushID("RendererMode");

    if (Widgets::RadioButton("Deferred", draw_deferred == true))
    {
        SetRenderMode(true);
    }

    if (Widgets::RadioButton("Forward", draw_deferred == false))
    {
        SetRenderMode(false);
    }
    ImGui::PopID();

    if (draw_deferred)
    {
        DeferredOptions();
    }

    ImGui::NewLine();
    ImGui::TextWrapped("Shadowmap blurring mode");
    ImGui::Separator();
    ImGui::TextWrapped("Enable for soft shadows, disable for hard shadows.");
    bool shadow_map_gaussian_blur_enabled = shadow_manager.IsGaussianBlurringEnabled();

    if (Widgets::Checkbox("Apply Gaussian blur", &shadow_map_gaussian_blur_enabled))
    {
        shadow_manager.SetGaussianBlurringEnabled(shadow_map_gaussian_blur_enabled);
        App->preferences->GetEditorPreference()->SetShadowMappingGaussianBlurringEnabled(shadow_map_gaussian_blur_enabled);
    }

    if (!draw_skybox)
    {
        ImGuiUtils::CompactColorPicker("Background color", App->editor->scene_background.ptr());
    }

    ImGui::NewLine();
    ImGui::TextWrapped("Bloom");
    ImGui::Separator();
    bloom_manager.DrawEditorContent();

    ImGui::NewLine();
    ImGui::TextWrapped("Ambient Occlusion");
    ImGui::Separator();
    bool ssao_mode_changed = Widgets::Checkbox("SSAO enabled", &ssao_enabled);
    if (ssao_mode_changed)
    {
        App->preferences->GetEditorPreference()->SetSSAOEnabled(ssao_enabled);
    }
    if (ssao_enabled)
    {
        ssao_manager.ShowInEditorUI();
    }

    ImGui::NewLine();
    ImGui::TextWrapped("Outline Blur");
    ImGui::Separator();
    BlurConfigUtil::ShowInEditorUI(&outline_blur_config);
    Widgets::Checkbox("Draw outlines", &draw_outlines);

    ImGui::NewLine();
    ImGui::TextWrapped("Anti-aliasing");
    ImGui::Separator();
    ImGui::TextWrapped("This will not be saved, just exists for debug purposes");
    Widgets::Checkbox("Enable FXAA", &anti_aliasing_enabled);
}

void Hachiko::ModuleRender::LoadingScreenOptions() const
{
    ImGui::NewLine();

    ImGui::Text("Loading screen");
    ImGui::Separator();

    if (loading_image != nullptr)
    {
        loading_image->DrawGui();
    }
    if (loading_video != nullptr)
    {
        loading_video->DrawGui();
    }
}

void Hachiko::ModuleRender::DeferredOptions()
{
    ImGui::NewLine();

    ImGui::TextWrapped("Deferred Rendering Options");
    ImGui::Separator();

    ImGui::PushID("Deferred Options");

    if (Widgets::RadioButton("Lighting Pass", deferred_mode == 0))
    {
        deferred_mode = 0;
    }

    if (Widgets::RadioButton("Diffuse", deferred_mode == 1))
    {
        deferred_mode = 1;
    }

    if (Widgets::RadioButton("Specular", deferred_mode == 2))
    {
        deferred_mode = 2;
    }

    if (Widgets::RadioButton("Smoothness", deferred_mode == 3))
    {
        deferred_mode = 3;
    }

    if (Widgets::RadioButton("Normal", deferred_mode == 4))
    {
        deferred_mode = 4;
    }

    if (Widgets::RadioButton("Position", deferred_mode == 5))
    {
        deferred_mode = 5;
    }

    if (Widgets::RadioButton("Emissive", deferred_mode == 6))
    {
        deferred_mode = 6;
    }

    if (Widgets::RadioButton("SSAO Color", deferred_mode == 7))
    {
        deferred_mode = 7;
    }

    Widgets::Checkbox("Forward Rendering Pass", &render_forward_pass);

    if (Widgets::Checkbox("Render Shadows", &shadow_pass_enabled))
    {
        App->preferences->GetEditorPreference()->SetShadowPassEnabled(shadow_pass_enabled);
    }

    ImGui::PopID();
}

void Hachiko::ModuleRender::PerformanceMenu()
{
    const float vram_free_mb = gpu.vram_free / 1024.0f;
    const float vram_usage_mb = gpu.vram_budget_mb - vram_free_mb;
    Widgets::Label("VRAM budget", StringUtils::Format("%.1f MB", gpu.vram_budget_mb));
    Widgets::Label("VRAM usage", StringUtils::Format("%.1f MB", vram_usage_mb));
    Widgets::Label("VRAM avaliable", StringUtils::Format("%.1f MB", vram_free_mb));
    ImGui::Separator();
    FpsGraph();
}

void Hachiko::ModuleRender::FpsGraph() const
{
    Widgets::Label("Fps", StringUtils::Format("%.1f", current_fps));

    char title[25];
    sprintf_s(title, 25, "Framerate %.1f", current_fps);
    ImGui::PlotHistogram("##framerate", &fps_log[0], static_cast<int>(fps_log.size()), 0, title, 0.0f, 1000.f, ImVec2(310, 100));
    sprintf_s(title, 25, "Milliseconds %0.1f", current_ms);
    ImGui::PlotHistogram("##milliseconds", &ms_log[0], static_cast<int>(ms_log.size()), 0, title, 0.0f, 20.0f, ImVec2(310, 100));
}

void Hachiko::ModuleRender::AddFrame(const float delta)
{
    static constexpr float update_frequency_seconds = 0.5f;
    static int filled_bins = 0;
    static int frames = 0;
    static float time = 0;

    ++frames;
    time += delta;

    if (time >= update_frequency_seconds)
    {
        if (filled_bins == n_bins)
        {
            for (int i = 0; i < n_bins - 1; ++i)
            {
                fps_log[i] = fps_log[i + 1];
                ms_log[i] = ms_log[i + 1];
            }
        }
        else
        {
            ++filled_bins;
        }
        fps_log[filled_bins - 1] = static_cast<float>(frames) / time;
        current_fps = fps_log[filled_bins - 1];
        ms_log[filled_bins - 1] = time * 1000.0f / static_cast<float>(frames);
        current_ms = ms_log[filled_bins - 1];
        time = 0;
        frames = 0;
    }
}

void Hachiko::ModuleRender::SetOpenGLAttributes() const
{
    // Enable hardware acceleration:
    SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
    // Set desired major version of OpenGL:
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    // Set desired minor version of OpenGL:
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
    // Set OpenGL to compatibility mode (we can use core mode as well): 
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);
    // Enable double buffer usage:
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    // Set buffer size to 32 bits, Red 8, Green 8, Blue 8, Alpha 8:
    SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 32);
    // Set depth component size (bits):
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24); // TODO: Ask Carlos why this can't be 32.
    // Set red color component size (bits):
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    // Set green color component size (bits):
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    // Set blue color component size (bits):
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    // Set alpha color component size (bits):
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
    // Set stencil component size (bits):
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    // Enable OpenGL context debug:
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
}

void Hachiko::ModuleRender::RetrieveLibVersions() const
{
    HE_LOG("GPU Vendor: %s", glGetString(GL_VENDOR));
    HE_LOG("Renderer: %s", glGetString(GL_RENDERER));
    HE_LOG("Using Glew %s", glewGetString(GLEW_VERSION));
    HE_LOG("OpenGL version supported %s", glGetString(GL_VERSION));
    HE_LOG("GLSL: %s", glGetString(GL_SHADING_LANGUAGE_VERSION));
}

void Hachiko::ModuleRender::RetrieveGpuInfo()
{
    gpu.name = (unsigned char*)glGetString(GL_RENDERER);
    gpu.brand = (unsigned char*)glGetString(GL_VENDOR);

    GLint count;
    glGetIntegerv(GL_NUM_EXTENSIONS, &count);
    for (GLint i = 0; i < count; ++i)
    {
        const char* extension = reinterpret_cast<const char*>(glGetStringi(GL_EXTENSIONS, i));
        if (!strcmp(extension, "GL_NVX_gpu_memory_info"))
        {
            glGetIntegerv(GL_GPU_MEMORY_INFO_TOTAL_AVAILABLE_MEMORY_NVX, &gpu.vram_budget_mb);
            glGetIntegerv(GL_GPU_MEMORY_INFO_CURRENT_AVAILABLE_VIDMEM_NVX, &gpu.vram_free);
        }
    }

    gpu.vram_budget_mb /= 1024;
}

void Hachiko::ModuleRender::GenerateFullScreenQuad()
{
    constexpr const float vertices[] = {
        1.0f, 1.0f, 0.0f, 1.0f, 1.0f, // top right
        1.0f, -1.0f, 0.0f, 1.0f, 0.0f, // bottom right
        -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, // bottom left
        -1.0f, 1.0f, 0.0f, 0.0f, 1.0f // top left
    };
    constexpr unsigned int indices[] = {2, 1, 0, 0, 3, 2};

    glGenVertexArrays(1, &full_screen_quad_vao);
    glGenBuffers(1, &full_screen_quad_vbo);
    glGenBuffers(1, &full_screen_quad_ebo);

    glBindVertexArray(full_screen_quad_vao);

    glBindBuffer(GL_ARRAY_BUFFER, full_screen_quad_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, full_screen_quad_ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Vertices:
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (void*)0);
    glEnableVertexAttribArray(0);

    // Texture coordinates:
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}

void Hachiko::ModuleRender::RenderFullScreenQuad() const
{
    glBindVertexArray(full_screen_quad_vao);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void Hachiko::ModuleRender::FreeFullScreenQuad() const
{
    glBindVertexArray(full_screen_quad_vao);
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDeleteBuffers(1, &full_screen_quad_ebo);
    glDeleteBuffers(1, &full_screen_quad_vbo);
    glDeleteVertexArrays(1, &full_screen_quad_vao);
    glBindVertexArray(0);
}

void Hachiko::ModuleRender::CreateNoiseTexture() 
{
    const unsigned width = 128;
    const unsigned height = 128;
    const double delta = 0.05f;

    OpenSimplex2S os;
    byte* result = new byte[width * height];

    for (int i = 0; i < width; ++i)
    {
        for (int j = 0; j < height; ++j)
        {
            result[i * height + j] = static_cast<byte>((os.noise2_XBeforeY((double)i * delta, (double)j * delta) + 1) * 127.0f);
        }
    }

    glGenTextures(1, &noise_id);
    glBindTexture(GL_TEXTURE_2D, noise_id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, result);
    glBindTexture(GL_TEXTURE_2D, 0);

    delete result;
}

bool Hachiko::ModuleRender::CleanUp()
{
    FreeFullScreenQuad();

    glDeleteTextures(1, &fb_texture);
    glDeleteBuffers(1, &depth_stencil_buffer);
    glDeleteBuffers(1, &frame_buffer);

    SDL_GL_DeleteContext(context);

    App->preferences->GetEditorPreference()->SetDrawSkybox(draw_skybox);
    App->preferences->GetEditorPreference()->SetDrawNavmesh(draw_navmesh);

    // Free outline textures:
    delete outline_texture;
    delete outline_texture_temp;

    // Free pre post process frame buffer texture:
    delete pre_post_process_frame_buffer;

    return true;
}

void Hachiko::ModuleRender::LoadLoadingScreen() 
{
    loading_game_object = new GameObject(nullptr, float4x4::identity, "Loading");
    loading_transform2d = static_cast<ComponentTransform2D*>(loading_game_object->CreateComponent(Component::Type::TRANSFORM_2D));
    loading_image = static_cast<ComponentImage*>(loading_game_object->CreateComponent(Component::Type::IMAGE));
    loading_video = static_cast<ComponentVideo*>(loading_game_object->CreateComponent(Component::Type::VIDEO));

    // For now the loading screen configuration will be hardcoded:
    {
        //Image
        YAML::Node node_image;
        node_image.SetTag("image");
        node_image[IMAGE_IMAGE_ID] = 9856915381281154687;
        node_image[IMAGE_HOVER_IMAGE_ID] = 0;
        node_image[IMAGE_COLOR] = float4::one;
        node_image[IMAGE_HOVER_COLOR] = float4::one;
        node_image[IMAGE_TILED] = true;
        node_image[IMAGE_RANDOMIZE_INITIAL_FRAME] = false;
        node_image[IMAGE_X_TILES] = 2;
        node_image[IMAGE_Y_TILES] = 2;
        node_image[IMAGE_TILES_PER_SEC] = 2;
        node_image[IMAGE_FILL_WINDOW] = true;

        loading_image->Load(node_image);

        //Video
        YAML::Node node_video;
        node_video.SetTag("video");
        node_video[VIDEO_ID] = 12654021748852338787;
        node_video[VIDEO_PROJECTED] = false;
        node_video[VIDEO_LOOP] = true;
        node_video[VIDEO_FLIP] = false;
        node_video[VIDEO_FPS] = 12;

        loading_video->Load(node_video);
        loading_video->SetAsInScene();
        loading_video->Start();
        loading_video->Preload(12 * 4);
        loading_video->Play();
    }
}

void Hachiko::ModuleRender::DeleteLoadingScreen() 
{
    delete loading_game_object;
}

void Hachiko::ModuleRender::DrawLoadingScreen(const float delta)
{
    OPTICK_CATEGORY("DrawLoadingScreen", Optick::Category::Rendering);

    Program* img_program = App->program->GetProgram(Program::Programs::UI_IMAGE);

    glDepthFunc(GL_ALWAYS);

    glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer);
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    int res_x, res_y;
    App->window->GetWindowSize(res_x, res_y);
    if (res_x != fb_width || res_y != fb_height)
    {
        ResizeFrameBuffer(res_x, res_y);
        fb_width = res_x;
        fb_height = res_y;
    }
    glViewport(0, 0, fb_width, fb_height);

    if (using_image)
    {
        loading_transform2d->SetSize(float2(fb_width, fb_height));
        loading_image->Update();

        ModuleProgram::CameraData camera_data;
        // position data is unused on the ui program
        camera_data.pos = float3::zero;
        camera_data.view = float4x4::identity;
        camera_data.proj = float4x4::D3DOrthoProjLH(-1, 1, static_cast<float>(fb_width), static_cast<float>(fb_height));

        App->program->UpdateCamera(camera_data);

        loading_image->Draw(loading_transform2d, img_program);
    }
    else
    {
        loading_video->Update();
        loading_video->Draw(nullptr, nullptr);
    }

    glDepthFunc(GL_LESS);

    glBindFramebuffer(GL_READ_FRAMEBUFFER, frame_buffer);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glBlitFramebuffer(0, 0, fb_width, fb_height, 0, 0, fb_width, fb_height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
}

void Hachiko::ModuleRender::BindNoiseTexture(Program* program)
{
    // Binding the noise texture
    glActiveTexture(GL_TEXTURE10);
    glBindTexture(GL_TEXTURE_2D, noise_id);
    program->BindUniformInt("noise", 10);
}