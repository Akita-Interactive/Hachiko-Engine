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
# Path to ScriptFactory.cpp:
script_factory_cpp_path = generated_path + 'Factory.cpp'
# Path to GeneratedSerialization.cpp:
serialization_cpp_path = generated_path + 'SerializationMethods.cpp'
# Name of all the files that are inside the scripts folder:
files_in_directory = os.listdir(scripting_path)

# Path to scripting folder in engine folder:
engine_scripting_path = 'scripting/'
engine_scripting_serialization_path = engine_scripting_path + 'serialization/'
# Namespace that base Script class belongs to:
scripting_namespace = 'Hachiko::Scripting::'
# Include statements for file serialization_cpp_path:
generated_includes_serialization = ('#include <vector>\n' + 
    '#include <string>\n'
    '#include <unordered_map>\n'
    '#include \"'+util_folder+'gameplaypch.h\"\n' 
    # '#include \"'+engine_scripting_serialization_path+'ISerializable.h\"\n' 
    # '#include \"'+engine_scripting_serialization_path+'SerializedField.h\"\n'
    )
# Include statements for file script_factory_cpp_path:
generated_includes_script_factory = ('#include \"'+util_folder+'gameplaypch.h\"\n' + 
    '#include \"'+generated_folder+'Factory.h\"\n')
# Body of the function InstantiateScript inside the file script_factory_cpp_path:
generated_body_script_factory = scripting_namespace + 'Script* InstantiateScript(Hachiko::GameObject* script_owner, const std::string& script_name)\n{'
# Body containing all the methods inside the file serialization_cpp_path:
generated_body_serialization = ''

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

print("Running Pre-Build Event \"Create ScriptFactory & Serialization\"")
print("Using header files:")

first_script = True

for file_name in files_in_directory:
    # Only chech header files:
    if file_name[-2:] == '.h':
        header_file = open(scripting_path+file_name)
        header_file_as_string = header_file.read()
        # File content without spaces, tabs and new lines:
        header_file_as_string_without_spaces = re.sub(r"[\n\t\s]*", "", header_file_as_string)
        # Assume class name to be the same with the file name:
        class_name = file_name[0:-2]
        # If the classs with the same name with the file name inherits 
        # from Script base class, then it's a script file:
        is_script_file = len(re.findall(re.escape('class'+class_name)+'(.*)'+re.escape(':publicScript'), header_file_as_string_without_spaces)) > 0

        if is_script_file:
            print("Script: " + file_name)

            # Include statement of the current script file from its file name:
            current_include = '#include \"' + scripting_folder + file_name + '\"\n'
            # Generate include statements for GeneratedSerialization.cpp:
            generated_includes_serialization += current_include
            # Generate include statements for ScriptFactory.cpp:
            generated_includes_script_factory += current_include

            # Generate if statement inside the ScriptFactory::InstantiateScript function for current script:
            generated_body_script_factory += '\n\tif (script_name == \"' + class_name + '\")\n\t{\n\t\treturn new ' + scripting_namespace + class_name + '(script_owner);\n\t}\n'

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
            for i in range(len(field_names)):
                current_name = field_names[i]
                current_type = field_types[i]

                deserialize_method_body += '\n'
                serialize_method_body += '\n'

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

            # Generate DeserializeFrom method using the generated body:
            current_deserialize_method = deserialize_method_format.format(
                script_namespace = scripting_namespace, 
                script_class_name = class_name, 
                new_line = '\n', 
                left_curly = '{', 
                right_curly = '}', 
                tab = '\t',
                body = deserialize_method_body)
            # Generate SerializeTo method using the generated body:
            current_serialize_method = serialize_method_format.format(
                script_namespace = scripting_namespace, 
                script_class_name = class_name, 
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
            
            

generated_body_script_factory += '\n\treturn nullptr;\n}'
generated_body_script_factory = generated_includes_script_factory + '\n\n' + generated_body_script_factory
open(script_factory_cpp_path, 'w').write(generated_body_script_factory)

generated_body_serialization = generated_includes_serialization + '\n' + generated_body_serialization
open(serialization_cpp_path, 'w').write(generated_body_serialization)