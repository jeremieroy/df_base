 project "df_base_tests"
  language "C++"
  kind     "ConsoleApp"
  files  { "../tests/**.h", "../tests/**.cpp" }
  ---vpaths { [""] = "df_system_test/src" }
  links { "UnitTest++", "df_base" }
  includedirs { "../third_party/UnitTest++/src", "../include" }
