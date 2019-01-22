#! /usr/bin/env python3
import os
import re

def find_sources(path):
    sources = tuple(filter(lambda f: f.endswith(".cpp"), os.listdir(path)))
    objects = tuple(map(lambda f: re.sub(".cpp$", ".o", f), sources))

    # add path prefix to files
    sources = tuple(map(lambda f: path + "/" + f, sources))
    objects = tuple(map(lambda f: "build/" + path + "/" + f, objects))

    return sources, objects


with open("build.ninja", "w") as bf:

    # find engine sources
    sources, objects = find_sources(".")

    # include rules
    print("include ninja/rules.ninja\n", file=bf)

    # create build directories
    print("build build/: mkdir\n", file=bf)
    print("build build/test: mkdir\n", file=bf)


    # build engine files
    for s, o in zip(sources, objects):
        print("build {}: cpp {}".format(o, s), file=bf)

    # build engine library
    print("\nbuild build/libwsdl2.so: link-shared " + " ".join(objects) + "\n", file=bf)


    # find test sources
    sources, objects = find_sources("test")
    binaries = tuple(map(lambda f: re.sub(".o$", "", f), objects))

    # build tests
    for s, o, b in zip(sources, objects, binaries):
        print("build {}: cpp {}".format(o, s), file=bf)
        print("build {}: link {}".format(b, o), file=bf)
        print("    lflags = $lflags build/libwsdl2.so\n", file=bf)
