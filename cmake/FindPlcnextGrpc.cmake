#[=======================================================================[.rst:
FindPlcnextGrpc
---------------

Find the the PLCnext gRPC Services. Fetches the PLCnext gRPC git repository
and buils the Service stubs. Link against the object library 
``PlcnextGrpc::PlcnextGrpc`` to use the services.

OBJECT Targets
^^^^^^^^^^^^^^

This module defines `OBJECT` target ``PlcnextGrpc``
alias ``PlcnextGrpc::PlcnextGrpc``.

Result Variables
^^^^^^^^^^^^^^^^

The following cache variables are available to set or use:

``PlcnextGrpc_INCLUDE_DIR``
``PlcnextGrpc_INCLUDE_DIRS``
  The include directories of the PLCnext gRPC services.

``PlcnextGrpc_SOURCE_FILES``
  The source files of the PLCnext gRPC services.

``PlcnextGrpc_GRPC_CPP_PLUGIN_EXECUTABLE``
  The gRPC C++ Protobuf plugin
#]=======================================================================]

find_package(gRPC REQUIRED)
find_package(Protobuf REQUIRED)
include(FindPackageHandleStandardArgs)
include(FetchContent)

FetchContent_Declare(
    PlcnextGrpcRepo
    GIT_REPOSITORY https://github.com/PLCnext/gRPC.git
    GIT_SHALLOW ON
    GIT_TAG origin/master)

if(NOT plcnextgrpcrepo_POPULATED)
    FetchContent_Populate(PlcnextGrpcRepo)
endif()

find_program(PlcnextGrpc_GRPC_CPP_PLUGIN_EXECUTABLE
    NAMES grpc_cpp_plugin
    DOC "The gRPC C++ Protobuf plugin")

if(NOT PlcnextGrpc_GRPC_CPP_PLUGIN_EXECUTABLE OR NOT EXISTS ${PlcnextGrpc_GRPC_CPP_PLUGIN_EXECUTABLE})
    message(FATAL_ERROR "Could not find the gRPC C++ Protocol Buffers plugin 'grpc_cpp_plugin'.")
endif()

if(NOT Protobuf_PROTOC_EXECUTABLE OR NOT EXISTS ${Protobuf_PROTOC_EXECUTABLE})
    message(FATAL_ERROR "Could not find the Protocol Buffers Compiler 'protoc'.")
endif()

# Clean up old generated files    
execute_process(
    COMMAND "${CMAKE_COMMAND}" "-E" "rm" "-rf" "${plcnextgrpcrepo_BINARY_DIR}/out"
    WORKING_DIRECTORY ${plcnextgrpcrepo_SOURCE_DIR}
    RESULT_VARIABLE PlcnextGrpc_cmdresult
    OUTPUT_VARIABLE PlcnextGrpc_cmdoutput)
execute_process(
    COMMAND "${CMAKE_COMMAND}" "-E" "make_directory" "${plcnextgrpcrepo_BINARY_DIR}/out"
    WORKING_DIRECTORY ${plcnextgrpcrepo_SOURCE_DIR}
    RESULT_VARIABLE PlcnextGrpc_cmdresult
    OUTPUT_VARIABLE PlcnextGrpc_cmdoutput)

file(GLOB_RECURSE PlcnextGrpc_PROTO_FILES CONFIGURE_DEPENDS
    "${plcnextgrpcrepo_SOURCE_DIR}/protobuf/*.proto")

foreach(proto_file ${PlcnextGrpc_PROTO_FILES})
    execute_process(
        COMMAND "${Protobuf_PROTOC_EXECUTABLE}"
            "--proto_path=${plcnextgrpcrepo_SOURCE_DIR}/protobuf"
            "--proto_path=${Protobuf_INCLUDE_DIR}"
            "--grpc_out=${plcnextgrpcrepo_BINARY_DIR}/out"
            "--plugin=protoc-gen-grpc=${PlcnextGrpc_GRPC_CPP_PLUGIN_EXECUTABLE}"
            "${proto_file}"
        COMMAND "${Protobuf_PROTOC_EXECUTABLE}"
            "--proto_path=${plcnextgrpcrepo_SOURCE_DIR}/protobuf"
            "--proto_path=${Protobuf_INCLUDE_DIR}"
            "--cpp_out=${plcnextgrpcrepo_BINARY_DIR}/out"
            "${proto_file}"
        WORKING_DIRECTORY ${plcnextgrpcrepo_SOURCE_DIR}
        RESULT_VARIABLE PlcnextGrpc_cmdresult
        OUTPUT_VARIABLE PlcnextGrpc_cmdoutput)
    if(${PlcnextGrpc_cmdresult})
        message("Generation of data model returned ${PlcnextGrpc_cmdresult} for proto ${proto_file}")
    endif()
endforeach()

set(PlcnextGrpc_INCLUDE_DIR ${plcnextgrpcrepo_BINARY_DIR}/out)
set(PlcnextGrpc_INCLUDE_DIRS ${PlcnextGrpc_INCLUDE_DIR})

file(GLOB_RECURSE PlcnextGrpc_SOURCE_FILES CONFIGURE_DEPENDS
    "${plcnextgrpcrepo_BINARY_DIR}/out/*.h" "${plcnextgrpcrepo_BINARY_DIR}/out/*.cc")

if(NOT TARGET PlcnextGrpc)
    add_library(PlcnextGrpc OBJECT ${PlcnextGrpc_SOURCE_FILES})
    target_link_libraries(PlcnextGrpc PUBLIC gRPC::grpc++ protobuf::libprotobuf)
    target_include_directories(PlcnextGrpc PUBLIC $<BUILD_INTERFACE:${PlcnextGrpc_INCLUDE_DIRS}>)
    add_library(PlcnextGrpc::PlcnextGrpc ALIAS PlcnextGrpc)
endif()

find_package_handle_standard_args(PlcnextGrpc
    REQUIRED_VARS PlcnextGrpc_INCLUDE_DIR PlcnextGrpc_GRPC_CPP_PLUGIN_EXECUTABLE PlcnextGrpc_SOURCE_FILES)
