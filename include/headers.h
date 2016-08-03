/* 
 * File:   headers.h
 * Author: michalm
 *
 * Created on 26 marzec 2013, 07:29
 */

#ifndef HEADERS_H
#define	HEADERS_H



#if defined __cplusplus

#include <QtGui/QApplication>
#include <QtCore/qcoreapplication.h>

#include "rc.h"
#include "main_menu.h"
#include "submenu_scan.h"
#include "QTvServiceManager.h"
#include "QOtherOptWindow.h"
#include "QDateTimeWindow.h"
#include "app_settings.h"
#include "QUpdateWin.h"

#include <sstream>
#include <cstdlib>
#include <stdlib.h>
#include <string>
#include <string.h>
#include <stdio.h>
#include <linux/watchdog.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <QtGui/qwidget.h>

#include "dvbtcontrol.h"

#include "rc.h"
#include "dtv_core.h"
#include "tv_bar.h"
#include "serialport.h"
#include "serialportinfo.h"
#include "QUserServiceList.h"
#include "QAutoScanWindow.h"
#include "gpio_ctrl.h"
#include "tsi.h"
#endif

#endif	/* HEADERS_H */

