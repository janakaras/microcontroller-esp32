idf_component_register(
	SRCS "foo.c"
	SRCS "jana_read_adc_new.c"
	REQUIRES esp_adc_cal
	INCLUDE_DIRS ${CMAKE_CURRENT_LIST_DIR}
	)
