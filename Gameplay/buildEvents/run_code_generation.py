import os
import re

# Path to Gameplay directory:
current_dir = os.curdir + '/../'
# Path to the generated folder:
generated_folder = 'generated/'
generated_path = current_dir + generated_folder
# Path to the util folder:
util_folder = 'scriptingUtil/'
util_path = current_dir + util_folder
# Path to the folder that has scripts inside:
scripting_folder = '' # Scripting folder is directly the same folder with current_dir.
scripting_path = current_dir
# Path to Factory.cpp:
script_factory_cpp_path = generated_path + 'Factory.cpp'
# Path to GeneratedSerialization.cpp:
serialization_cpp_path = generated_path + 'SerializationMethods.cpp'
# Path to GeneratedEditor.cpp:
editor_cpp_path = generated_path + 'GeneratedEditor.cpp'
# Path to SaveAndLoadMethods.cpp:
save_load_cpp_path = generated_path + 'SaveAndLoadMethods.cpp'

# Name of all the files that are inside the scripts folder:
files_in_directory = os.listdir(scripting_path)

# Path to scripting folder in engine folder:
engine_scripting_path = 'scripting/'
engine_scripting_serialization_path = engine_scripting_path + 'serialization/'
# Namespace that base Script class belongs to:
scripting_namespace = 'Hachiko::Scripting::'

# Include start:
include_start = '#include \"'

# Include statements for file serialization_cpp_path:
generated_includes_serialization = ('#include <vector>\n' +
    '#include <string>\n'+
    '#include <unordered_map>\n'+
    (include_start + util_folder + 'gameplaypch.h\"\n'))
# Include statements for file script_factory_cpp_path:
generated_includes_script_factory = ('#include \"'+util_folder+'gameplaypch.h\"\n' + 
    include_start + generated_folder + 'Factory.h\"\n')
# Include statements for file editor_cpp_path:
generated_includes_on_editor = (
    include_start + util_folder + 'gameplaypch.h\"\n')
#include statements for save_load_cpp_path:
generated_includes_save_load = (
    include_start + util_folder + 'gameplaypch.h\"\n'
    '#include <yaml-cpp/yaml.h>\n#include <core/serialization/TypeConverter.h>\n'
)

# Body containing all the methods inside the file serialization_cpp_path:
generated_body_serialization = ''
# Body of the function InstantiateScript inside the file script_factory_cpp_path:
generated_body_script_factory = scripting_namespace + 'Script* InstantiateScript(Hachiko::GameObject* script_owner, const std::string& script_name)\n{'
# Body containing all OnEditor methods for scripts inside the editor_cpp_path:
generated_body_editor = ''
# Body containing all OnLoad and OnSave methods for scripts inside the save_load_cpp_path:
generated_body_save_load = ''

# Formatted string that generates SerializeTo method:
serialize_method_format = ('{new_line}void {script_namespace}{script_class_name}::SerializeTo(std::unordered_map<std::string, SerializedField>& serialized_fields)'
+ '{new_line}{left_curly}'
+ '{new_line}{tab}{script_namespace}Script::SerializeTo(serialized_fields);'
+ '{body}'
+ '{new_line}{right_curly}')
# Formatted string that generates needed code for one member field:
serialize_method_field_format = (
    '{new_line}{tab}serialized_fields[\"{field_name}\"] = SerializedField(std::string(\"{field_name}\"), std::make_any<{field_type}>({field_name}), std::string(\"{field_type}\"));'
)
# Formatted string that generates DeserializeFrom method:
deserialize_method_format = ('{new_line}void {script_namespace}{script_class_name}::DeserializeFrom(std::unordered_map<std::string, SerializedField>& serialized_fields)'
+ '{new_line}{left_curly}'
+ '{new_line}{tab}{script_namespace}Script::DeserializeFrom(serialized_fields);'
+ '{body}'
+ '{new_line}{right_curly}')
# Formatted string that generates the if statements inside DeserializeFrom method:
deserialize_method_if_format = ('' 
+ '{new_line}{tab}if(serialized_fields.find(\"{field_name}\") != serialized_fields.end())' 
+ '{new_line}{tab}{left_curly}'
+ '{new_line}{tab}{tab}const SerializedField& {field_name}_sf = serialized_fields[\"{field_name}\"];'
+ '{new_line}{tab}{tab}if ({field_name}_sf.type_name == \"{field_type}\")'
+ '{new_line}{tab}{tab}{left_curly}'
+ '{new_line}{tab}{tab}{tab}{field_name} = std::any_cast<{field_type}>({field_name}_sf.copy);'
+ '{new_line}{tab}{tab}{right_curly}'     
+ '{new_line}{tab}{right_curly}')

# Formatted string that generates Script::OnEditor for said script:
on_editor_method_format = (
    '{new_line}void {script_namespace}{script_class_name}::OnEditor()'
    '{new_line}{left_curly}'
    '{body}'
    '{new_line}{right_curly}'
)
# Formatted string that generates Editor::Show for Default types:
editor_show_default_format = (
    '{new_line}{tab}Editor::Show(\"{field_name}\", {field_name});'
)
# Formatted string that generates Editor::Show for Components (and scripts):
editor_show_components_format = (
    '{new_line}{tab}Editor::Show<{non_ptr_field_type}>(\"{field_name}\", \"{field_type}\", {field_name});'
)

# Formatted string that generates Script::OnSave:
on_save_method_format = (
    '{new_line}void {script_namespace}{script_class_name}::OnSave(YAML::Node& node) const'
    '{new_line}{left_curly}'
    '{body}'
    '{new_line}{right_curly}'
)
# Formatted string that generates Script::OnLoad:
on_load_method_format = (
    '{new_line}void {script_namespace}{script_class_name}::OnLoad()'
    '{new_line}{left_curly}'
    '{body}'
    '{new_line}{right_curly}'
)
# Formatted string that generates save instruction for Default types except GameObject*:
save_default_except_game_object_ptr_body_format = (
    '{new_line}{tab}node[\"\'{field_name}@{field_type}\'\"] = {field_name};' 
)
# Formatted string that generates save instruction for GameObject*:
save_game_object_ptr_body_format = (
    '{new_line}{tab}if ({field_name} != nullptr)'
    '{new_line}{tab}{left_curly}'
    '{new_line}{tab}{tab}node[\"\'{field_name}@{field_type}\'\"] = {field_name}->GetID();' 
    '{new_line}{tab}{right_curly}'
    '{new_line}{tab}else'
    '{new_line}{tab}{left_curly}'
    '{new_line}{tab}{tab}node[\"\'{field_name}@{field_type}\'\"] = 0;' 
    '{new_line}{tab}{right_curly}'
)
# Formatted string that generates save instruction for Component*:
save_component_ptr_body_format = (
    '{new_line}{tab}if ({field_name} != nullptr && {field_name}->GetGameObject() != nullptr)'
    '{new_line}{tab}{left_curly}'
    '{new_line}{tab}{tab}node[\"\'{field_name}@{field_type}\'\"] = {field_name}->GetGameObject()->GetID();' 
    '{new_line}{tab}{right_curly}'
    '{new_line}{tab}else'
    '{new_line}{tab}{left_curly}'
    '{new_line}{tab}{tab}node[\"\'{field_name}@{field_type}\'\"] = 0;' 
    '{new_line}{tab}{right_curly}'
)

# Formatted string that generates load instruction for Default types except GameObject*:
load_default_except_game_object_ptr_body_format = (
    '{new_line}{tab}if (load_node[\"\'{field_name}@{field_type}\'\"].IsDefined())'
    '{new_line}{tab}{left_curly}'
    '{new_line}{tab}{tab}{field_name} = load_node[\"\'{field_name}@{field_type}\'\"].as<{field_type}>();' 
    '{new_line}{tab}{right_curly}'
)
# Formatted string that generates load instruction for GameObject*:
load_game_object_ptr_body_format = (
    '{new_line}{tab}if (load_node[\"\'{field_name}@{field_type}\'\"].IsDefined())'
    '{new_line}{tab}{left_curly}'
    '{new_line}{tab}{tab}{field_name} = SceneManagement::FindInCurrentScene(load_node[\"\'{field_name}@{field_type}\'\"].as<unsigned long long>());'
    '{new_line}{tab}{right_curly}'
)
# Formatted string that generates load instruction for Component*:
load_component_ptr_body_format = (
    '{new_line}{tab}if (load_node[\"\'{field_name}@{field_type}\'\"].IsDefined())'
    '{new_line}{tab}{left_curly}'
    '{new_line}{tab}{tab}GameObject* {field_name}_owner__temp = SceneManagement::FindInCurrentScene(load_node[\"\'{field_name}@{field_type}\'\"].as<unsigned long long>());'
    '{new_line}{tab}{tab}if ({field_name}_owner__temp != nullptr)'
    '{new_line}{tab}{tab}{left_curly}'
    '{new_line}{tab}{tab}{tab}{field_name} = {field_name}_owner__temp->GetComponent<{non_ptr_field_type}>();' 
    '{new_line}{tab}{tab}{right_curly}'
    '{new_line}{tab}{right_curly}'
)

# Allowed editor types for scripts:
editor_allowed_default_types = [
    'int', 'unsigned int', 'unsigned', 'uint', 'float', 
    'double', 'bool', 'string', 'std::string', 
    'math::float2', 'math::float3', 'math::float4', 'math::Quat',
    'float2', 'float3', 'float4', 'Quat', 'GameObject*'
]
editor_allowed_component_types = [
    'ComponentAnimation*', 'ComponentButton*',
    'ComponentCamera*', 'ComponentCanvas*',
    'ComponentCanvasRenderer*', 'ComponentDirLight*',
    'ComponentDirLight*', 'ComponentImage*',
    'ComponentMaterial*', 'ComponentMesh*',
    'ComponentPointLight*', 'ComponentProgressBar*',
    'ComponentSpotLight*', 'ComponentText*',
    'ComponentTransform*', 'ComponentTransform2D*'
]

# This beautiful piece of function was found from stack overflow:
def delete_comments_from_file(text):
    def replacer(match):
        s = match.group(0)
        if s.startswith('/'):
            return " "
        else:
            return s

    pattern = re.compile(
        r'//.*?$|/\*.*?\*/|\'(?:\\.|[^\\\'])*\'|"(?:\\.|[^\\"])*"',
        re.DOTALL | re.MULTILINE
    )
    return re.sub(pattern, replacer, text)

print("Running Pre-Build Event \"Create ScriptFactory & Serialization\"")
print("Using header files:")

# Take all the script header files in the directory:
script_classes = []
for file_name in files_in_directory:
    # Only check header files:
    if file_name[-2:] == '.h':
        header_file = open(scripting_path+file_name)
        header_file_as_string = header_file.read()
        header_file_as_string = (delete_comments_from_file(header_file_as_string))

        # File content without spaces, tabs and new lines:
        header_file_as_string_without_spaces = re.sub(r"[\n\t\s]*", "", header_file_as_string)
        # Assume class name to be the same with the file name:
        class_name = file_name[0:-2]
        # If the classs with the same name with the file name inherits 
        # from Script base class, then it's a script file:
        is_script_file = len(re.findall(re.escape('class'+class_name)+'(.*)'+re.escape(':publicScript'), header_file_as_string_without_spaces)) > 0

        if is_script_file:
            print("Script: " + file_name)
            script_classes.append(class_name)

first_script = True

for script_class in script_classes:
    header_file = open(scripting_path + script_class + '.h')
    header_file_as_string = header_file.read()
    header_file_as_string = (delete_comments_from_file(header_file_as_string))

    # File content without spaces, tabs and new lines:
    header_file_as_string_without_spaces = re.sub(r"[\n\t\s]*", "", header_file_as_string)
    # Include statement of the current script file from its file name:
    current_include = include_start + scripting_folder + script_class + '.h' + '\"\n'
    # Generate include statements for GeneratedSerialization.cpp:
    generated_includes_serialization += current_include
    # Generate include statements for ScriptFactory.cpp:
    generated_includes_script_factory += current_include
    # Generate include statements for GeneratedEditor.cpp:
    generated_includes_on_editor += current_include
    # Generate include statements for SaveAndLoadMethods.cpp:
    generated_includes_save_load += current_include

    # Generate if statement inside the ScriptFactory::InstantiateScript function for current script:
    generated_body_script_factory += '\n\tif (script_name == \"' + script_class + '\")\n\t{\n\t\treturn new ' + scripting_namespace + script_class + '(script_owner);\n\t}\n'

    # Get all the SERIALIZE_FIELD statements in the current 
    # header file:
    serialize_fields = re.findall(re.escape('SERIALIZE_FIELD(')+'(.*?)'+re.escape(');'), header_file_as_string_without_spaces)
    
    # Get all the fields marked as SERIALIZE_FIELD, separate
    # them to type and name, and put those into lists:
    field_types = []
    field_names = []
    for serialize_field in serialize_fields:
        # Find the comma that comes last, as a template type
        # may have multiple commas in it. But for sure,
        # the last comma will be the comma that separates
        # type and name:
        separator_comma_index = serialize_field.rfind(',')

        field_types.append(serialize_field[:separator_comma_index])
        field_names.append(serialize_field[min(len(serialize_field) - 1, separator_comma_index + 1):])

    # Generate DeserializeFrom and SerializeTo method bodies 
    # from the serialize_fields for current class: 
    deserialize_method_body = ''
    serialize_method_body = ''
    # Generate OnEditor method bodues from the serialize_fields 
    # for current class:
    on_editor_method_body = ''
    # Generate OnLoad and OnSave method bodies from the serialize_fields
    # for current class:
    on_save_method_body = ''
    on_load_method_body = ''
    first_field = True
    for i in range(len(field_names)):
        current_name = field_names[i]
        current_type = field_types[i]

        deserialize_method_body += '\n'
        serialize_method_body += '\n'
        
        save_load_line_break = ''
        if first_field == False:
            save_load_line_break = '\n'
        first_field = False

        # Append the if statement for the current_name and current_type
        # to be used inside deserialize method:
        deserialize_method_body += deserialize_method_if_format.format(
            field_name = current_name,
            field_type = current_type,
            new_line = '\n',
            tab = '\t',
            left_curly = '{', 
            right_curly = '}'
        )
        # Append the statement for serializing the field corresponding
        # to current_name and current_type, to be used inside 
        # serialize method:
        serialize_method_body += serialize_method_field_format.format(
            field_name = current_name,
            field_type = current_type,
            new_line = '\n',
            tab = '\t'
        )

        # OnEditor, OnSave and OnLoad related:
        if current_type in editor_allowed_default_types:
            on_editor_method_body += editor_show_default_format.format(
                field_name = current_name,
                new_line = '\n',
                tab = '\t'
            )
            if current_type == "GameObject*":
                
                on_save_method_body += save_load_line_break
                on_load_method_body += save_load_line_break

                on_save_method_body += save_game_object_ptr_body_format.format(
                    field_name = current_name,
                    field_type = current_type,
                    left_curly = '{',
                    right_curly = '}',
                    new_line = '\n',
                    tab = '\t'
                )
                on_load_method_body += load_game_object_ptr_body_format.format(
                    field_name = current_name,
                    field_type = current_type,
                    left_curly = '{',
                    right_curly = '}',
                    new_line = '\n',
                    tab = '\t'
                )
            else:
                on_save_method_body += save_load_line_break
                on_load_method_body += save_load_line_break

                on_save_method_body += save_default_except_game_object_ptr_body_format.format(
                    field_name = current_name,
                    field_type = current_type,
                    new_line = '\n',
                    tab = '\t'
                )
                on_load_method_body += load_default_except_game_object_ptr_body_format.format(
                    field_name = current_name,
                    field_type = current_type,
                    left_curly = '{',
                    right_curly = '}',
                    new_line = '\n',
                    tab = '\t'
                )
        elif current_type in editor_allowed_component_types:
            # As the types inside editor_allowed_component_types are typed by hand,
            # we assume it has no * in somewhere nonsense, but at the end:
            without_pointer = current_type[:-1]
            on_editor_method_body += editor_show_components_format.format(
                field_name = current_name,
                field_type = current_type,
                non_ptr_field_type = without_pointer,
                new_line = '\n',
                tab = '\t'
            )

            on_save_method_body += save_load_line_break
            on_load_method_body += save_load_line_break

            on_save_method_body += save_component_ptr_body_format.format(
                field_name = current_name,
                field_type = current_type,
                left_curly = '{',
                right_curly = '}',
                new_line = '\n',
                tab = '\t'
            )
            on_load_method_body += load_component_ptr_body_format.format(
                field_name = current_name,
                field_type = current_type,
                non_ptr_field_type = without_pointer,
                left_curly = '{',
                right_curly = '}',
                new_line = '\n',
                tab = '\t'
            )
        else:
            # If the type has an asterisk at the end:
            if current_type[-1] == '*':
                # Get the type without asterisk:
                without_pointer = current_type[:-1]
                # If the type without pointer is a script:
                if without_pointer in script_classes:
                    on_editor_method_body += editor_show_components_format.format(
                        field_name = current_name,
                        field_type = current_type,
                        non_ptr_field_type = without_pointer,
                        new_line = '\n',
                        tab = '\t'
                    )
                    # TODO: Add script* save & load here

    # Generate OnSave method using the generated body:
    current_on_save_method = on_save_method_format.format(
        script_namespace = scripting_namespace, 
        script_class_name = script_class, 
        new_line = '\n', 
        left_curly = '{', 
        right_curly = '}', 
        body = on_save_method_body
    )
    # Generate OnLoad method using the generated body:
    current_on_load_method = on_load_method_format.format(
        script_namespace = scripting_namespace, 
        script_class_name = script_class, 
        new_line = '\n', 
        left_curly = '{', 
        right_curly = '}', 
        body = on_load_method_body
    )

    # Generate OnEditor method using the generated body:
    current_on_editor_method = on_editor_method_format.format(
        script_namespace = scripting_namespace, 
        script_class_name = script_class, 
        new_line = '\n', 
        left_curly = '{', 
        right_curly = '}', 
        body = on_editor_method_body
    )

    # Generate DeserializeFrom method using the generated body:
    current_deserialize_method = deserialize_method_format.format(
        script_namespace = scripting_namespace, 
        script_class_name = script_class, 
        new_line = '\n', 
        left_curly = '{', 
        right_curly = '}', 
        tab = '\t',
        body = deserialize_method_body)
    # Generate SerializeTo method using the generated body:
    current_serialize_method = serialize_method_format.format(
        script_namespace = scripting_namespace, 
        script_class_name = script_class, 
        new_line = '\n', 
        left_curly = '{', 
        right_curly = '}', 
        tab = '\t',
        body = serialize_method_body
    )

    extra_new_line = '' if first_script == True else '\n'
    first_script = False

    # Add the methods to the string that will be used to generate
    # the file serialization_cpp_path:
    generated_body_serialization += extra_new_line + current_deserialize_method
    generated_body_serialization += '\n' + current_serialize_method
    generated_body_editor += '\n' + current_on_editor_method
    generated_body_save_load += '\n' + current_on_save_method
    generated_body_save_load += '\n' + current_on_load_method
            

generated_body_script_factory += '\n\treturn nullptr;\n}'
generated_body_script_factory = generated_includes_script_factory + '\n\n' + generated_body_script_factory
open(script_factory_cpp_path, 'w').write(generated_body_script_factory)

generated_body_serialization = generated_includes_serialization + '\n' + generated_body_serialization
open(serialization_cpp_path, 'w').write(generated_body_serialization)

generated_body_editor = generated_includes_on_editor + generated_body_editor
open(editor_cpp_path, 'w').write(generated_body_editor)

generated_body_save_load = generated_includes_save_load + '\n' + generated_body_save_load
open(save_load_cpp_path, 'w').write(generated_body_save_load)