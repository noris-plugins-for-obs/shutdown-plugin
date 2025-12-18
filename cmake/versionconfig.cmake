if(DEFINED VERSION_DESCRIPTION_OVERRIDE)
	set(VERSION_DESCRIPTION ${VERSION_DESCRIPTION_OVERRIDE})
elseif(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/.git")
	execute_process(
		COMMAND git describe --always --tags --dirty=-modified
		OUTPUT_VARIABLE _git_version
		ERROR_VARIABLE _git_describe_err
		WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
		RESULT_VARIABLE _git_describe_result
		OUTPUT_STRIP_TRAILING_WHITESPACE
	)
	if(_git_describe_err)
		message(FATAL_ERROR "Failed to run git-describe.\n" ${_git_describe_err})
	endif()
	if (_git_describe_result EQUAL 0)
		set(VERSION_DESCRIPTION ${_git_version})
	endif()
endif()
