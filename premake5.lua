workspace "bezier"
	configurations { "debug", "release" }

project "bezier"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"

	targetdir "bin/%{cfg.buildcfg}"
	objdir "obj"

	files { "source/**.hpp", "source/**.cpp" }

	links { "SDL2" }

	filter { "configurations:debug" }
		symbols "On"

	filter { "configurations:release" }
		optimize "On"