import os
import sys
import unreal

if (sys.argv): 
    
    pair_system_name = sys.argv[0]


    actor_subsystem = unreal.EditorActorSubsystem()
    actors = actor_subsystem.get_all_level_actors()

    pair_system = False

    for actor in actors:
        if actor.get_actor_label() == pair_system_name: pair_system = actor

    # Now that we've found our pair system, we can work with the actors inside of it
    if (pair_system):


        # GOING TO PAUSE HERE, THIS IS STARTING TO FEEL TOO REDUNDANT TO WHAT I DID IN C++ BIND
        def withinPairSystem(actor):
            bounding_box = pair_system.get_actor_bounds()
            actor_loc = actor.get_actor_location()
            return False

        actors_within_pair_system = filter(withinPairSystem, actors)





        def isLMpair(actor):
            # Ideally would've been directly able to get the class name
            # this is kind of a workaround assuming that the default name will always be the same as the class name
            default_label = actor.get_default_actor_label()
            return default_label == "LMpair"

        LMpairs = list(filter(isLMpair, actors_within_pair_system)) # Note: filter returns an iterator, which you must manually convert back into a list

        for pair in LMpairs: print(pair.get_actor_label())




        def calculateMirrorRotation(LMpair):
            # Each mirror physically "points at" what its up vector is in the engine
            up_vector = LMpair.get_actor_up_vector()
            up_vector_rotation = up_vector.rotator()
            return up_vector_rotation

        rotators = list(map(calculateMirrorRotation, LMpairs))

        for rotator in rotators: print(rotator)

else:
    print("You need to provide the name of the pair system configuration you would like to export!")