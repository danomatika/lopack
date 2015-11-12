--[[ A solution contains projects, and defines the available configurations

http://industriousone.com/premake/user-guide

example: http://opende.svn.sourceforge.net/viewvc/opende/trunk/build/premake4.lua?revision=1708&view=markup

http://bitbucket.org/anders/lightweight/src/tip/premake4.lua

]]
solution "lopack"
	configurations { "Debug", "Release" }
	objdir "obj"
 
-- lopack library
project "lopack"
	kind "StaticLib"
	language "C++"
	targetdir "../src/lopack"
	files { "../src/lopack/**.h", "../src/lopack/**.cpp" }
	
	configuration "linux"
		buildoptions { "`pkg-config --cflags liblo`" }
		linkoptions { "`pkg-config --libs liblo`" }
	
	configuration "macosx"
		-- Homebrew & MacPorts
		includedirs { "/usr/local/include", "/opt/local/include"}
		libdirs { "/usr/local/lib", "/opt/local/lib" }

	configuration "Debug"
		defines { "DEBUG" }
		flags { "Symbols" }

	configuration "Release"
		defines { "NDEBUG" }
		flags { "Optimize" } 

-- test executable
project "lptest"
	kind "ConsoleApp"
	language "C++"
	targetdir "../src/lptest"
	files { "../src/lptest/**.h", "../src/lptest/**.cpp" }

	includedirs { "../src" }
	links { "lopack" }

	configuration "linux"
		buildoptions { "`pkg-config --cflags liblo`" }
		linkoptions { "`pkg-config --libs liblo`" }

	configuration 'macosx'
		-- Homebrew & MacPorts
		includedirs { "/usr/local/include", "/opt/local/include"}
		libdirs { "/usr/local/lib", "/opt/local/lib" }
		links { "lo", "pthread" }

	configuration "Debug"
		defines { "DEBUG" }
		flags { "Symbols" }

	configuration "Release"
		defines { "NDEBUG" }
		flags { "Optimize" }
