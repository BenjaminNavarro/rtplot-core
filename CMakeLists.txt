CMAKE_MINIMUM_REQUIRED(VERSION 3.0.2)
set(WORKSPACE_DIR ${CMAKE_SOURCE_DIR}/../.. CACHE PATH "root of the packages workspace directory")
list(APPEND CMAKE_MODULE_PATH ${WORKSPACE_DIR}/share/cmake/system) # using generic scripts/modules of the workspace
include(Package_Definition NO_POLICY_SCOPE)

PROJECT(rtplot-core)

PID_Package(
	AUTHOR 			Benjamin Navarro
	INSTITUTION		LIRMM
	MAIL			navarro@lirmm.fr
	ADDRESS 		git@github.com:BenjaminNavarro/rtplot-core.git
	PUBLIC_ADDRESS 	https://github.com/BenjaminNavarro/rtplot-core.git
	YEAR 			2018
	LICENSE 		CeCILL
	CODE_STYLE		pid11
	DESCRIPTION 	Core functionalities to be used by GUI libraries for real time plotting
	VERSION			0.3.1
)

build_PID_Package()
