add_library(libbtree
            BPlusStore.hpp
            DataNode.hpp
            ErrorCodes.h
            IndexNode.hpp
            TypeMarshaller.hpp
            TypeUID.h
)


target_include_directories(libbtree PUBLIC
            "${PROJECT_SOURCE_DIR}/../libcache"
)

set_target_properties(libbtree PROPERTIES LINKER_LANGUAGE CXX)

target_link_libraries(libbtree PUBLIC haldendb_compiler_flags)