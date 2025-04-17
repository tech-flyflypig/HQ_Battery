cmake_minimum_required(VERSION 3.16)

project(HQ_Battery LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 设置UTF-8字符集支持，避免中文乱码
if(MSVC)
    add_compile_options(/utf-8)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /source-charset:utf-8")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /source-charset:utf-8")
endif()

# 设置自动生成MOC文件
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_PREFIX_PATH "D:/Qt/5.15.2/msvc2019_64")

# Windows平台特定配置
if(WIN32)
    # 启用Unicode字符集
    add_definitions(-DUNICODE -D_UNICODE)
    # 禁用控制台窗口（如果是GUI应用）
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS")
endif()

# 查找Qt包
find_package(Qt5 COMPONENTS Core Gui Widgets SerialPort REQUIRED)

# 设置包含目录
include_directories(
    component
    utils
    protocols
)

# 使用aux_source_directory直接收集所有源文件到SOURCES变量
file(GLOB_RECURSE SOURCES
    "*.cpp"
)

# UI文件列表
set(UI_FILES
    mainwindow.ui
)

# 资源文件
set(RESOURCE_FILES
    res.qrc
)

# 添加可执行文件
add_executable(${PROJECT_NAME}
    ${SOURCES}
    ${UI_FILES}
    ${RESOURCE_FILES}
)

# 链接Qt库
target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt5::Core
    Qt5::Gui
    Qt5::Widgets
    Qt5::SerialPort
)

# 设置安装路径（根据平台）
if(WIN32)
    # Windows安装配置
    install(TARGETS ${PROJECT_NAME} DESTINATION .)
    
    # 安装Qt DLL文件（可选，取决于您是否需要自动部署Qt库）
    include(InstallRequiredSystemLibraries)
    set(CMAKE_INSTALL_SYSTEM_RUNTIME_DESTINATION ".")
    
    # 添加安装后运行脚本，用于复制Qt DLL（可选）
    # install(CODE "execute_process(COMMAND \"${CMAKE_PREFIX_PATH}/bin/windeployqt.exe\" \"$<TARGET_FILE:${PROJECT_NAME}>\")")
elseif(UNIX AND NOT APPLE)
    # Linux安装配置
    install(TARGETS ${PROJECT_NAME} DESTINATION /opt/${PROJECT_NAME}/bin)
endif() 