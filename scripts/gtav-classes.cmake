include(FetchContent)

FetchContent_Declare(
    gtav_classes
    GIT_REPOSITORY https://github.com/Yimura/GTAV-Classes.git
    GIT_TAG        0a23b823e2dd2e52004b206b7cce735d326c209f
    GIT_PROGRESS TRUE
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
)
message("GTAV-Classes")
if(NOT gtav_classes_POPULATED)
    FetchContent_Populate(gtav_classes)

    file(GLOB_RECURSE SRC_GTAV_CLASSES "${gtav_classes_SOURCE_DIR}/*.hpp")

    # Show GTAV-Classes project
    add_library(gtav_classes "${SRC_GTAV_CLASSES}")

    source_group(TREE "${gtav_classes_SOURCE_DIR}" PREFIX "GTAV-Classes" FILES "${SRC_GTAV_CLASSES}")
endif()
set_property(TARGET gtav_classes PROPERTY CXX_STANDARD 23)
set_target_properties(gtav_classes PROPERTIES LINKER_LANGUAGE CXX)
