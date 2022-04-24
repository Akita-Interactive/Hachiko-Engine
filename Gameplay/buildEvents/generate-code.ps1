$new_line = "`n`n"

# Path to Gameplay directory:
$current_dir =  Split-Path -Path (Get-Location).Path -Parent
# Path to the generated folder:
$generated_folder = 'generated/'
$generated_path = "$current_dir/$generated_folder"
# Path to the util folder:
$util_folder = 'scriptingUtil/'
$util_path = "$current_dir/$util_folder"
# Path to the folder that has scripts inside:
$scripting_folder = '' # Scripting folder is directly the same folder with current_dir.
$scripting_path = $current_dir
# Path to ScriptFactory.cpp:
$script_factory_cpp_path = "$($generated_path)Factory.cpp"
# Path to GeneratedSerialization.cpp:
$serialization_cpp_path = "$($generated_path)SerializationMethods.cpp"
# Name of all the files that are inside the scripts folder:
$files_in_directory = Get-ChildItem -Path $scripting_path

# Path to scripting folder in engine folder:
$engine_scripting_path = 'scripting/'
$engine_scripting_serialization_path = "$($engine_scripting_path)serialization/"
# Namespace that base Script class belongs to:
$scripting_namespace = "Hachiko::Scripting::"
# Include statements for file serialization_cpp_path:
$generated_includes_serialization = @"
#include "$($util_folder)gameplaypch.h"
#include <vector> 
#include <string>
#include <unordered_map>
//#include "$($engine_scripting_serialization_path)ISerializable.h"
//#include "$($engine_scripting_serialization_path)SerializedField.h"

"@
# Include statements for file script_factory_cpp_path:
$generated_includes_script_factory = @"
#include "$($util_folder)gameplaypch.h" 
#include "$($generated_folder)Factory.h"

"@
# Body of the function InstantiateScript inside the file script_factory_cpp_path:
$generated_body_script_factory = @"
$($scripting_namespace)Script* InstantiateScript(Hachiko::GameObject* script_owner, const std::string& script_name)
{
"@
# Body containing all the methods inside the file serialization_cpp_path:
$generated_body_serialization = ''
# Formatted string that generates SerializeTo method:
#0 = script_namespace
#1 = script_class_name
#2 = body
$serialize_method_format = @"
void {0}{1}::SerializeTo(std::unordered_map<std::string, SerializedField>& serialized_fields)
{{
    {0}Script::SerializeTo(serialized_fields);{2}
}}
"@
# Formatted string that generates needed code for one member field:
#0 = field_name
#1 = field_type
$serialize_method_field_format = @"
    serialized_fields["{0}"] = SerializedField(std::string("{0}"), std::make_any<{1}>({0}), std::string("{1}"));
"@
# Formatted string that generates DeserializeFrom method:
#0 = script_namespace
#1 = script_class_name
#2 = body
$deserialize_method_format = @"
void {0}{1}::DeserializeFrom(std::unordered_map<std::string, SerializedField>& serialized_fields)
{{
    {0}Script::DeserializeFrom(serialized_fields);{2}
}}
"@
# Formatted string that generates the if statements inside DeserializeFrom method:
#0 = field_name
#1 = field_type
$deserialize_method_if_format = @"
    if(serialized_fields.find("{0}") != serialized_fields.end())
    {{
        const SerializedField& {0}_sf = serialized_fields["{0}"];
        if ({0}_sf.type_name == "{1}")
        {{
            {0} = std::any_cast<{1}>({0}_sf.copy);
        }}
    }}
"@

Write-Host "Running Pre-Build Event `"Create ScriptFactory & Serialization`""
Write-Host  "Using header files:"

$first_script = $true

foreach ($file in $files_in_directory)
{
    # Only chech header files: (can also use "-Filter *.h" when declaring files_in_directory variable)
    if ($file.Extension -eq ".h") 
    {
        $comment = $false
        $multiline_comment = $false  
        $header_file_as_string = Get-Content $scripting_path/$($file.Name) | ForEach-Object {
            if ($_ -match "/\*") { $multiline_comment =  $true }
            if (-not $multiline_comment) {
                if ($_ -match '^\s*//') { $comment = $true  }
                if (-not $comment) { $_ }
                if ($_ -match '^\s*//') { $comment = $false }
            }
            if ($_ -match "\*/") { $multiline_comment = $false }
        }
        # File content without spaces, tabs and new lines:
        $header_file_as_string_without_spaces = $header_file_as_string -replace '\s+',''
        # Assume class name to be the same with the file name:
        $class_name = [IO.Path]::GetFileNameWithoutExtension($file)
        # If the classs with the same name with the file name inherits 
        # from Script base class, then it's a script file:
        $is_script_file = $header_file_as_string_without_spaces -Match "class$($class_name):publicScript"
        if ($is_script_file.Count -gt 0)
        {
            write-host "Script $($file.Name)"

            # Include statement of the current script file from its file name:
            $current_include = @"
#include "$scripting_folder$($file.Name)"

"@
            # Generate include statements for GeneratedSerialization.cpp:
            $generated_includes_serialization += $current_include
            # Generate include statements for ScriptFactory.cpp:
            $generated_includes_script_factory += $current_include

            # Generate if statement inside the ScriptFactory::InstantiateScript function for current script:
            $generated_body_script_factory += @"

    if (script_name == "$class_name")
    {
        return new $scripting_namespace$class_name(script_owner);
    }

"@ 
            # Get all the SERIALIZE_FIELD statements in the current 
            # header file:
            $parameter_list = select-string -Pattern "(?<=SERIALIZE_FIELD\()(.+?)(?=\);)" -InputObject $header_file_as_string_without_spaces -AllMatches #matches the parameter list

            # Get all the fields marked as SERIALIZE_FIELD, separate
            # them to type and name, and put those into lists:
            $field_types = @()
            $field_names = @()

            foreach ($serialize_field in $parameter_list.Matches.Groups)
            {
                if ($serialize_field.Name -ne "0") #ignore first capture group (it groups all values for the current match)
                {
                    $parameters = select-string -Pattern "(?<type>.+?>,|.+),(?<name>.+)" -InputObject $serialize_field.Value -AllMatches
                    foreach ($param in $parameters.Matches.Groups)
                    {
                        if($param.Name -eq "type"){
                            $field_types += $param.Value
                        }
                        if($param.Name -eq "name"){
                            $field_names += $param.Value
                        }
                    }
                }
            }
            $deserialize_method_body = ''
            $serialize_method_body = ''
            
            for (($i = 0); $i -lt $field_types.Count; $i++)
            {
                $current_type = $field_types[$i]
                $current_name = $field_names[$i]

                $deserialize_method_body += $new_line
                $serialize_method_body += $new_line

                # Append the if statement for the current_name and current_type
                # to be used inside deserialize method:
                $deserialize_method_body += $deserialize_method_if_format -f $current_name,$current_type
                
                # Append the statement for serializing the field corresponding
                # to current_name and current_type, to be used inside serialize method:
                $serialize_method_body += $serialize_method_field_format -f $current_name,$current_type
            }

            # Generate DeserializeFrom method using the generated body:
            $current_deserialize_method = $deserialize_method_format -f $scripting_namespace,$class_name,$deserialize_method_body

            # Generate SerializeTo method using the generated body:
            $current_serialize_method = $serialize_method_format -f $scripting_namespace,$class_name,$serialize_method_body

            $extra_new_line = if ($first_script) { '' } else { $new_line }

            $first_script = $false

            # Add the methods to the string that will be used to generate
            # the file serialization_cpp_path:
            $generated_body_serialization += $extra_new_line + $current_deserialize_method
            $generated_body_serialization += $new_line + $current_serialize_method
        }
    }
}

$generated_body_script_factory += @"

    return nullptr;
}
"@

$generated_body_script_factory = $generated_includes_script_factory + $new_line + $generated_body_script_factory

$generated_body_script_factory | Out-File -FilePath $script_factory_cpp_path

$generated_body_serialization = $generated_includes_serialization + "`n" + $generated_body_serialization
$generated_body_serialization | Out-File -FilePath $serialization_cpp_path

