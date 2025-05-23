# BUILD OPTIONS: HYBUILD_Qt, HYBUILD_SDL2, HYBUILD_SDL_NET, HYBUILD_STATIC
# BUILD EXTRAS:  HYBUILD_SPINE, HYBUILD_RAPIDJSON, HYBUILD_ICU

add_subdirectory(vendor/box2d)
set_target_properties(box2d PROPERTIES FOLDER "Engine Libs")

add_subdirectory(vendor/soil2)

if(HYBUILD_SPINE)
	add_subdirectory(extras/Spine/spine-cpp)
	set_target_properties(spine-cpp PROPERTIES FOLDER "Engine Libs")
	set_target_properties(spine-cpp-lite PROPERTIES FOLDER "Engine Libs")
endif()

if(HYBUILD_RAPIDJSON)
	
else() #nlohmann_json
	# If you only include this third party in PRIVATE source files, you do not need to install it when your main project gets installed.
	set(JSON_Install OFF CACHE INTERNAL "")
	add_subdirectory(vendor/json)
endif()

# Emscripten provides its own ports of SDL, GLFW, and GLEW
if(NOT "${CMAKE_SYSTEM_NAME}" STREQUAL "Emscripten")
	add_subdirectory(vendor/glad)
	
	# Qt provides its own implementation of SDL/GLFW facilities
	if(HYBUILD_Qt)
		set(HYBUILD_SDL2 OFF)
		set(HYBUILD_SDL_NET OFF)
	else()
		# Below is typical desktop deployment
		if(HYBUILD_GLFW)
			add_compile_definitions(HY_USE_GLFW)
			set(GLFW_BUILD_EXAMPLES, OFF)
			set(GLFW_BUILD_TESTS, OFF)
			set(GLFW_BUILD_DOCS, OFF)
			set(GLFW_INSTALL, OFF)
			add_subdirectory(vendor/glfw)
			set_target_properties(glfw PROPERTIES FOLDER "Engine Libs/GLFW3")
			set_target_properties(update_mappings PROPERTIES FOLDER "Engine Libs/GLFW3")
		endif()
		
		if(HYBUILD_SDL2)
			if(HYBUILD_STATIC)
				set(SDL_STATIC ON)
				set(SDL_SHARED OFF)
			else()
				set(SDL_SHARED ON)
			endif()
			
			if (MSVC)
				list(APPEND EXTRA_LIBS vcruntime) # prevents memset issue in vcruntime.dll
			endif()
			
			add_subdirectory(vendor/SDL)
			include_directories(vendor/SDL/include)
			set_target_properties(SDL2main PROPERTIES FOLDER "Engine Libs")
			set_target_properties(SDL2-static PROPERTIES FOLDER "Engine Libs")
			set_target_properties(SDL2 PROPERTIES FOLDER "Engine Libs")

			# Optional SDL2 extensions
			if(HYBUILD_SDL_NET)
				add_subdirectory(vendor/SDL_net)
			endif()
		endif()
	endif()
else()
	set(HYBUILD_STATIC ON)
endif()

set(HYENGINE_SOURCE_FILES
	src/HyEngine.cpp
	src/Afx/HyStdAfx.cpp
	src/Afx/Platforms/HyPlatform_Browser.cpp
	src/Assets/HyAssets.cpp
	src/Assets/Files/HyFileAtlas.cpp
	src/Assets/Files/HyFileAudio.cpp
	src/Assets/Files/HyFilesManifest.cpp
	src/Assets/Files/HyGLTF.cpp
	src/Assets/Nodes/HyNodePath.cpp
	src/Assets/Nodes/IHyNodeData.cpp
	src/Assets/Nodes/Objects/HyAudioData.cpp
	src/Assets/Nodes/Objects/HyPrefabData.cpp
	src/Assets/Nodes/Objects/HySpineData.cpp
	src/Assets/Nodes/Objects/HySpriteData.cpp
	src/Assets/Nodes/Objects/HyTextData.cpp
	src/Assets/Nodes/Objects/HyTexturedQuadData.cpp
	src/Audio/HyAudioCore.cpp
	src/Audio/HySoundAsset.cpp
	src/Diagnostics/HyDiagnostics.cpp
	src/Diagnostics/HyProfiler.cpp
	src/Diagnostics/Console/IHyConsole.cpp
	src/Diagnostics/Console/Interop/HyConsole_Std.cpp
	src/Diagnostics/Console/Interop/HyConsole_Win.cpp
	src/Diagnostics/GuiComms/HyGuiComms.cpp
	src/Diagnostics/GuiComms/HyGuiMessage.cpp
	src/Diagnostics/GuiComms/HyLiveVarManager.cpp
	src/Input/HyInput.cpp
	src/Input/HyInputMap.cpp
	src/Input/IHyController.cpp
	src/Networking/Sockets/HyNullSocket.cpp
	src/Networking/Sockets/HyPosix.cpp
	src/Networking/Sockets/HySdlNet.cpp
	src/Networking/Sockets/HyWinsock.cpp
	src/Networking/IHyNetworkClass.cpp
	src/Networking/IHyNetworking.cpp
	src/Renderer/IHyRenderer.cpp
	src/Renderer/Components/HyMeshBuffer.cpp
	src/Renderer/Components/HyRenderBuffer.cpp
	src/Renderer/Components/HyShaderUniforms.cpp
	src/Renderer/Components/HyVertexBuffer.cpp
	src/Renderer/Effects/HyShader.cpp
	src/Renderer/Effects/HyStencil.cpp
	src/Renderer/OpenGL/HyOpenGL.cpp
	src/Scene/HyScene.cpp
	src/Scene/AnimFloats/HyAnimFloat.cpp
	src/Scene/AnimFloats/HyAnimVec2.cpp
	src/Scene/AnimFloats/HyAnimVec3.cpp
	src/Scene/AnimFloats/HyTweenFuncs.cpp
	src/Scene/Nodes/IHyNode.cpp
	src/Scene/Nodes/IHyNode2d.cpp
	src/Scene/Nodes/IHyNode3d.cpp
	src/Scene/Nodes/Loadables/IHyLoadable.cpp
	src/Scene/Nodes/Loadables/IHyLoadable2d.cpp
	src/Scene/Nodes/Loadables/IHyLoadable3d.cpp
	src/Scene/Nodes/Loadables/Bodies/IHyBody.cpp
	src/Scene/Nodes/Loadables/Bodies/IHyBody2d.cpp
	src/Scene/Nodes/Loadables/Bodies/IHyBody3d.cpp
	src/Scene/Nodes/Loadables/Bodies/Drawables/IHyDrawable.cpp
	src/Scene/Nodes/Loadables/Bodies/Drawables/IHyDrawable2d.cpp
	src/Scene/Nodes/Loadables/Bodies/Drawables/IHyDrawable3d.cpp
	src/Scene/Nodes/Loadables/Bodies/Drawables/Objects/HyPrefab3d.cpp
	src/Scene/Nodes/Loadables/Bodies/Drawables/Objects/HyPrimitive2d.cpp
	src/Scene/Nodes/Loadables/Bodies/Drawables/Objects/HySpine2d.cpp
	src/Scene/Nodes/Loadables/Bodies/Drawables/Objects/HySprite2d.cpp
	src/Scene/Nodes/Loadables/Bodies/Drawables/Objects/HyText2d.cpp
	src/Scene/Nodes/Loadables/Bodies/Drawables/Objects/HyTexturedQuad2d.cpp
	src/Scene/Nodes/Loadables/Bodies/Drawables/Objects/IHySprite.cpp
	src/Scene/Nodes/Loadables/Bodies/Drawables/Objects/IHySpine.cpp
	src/Scene/Nodes/Loadables/Bodies/Drawables/Objects/IHyText.cpp
	src/Scene/Nodes/Loadables/Bodies/Objects/HyActor2d.cpp
	src/Scene/Nodes/Loadables/Bodies/Objects/HyEntity2d.cpp
	src/Scene/Nodes/Loadables/Bodies/Objects/HyEntity3d.cpp
	src/Scene/Nodes/Loadables/Bodies/Objects/HyPhysicsGrid2d.cpp
	src/Scene/Nodes/Loadables/Objects/HyAudio2d.cpp
	src/Scene/Nodes/Loadables/Objects/HyAudio3d.cpp
	src/Scene/Nodes/Loadables/Objects/IHyAudio.cpp
	src/Scene/Nodes/Objects/HyCamera.cpp
	src/Scene/Nodes/Objects/IHyCamera.cpp
	src/Scene/Physics/HyPhysicsBody.cpp
	src/Scene/Physics/HyPhysicsDraw.cpp
	src/Scene/Physics/Fixtures/HyChain2d.cpp
	src/Scene/Physics/Fixtures/HyShape2d.cpp
	src/Scene/Physics/Fixtures/IHyFixture2d.cpp
	src/Threading/IHyThreadClass.cpp
	src/Time/HyTime.cpp
	src/Time/Watches/HyStopwatch.cpp
	src/Time/Watches/HyTimer.cpp
	src/Time/Watches/IHyTimeInst.cpp
	src/UI/IHyEntityUi.cpp
	src/UI/HyUiContainer.cpp
	src/UI/Widgets/HyBarMeter.cpp
	src/UI/Widgets/HyButton.cpp
	src/UI/Widgets/HyCheckBox.cpp
	src/UI/Widgets/HyComboBox.cpp
	src/UI/Widgets/HyDividerLine.cpp
	src/UI/Widgets/HyLabel.cpp
	src/UI/Widgets/HyRackMeter.cpp
	src/UI/Widgets/HyRadioButton.cpp
	src/UI/Widgets/HyRichText.cpp
	src/UI/Widgets/HySlider.cpp
	src/UI/Widgets/HyTextField.cpp
	src/UI/Widgets/IHyWidget.cpp
	src/UI/Components/HyButtonGroup.cpp
	src/UI/Components/HyToolButton.cpp
	src/UI/Components/HyLayout.cpp
	src/UI/Components/HyPanel.cpp
	src/UI/Components/HyScrollBar.cpp
	src/UI/Components/HySpacer.cpp
	src/UI/Dialogs/HyRadioDialog.cpp
	src/UI/Dialogs/IHyDialog.cpp
	src/UI/Localization/HyLocale.cpp
	src/UI/Localization/HyNumberFormat.cpp
	src/UI/Localization/HyTranslateTable.cpp
	src/Utilities/Crc32.cpp
	src/Utilities/HyIO.cpp
	src/Utilities/HyJson.cpp
	src/Utilities/HyLocomotion.cpp
	src/Utilities/HyMain.cpp
	src/Utilities/HyMath.cpp
	src/Utilities/HyStorage.cpp
	src/Window/HyWindow.cpp
	src/Window/HyWindowManager.cpp)

set(HYENGINE_HEADER_FILES
	include/HyEngine.h
	include/Afx/HyFlags.h
	include/Afx/HyInteropAfx.h
	include/Afx/HyStdAfx.h
	include/Afx/Compilers/HyCompiler.h
	include/Afx/Compilers/HyCompiler_GNU.h
	include/Afx/Compilers/HyCompiler_MSVC.h
	include/Afx/Compilers/HyCompiler_MWerks.h
	include/Afx/Platforms/HyPlatAfx.h
	include/Afx/Platforms/HyPlatform.h
	include/Afx/Platforms/HyPlatform_Browser.h
	include/Afx/Platforms/HyPlatform_Gui.h
	include/Afx/Platforms/HyPlatform_OSX.h
	include/Afx/Platforms/HyPlatform_Unix.h
	include/Afx/Platforms/HyPlatform_Windows.h
	include/Assets/HyAssets.h
	include/Assets/Files/HyFileAtlas.h
	include/Assets/Files/HyFileAudio.h
	include/Assets/Files/HyFilesManifest.h
	include/Assets/Files/HyGLTF.h
	include/Assets/Files/IHyFile.h
	include/Assets/Nodes/HyNodePath.h
	include/Assets/Nodes/IHyNodeData.h
	include/Assets/Nodes/Objects/HyAudioData.h
	include/Assets/Nodes/Objects/HyPrefabData.h
	include/Assets/Nodes/Objects/HySpineData.h
	include/Assets/Nodes/Objects/HySpriteData.h
	include/Assets/Nodes/Objects/HyTextData.h
	include/Assets/Nodes/Objects/HyTexturedQuadData.h
	include/Audio/HyAudioCore.h
	include/Audio/HySoundAsset.h
	include/Diagnostics/HyDiagnostics.h
	include/Diagnostics/HyProfiler.h
	include/Diagnostics/Console/IHyConsole.h
	include/Diagnostics/Console/Interop/HyConsole_Std.h
	include/Diagnostics/Console/Interop/HyConsole_Win.h
	include/Diagnostics/GuiComms/HyGuiComms.h
	include/Diagnostics/GuiComms/HyGuiMessage.h
	include/Diagnostics/GuiComms/HyLiveVarManager.h
	include/Input/HyInput.h
	include/Input/HyInputKeys.h
	include/Input/HyInputMap.h
	include/Input/IHyController.h
	include/Networking/Sockets/HyNullSocket.h
	include/Networking/Sockets/HyPosix.h
	include/Networking/Sockets/HySdlNet.h
	include/Networking/Sockets/HyWinsock.h
	include/Networking/IHyNetworkClass.h
	include/Networking/IHyNetworking.h
	include/Renderer/IHyRenderer.h
	include/Renderer/Components/HyMeshBuffer.h
	include/Renderer/Components/HyRenderBuffer.h
	include/Renderer/Components/HyShaderUniforms.h
	include/Renderer/Components/HyVertexBuffer.h
	include/Renderer/Effects/HyShader.h
	include/Renderer/Effects/HyStencil.h
	include/Renderer/OpenGL/HyOpenGL.h
	include/Renderer/OpenGL/HyOpenGLESShaderSrc.h
	include/Renderer/OpenGL/HyOpenGLShaderSrc.h
	include/Scene/HyScene.h
	include/Scene/AnimFloats/HyAnimFloat.h
	include/Scene/AnimFloats/HyAnimVec2.h
	include/Scene/AnimFloats/HyAnimVec3.h
	include/Scene/AnimFloats/HyTweenFuncs.h
	include/Scene/Nodes/IHyNode.h
	include/Scene/Nodes/IHyNode2d.h
	include/Scene/Nodes/IHyNode3d.h
	include/Scene/Nodes/Loadables/IHyLoadable.h
	include/Scene/Nodes/Loadables/IHyLoadable2d.h
	include/Scene/Nodes/Loadables/IHyLoadable3d.h
	include/Scene/Nodes/Loadables/Bodies/IHyBody.h
	include/Scene/Nodes/Loadables/Bodies/IHyBody2d.h
	include/Scene/Nodes/Loadables/Bodies/IHyBody3d.h
	include/Scene/Nodes/Loadables/Bodies/Drawables/IHyDrawable.h
	include/Scene/Nodes/Loadables/Bodies/Drawables/IHyDrawable2d.h
	include/Scene/Nodes/Loadables/Bodies/Drawables/IHyDrawable3d.h
	include/Scene/Nodes/Loadables/Bodies/Drawables/Objects/HyPrefab3d.h
	include/Scene/Nodes/Loadables/Bodies/Drawables/Objects/HyPrimitive2d.h
	include/Scene/Nodes/Loadables/Bodies/Drawables/Objects/HySpine2d.h
	include/Scene/Nodes/Loadables/Bodies/Drawables/Objects/HySprite2d.h
	include/Scene/Nodes/Loadables/Bodies/Drawables/Objects/HyText2d.h
	include/Scene/Nodes/Loadables/Bodies/Drawables/Objects/HyTexturedQuad2d.h
	include/Scene/Nodes/Loadables/Bodies/Drawables/Objects/IHySprite.h
	include/Scene/Nodes/Loadables/Bodies/Drawables/Objects/IHySpine.h
	include/Scene/Nodes/Loadables/Bodies/Drawables/Objects/IHyText.h
	include/Scene/Nodes/Loadables/Bodies/Objects/HyActor2d.h
	include/Scene/Nodes/Loadables/Bodies/Objects/HyEntity2d.h
	include/Scene/Nodes/Loadables/Bodies/Objects/HyEntity3d.h
	include/Scene/Nodes/Loadables/Bodies/Objects/HyEntityWrapper2d.h
	include/Scene/Nodes/Loadables/Bodies/Objects/HyPhysicsGrid2d.h
	include/Scene/Nodes/Loadables/Objects/IHyAudio.h
	include/Scene/Nodes/Loadables/Objects/HyAudio2d.h
	include/Scene/Nodes/Loadables/Objects/HyAudio3d.h
	include/Scene/Nodes/Objects/HyCamera.h
	include/Scene/Nodes/Objects/IHyCamera.h
	include/Scene/Physics/HyPhysicsBody.h
	include/Scene/Physics/HyPhysicsDraw.h
	include/Scene/Physics/Fixtures/HyChain2d.h
	include/Scene/Physics/Fixtures/HyShape2d.h
	include/Scene/Physics/Fixtures/IHyFixture2d.h
	include/Threading/IHyThreadClass.h
	include/Time/HyTime.h
	include/Time/Watches/HyStopwatch.h
	include/Time/Watches/HyTimer.h
	include/Time/Watches/IHyTimeInst.h
	include/UI/IHyEntityUi.h
	include/UI/HyUiContainer.h
	include/UI/Widgets/HyBarMeter.h
	include/UI/Widgets/HyButton.h
	include/UI/Widgets/HyCheckBox.h
	include/UI/Widgets/HyComboBox.h
	include/UI/Widgets/HyDividerLine.h
	include/UI/Widgets/HyLabel.h
	include/UI/Widgets/HyRackMeter.h
	include/UI/Widgets/HyRadioButton.h
	include/UI/Widgets/HyRichText.h
	include/UI/Widgets/HySlider.h
	include/UI/Widgets/HyTextField.h
	include/UI/Widgets/IHyWidget.h
	include/UI/Components/HyButtonGroup.h
	include/UI/Components/HyToolButton.h
	include/UI/Components/HyLayout.h
	include/UI/Components/HyPanel.h
	include/UI/Components/HyScrollBar.h
	include/UI/Components/HySpacer.h
	include/UI/Dialogs/HyRadioDialog.h
	include/UI/Dialogs/IHyDialog.h
	include/UI/Localization/HyLocale.h
	include/UI/Localization/HyNumpunct.h
	include/UI/Localization/HyNumberFormat.h
	include/UI/Localization/HyTranslateTable.h
	include/Utilities/Crc32.h
	include/Utilities/HyIO.h
	include/Utilities/HyJson.h
	include/Utilities/HyLocomotion.h
	include/Utilities/HyMain.h
	include/Utilities/HyMath.h
	include/Utilities/HyRand.h
	include/Utilities/HyStorage.h
	include/Window/HyWindow.h
	include/Window/HyWindowManager.h)

add_library(HyEngine STATIC ${HYENGINE_SOURCE_FILES} ${HYENGINE_HEADER_FILES})
set_target_properties(HyEngine PROPERTIES
	CXX_STANDARD 17
	CXX_STANDARD_REQUIRED YES
	CXX_EXTENSIONS NO)

target_include_directories(HyEngine PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")	
target_include_directories(HyEngine PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")

target_link_libraries(HyEngine PUBLIC box2d soil2)

if(HYBUILD_SPINE)
	target_link_libraries(HyEngine PUBLIC spine-cpp)
endif()

if(HYBUILD_RAPIDJSON)
	
else() #nlohmann_json
	target_link_libraries(HyEngine PRIVATE nlohmann_json::nlohmann_json)
endif()

# Emscripten provides its own ports of SDL and GLEW
if(NOT "${CMAKE_SYSTEM_NAME}" STREQUAL "Emscripten")
	target_include_directories(HyEngine PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/vendor/glad/include")
	target_link_libraries(HyEngine PUBLIC glad)
	target_precompile_headers(HyEngine PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include/Afx/HyStdAfx.h")
	
	# Qt provides its own implementation of SDL facitlities
	if(NOT HYBUILD_Qt)
		# Below is typical desktop deployment
		if(HYBUILD_GLFW)
			target_include_directories(HyEngine PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/vendor/glfw")
			target_link_libraries(HyEngine PUBLIC glfw)
		endif()
		
		
		if(HYBUILD_SDL2)
			target_link_libraries(HyEngine PUBLIC SDL2 SDL2main)
			
			if(HYBUILD_SDL_NET)
				target_include_directories(HyEngine PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/vendor/SDL_net")
				target_link_libraries(HyEngine PUBLIC SDL2_net)
			endif()
		endif()
		
		
		if(HYBUILD_ICU)
			# ICU - International Components for Unicode (aka Insane Clown Union)
			# ICU Components Reference: https://cmake.org/cmake/help/latest/module/FindICU.html
			
			cmake_policy(SET CMP0074 NEW) # find_package() command now searches prefixes specified by <PackageName>_ROOT
			#set(ICU_DEBUG ON)
			set(ICU_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/extras/ICU")
			if(WIN32)
				find_package(ICU 69.0 COMPONENTS uc in REQUIRED)
				target_link_libraries(HyEngine PUBLIC ICU::uc ICU::in)
			else()
				find_package(ICU 69.0 COMPONENTS uc i18n REQUIRED)
				target_link_libraries(HyEngine PUBLIC ICU::uc ICU::i18n)
			endif()
		endif()
	endif()
endif()

if(HYBUILD_SDL_NET)
	target_compile_definitions(HyEngine PUBLIC HY_USE_SDL2_NET)
endif()

if (MSVC)
	target_compile_definitions(HyEngine PUBLIC _CRT_SECURE_NO_WARNINGS)
endif()

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/src" PREFIX "Source Files" FILES ${HYENGINE_SOURCE_FILES})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/include/" PREFIX "Source Files" FILES ${HYENGINE_HEADER_FILES})
