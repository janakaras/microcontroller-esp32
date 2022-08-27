idf_component_register(
        SRCS "jana_spi_slave_receive.c"
        REQUIRES nvs_flash
        INCLUDE_DIRS ${CMAKE_CURRENT_LIST_DIR}
    )