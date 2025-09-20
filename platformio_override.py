from SCons.Script import AddPreAction

def add_switch_enum_only_to_src(target, source, env):
    src_path = str(source[0])
    if "/src/" in src_path:   # Only files in your src folder
        env.Append(CCFLAGS=["-Werror=switch-enum"])

AddPreAction("$BUILD_DIR/src/*.o", add_switch_enum_only_to_src)
