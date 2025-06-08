set(APP_NAME example)

find_package(fmt REQUIRED)
find_package(dpdk_informer REQUIRED)

add_executable(${APP_NAME}
        main.cpp
)

target_link_libraries(${APP_NAME} PRIVATE
        dpdk_informer::dpdk_informer
        fmt::fmt
)