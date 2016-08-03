# This file is generated automatically. Do not edit.
# Use project properties -> Build -> Qt -> Expert -> Custom Definitions.
TEMPLATE = app
DESTDIR = dist/Release/ARM11_SSD1938-Linux-x86
TARGET = tv_player
VERSION = 1.0.0
CONFIG -= debug_and_release app_bundle lib_bundle
CONFIG += release 
QT = core gui
SOURCES += src/libdtv/dtv_usr/dtv_usr_time.cpp src/libdtv/service_management/service_management.cpp src/libdtv/misc/dvb_srvice_pid_list.cpp src/rc/uart_io.cpp src/libdtv/dtv_usr/dtv_core.cpp src/libdtv/dtv_usr/dtv_usr_epg.cpp src/overlay/overlay.c src/QT/main_bar/tv_bar.cpp main.cpp src/libdtv/misc/crc32.cpp src/play_ctrl.cpp src/libdtv/nit/dvb_network_info.cpp src/libdtv/interface/section.cpp src/libdtv/misc/misc_func.cpp src/libdtv/misc/descriptors.cpp src/libdtv/pmt/dvb_pmt_data.cpp src/libdtv/misc/service_list.cpp src/libdtv/epg/eit.cpp src/libdtv/misc/xml_read_write.cpp src/libdtv/gst/dma_mem_src.cpp src/libdtv/pat/dvb_pat_data.cpp src/libdtv/sdt/dvb_sdt_data.cpp src/libdtv/ts_live/interface.cpp src/rc/player_ctrl_thrads.cpp src/libdtv/interface/ts_file.cpp src/libdtv/gst/gst_core.cpp src/libdtv/ts_live/tsi_live.cpp src/QT/volume/volume_bar.cpp src/libdtv/misc/epg_events_list.cpp src/libdtv/dtv_usr/dtv_usr_record.cpp src/libdtv/dvb_time/dtv_time.cpp
HEADERS += include/service_management.h include/interface.h include/tve.h include/eit.h include/audio_player.h include/dvb_srvice_pid_list.h include/libucsi/crc32.h include/dvb_network_info.h include/epg_events_list.h include/dma_mem_src.h include/type.h include/dvb_pmt_data.h include/play_ctrl.h include/overlay.h include/bitmap.h include/descriptors.h include/GstPlayer.h include/eit_struct_define.h include/service_list.h include/mapm.h include/dvbtcontrol.h include/dtv_time.h include/vpp_external.h include/gst_core.h include/multiplex.h include/tsi.h include/tsi_gst_test.h include/magusoverlay.h include/libucsi/endianops.h include/struct_define.h include/libucsi/section_buf.h include/tv_bar.h include/volume_bar.h include/dvb_sdt_data.h include/libucsi/types.h include/dvb_service.h include/xml_read_write.h include/libucsi/transport_packet.h include/rc.h include/list.h include/MemTrack.h include/uart_io.h include/libucsi/descriptor.h include/crc32.h include/libucsi/section.h include/section.h include/dtv_core.h include/dvb_pat_data.h include/misc_func.h include/gpio_ctrl.h
FORMS +=
RESOURCES +=
TRANSLATIONS +=
OBJECTS_DIR = build/Release/ARM11_SSD1938-Linux-x86
MOC_DIR = 
RCC_DIR = 
UI_DIR = 
QMAKE_CC = arm-linux-gcc
QMAKE_CXX = arm-linux-g++
DEFINES += 
INCLUDEPATH += 
LIBS += 
