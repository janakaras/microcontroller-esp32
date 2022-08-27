idf_component_register(
        SRCS "jana_spi_master_send.c"
        REQUIRES nvs_flash
        INCLUDE_DIRS ${CMAKE_CURRENT_LIST_DIR}
)
