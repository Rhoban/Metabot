cmake_minimum_required(VERSION 2.8)
project(metabot)

# Directories
set (BASE "${CMAKE_SOURCE_DIR}/..")
set (RC_BASE "${BASE}/RobotsWar/LibRobotCampus/")
set (RHOCK_BASE "${BASE}/Rhock/src")

# Compilation options
set(CMAKE_C_COMPILER emcc)
set(CMAKE_CXX_COMPILER em++)
set(CMAKE_AR emar)
set(CMAKE_RANLIB emranlib)
set(CMAKE_C_FLAGS -g)
set(CMAKE_CXX_FLAGS -g)
set (CMAKE_C_FLAGS " -std=c99 -Wall -U__STRICT_ANSI__ -Wno-warn-absolute-paths")
set (CMAKE_CXX_FLAGS "--bind -std=c++11 -Wall -Wno-reorder -pedantic -U__STRICT_ANSI__ -Wno-warn-absolute-paths")

# Includes
include_directories (${RHOCK_BASE} ${RC_BASE})

# Sources
set (SRC_METABOT
    ${BASE}/kinematic.cpp
    ${BASE}/locomotion.cpp
    )

set (SRC_RC
    ${RC_BASE}/function.cpp
    )

set (SRC_RHOCK
	${RHOCK_BASE}/rhock/chain.c		
	${RHOCK_BASE}/rhock/context.c	
	${RHOCK_BASE}/rhock/crc16.c		
	${RHOCK_BASE}/rhock/memory.c	
	${RHOCK_BASE}/rhock/native.c	
	${RHOCK_BASE}/rhock/natives.c	
	${RHOCK_BASE}/rhock/obj.c		
	${RHOCK_BASE}/rhock/obj_write.c 
	${RHOCK_BASE}/rhock/opcode.c	
	${RHOCK_BASE}/rhock/program.c	
	${RHOCK_BASE}/rhock/vm.c
    )

# Produce the module
add_executable (module.js ${SRC_METABOT} ${SRC_RC} ${SRC_RHOCK})

add_custom_target (module.min.js 
    COMMAND uglifyjs module.js > module.min.js
    DEPENDS module.js
    )

add_custom_target (module.tgz
    COMMAND tar zcvf module.tgz module.min.js
    DEPENDS module.min.js
    )
