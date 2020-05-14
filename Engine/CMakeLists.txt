add_subdirectory(external/box2d)
add_subdirectory(external/glad)
add_subdirectory(external/soil2)

if(NOT HYBUILD_Emscripten)
	add_subdirectory(external/glfw)
endif()

set(HYENGINE_SOURCE_FILES
	src/HyEngine.cpp
	src/Afx/HyStdAfx.cpp
	src/Assets/HyAssets.cpp
	src/Assets/Files/HyAtlas.cpp
	src/Assets/Files/HyAudioBank.cpp
	src/Assets/Files/HyFileIO.cpp
	src/Assets/Files/HyGLTF.cpp
	src/Assets/Nodes/HyAudioData.cpp
	src/Assets/Nodes/HyEntityData.cpp
	src/Assets/Nodes/HyPrefabData.cpp
	src/Assets/Nodes/HySpine2dData.cpp
	src/Assets/Nodes/HySprite2dData.cpp
	src/Assets/Nodes/HyText2dData.cpp
	src/Assets/Nodes/HyTexturedQuad2dData.cpp
	src/Assets/Nodes/IHyNodeData.cpp
	src/Audio/HyAudio.cpp
	src/Diagnostics/HyDiagnostics.cpp
	src/Diagnostics/Console/HyConsole.cpp
	src/Diagnostics/Console/Interop/HyConsole_Std.cpp
	src/Diagnostics/Console/Interop/HyConsole_Win.cpp
	src/Diagnostics/GuiComms/HyGuiComms.cpp
	src/Diagnostics/GuiComms/HyGuiMessage.cpp
	src/Diagnostics/GuiComms/HyLiveVarManager.cpp
	src/Diagnostics/Output/HyDiagOutput.cpp
	src/Diagnostics/Output/HyProfiler.cpp
	src/Input/HyInput.cpp
	src/Input/HyInputMap.cpp
	src/Renderer/IHyRenderer.cpp
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
	src/Scene/Nodes/Loadables/Drawables/IHyDrawable.cpp
	src/Scene/Nodes/Loadables/Drawables/IHyDrawable2d.cpp
	src/Scene/Nodes/Loadables/Drawables/IHyDrawable3d.cpp
	src/Scene/Nodes/Loadables/Drawables/Instances/IHyInstance.cpp
	src/Scene/Nodes/Loadables/Drawables/Instances/IHyInstance2d.cpp
	src/Scene/Nodes/Loadables/Drawables/Instances/IHyInstance3d.cpp
	src/Scene/Nodes/Loadables/Drawables/Instances/Objects/HyPrefab3d.cpp
	src/Scene/Nodes/Loadables/Drawables/Instances/Objects/HyPrimitive2d.cpp
	src/Scene/Nodes/Loadables/Drawables/Instances/Objects/HySpine2d.cpp
	src/Scene/Nodes/Loadables/Drawables/Instances/Objects/HySprite2d.cpp
	src/Scene/Nodes/Loadables/Drawables/Instances/Objects/HyText2d.cpp
	src/Scene/Nodes/Loadables/Drawables/Instances/Objects/HyTexturedQuad2d.cpp
	src/Scene/Nodes/Loadables/Drawables/Objects/HyEntity2d.cpp
	src/Scene/Nodes/Loadables/Drawables/Objects/HyEntity3d.cpp
	src/Scene/Nodes/Loadables/Objects/HyAudio2d.cpp
	src/Scene/Nodes/Objects/HyCamera.cpp
	src/Scene/Physics/HyBox2dRuntime.cpp
	src/Scene/Physics/HyPhysEntity2d.cpp
	src/Scene/Physics/HyPhysicsCollider.cpp
	src/Scene/Physics/HyPhysicsDebug2d.cpp
	src/Scene/Physics/HyPhysicsGrid2d.cpp
	src/Scene/Physics/HyShape2d.cpp
	src/Threading/IHyThreadClass.cpp
	src/Time/HyTime.cpp
	src/Time/Watches/HyStopwatch.cpp
	src/Time/Watches/HyTimer.cpp
	src/Time/Watches/IHyTimeInst.cpp
	src/UI/HyButton.cpp
	src/UI/HyInfoPanel.cpp
	src/UI/HyMeter.cpp
	src/UI/IHy9Slice.cpp
	src/Utilities/jasonxx.cpp
	src/Utilities/Crc32.cpp
	src/Utilities/HyBitManip.cpp
	src/Utilities/HyImage.cpp
	src/Utilities/HyMath.cpp
	src/Utilities/HyStrManip.cpp
	src/Utilities/HyVariant.cpp
	src/Window/HyWindow.cpp
	src/Window/HyWindowManager.cpp)

set(HYENGINE_HEADER_FILES
	include/Harmony/HyEngine.h
	include/Harmony/Afx/HyInteropAfx.h
	include/Harmony/Afx/HyStdAfx.h
	include/Harmony/Afx/Compilers/HyCompiler.h
	include/Harmony/Afx/Compilers/HyCompiler_GNU.h
	include/Harmony/Afx/Compilers/HyCompiler_MSVC.h
	include/Harmony/Afx/Compilers/HyCompiler_MWerks.h
	include/Harmony/Afx/Platforms/HyPlatAfx.h
	include/Harmony/Afx/Platforms/HyPlatform.h
	include/Harmony/Afx/Platforms/HyPlatform_Browser.h
	include/Harmony/Afx/Platforms/HyPlatform_Gui.h
	include/Harmony/Afx/Platforms/HyPlatform_OSX.h
	include/Harmony/Afx/Platforms/HyPlatform_Unix.h
	include/Harmony/Afx/Platforms/HyPlatform_Windows.h
	include/Harmony/Assets/HyAssets.h
	include/Harmony/Assets/Files/HyAtlas.h
	include/Harmony/Assets/Files/HyAudioBank.h
	include/Harmony/Assets/Files/HyFileIO.h
	include/Harmony/Assets/Files/HyGLTF.h
	include/Harmony/Assets/Files/IHyFileData.h
	include/Harmony/Assets/Nodes/HyAudioData.h
	include/Harmony/Assets/Nodes/HyEntityData.h
	include/Harmony/Assets/Nodes/HyPrefabData.h
	include/Harmony/Assets/Nodes/HySpine2dData.h
	include/Harmony/Assets/Nodes/HySprite2dData.h
	include/Harmony/Assets/Nodes/HyText2dData.h
	include/Harmony/Assets/Nodes/HyTexturedQuad2dData.h
	include/Harmony/Assets/Nodes/IHyNodeData.h
	include/Harmony/Audio/HyAudio.h
	include/Harmony/Audio/Harness/IHyAudio.h
	include/Harmony/Audio/Harness/IHyAudioBank.h
	include/Harmony/Audio/Harness/IHyAudioInst.h
	include/Harmony/Diagnostics/HyDiagnostics.h
	include/Harmony/Diagnostics/Console/HyConsole.h
	include/Harmony/Diagnostics/Console/Interop/HyConsole_Std.h
	include/Harmony/Diagnostics/Console/Interop/HyConsole_Win.h
	include/Harmony/Diagnostics/GuiComms/HyGuiComms.h
	include/Harmony/Diagnostics/GuiComms/HyGuiMessage.h
	include/Harmony/Diagnostics/GuiComms/HyLiveVarManager.h
	include/Harmony/Diagnostics/Output/HyDiagOutput.h
	include/Harmony/Diagnostics/Output/HyProfiler.h
	include/Harmony/Input/HyInput.h
	include/Harmony/Input/HyInputKeys.h
	include/Harmony/Input/HyInputMap.h
	include/Harmony/Renderer/IHyRenderer.h
	include/Harmony/Renderer/Components/HyRenderBuffer.h
	include/Harmony/Renderer/Components/HyShaderUniforms.h
	include/Harmony/Renderer/Components/HyVertexBuffer.h
	include/Harmony/Renderer/Effects/HyShader.h
	include/Harmony/Renderer/Effects/HyStencil.h
	include/Harmony/Renderer/OpenGL/HyOpenGL.h
	include/Harmony/Renderer/OpenGL/HyOpenGLShaderSrc.h
	include/Harmony/Scene/HyScene.h
	include/Harmony/Scene/AnimFloats/HyAnimFloat.h
	include/Harmony/Scene/AnimFloats/HyAnimVec2.h
	include/Harmony/Scene/AnimFloats/HyAnimVec3.h
	include/Harmony/Scene/AnimFloats/HyTweenFuncs.h
	include/Harmony/Scene/Nodes/IHyNode.h
	include/Harmony/Scene/Nodes/IHyNode2d.h
	include/Harmony/Scene/Nodes/IHyNode3d.h
	include/Harmony/Scene/Nodes/Loadables/IHyLoadable.h
	include/Harmony/Scene/Nodes/Loadables/IHyLoadable2d.h
	include/Harmony/Scene/Nodes/Loadables/IHyLoadable3d.h
	include/Harmony/Scene/Nodes/Loadables/Drawables/IHyDrawable.h
	include/Harmony/Scene/Nodes/Loadables/Drawables/IHyDrawable2d.h
	include/Harmony/Scene/Nodes/Loadables/Drawables/IHyDrawable3d.h
	include/Harmony/Scene/Nodes/Loadables/Drawables/Instances/IHyInstance.h
	include/Harmony/Scene/Nodes/Loadables/Drawables/Instances/IHyInstance2d.h
	include/Harmony/Scene/Nodes/Loadables/Drawables/Instances/IHyInstance3d.h
	include/Harmony/Scene/Nodes/Loadables/Drawables/Instances/Objects/HyPrefab3d.h
	include/Harmony/Scene/Nodes/Loadables/Drawables/Instances/Objects/HyPrimitive2d.h
	include/Harmony/Scene/Nodes/Loadables/Drawables/Instances/Objects/HySpine2d.h
	include/Harmony/Scene/Nodes/Loadables/Drawables/Instances/Objects/HySprite2d.h
	include/Harmony/Scene/Nodes/Loadables/Drawables/Instances/Objects/HyText2d.h
	include/Harmony/Scene/Nodes/Loadables/Drawables/Instances/Objects/HyTexturedQuad2d.h
	include/Harmony/Scene/Nodes/Loadables/Drawables/Objects/HyEntity2d.h
	include/Harmony/Scene/Nodes/Loadables/Drawables/Objects/HyEntity3d.h
	include/Harmony/Scene/Nodes/Loadables/Drawables/Objects/HyEntityLeaf2d.h
	include/Harmony/Scene/Nodes/Loadables/Objects/HyAudio2d.h
	include/Harmony/Scene/Nodes/Objects/HyCamera.h
	include/Harmony/Scene/Nodes/Objects/IHyCamera.h
	include/Harmony/Scene/Physics/HyBox2dRuntime.h
	include/Harmony/Scene/Physics/HyPhysEntity2d.h
	include/Harmony/Scene/Physics/HyPhysicsCollider.h
	include/Harmony/Scene/Physics/HyPhysicsDebug2d.h
	include/Harmony/Scene/Physics/HyPhysicsGrid2d.h
	include/Harmony/Scene/Physics/HyShape2d.h
	include/Harmony/Threading/IHyThreadClass.h
	include/Harmony/Time/HyTime.h
	include/Harmony/Time/Watches/HyStopwatch.h
	include/Harmony/Time/Watches/HyTimer.h
	include/Harmony/Time/Watches/IHyTimeInst.h
	include/Harmony/UI/HyButton.h
	include/Harmony/UI/HyInfoPanel.h
	include/Harmony/UI/HyMeter.h
	include/Harmony/UI/IHy9Slice.h
	include/Harmony/Utilities/jsonxx.h
	include/Harmony/Utilities/Crc32.h
	include/Harmony/Utilities/HyBitManip.h
	include/Harmony/Utilities/HyImage.h
	include/Harmony/Utilities/HyMath.h
	include/Harmony/Utilities/HyRand.h
	include/Harmony/Utilities/HyStrManip.h
	include/Harmony/Utilities/HyVariant.h
	include/Harmony/Window/HyWindow.h
	include/Harmony/Window/HyWindowManager.h)

add_library(HyEngine STATIC ${HYENGINE_SOURCE_FILES} ${HYENGINE_HEADER_FILES} include/tinygltf/tiny_gltf.h)
target_include_directories(HyEngine PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_include_directories(HyEngine PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include/Harmony")
target_include_directories(HyEngine PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/external/box2d/include")
target_include_directories(HyEngine PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/external/glad/include")
target_include_directories(HyEngine PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/external/soil2/include")
set_target_properties(HyEngine PROPERTIES
	CXX_STANDARD 14
	CXX_STANDARD_REQUIRED YES
	CXX_EXTENSIONS NO)

target_link_libraries(HyEngine PUBLIC glad box2d soil2)

if(NOT HYBUILD_Emscripten)
	target_include_directories(HyEngine PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/external/glfw/include")
	target_link_libraries(HyEngine PUBLIC glfw)
	target_precompile_headers(HyEngine PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include/Harmony/Afx/HyStdAfx.h")
endif()

if (MSVC)
	target_compile_definitions(HyEngine PRIVATE _CRT_SECURE_NO_WARNINGS)
endif()


# Sets up filters for VS
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/src" FILES ${HYENGINE_SOURCE_FILES})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/include/Harmony/" FILES ${HYENGINE_HEADER_FILES})