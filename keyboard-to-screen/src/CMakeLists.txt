cmake_minimum_required(VERSION 3.13)

# Définir PICO_SDK_PATH si non défini
if(NOT DEFINED ENV{PICO_SDK_PATH})
    set(ENV{PICO_SDK_PATH} "${CMAKE_CURRENT_LIST_DIR}/../pico-sdk")
endif()

include(pico_sdk_import.cmake)

project(pico_usb_host C CXX ASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()

include_directories(${CMAKE_CURRENT_LIST_DIR}/src)

# Télécharger PIO-USB si pas présent
if(NOT EXISTS ${CMAKE_CURRENT_LIST_DIR}/Pico-PIO-USB)
    message(STATUS "Téléchargement de Pico-PIO-USB...")
    execute_process(
        COMMAND git clone https://github.com/sekigon-gonnoc/Pico-PIO-USB.git
        WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
    )
endif()

# Ajouter PIO-USB
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/Pico-PIO-USB ${CMAKE_BINARY_DIR}/pio_usb)

# Exécutable
add_executable(usb_host_keyboard
    KeyAnalyser.c
    tusb_hcd_pio_usb.c
)

# Options de compilation pour PIO-USB
target_compile_definitions(usb_host_keyboard PRIVATE
    # TinyUSB Host activé
    CFG_TUH_ENABLED=1
    
    # Utiliser PIO pour USB
    CFG_TUH_RPI_PIO_USB=1
    
    # Nombre de devices supportés
    CFG_TUH_DEVICE_MAX=1
    CFG_TUH_HUB=0
    CFG_TUH_HID=1
    
    # Configuration endpoints
    CFG_TUH_ENDPOINT_MAX=8
    
    # Board
    BOARD_TUH_RHPORT=1
    
    # PIO USB
    PIO_USB_USE_TINYUSB=1
)

# Bibliothèques
target_link_libraries(usb_host_keyboard
    pico_stdlib
    tinyusb_host
    tinyusb_board
    pico_pio_usb
)
target_link_libraries(pico_pio_usb INTERFACE
    pico_stdlib
    hardware_pio
    hardware_dma
    hardware_irq
    hardware_clocks
)

target_include_directories(usb_host_keyboard PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
    ${CMAKE_CURRENT_LIST_DIR}/Pico-PIO-USB/src
)

# Overclock à 120MHz (recommandé pour PIO-USB)
target_compile_definitions(usb_host_keyboard PRIVATE
    PICO_DEFAULT_SYS_CLK_KHZ=120000
)

# Créer les fichiers de sortie
pico_add_extra_outputs(usb_host_keyboard)

# IMPORTANT: Utiliser UART pour printf car l'USB est pour le Host
pico_enable_stdio_usb(usb_host_keyboard 0)
pico_enable_stdio_uart(usb_host_keyboard 1)

# Infos de compilation
pico_set_program_name(usb_host_keyboard "Pico USB Host Keyboard")
pico_set_program_version(usb_host_keyboard "1.0")
pico_set_program_description(usb_host_keyboard "USB Host pour clavier via GPIO avec PIO-USB")