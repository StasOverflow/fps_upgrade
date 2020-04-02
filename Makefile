CPU     = cortex-m3
PART    = STM32F2XX
F_XTAL  = 25000000UL

# Device supported:
# 714200
# 714153
BOARD   = 714200
DEVICE  = Special
PROJECT = MbTestTool
TARGETS = $(BINDIR)/$(PROJECT).elf \
          $(BINDIR)/$(PROJECT).bin \
          size

#
# Path 
#
RTOS                   = $(EMLIB)/rtos/FreeRTOSv10.2.1
GRLIB_PATH             = $(EMLIB)/graphics/16.03.13
LIBGR                  = $(GRLIB_PATH)/grlib/build/lib/libgr-impuls.a
STM32_SPL_PATH         = $(EMLIB)/stm32/Libraries
CMSIS_PATH             = $(STM32_SPL_PATH)/CMSIS/CM3
STM32_SPL_DRIVERS_PATH = $(STM32_SPL_PATH)/STM32F2xx_StdPeriph_Driver

LWIP_PATH              = lwip-2.0.2

IMG_OUT_DIR            = ./interface/images
IMG_SRC_DIR            = ./interface/images/img_data


# Include the common make definitions.1
#
include $(EMLIB)/stm32/makedefs

#
# Common definitions
#
COMMON += -D${PART}
COMMON += -DBOARD=${BOARD}
COMMON += -DUSE_STDPERIPH_DRIVER
ifdef F_XTAL
COMMON += -DHSE_VALUE=${F_XTAL}
else
COMMON += -DINTERNAL_CLOCK_SOURCE
endif

#
# Include include 
# 
IPATH   = .                                                 \
          $(EMLIB)                                          \
          $(GRLIB_PATH)                                     \
          $(CMSIS_PATH)/CoreSupport                         \
          $(CMSIS_PATH)/DeviceSupport/ST/STM32F2xx          \
          $(STM32_SPL_DRIVERS_PATH)/inc                     \
          $(RTOS)/include                                   \
          $(RTOS)/portable/GCC/ARM_CM3                      \
          lib                                               \
          comm                                              \
          drivers                                           \
          interface                                         \
          interface/images                                  \
          property                                          \
          network                                           \
          network/httpd                                     \
          network/httpd/utils                               \
          web_server                                        \
          web_server/utils                                  \
          system                                            \
          $(GRLIB_PATH)/grlib                               \
          $(GRLIB_PATH)/grlib/widgets                       \
          $(GRLIB_PATH)/grlib/fonts                         \
          $(LWIP_PATH)                                      \
          $(LWIP_PATH)/ports/                               \
          $(LWIP_PATH)/src/include

#
# Compile path
#
VPATH   = .                                                 \
          $(CMSIS_PATH)/CoreSupport                         \
          $(CMSIS_PATH)/DeviceSupport/ST/STM32F2xx          \
          $(STM32_SPL_DRIVERS_PATH)/src                     \
          $(RTOS)                                           \
          $(RTOS)/portable/GCC/ARM_CM3                      \
          $(RTOS)/portable/MemMang                          \
          lib                                               \
          comm                                              \
          property                                          \
          drivers                                           \
          interface                                         \
          interface/windows/debug                           \
          system                                            \
          interface/debug_test                              \
          interface/windows                                 \
          interface/tables                                  \
          interface/dialogs                                 \
          interface/controls                                \
          interface/images                                  \
          network                                           \
          network/httpd                                     \
          network/httpd/utils                               \
          web_server                                        \
          web_server/utils                                  \
          $(LWIP_PATH)


make_images: ${IMG_OUT_DIR}/img_main.c
make_images: ${IMG_OUT_DIR}/img_main_subwindow.c
make_images: ${IMG_OUT_DIR}/img_main_with_shapka.c
make_images: ${IMG_OUT_DIR}/img_debug.c
make_images: ${IMG_OUT_DIR}/img_info.c

make_images: ${IMG_OUT_DIR}/img_bad_crosshair.c

make_images: ${IMG_OUT_DIR}/img_led_yellow_small.c
make_images: ${IMG_OUT_DIR}/img_led_green_small.c
make_images: ${IMG_OUT_DIR}/img_led_green_upgraded_small.c
make_images: ${IMG_OUT_DIR}/img_led_red_small.c
make_images: ${IMG_OUT_DIR}/img_led_disabled_small.c
make_images: ${IMG_OUT_DIR}/img_led_disabled_dark_small.c
make_images: ${IMG_OUT_DIR}/img_led_empty_small.c
make_images: ${IMG_OUT_DIR}/img_led_cyan_small.c

make_images: ${IMG_OUT_DIR}/img_led_green_medium.c
make_images: ${IMG_OUT_DIR}/img_led_red_medium.c
make_images: ${IMG_OUT_DIR}/img_led_disabled_medium.c

make_images: ${IMG_OUT_DIR}/img_pulse_logo.c

make_images: ${IMG_OUT_DIR}/img_btn_ohm.c
make_images: ${IMG_OUT_DIR}/img_btn_battery.c
make_images: ${IMG_OUT_DIR}/img_btn_settings.c

make_images: ${IMG_OUT_DIR}/img_mode_btn_selected.c

make_images: ${IMG_OUT_DIR}/img_icon_battery.c
make_images: ${IMG_OUT_DIR}/img_icon_air.c

make_images: ${IMG_OUT_DIR}/img_icon_conn_disconnected_alarm.c
make_images: ${IMG_OUT_DIR}/img_icon_conn_connecting_alarm.c
make_images: ${IMG_OUT_DIR}/img_icon_conn_connected_alarm.c


$(BINDIR)/$(PROJECT).elf: make_images

$(BINDIR)/$(PROJECT).elf: makefsdata

$(BINDIR)/$(PROJECT).elf: ${OBJDIR}/ustdlib.o
$(BINDIR)/$(PROJECT).elf: ${OBJDIR}/string_utils.o

#
# System section
#
$(BINDIR)/$(PROJECT).elf: ${OBJDIR}/list.o
$(BINDIR)/$(PROJECT).elf: ${OBJDIR}/queue.o
$(BINDIR)/$(PROJECT).elf: ${OBJDIR}/tasks.o
$(BINDIR)/$(PROJECT).elf: ${OBJDIR}/port.o
#$(BINDIR)/$(PROJECT).elf: ${OBJDIR}/heap_2.o - I.Otrokha 2014-05-07
$(BINDIR)/$(PROJECT).elf: ${OBJDIR}/heap_4.o

$(BINDIR)/$(PROJECT).elf: ${OBJDIR}/system_stm32f2xx.o
$(BINDIR)/$(PROJECT).elf: ${OBJDIR}/stm32f2xx_rcc.o
$(BINDIR)/$(PROJECT).elf: ${OBJDIR}/stm32f2xx_fsmc.o
$(BINDIR)/$(PROJECT).elf: ${OBJDIR}/stm32f2xx_gpio.o
$(BINDIR)/$(PROJECT).elf: ${OBJDIR}/stm32f2xx_tim.o
$(BINDIR)/$(PROJECT).elf: ${OBJDIR}/stm32f2xx_spi.o
$(BINDIR)/$(PROJECT).elf: ${OBJDIR}/stm32f2xx_adc.o
$(BINDIR)/$(PROJECT).elf: ${OBJDIR}/stm32f2xx_flash.o
$(BINDIR)/$(PROJECT).elf: ${OBJDIR}/stm32f2xx_usart.o
$(BINDIR)/$(PROJECT).elf: ${OBJDIR}/stm32f2xx_dma.o
$(BINDIR)/$(PROJECT).elf: ${OBJDIR}/stm32f2xx_syscfg.o
$(BINDIR)/$(PROJECT).elf: ${OBJDIR}/stm32f2xx_iwdg.o
$(BINDIR)/$(PROJECT).elf: ${OBJDIR}/misc.o

$(BINDIR)/$(PROJECT).elf: ${OBJDIR}/crc.o
$(BINDIR)/$(PROJECT).elf: ${OBJDIR}/ustdlib.o

#
# Drivers section
#
$(BINDIR)/$(PROJECT).elf: ${OBJDIR}/fsmc.o
$(BINDIR)/$(PROJECT).elf: ${OBJDIR}/spi.o
$(BINDIR)/$(PROJECT).elf: ${OBJDIR}/uart1.o
$(BINDIR)/$(PROJECT).elf: ${OBJDIR}/uart3.o
$(BINDIR)/$(PROJECT).elf: ${OBJDIR}/sram.o
$(BINDIR)/$(PROJECT).elf: ${OBJDIR}/i2c_sw.o
$(BINDIR)/$(PROJECT).elf: ${OBJDIR}/app_data.o
$(BINDIR)/$(PROJECT).elf: ${OBJDIR}/lcd_driver.o
$(BINDIR)/$(PROJECT).elf: ${OBJDIR}/touch.o
$(BINDIR)/$(PROJECT).elf: ${OBJDIR}/modbus_master.o
$(BINDIR)/$(PROJECT).elf: ${OBJDIR}/modbus_slave.o
$(BINDIR)/$(PROJECT).elf: ${OBJDIR}/interface.o
$(BINDIR)/$(PROJECT).elf: ${OBJDIR}/inter_comm.o
$(BINDIR)/$(PROJECT).elf: ${OBJDIR}/ext_comm.o
$(BINDIR)/$(PROJECT).elf: ${OBJDIR}/storage.o
$(BINDIR)/$(PROJECT).elf: ${OBJDIR}/at_eeprom.o
#$(BINDIR)/$(PROJECT).elf: ${OBJDIR}/eeprom_at24c04.o

#
# System section
#
$(BINDIR)/$(PROJECT).elf: ${OBJDIR}/sys_timer.o

#
# Interface section
#
$(BINDIR)/$(PROJECT).elf: ${OBJDIR}/control_numeric_editor.o
$(BINDIR)/$(PROJECT).elf: ${OBJDIR}/control_adjustable_editor.o
$(BINDIR)/$(PROJECT).elf: ${OBJDIR}/adj_editor_themed.o

$(BINDIR)/$(PROJECT).elf: ${OBJDIR}/dialog_confirmation.o
$(BINDIR)/$(PROJECT).elf: ${OBJDIR}/dialog_numeric_editor.o
$(BINDIR)/$(PROJECT).elf: ${OBJDIR}/dialog_adjust.o
$(BINDIR)/$(PROJECT).elf: ${OBJDIR}/dialog_adjust_double.o

$(BINDIR)/$(PROJECT).elf: ${OBJDIR}/main_table.o
$(BINDIR)/$(PROJECT).elf: ${OBJDIR}/modbus_table.o
$(BINDIR)/$(PROJECT).elf: ${OBJDIR}/table_window.o

$(BINDIR)/$(PROJECT).elf: ${OBJDIR}/color_palette.o

$(BINDIR)/$(PROJECT).elf: ${OBJDIR}/window_confirm.o

$(BINDIR)/$(PROJECT).elf: ${OBJDIR}/window_manager.o

$(BINDIR)/$(PROJECT).elf: ${OBJDIR}/window_main.o
$(BINDIR)/$(PROJECT).elf: ${OBJDIR}/window_irps_tester.o

$(BINDIR)/$(PROJECT).elf: ${OBJDIR}/window_setup_mbus_props.o

$(BINDIR)/$(PROJECT).elf: ${OBJDIR}/horizontal_debug_tab.o

$(BINDIR)/$(PROJECT).elf: ${OBJDIR}/window_cell_setup_popup.o

#$(BINDIR)/$(PROJECT).elf: ${OBJDIR}/debug_input.o
#$(BINDIR)/$(PROJECT).elf: ${OBJDIR}/debug_output.o
#$(BINDIR)/$(PROJECT).elf: ${OBJDIR}/debug_psu.o
#$(BINDIR)/$(PROJECT).elf: ${OBJDIR}/debug_battery.o
#$(BINDIR)/$(PROJECT).elf: ${OBJDIR}/debug_ins_mon.o
#$(BINDIR)/$(PROJECT).elf: ${OBJDIR}/debug_system.o
#$(BINDIR)/$(PROJECT).elf: ${OBJDIR}/debug_cfg.o
#$(BINDIR)/$(PROJECT).elf: ${OBJDIR}/debug_extra.o

#$(BINDIR)/$(PROJECT).elf: ${OBJDIR}/debug_ac_horizontal.o
#$(BINDIR)/$(PROJECT).elf: ${OBJDIR}/debug_batt_swire_horizontal.o
#$(BINDIR)/$(PROJECT).elf: ${OBJDIR}/debug_io_horizontal.o
#$(BINDIR)/$(PROJECT).elf: ${OBJDIR}/debug_system_horizontal.o
#$(BINDIR)/$(PROJECT).elf: ${OBJDIR}/debug_debug_horizontal.o

# $(BINDIR)/$(PROJECT).elf: ${OBJDIR}/debug_inverters.o
# $(BINDIR)/$(PROJECT).elf: ${OBJDIR}/debug_task.o


#$(BINDIR)/$(PROJECT).elf: ${OBJDIR}/protocol_driver.o


#
# Ethernet section
#
$(BINDIR)/$(PROJECT).elf: $(OBJDIR)/lwiplib.o
$(BINDIR)/$(PROJECT).elf: $(OBJDIR)/network.o
$(BINDIR)/$(PROJECT).elf: ${OBJDIR}/rom_fsdata.o
$(BINDIR)/$(PROJECT).elf: ${OBJDIR}/cgi_fsdata.o
$(BINDIR)/$(PROJECT).elf: $(OBJDIR)/fs.o
$(BINDIR)/$(PROJECT).elf: $(OBJDIR)/json.o
$(BINDIR)/$(PROJECT).elf: $(OBJDIR)/cgi.o
$(BINDIR)/$(PROJECT).elf: $(OBJDIR)/httpd.o


# $(BINDIR)/$(PROJECT).elf: $(OBJDIR)/lwiplib.o
# $(BINDIR)/$(PROJECT).elf: $(OBJDIR)/json.o
# $(BINDIR)/$(PROJECT).elf: $(OBJDIR)/cgibin.o
# $(BINDIR)/$(PROJECT).elf: $(OBJDIR)/cgifuncs.o
# $(BINDIR)/$(PROJECT).elf: $(OBJDIR)/httpd-netconn.o

#
# Property
#
#$(BINDIR)/$(PROJECT).elf: ${OBJDIR}/im_property.o
#$(BINDIR)/$(PROJECT).elf: ${OBJDIR}/input_property.o
#$(BINDIR)/$(PROJECT).elf: ${OBJDIR}/battery_property.o
#$(BINDIR)/$(PROJECT).elf: ${OBJDIR}/ups_property.o
#$(BINDIR)/$(PROJECT).elf: ${OBJDIR}/psu_property.o
#$(BINDIR)/$(PROJECT).elf: ${OBJDIR}/inverter_property.o
#$(BINDIR)/$(PROJECT).elf: ${OBJDIR}/ac_output_property.o
#$(BINDIR)/$(PROJECT).elf: ${OBJDIR}/dc_output_property.o
#$(BINDIR)/$(PROJECT).elf: ${OBJDIR}/modbus_property.o
#$(BINDIR)/$(PROJECT).elf: ${OBJDIR}/net_property.o
#$(BINDIR)/$(PROJECT).elf: ${OBJDIR}/debug_property.o
#$(BINDIR)/$(PROJECT).elf: ${OBJDIR}/heater_property.o

$(BINDIR)/$(PROJECT).elf: ${OBJDIR}/timers.o


#
# Main section
#
$(BINDIR)/$(PROJECT).elf: ${OBJDIR}/runtime.o
$(BINDIR)/$(PROJECT).elf: ${OBJDIR}/startup.o
$(BINDIR)/$(PROJECT).elf: ${OBJDIR}/main.o
$(BINDIR)/$(PROJECT).elf: $(LIBGR)

