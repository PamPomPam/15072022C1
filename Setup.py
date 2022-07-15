from distutils.core import setup, Extension

MOD = "Mylib"

setup(name = MOD, ext_modules = 
    [
        Extension(
            MOD,
            sources=['Main.cpp', 'Map.cpp', 'Globals.cpp', 'Gamestate.cpp'],
            extra_compile_args=["-O3"]
        )
    ],
    description = "My C Extension For Matrix Operations"
)
