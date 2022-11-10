import glob
import yaml
import os

PREFAB_RESOURCE_TYPE = 13
GENERATED_PREFABS_PATH = "../Game/assets/prefabs/generated"


# Find all prefab references
used_prefabs = set()
for f in glob.iglob("../Game/**/*.meta", recursive=True):
    if "generated" in f: continue
    with open(f, "r") as stream:
        try:
            metadata = yaml.safe_load(stream)
            resources = metadata["resources"]
            for resource in resources:
                if resource["resource_type"] == PREFAB_RESOURCE_TYPE:
                    used_prefabs.add(resource["resource_id"])
        except yaml.YAMLError as exc:
            print(exc)

# Find all generated prefabs ids
all_generated_prefabs = set()
for f in glob.iglob(f"{GENERATED_PREFABS_PATH}/*.meta", recursive=True):
    prefab_id = int(f.split("\\")[-1].split(".")[0])
    all_generated_prefabs.add(prefab_id)

# Remove generated prefabs that dont exist in any meta file (their model was removed)
unused_generated_prefabs = all_generated_prefabs - used_prefabs

print("Unused generated prefabs to clean")
print(unused_generated_prefabs)

for unused_generated_id in unused_generated_prefabs:
    generated_prefab_file = f"{GENERATED_PREFABS_PATH}/{unused_generated_id}.prefab"
    generated_prefab_meta_file = f"{generated_prefab_file}.meta"
    try:
        os.remove(generated_prefab_file)
    except FileNotFoundError: 
        print(f"{generated_prefab_file} not found")
    try:
        os.remove(generated_prefab_meta_file)
    except FileNotFoundError:
        print(f"{generated_prefab_meta_file} not found")
