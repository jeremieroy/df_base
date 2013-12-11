-- sample starter premake project. 
-- Duplicate it in any new project and update dofile include path accordingly

-- Dependencies overview

solution "df"
  configurations { "Debug", "Release" }
  platforms { "x32", "x64" }
  location "_build"
 
  configuration "Debug"
    targetdir "_bin/debug"
    debugdir "_env"
    defines { "_DEBUG", "DEBUG" }
    flags   { "Symbols" }

  configuration "Release"
    targetdir "_bin/release"
    debugdir "_env"
    defines { "NDEBUG" }
    flags   { "Optimize" }

  configuration "vs*"
    defines { "_CRT_SECURE_NO_WARNINGS",   "__STDC_LIMIT_MACROS", "__STDC_CONSTANT_MACROS", "__STDC_FORMAT_MACROS",
     "_HAS_EXCEPTIONS=0",
      "_HAS_ITERATOR_DEBUGGING=0",
      "_SCL_SECURE=0",
      "_SECURE_SCL=0",
      "_SCL_SECURE_NO_WARNINGS",
      "_CRT_SECURE_NO_WARNINGS",
      "_CRT_SECURE_NO_DEPRECATE" }
 
  if _ACTION == "clean" then
    os.rmdir("_bin")
    os.rmdir("_build")
  end


-- Projects description

dofile "premake/UnitTest++.lua"

dofile "premake/df_base.lua"

dofile "premake/df_base_tests.lua"

--[[

]]