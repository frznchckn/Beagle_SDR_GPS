/*
--------------------------------------------------------------------------------
This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Library General Public
License as published by the Free Software Foundation; either
version 2 of the License, or (at your option) any later version.
This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Library General Public License for more details.
You should have received a copy of the GNU Library General Public
License along with this library; if not, write to the
Free Software Foundation, Inc., 51 Franklin St, Fifth Floor,
Boston, MA  02110-1301, USA.
--------------------------------------------------------------------------------
*/

// Copyright (c) 2015 John Seamons, ZL/KF6VO

#ifndef _KIWI_H_
#define _KIWI_H_

#include "types.h"
#include "kiwi.gen.h"
#include "web.h"
#include "coroutines.h"

#define	I	0
#define	Q	1
#define	IQ	2

// ADC clk generated from FPGA via multiplied GPS TCXO
	#define	GPS_CLOCK			(16.368*MHz)		// 61.095 ns
	#define ADC_CLOCK_66M_NOM	(66.666600*MHz)		// 66.6666 MHz 15.0 ns
	#define ADC_CLOCK_66M_TYP	(66.666070*MHz)		// typical 25 degC value on proto1
	#define ADC_CLOCK_66M_TEST	(66.665000*MHz)		// intentionally low
	#define ADC_CLOCK_65M		(GPS_CLOCK*4)		// 65.472 MHz 15.274 ns
	#define ADC_CLOCK_49M		(GPS_CLOCK*3)		// 49.104 MHz
	#define ADC_CLOCK_82M		(GPS_CLOCK*5)		// 81.840 MHz

// use 80M ADC clk on HF1, generate sync 20 MHz CPU clock in FPGA (GPS won't work)
	#define	ADC_CLOCK_80M_SYNC	(80.0*MHz)			// 12.500 ns

// use 80M ADC clk on HF1, generate async CPU/GPS clock from GPS TCXO
	#define	ADC_CLOCK_80M_ASYNC	(80.0*MHz)			// 12.500 ns

#define	ADC_MAX		0x1fff		// +8K
#define	ADC_MIN		0x2000		// -8K

// S-Meter calibration offset added to make reading absolute dBm
// Measured against 8642A as source on 11-Oct-14
#define SMETER_CALIBRATION -12	

// Use odd values so periodic signals like radars running at even-Hz rates don't
// beat against update rate and produce artifacts or blanking.
#define	WF_SPEED_MAX		23
#define	WF_SPEED_SLOW		7
#define	WF_SPEED_MED		17
#define	WF_SPEED_FAST		WF_SPEED_MAX

#define	WF_SPEED_OTHERS		5

#define	WEB_SERVER_POLL_US	(1000000 / WF_SPEED_MAX / 2)

extern rx_chan_t rx_chan[];
extern conn_t conns[];
extern bool background_mode;
extern int p0, p1, p2, wf_sim, wf_real, wf_time, ev_dump, wf_flip, wf_exit, wf_start, tone, down, navg,
	rx_cordic, rx_cic, rx_cic2, rx_dump, wf_cordic, wf_cic, wf_mult, wf_mult_gen, show_adc, wspr, meas,
	rx_yield, gps_chans, spi_clkg, spi_speed, wf_max, rx_num, wf_num, do_slice, do_gps, do_sdr, wf_olap,
	spi_delay, do_fft, noisePwr, unwrap, rev_iq, ineg, qneg, fft_file, fftsize, fftuse, bg,
	color_map, port_override, kiwiSDR, print_stats, ecpu_cmds, ecpu_tcmds;
extern float g_genfreq, g_genampl, g_mixfreq;
extern double adc_clock_nom, adc_clock;

extern lock_t overlapped_sampling_lock, spi_lock;
extern volatile int audio_bytes, waterfall_bytes, waterfall_frames[], http_bytes;

#define	ROM_WIDTH	16
#define	ROM_DEPTH	13
extern u4_t rom_sin[1<<ROM_DEPTH];


// sound
extern const char *mode_s[6];	// = { "am", "amn", "usb", "lsb", "cw", "cwn" };
enum mode_e { MODE_AM, MODE_AMN, MODE_USB, MODE_LSB, MODE_CW, MODE_CWN };

// waterfall
#define	WF_SPLIT			0
#define	WF_SPECTRUM			1
#define	WF_NORM				2
#define	WF_WEAK				3
#define	WF_STRONG			4

#define META_WSPR_INIT		8
#define META_WSPR_SYNC		9
#define META_WSPR_DATA		10
#define	META_WSPR_PEAKS		11
#define META_WSPR_DECODING	12
#define META_WSPR_DECODED	13

#define	KEEPALIVE_MS		16000

#define GPS_TASKS			(GPS_CHANS + 3)			// chan*n + search + solve + user
#define	RX_TASKS			(RX_CHANS*2 + 1)		// chan*n + web
#define	MAX_TASKS			(GPS_TASKS + RX_TASKS + 4)

void rx_server_init();
void rx_server_remove(conn_t *c);
int rx_server_users();

enum websocket_mode_e { WS_MODE_ALLOC, WS_MODE_LOOKUP, WS_MODE_CLOSE };
conn_t *rx_server_websocket(struct mg_connection *mc, websocket_mode_e);

void w2a_sound_init();
void w2a_sound(void *param);

void w2a_waterfall_init();
void w2a_waterfall(void *param);

void w2a_admin(void *param);

enum logtype_e { LOG_ARRIVED, LOG_UPDATE, LOG_UPDATE_NC, LOG_LEAVING };
void loguser(conn_t *c, logtype_e type);
void webserver_collect_print_stats(int print);

// override printf so we can add a timestamp, log it, etc.
#define ALT_PRINTF alt_printf
//#define ALT_PRINTF printf
#define printf ALT_PRINTF
void alt_printf(const char *fmt, ...);

// versions of printf & lprintf that preface message with rx channel
void cprintf(conn_t *c, const char *fmt, ...);
void clprintf(conn_t *c, const char *fmt, ...);

#endif