#pragma once

//////////////////////////
//						//
//     Importers        //
//						//
//////////////////////////

// General
//#define GENERAL_ID "id"
#define RESOURCES "resources" // Resources that asset is responsible of generating
#define RESOURCE_ID "resource_id"
#define RESOURCE_TYPE "resource_type"
#define ASSET_HASH "asset_hash"
#define RESOURCE_HASH "resource_hash"

// Texture
#define TEXTURE_FILE_PATH "file_path"
#define TEXTURE_MIN_FILTER "min_filter"
#define TEXTURE_MAG_FILTER "mag_filter"
#define TEXTURE_WRAP_MODE "wrap_mode"

// Model
#define MESHES "meshes"
#define MATERIALS "materials"
#define ANIMATIONS "animations"
#define ANIMATION_NAMES "animation_names"
#define MODEL_NAME "model_name"
#define PREFAB_ID "prefab_id"
#define M_STATE_MACHINE "state_machine"

// Prefab
#define PREFAB_NAME "prefab_name"

// Node
#define NODE_NAME "name"
#define NODE_CHILD "child"
#define NODE_TRANSFORM "transform"

// Mesh
#define MIN_POINT "min_point"
#define MAX_POINT "max_point"
#define VERTICES_NODE "vertices"
#define NORMALS_NODE "normals"
#define INDICES_NODE "indices"
#define TEXTURE_COORDS_NODE "texture_coords"

// Material
#define MATERIAL_NAME "name"
#define MATERIAL_DIFFUSE_ID "diffuse"
#define MATERIAL_SPECULAR_ID "specular"
#define MATERIAL_NORMALS_ID "normals"
#define MATERIAL_METALNESS_ID "metalness"
#define MATERIAL_EMISSIVE_ID "emissive"
#define MATERIAL_DIFFUSE_PATH "diffuse_path"
#define MATERIAL_SPECULAR_PATH "specular_path"
#define MATERIAL_NORMALS_PATH "normals_path"
#define MATERIAL_METALNESS_PATH "metalness_path"
#define MATERIAL_EMISSIVE_PATH "emissive_path"
#define MATERIAL_DIFFUSE_COLOR "diffuse_color"
#define MATERIAL_SPECULAR_COLOR "specular_color"
#define MATERIAL_EMISSIVE_COLOR "emissive_color"
#define MATERIAL_SMOOTHNESS "smoothness"
#define MATERIAL_METALNESS_VALUE "metalness_value"
#define MATERIAL_IS_METALLIC "is_metallic"
#define MATERIAL_ALPHA_CHANNEL "smoothness_alpha"
#define MATERIAL_IS_TRANSPARENT "is_transparent"

// State machine
#define SM_NAME "name"
#define SM_DEFAULT_NODE "default_node"

#define SM_CLIPS "clips"
#define SM_CLIP_NAME "clip_name"
#define SM_CLIP_ID "clips_id"
#define SM_CLIP_LOOP "clips_loop"

#define SM_NODES "nodes"
#define SM_NODE_NAME "node_name"
#define SM_NODE_CLIP_NAME "node_clip_name"

#define SM_TRANSITIONS "transitions"
#define SM_TRANSITION_SOURCE "transition_source"
#define SM_TRANSITION_TARGET "transition_target"
#define SM_TRANSITION_TRIGGER "transition_trigger"
#define SM_TRANSITION_BLEND "transition_blend"

//////////////////////////
//						//
// Importer extensions  //
//						//
//////////////////////////

#define SCENE_EXTENSION ".scene"
#define MODEL_EXTENSION ".model"
#define MESH_EXTENSION ".mesh"
#define MATERIAL_EXTENSION ".mat"
#define META_EXTENSION ".meta"
#define PREFAB_EXTENSION ".prefab"
#define JPG_TEXTURE_EXTENSION ".jpg"
#define PNG_TEXTURE_EXTENSION ".png"
#define TIF_TEXTURE_EXTENSION ".tif"
#define DEFAULT_SHADER_EXTENSION ".glsl"
#define MODEL_EXTENSION ".fbx"
#define SKYBOX_EXTENSION ".hdr"
#define FONT_EXTENSION ".ttf"
#define STATE_MACHINE_EXTENSION ".stm"
#define MP4_VIDEO_EXTENSION ".mp4"
 
//////////////////////////
//						//
//     Preferences      //
//						//
//////////////////////////

// Editor
#define EDITOR_NODE "editor"
#define DISPLAY_CAMERA_SETTINGS "display_camera_settings"
#define DISPLAY_DEBUG_DRAW "display_debug_draw"
#define DISPLAY_QUADTREE "display_quadtree"
#define DISPLAY_SKYBOX "display_skybox"
#define DISPLAY_NAVMESH "display_navmesh"
#define UNDO_REDO_ACTIVE "undo_redo_active"
#define FULLSCREEN_NODE "fullscreen"
#define THEME "theme"
#define VSYNC "vsync"
#define RESIZABLE "resizable"
#define THEME_LIGHT "light"
#define THEME_DARK "dark"
#define SCENE_BACKGROUND_COLOR "scene_background_color"
#define SCENE_AUTOSAVE "scene_autosave"
#define SHADOW_MAP_GAUSSIAN_BLURRING_ENABLED "shadow_map_gaussian_blurring"
#define SHADOW_PASS_ENABLED "shadow_pass_enabled"

// Resources
#define RESOURCES_NODE "resources"

// Assets
#define ASSETS_COMMON_NAME "_assets"

// Library
#define LIBRARY_COMMON_NAME "_library"

// Render
#define RENDER_NODE "render"
#define MAX_FPS "max_fps"
#define FPS_THRESHOLD "fps_threshold"

// Camera
#define CAMERA_NODE "camera"
#define MOVE_SPEED "move_speed"
#define ROTATION_SPEED "rotation_speed"
#define ORBIT_SPEED "orbit_speed"
#define ZOOM_SPEED "zoom_speed"
#define FOV "fov"
#define CAMERA_POS "position"

// Globals
#define GLOBAL_NODE "globals"
#define TITLE_NODE "title"
#define VERSION_NODE "version"

// Audio
#define AUDIO_NODE "audio"
#define SFX_VOLUME "sfx_volume"
#define MUSIC_VOLUME "music_volume"

//////////////////////////
//						//
// Scene Serialization  //
//						//
//////////////////////////

// Scene
#define SCENE_ID "scene_id"
#define SCENE_NAME "scene_name"
#define UNNAMED_SCENE "UnnamedScene"
#define ROOT_ID "root_id"
#define NAVMESH_ID "navmesh_id"
#define IBL "ibl"
#define SKYBOX_NODE "skybox"
#define CUBE_ID "_cube_id"
#define AMBIENT_LIGHT "ambient_light"
#define AMBIENT_LIGHT_INTENSITY "intensity"
#define AMBIENT_LIGHT_COLOR "color"
#define FOG "fog"
#define FOG_ENABLED "enabled"
#define FOG_COLOR "color"
#define FOG_GLOBAL_DENSITY "global_density"
#define FOG_HEIGHT_FALLOFF "height_falloff"

// Game Object
#define GAME_OBJECT_NAME "name"
#define GAME_OBJECT_ENABLED "enabled"
#define GAME_OBJECT_ID "id"

// Component
#define COMPONENT_NODE "component"
#define COMPONENT_NAME "name"
#define COMPONENT_ENABLED "enabled"
#define COMPONENT_ID "id"
#define COMPONENT_TYPE "type"
#define COMPONENT_DATA "data"

// Child 
#define CHILD_NODE "child"

// Component Transform
#define TRANSFORM_POSITION "position"
#define TRANSFORM_ROTATION "rotation"
#define TRANSFORM_SCALE "scale"
#define TRANSFORM_SIZE "size"
#define TRANSFORM_PIVOT "pivot"
#define TRANSFORM_ANCHOR "anchor"

// Component Mesh
#define MESH_NODE "mesh"
#define MESH_ENABLED "enabled"
#define MESH_TEXTURE "texture_file_name"
#define MESH_TEXTURE_TYPE "texture_type"
#define MESH_NAVIGABLE "navigable"
#define MESH_VISIBLE "visible"

// Component Lights
#define LIGHT_TYPE "light_type"
#define LIGHT_COLOR "color"
#define LIGHT_INTENSITY "intensity"
#define LIGHT_RADIUS "radius"
#define LIGHT_DRAW_SPHERE "draw_sphere"
#define LIGHT_DRAW_DIRECTION "draw_direction"
#define LIGHT_DRAW_CONE "draw_cone"
#define LIGHT_INNER "inner"
#define LIGHT_OUTER "outer"

// Component Camera
#define FRUSTUM "frustum"
#define NEAR_DISTANCE "near_distance"
#define FAR_DISTANCE "far_distance"
#define CAMERA_POSITION "position"
#define CAMERA_FRONT "front"
#define CAMERA_UP "up"
#define CAMERA_TYPE "type"
#define PINNED_CAMERA "pinned_camera"

// Component Image
#define IMAGE_IMAGE_ID "image_id"
#define IMAGE_HOVER_IMAGE_ID "hover_image_id"
#define IMAGE_COLOR "image_color"
#define IMAGE_HOVER_COLOR "hover_image_color"
#define IMAGE_TILED "image_is_tiled"
#define IMAGE_RANDOMIZE_INITIAL_FRAME "image_randomize_initial_frame"
#define IMAGE_X_TILES "image_x_tiles"
#define IMAGE_Y_TILES "image_y_tiles"
#define IMAGE_TILES_PER_SEC "image_tiles_per_second"
#define IMAGE_FILL_WINDOW "fill_window"

// Component Text
#define FONT_ID "font_id"
#define FONT_COLOR "color"
#define FONT_SIZE "size"
#define FONT_LABEL_TEXT "label_text"

// Component Progress Bar
#define BAR_PARAMS "params"
#define BAR_MIN "min"
#define BAR_MAX "max"
#define BAR_FILLED "filled"
#define BAR_DIRECTION "direction"

// Component Obstacle
#define OBSTACLE_TYPE "obstacle_type"
#define OBSTACLE_SIZE "obstacle_size"

// Component Agent
#define MAX_SPEED "max_speed"
#define MAX_ACCELERATION "max_acceleration"
#define AVOID_OBSTACLES "avoid_obstacles"
#define AGENT_RADIUS "radius"
#define AGENT_IS_PLAYER "is_player"

// Component Billboard
#define BILLBOARD_TEXTURE_ID "texture_id"
#define BILLBOARD_LIFETIME "billboard_lifetime"
#define BILLBOARD_PLAY_ON_AWAKE "play_on_awake"
#define ANIMATION_CYCLES "animation_cycles"
#define ANIMATION_SECTION "animation_section"
#define COLOR_SECTION "color_section"
#define SIZE_SECTION "size_section"
#define SIZE_OVERTIME "size_over_time"
#define ROTATION_SECTION "rotation_section"
#define ROTATION_OVERTIME "rotation_over_time"
#define START_DELAY "start_delay"
#define START_SIZE "start_size"
#define START_ROTATION "start_rotation"
#define BILLBOARD_PROPERTIES "properties"
#define BILLBOARD_PROJECTION "projection"

#define COLOR_CYCLES "color_cycles"
#define COLOR_GRADIENT "color_gradient"
#define ANIMATION_LOOP "animation_loop"

// Component Particle System
// sections
#define PARTICLE_SECTIONS "sections"
#define PARAMETER_SECTION "parameters"
#define EMISSION_SECTION "emission"
#define SHAPE_SECTION "shape"
#define LIGHTS_SECTION "lights"
#define RENDERER_SECTION "renderer"
//particle config
#define PARTICLE_PARAMETERS "parameters"
#define PARTICLES_DURATION "duration"
#define PARTICLES_LOOP "loop"
#define PARTICLES_LIFE "life"
#define PARTICLES_SPEED "speed"
#define PARTICLES_SIZE "size"
#define PARTICLES_ROTATION "rotation"
#define PARTICLES_DELAY "delay"
#define PARTICLES_PROPERTIES "properties"
// emission
#define PARTICLE_EMISSION "emission"
#define RATE "rate"
#define RATE_BURST "rate_burst"
#define BURST "burst"
// emitter
#define EMITTER "emitter_shape"
#define EMITTER_TYPE "type"
#define EMITTER_PROPERTIES "properties"
// texture
#define PARTICLES_TEXTURE "texture"
#define PARTICLES_TEXTURE_ID "texture_id"
#define TILES "tiles"
#define TOTAL_TILES "total_tiles"
#define Y_TILES "y_tiles"
#define FLIP "flip"
#define FLIP_Y "flip_y"
// particle modifiers
#define PARTICLE_MODIFIERS "modifiers"
#define ACTIVE "active"
#define MODIFIER_VELOCITY "velocity"
#define SPEED "speed"
#define MODIFIER_SIZE "size"
#define SIZE "size"
#define MODIFIER_COLOR "color"
#define MODIFIER_FORCE "force"
#define X_FORCE "x_force"
#define Y_FORCE "y_force"
#define Z_FORCE "z_force"
#define ROTATION "rotation"
#define MODIFIER_ANIMATION "animation"
#define ANIMATION_SPEED "animation_speed"
#define BLEND_FACTOR "blend_factor"
#define FIT_TO_LIFETIME "fit_to_lifetime"
#define MODIFIER_NOISE "noise"
#define NOISE_SEPARATE_AXES "separate_axes"
#define NOISE_STRENGTH "strength"
#define NOISE_X "x"
#define NOISE_Y "y"
#define NOISE_Z "z"
#define NOISE_POSITION "position"
#define NOISE_ROTATION "rotation"
#define NOISE_SCALE "scale"

// ComponentMeshRenderer
#define RENDERER_MESH_ID "mesh_id"
#define RENDERER_MATERIAL_ID "material_id"
#define RENDERER_TINT_COLOR "tint_color"

// ComponentVideo
#define VIDEO_ID "video_id"
#define VIDEO_PROJECTED "projected"
#define VIDEO_LOOP "loop"
#define VIDEO_FLIP "flip"
#define VIDEO_FPS "fps"

// Script
#define SCRIPT_NAME "class_name"