workspace "Jass"
	architecture "x64"
	configurations { "Debug", "Release", "Dist" }
	startproject "Sandbox"

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

--include directories relative to solution directory
IncludeDir = {}
IncludeDir["GLFW"] = "Jass/vendor/GLFW/include"
IncludeDir["Glad"] = "Jass/vendor/Glad/include"
IncludeDir["ImGui"] = "Jass/vendor/imgui"
IncludeDir["glm"] = "Jass/vendor/glm"

include "Jass/vendor/GLFW"
include "Jass/vendor/Glad"
include "Jass/vendor/imgui"

project "Jass"
	location "Jass"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"
	staticruntime "On"

	targetdir ("bin/".. outputdir .. "/%{prj.name}")
	objdir ("bin-int/".. outputdir .. "/%{prj.name}")

	pchheader "jasspch.h"
	pchsource "Jass/src/jasspch.cpp"

	files {
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp",
		"%{prj.name}/vendor/glm/glm/**.hpp",
		"%{prj.name}/vendor/glm/glm/**.inl"
	}

	includedirs {
		"%{prj.name}/vendor/spdlog/include",
		"Jass/src",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.Glad}",
		"%{IncludeDir.ImGui}",
		"%{IncludeDir.glm}"
	}

	links { "GLFW", "opengl32.lib", "Glad", "ImGui" }

	filter "system:windows"
		systemversion "latest"
		
		defines { "JASS_PLATFORM_WINDOWS", "JASS_BUILD_DLL", "GLFW_INCLUDE_NONE" }

	filter "configurations:Debug"
		defines "JASS_DEBUG"
		runtime "Debug"
		symbols "On"

	filter "configurations:Release"
		defines "JASS_RELEASE"
		runtime "Release"
		optimize "On"

	filter "configurations:Dist"
		defines "JASS_DIST"
		runtime "Release"
		optimize "On"

project "Sandbox"
	location "Sandbox"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "On"

	targetdir ("bin/".. outputdir .. "/%{prj.name}")
	objdir ("bin-int/".. outputdir .. "/%{prj.name}")

	files { "%{prj.name}/src/**.h", "%{prj.name}/src/**.cpp" }

	includedirs { "Jass/vendor/spdlog/include", "Jass/src", "%{IncludeDir.glm}", "%{IncludeDir.ImGui}" }

	links { "Jass" }

	filter "system:windows"
		systemversion "latest"

		defines { "JASS_PLATFORM_WINDOWS" }

	filter "configurations:Debug"
		defines "JASS_DEBUG"
		runtime "Debug"
		symbols "On"

	filter "configurations:Release"
		defines "JASS_RELEASE"
		runtime "Release"
		optimize "On"

	filter "configurations:Dist"
		defines "JASS_DIST"
		runtime "Release"
		optimize "On"