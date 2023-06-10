import os


def create_and_write_file():
    print("writing")
    dest = "../../../../../../Users/soula/Documents/Unreal Projects/MirrorsRaytracingV3/Content/PythonScriptOutput/test.txt"
    fi = open(dest, "w+") # we want w+ because it overwrites any old file instead of appending to it
    fi.write("testing \n")
    fi.write("testing2 \n")
    fi.close()

create_and_write_file()