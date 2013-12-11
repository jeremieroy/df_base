project "df_base"
  language "C++"
  kind     "StaticLib"
  files  { "../include/**.h", "../src/**.cpp" }
  if os.is('windows') then
    excludes { "../src/df/system/posix/**.*" }
  else
    excludes { "d../src/df/system/win32/**.*" }
  end 
  -- uncomment next line to make a shared library + change project "kind"
  --defines { "DF_SYSTEM_SHARED", "DF_SYSTEM_EXPORTS" }
  --vpaths { [""] = "df_system" }
  includedirs { "../include", "../src" }

