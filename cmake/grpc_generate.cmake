
set(protobuf_MODULE_COMPATIBLE TRUE)
find_package(Protobuf REQUIRED)
find_package(gRPC REQUIRED)

function(grpc_generate PROTO_FILE OUTPUT_DIR)
    get_filename_component(PROTO_DIR ${PROTO_FILE} DIRECTORY)
    get_filename_component(PROTO_BASENAME ${PROTO_FILE} NAME_WE)

    SET(GRPC_HEADERS 
        "${GRPC_PROTO_GENS_DIR}/${PROTO_DIR}/${PROTO_BASENAME}.grpc.pb.h" 
        "${GRPC_PROTO_GENS_DIR}/${PROTO_DIR}/${PROTO_BASENAME}.pb.h")
    SET(GRPC_SRCS
        "${GRPC_PROTO_GENS_DIR}/${PROTO_DIR}/${PROTO_BASENAME}.grpc.pb.cc"
        "${GRPC_PROTO_GENS_DIR}/${PROTO_DIR}/${PROTO_BASENAME}.pb.cc")

    add_custom_command(
        OUTPUT ${GRPC_SRCS} ${GRPC_HEADERS} 
        COMMAND ${Protobuf_PROTOC_EXECUTABLE}
        ARGS --grpc_out=${GRPC_PROTO_GENS_DIR}
            --cpp_out=${GRPC_PROTO_GENS_DIR}
            --plugin=protoc-gen-grpc=${GRPC_CPP_PLUGIN_PROGRAM}
            -I . -I "${protobuf_INCLUDE_DIR}"
            ${PROTO_FILE}
        DEPENDS ${PROTO_FILE} ${Protobuf_PROTOC_EXECUTABLE} ${GRPC_CPP_PLUGIN_PROGRAM}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Running gRPC C++ protobuf compiler for ${PROTO_FILE}"
        VERBATIM)

    set(GRPC_GENERATED_SOURCES ${GRPC_HEADERS} ${GRPC_SRCS} PARENT_SCOPE)

endfunction()