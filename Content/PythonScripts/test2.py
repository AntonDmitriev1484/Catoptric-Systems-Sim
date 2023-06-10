import os
import unreal #Always running this script in UE5 console, so this will be resolved

# The unreal module exposes nearly everything that is exposed from C++ to Blueprints in your Editor environment. 
# It's not pre-generated; it automatically reflects whatever is available in Blueprints in your Editor. 
# As you enable new plugins in the Unreal Editor, anything those plugins expose to Blueprints also becomes available in Python as well. 
# The same goes for any C++ code that you write in your Project and expose to Blueprints.

actor_subsystem = unreal.EditorActorSubsystem()
actors = actor_subsystem.get_all_level_actors()

for actor in actors:
    print(actor.get_actor_label())

# It's actually this easy to iterate over all actors visible in the world editor wow this is sick.