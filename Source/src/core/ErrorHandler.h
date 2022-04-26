#pragma once
#include "core/hepch.h"

namespace Hachiko
{
    class ErrorHandler
    {
    public:
        static void HandleOpenGlError(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
        {
            auto tmp_source = "", tmp_type = "", tmp_severity = "";

            switch (source)
            {
            case GL_DEBUG_SOURCE_API:
                tmp_source = "API";
                break;
            case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
                tmp_source = "Window System";
                break;
            case GL_DEBUG_SOURCE_SHADER_COMPILER:
                tmp_source = "Shader Compiler";
                break;
            case GL_DEBUG_SOURCE_THIRD_PARTY:
                tmp_source = "Third Party";
                break;
            case GL_DEBUG_SOURCE_APPLICATION:
                tmp_source = "Application";
                break;
            case GL_DEBUG_SOURCE_OTHER:
            default: 
                tmp_source = "Other";
                break;
            }

            switch (type)
            {
            case GL_DEBUG_TYPE_ERROR:
                tmp_type = "Error";
                break;
            case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
                tmp_type = "Deprecated Behaviour";
                break;
            case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
                tmp_type = "Undefined Behaviour";
                break;
            case GL_DEBUG_TYPE_PORTABILITY:
                tmp_type = "Portability";
                break;
            case GL_DEBUG_TYPE_PERFORMANCE:
                tmp_type = "Performance";
                break;
            case GL_DEBUG_TYPE_MARKER:
                tmp_type = "Marker";
                break;
            case GL_DEBUG_TYPE_PUSH_GROUP:
                tmp_type = "Push Group";
                break;
            case GL_DEBUG_TYPE_POP_GROUP:
                tmp_type = "Pop Group";
                break;
            case GL_DEBUG_TYPE_OTHER:
            default:
                tmp_type = "Other";
                break;
            }

            switch (severity)
            {
            case GL_DEBUG_SEVERITY_HIGH:
                tmp_severity = "high";
                break;
            case GL_DEBUG_SEVERITY_MEDIUM:
                tmp_severity = "medium";
                break;
            case GL_DEBUG_SEVERITY_LOW:
                return;
            case GL_DEBUG_SEVERITY_NOTIFICATION:
                tmp_severity = "notification";
                break;
            default:
                return;
            }

            const std::string& msg = StringUtils::Format("<Source:%s> <Type:%s> <Severity:%s> <ID:%d> <Message:%s>", tmp_source, tmp_type, tmp_severity, id, message);

            if (severity == GL_DEBUG_SEVERITY_HIGH)
            {
                HE_ERROR(msg.c_str());
            }
            else
            {
                HE_LOG(msg.c_str());
            }
        }
    };
}
