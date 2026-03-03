project(SBE-external-iniparser)

set(SBE_INIPARSER_PATH ${CMAKE_SOURCE_DIR}/../iniparser
	CACHE PATH
	"Path to the iniparser library sources"
)

set(_sbe_iniparser_src_dir "${SBE_INIPARSER_PATH}/src")
aux_source_directory(${_sbe_iniparser_src_dir} PROJECT_SOURCES)

if(PROJECT_SOURCES)
	add_library(${PROJECT_NAME} STATIC)
	target_sources(${PROJECT_NAME}
		PRIVATE ${PROJECT_SOURCES}
	)
	target_include_directories(${PROJECT_NAME}
		PUBLIC ${_sbe_iniparser_src_dir}
	)
else()
	message(WARNING "iniparser sources were not found in '${_sbe_iniparser_src_dir}'. Building '${PROJECT_NAME}' as an INTERFACE stub target.")
	add_library(${PROJECT_NAME} INTERFACE)
	if(EXISTS "${_sbe_iniparser_src_dir}")
		target_include_directories(${PROJECT_NAME}
			INTERFACE ${_sbe_iniparser_src_dir}
		)
	endif()
endif()
