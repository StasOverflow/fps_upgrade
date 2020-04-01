/*
 * images.h
 *
 *  Created on: 07.06.2014
 *      Author: V.Dudnik
 */

#ifndef IMAGES_H_
#define IMAGES_H_

//*****************************************************************************
//
// If building with a C++ compiler, make all of the definitions in this header
// have a C binding.
//
//*****************************************************************************
#ifdef __cplusplus
extern "C"
{
#endif


//extern const unsigned char g_pucIconLogo[];
//extern const unsigned char g_pucIconSmile[];
//extern const unsigned char g_pucIconWarning[];
//extern const unsigned char g_pucIconBattery[];
//extern const unsigned char g_pucIconBatteryDischarge[];
//extern const unsigned char g_pucIconBatteryLow[];
//extern const unsigned char g_pucIconBatteryOFF[];
//extern const unsigned char g_pucIconBatteryOverheat[];
//extern const unsigned char g_pucIconBypass[];
//extern const unsigned char g_pucIconAlarm[];


extern const unsigned char g_pucimg_main[];
extern const unsigned char g_pucimg_main_subwindow[];
extern const unsigned char g_pucimg_main_with_shapka[];
extern const unsigned char g_pucimg_debug[];
extern const unsigned char g_pucimg_info[];

extern const unsigned char g_pucimg_bad_crosshair[];

extern const unsigned char g_pucimg_led_yellow_small[];
extern const unsigned char g_pucimg_led_green_small[];
extern const unsigned char g_pucimg_led_green_upgraded_small[];
extern const unsigned char g_pucimg_led_red_small[];
extern const unsigned char g_pucimg_led_disabled_small[];
extern const unsigned char g_pucimg_led_disabled_dark_small[];
extern const unsigned char g_pucimg_led_empty_small[];
extern const unsigned char g_pucimg_led_cyan_small[];

extern const unsigned char g_pucimg_led_green_medium[];
extern const unsigned char g_pucimg_led_red_medium[];
extern const unsigned char g_pucimg_led_disabled_medium[];

extern const unsigned char g_pucimg_btn_ohm[];
extern const unsigned char g_pucimg_btn_battery[];
extern const unsigned char g_pucimg_btn_settings[];

extern const unsigned char g_pucimg_icon_battery[];
extern const unsigned char g_pucimg_icon_air[];

extern const unsigned char g_pucimg_mode_btn_selected[];

extern const unsigned char g_pucimg_pulse_logo[];

extern const unsigned char g_pucimg_icon_conn_disconnected_alarm[];
extern const unsigned char g_pucimg_icon_conn_connecting_alarm[];
extern const unsigned char g_pucimg_icon_conn_connected_alarm[];

//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef __cplusplus
}
#endif

#endif /* IMAGES_H_ */
