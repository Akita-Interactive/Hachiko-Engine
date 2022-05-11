#pragma once

//////////////////////////
//						//
//     Importers        //
//						//
//////////////////////////

// General
#define GENERAL_NODE "General"
#define GENERAL_ID "id"
#define GENERAL_TYPE "type"
#define GENERAL_HASH "hash"

// Texture
#define TEXTURE_ID "id"
#define TEXTURE_FILE_PATH "file_path"

// Model
#define MODEL_ID "id"
#define MODEL_NAME "model_name"
#define MODEL_FILE_PATH "model_file_path"
#define MODEL_MESH_ID "id"
#define MODEL_MESH_NODE "mesh"
#define MODEL_MATERIAL_ID "id"
#define MODEL_MATERIAL_NODE "material"
#define ANIMATIONS "animations"
#define ANIMATION_IDS "animation_id"

// Node
#define NODE_NAME "name"
#define NODE_CHILD "child"
#define NODE_TRANSFORM "transform"
#define NODE_MESH_INDEX "mesh_index"
#define NODE_MATERIAL_INDEX "material_index"

// Mesh
#define MESH_ID "mesh_id"
#define MIN_POINT "min_point"
#define MAX_POINT "max_point"
#define VERTICES_NODE "vertices"
#define NORMALS_NODE "normals"
#define INDICES_NODE "indices"
#define TEXTURE_COORDS_NODE "texture_coords"

// Material
#define MATERIAL_ID "id"
#define MATERIAL_NAME "name"
#define MATERIAL_DIFFUSE_ID "diffuse"
#define MATERIAL_SPECULAR_ID "specular"
#define MATERIAL_NORMALS_ID "normals"
#define MATERIAL_METALNESS_ID "metalness"
#define MATERIAL_DIFFUSE_PATH "diffuse_path"
#define MATERIAL_SPECULAR_PATH "specular_path"
#define MATERIAL_NORMALS_PATH "normals_path"
#define MATERIAL_METALNESS_PATH "metalness_path"
#define MATERIAL_DIFFUSE_COLOR "diffuse_color"
#define MATERIAL_SPECULAR_COLOR "specular_color"
#define MATERIAL_SMOOTHNESS "smoothness"
#define MATERIAL_METALNESS_VALUE "metalness_value"
#define MATERIAL_IS_METALLIC "is_metallic"
#define MATERIAL_ALPHA_CHANNEL "smoothness_alpha"
#define MATERIAL_IS_TRANSPARENT "is_transparent"

// ComponentMeshRenderer
#define RENDERER_MESH_ID "mesh_id"
#define RENDERER_MATERIAL_ID "material_id"

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
 
//////////////////////////
//						//
//     Preferences      //
//						//
//////////////////////////

// Editor
#define EDITOR_NODE "editor"
#define DISPLAY_CAMERA_SETTINGS "display_camera_settings"
#define DISPLAY_DEBUG_DRAW "display_debug_draw"
#define FULLSCREEN_NODE "fullscreen"
#define THEME "theme"
#define VSYNC "vsync"
#define THEME_LIGHT "light"
#define THEME_DARK "dark"
#define SCENE_BACKGROUND_COLOR "scene_background_color"
#define SCENE_AUTOSAVE "scene_autosave"

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

// Globals
#define GLOBAL_NODE "globals"
#define TITLE_NODE "title"
#define VERSION_NODE "version"

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
#define SCENE_TEMP_NAME "temp_scene"

// Game Object
#define GAME_OBJECT_NAME "name"
#define GAME_OBJECT_ENABLED "enabled"
#define GAME_OBJECT_ID "id"
#define GAME_OBJECT_PARENT_ID "parent_id"

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
#define IMAGE_PATH "image_path"
#define HOVER_IMAGE_PATH "hover_image_path"

// Component Text
#define FONT_PATH "font_path"
#define FONT_COLOR "color"
#define FONT_SIZE "size"
#define FONT_LABEL_TEXT "label_text"

// Component Progress Bar
#define BAR_PARAMS "params"
#define BAR_MIN "min"
#define BAR_MAX "max"
#define BAR_FILLED "filled"
#define BAR_DIRECTION "direction"

// Script
#define SCRIPT_NAME "class_name"