# This file is generated automatically. Do not edit.
# Use project properties -> Build -> Qt -> Expert -> Custom Definitions.
TEMPLATE = app
DESTDIR = dist/Debug/GNU_1-Linux-x86
TARGET = tv_player
VERSION = 1.0.0
CONFIG -= debug_and_release app_bundle lib_bundle
CONFIG += link_pkgconfig release 
PKGCONFIG += gstreamer-0.10
QT = core gui
SOURCES += main.cpp src/Language/AS_language.cpp src/MP3ID3.cpp src/QT/QAutoScanThread/QAutoScanThread.cpp src/QT/QAutoScanWindow/QAutoScanWindow.cpp src/QT/QChannelSwitchThrd/QChannelSwitchThrd.cpp src/QT/QFastList/QFastList.cpp src/QT/QInitThread/QInitThread.cpp src/QT/QLabel1.cpp src/QT/QMediaPlayer/QMediaAudioWindow.cpp src/QT/QMediaPlayer/QMediaPlayer.cpp src/QT/QMediaPlayer/QMediaVideoWindow.cpp src/QT/QMediaPlayer/QMpStartThrd.cpp src/QT/QMediaPlayer/file_browser.cpp src/QT/QMenuInfoWindow/QMenuInfoWindow.cpp src/QT/QMessageWindow/QMessageWindow.cpp src/QT/QMoreInfoWindow/QMoreInfoWindow.cpp src/QT/QMoreInfoWindow/QMoreLongInfo.cpp src/QT/QMostFileBrowser/QMostFileBrowser.cpp src/QT/QMostInterface/QMediaItem.cpp src/QT/QMostInterface/QMostInterface.cpp src/QT/QMostInterface/QMostItem.cpp src/QT/QMostMediaPlayer/QMostMediaPlayer.cpp src/QT/QMostTvList/QMostTvList.cpp src/QT/QMostTvList/most_tv_item.cpp src/QT/QOtherOptWindow/QOtherOptWindow.cpp src/QT/QProgressIndicator/QProgressIndicator.cpp src/QT/QRNS_Buttons/QRNS_Buttons.cpp src/QT/QScreenScallingWin/QScreenScallingWin.cpp src/QT/QTvServiceList/QTvList.cpp src/QT/QTvServiceManager/QTvServiceManager.cpp src/QT/QmenuTvItem/QmenuTvItem.cpp src/QT/app_settings/app_settings.cpp src/QT/main_bar/tv_bar.cpp src/QT/main_menu/main_menu.cpp src/QT/main_menu/manual_scan.cpp src/QT/main_menu/submenu_scan.cpp src/QT/main_menu_root.cpp src/QT/qserialdevice-qserialdevice/serialport.cpp src/QT/qserialdevice-qserialdevice/serialportengine_p_unix.cpp src/QT/qserialdevice-qserialdevice/serialportinfo.cpp src/QT/qserialdevice-qserialdevice/serialportinfo_unix.cpp src/QT/qserialdevice-qserialdevice/ttylocker_p_unix.cpp src/QT/scrolltext.cpp src/QT/volume/volume_bar.cpp src/QT/wait_animation.cpp src/libdtv/dtv_usr/dtv_core.cpp src/libdtv/dtv_usr/dtv_usr_epg.cpp src/libdtv/dtv_usr/dtv_usr_record.cpp src/libdtv/dtv_usr/dtv_usr_time.cpp src/libdtv/dvb_time/dtv_time.cpp src/libdtv/epg/eit.cpp src/libdtv/gst/dma_mem_src.cpp src/libdtv/gst/gst_core.cpp src/libdtv/interface/section.cpp src/libdtv/misc/crc32.cpp src/libdtv/misc/descriptors.cpp src/libdtv/misc/dvb_srvice_pid_list.cpp src/libdtv/misc/epg_events_list.cpp src/libdtv/misc/misc_func.cpp src/libdtv/misc/service_list.cpp src/libdtv/misc/xml_read_write.cpp src/libdtv/nit/dvb_network_info.cpp src/libdtv/pat/dvb_pat_data.cpp src/libdtv/pmt/dvb_pmt_data.cpp src/libdtv/sdt/dvb_sdt_data.cpp src/libdtv/service_management/service_management.cpp src/libdtv/ts_live/interface.cpp src/libdtv/ts_live/tsi_live.cpp src/media_player/GstPlayer.cpp src/media_player/GstPlayerPipeline.cpp src/media_player/mapm.c src/overlay/overlay.c src/overlay/sslLcdcApi.c src/play_ctrl.cpp src/player_ctrl_most.cpp src/player_ctrl_thrads.cpp
HEADERS += include/GstPlayer.h include/MemTrack.h include/QAutoScanThread.h include/QAutoScanWindow.h include/QChannelSwitchThrd.h include/QInitThread.h include/QLabel1.h include/QMenuInfoWindow.h include/QMessageWindow.h include/QMoreInfoWindow.h include/QMoreLongInfo.h include/QMostMediaPlayer.h include/QOtherOptWindow.h include/QProgressIndicator.h include/QRNS_Buttons.h include/QTvServiceManager.h include/QmenuTvItem.h include/app_settings.h include/audio_player.h include/bitmap.h include/crc32.h include/descriptors.h include/dma_mem_src.h include/dtv_core.h include/dtv_time.h include/dvb_network_info.h include/dvb_pat_data.h include/dvb_pmt_data.h include/dvb_sdt_data.h include/dvb_service.h include/dvb_srvice_pid_list.h include/dvbtcontrol.h include/eit.h include/eit_struct_define.h include/epg_events_list.h include/gpio_ctrl.h include/gst_core.h include/headers.h include/interface.h include/libucsi/crc32.h include/libucsi/descriptor.h include/libucsi/endianops.h include/libucsi/section.h include/libucsi/section_buf.h include/libucsi/transport_packet.h include/libucsi/types.h include/magusoverlay.h include/main_menu.h include/main_menu_root.h include/manual_scan.h include/mapm.h include/misc_func.h include/multiplex.h include/overlay.h include/play_ctrl.h include/rc.h include/section.h include/service_list.h include/service_management.h include/sslLcdcApi.h include/struct_define.h include/submenu_scan.h include/tsi.h include/tsi_gst_test.h include/tv_bar.h include/tve.h include/type.h include/uart_io.h include/version.h include/volume_bar.h include/vpp_external.h include/wait_animation.h include/xml_read_write.h src/QT/qserialdevice-qserialdevice/ringbuffer_p.h src/QT/qserialdevice-qserialdevice/serialport_p.h src/QT/qserialdevice-qserialdevice/serialportengine_p.h src/QT/qserialdevice-qserialdevice/serialportengine_p_unix.h src/QT/qserialdevice-qserialdevice/serialportinfo_p.h src/QT/qserialdevice-qserialdevice/ttylocker_p_unix.h
FORMS +=
RESOURCES += Images.qrc
TRANSLATIONS +=
OBJECTS_DIR = build/Debug/GNU_1-Linux-x86
MOC_DIR = moc
RCC_DIR = qrc
UI_DIR = 
QMAKE_CC = arm-linux-gcc
QMAKE_CXX = arm-linux-g++
DEFINES += DTV_CORE_DEBUG_T DTV_CORE_ERROR_DEBUG_T ENABLE_MOST SSL_GST_PORT_VQ SSL_GST_PORT_VQ_AQBASE=500 SSL_GST_VPPINSINK SSL_GST_VPPINSINK_MAX_LATENESS_IN_MS=80 TSI_LIVE 
INCLUDEPATH += /usr/local/include/gstreamer-0.10 include /usr/include/libxml2 /usr/SSD1938/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/libxml2/libxml . 
LIBS += -ldibcomcontrol -lgpioctrl -ltsi -lucsi -liconv -lgstbase-0.10 -lgstapp-0.10 -lid3tag  
equals(QT_MAJOR_VERSION, 4) {
QMAKE_CXXFLAGS += -std=c++98
}
"CONFIG +=link_pkgconfig"
PKGCONFIG = gstreamer-0.10
LIBS += -L./lib
QMAKE_CXXFLAGS+=-Wno-write-strings -Wno-multichar -Wall
QMAKE_LDFLAGS +=-liconv -lucsi -lgstapp-0.10 -lgstbase-0.10
QMAKE_CFLAGS +=-DTSI_LIVE -I/usr/local/include/gstreamer-0.10 -Iinclude -Wall
HEADERS += include/most_tv_item.h
QMAKE_CFLAGS_RELEASE+=-O2
SOURCES +=src/libdtv/misc/burst.S
HEADERS += include/serialport.h
HEADERS += include/QMediaItem.h
HEADERS += include/MP3ID3.h
HEADERS += include/QMostTvList.h
SOURCES +=src/libdtv/misc/vpu_asm.S
HEADERS += include/MP3Info.h
HEADERS += include/scrolltext.h
HEADERS += include/QScreenScallingWin.h
HEADERS += include/AS_Language.h
HEADERS += include/QMostItem.h
HEADERS += include/QMostInterface.h
HEADERS += include/QMostFileBrowser.h
HEADERS += include/QMpStartThrd.h
HEADERS += include/QMediaAudioWindow.h
HEADERS += include/file_browser.h
HEADERS += include/QMediaPlayer.h
HEADERS += include/QMediaVideoWindow.h
HEADERS += include/QFastList.h
HEADERS += include/QTvList.h
