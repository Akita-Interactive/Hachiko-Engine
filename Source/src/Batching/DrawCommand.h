#pragma once

namespace Hachiko
{
    struct DrawCommand
    {
        unsigned count; // number of indices in the mesh
        const unsigned instance_count = 1; // number of instances to render
        unsigned first_index; // location of the first vertex relative the current object
        unsigned base_vertex; //  location of current objects first vertex relative buffer
        unsigned base_instance; // current instance for the indirect draw
    };
} // namespace Hachiko