project "UnitTest++"
  language "C++"
  kind     "StaticLib"
  files  { "../third_party/UnitTest++/src/**.h", "../third_party/UnitTest++/src/**.cpp" }
  vpaths { [""] = "../third_party/UnitTest++/src" }  
  defines { "_LIB", "_CRT_SECURE_NO_DEPRECATE" }
  excludes { "../third_party/UnitTest++/src/tests/**.*" }
  if os.is('windows') then
    excludes { "../third_party/UnitTest++/src/Posix/**.*" }
  else
    excludes { "../third_party/UnitTest++/src/Win32/**.*" }
  end  