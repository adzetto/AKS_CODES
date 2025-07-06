/**
 * @file lv_conf.h
 * Configuration file for v8.3.11
 */

#ifndef LV_CONF_H
#define LV_CONF_H

#include <stdint.h>

/*====================
   COLOR SETTINGS
 *====================*/

#define LV_COLOR_DEPTH 16

#define LV_COLOR_16_SWAP 0

#define LV_COLOR_SCREEN_TRANSP 0

#define LV_COLOR_MIX_ROUND_OFS 0

#define LV_COLOR_CHROMA_KEY lv_color_hex(0x00ff00)

/*=========================
   MEMORY SETTINGS
 *=========================*/

#define LV_MEM_CUSTOM 0
#if LV_MEM_CUSTOM == 0
    #define LV_MEM_SIZE (64U * 1024U)
    #define LV_MEM_ADR 0
    #define LV_MEM_AUTO_DEFRAG 1
#else
    #define LV_MEM_CUSTOM_INCLUDE <stdlib.h>
    #define LV_MEM_CUSTOM_ALLOC malloc
    #define LV_MEM_CUSTOM_FREE free
    #define LV_MEM_CUSTOM_REALLOC realloc
#endif

#define LV_MEMCPY_MEMSET_STD 0

/*====================
   HAL SETTINGS
 *====================*/

#define LV_DISP_DEF_REFR_PERIOD 16
#define LV_INDEV_DEF_READ_PERIOD 16

/*====================
   FEATURE CONFIGURATION
 *====================*/

#define LV_USE_FILESYSTEM 1
#define LV_USE_PNG 1
#define LV_USE_SJPG 1

/*==================
 * FONT USAGE
 *==================*/

#define LV_FONT_MONTSERRAT_8  0
#define LV_FONT_MONTSERRAT_10 0
#define LV_FONT_MONTSERRAT_12 0
#define LV_FONT_MONTSERRAT_14 1
#define LV_FONT_MONTSERRAT_16 0
#define LV_FONT_MONTSERRAT_18 1
#define LV_FONT_MONTSERRAT_20 0
#define LV_FONT_MONTSERRAT_22 1
#define LV_FONT_MONTSERRAT_24 0
#define LV_FONT_MONTSERRAT_26 1
#define LV_FONT_MONTSERRAT_28 0
#define LV_FONT_MONTSERRAT_30 1
#define LV_FONT_MONTSERRAT_32 0
#define LV_FONT_MONTSERRAT_34 1
#define LV_FONT_MONTSERRAT_36 0
#define LV_FONT_MONTSERRAT_38 1
#define LV_FONT_MONTSERRAT_40 0
#define LV_FONT_MONTSERRAT_42 1
#define LV_FONT_MONTSERRAT_44 0
#define LV_FONT_MONTSERRAT_46 1
#define LV_FONT_MONTSERRAT_48 1

#define LV_FONT_DEFAULT &lv_font_montserrat_14

#define LV_FONT_FMT_TXT_LARGE 0

#define LV_USE_FONT_COMPRESSED 0

#define LV_USE_FONT_SUBPX 0

/*=========================
   THEME USAGE
 *=========================*/

#define LV_USE_THEME_DEFAULT 1
#if LV_USE_THEME_DEFAULT
    #define LV_THEME_DEFAULT_DARK 0
    #define LV_THEME_DEFAULT_GROW 1
    #define LV_THEME_DEFAULT_TRANSITION_TIME 80
#endif

#define LV_USE_THEME_BASIC 1

/*===================
   OTHERS
 *===================*/

#define LV_USE_PERF_MONITOR 0
#define LV_USE_MEM_MONITOR 0

#define LV_USE_USER_DATA 1

#define LV_ENABLE_GC 1
#if LV_ENABLE_GC != 0
    #define LV_GC_INCLUDE "gc.h"
#endif

#define LV_BIG_ENDIAN_SYSTEM 0

#define LV_ATTRIBUTE_TICK_INC
#define LV_ATTRIBUTE_TIMER_HANDLER
#define LV_ATTRIBUTE_FLUSH_READY
#define LV_ATTRIBUTE_MEM_ALIGN
#define LV_ATTRIBUTE_LARGE_CONST

#define LV_EXPORT_CONST_INT(int_value) struct _silence_gcc_warning

#define LV_USE_ASSERT_NULL 1
#define LV_USE_ASSERT_MALLOC 1
#define LV_USE_ASSERT_STYLE 0
#define LV_USE_ASSERT_MEM_INTEGRITY 0
#define LV_USE_ASSERT_OBJ 0

#define LV_ASSERT_HANDLER_INCLUDE <stdint.h>
#define LV_ASSERT_HANDLER while(1);

#define LV_USE_LOG 0
#if LV_USE_LOG
    #define LV_LOG_LEVEL LV_LOG_LEVEL_WARN
    #define LV_LOG_PRINTF 0
    #define LV_LOG_USE_TIMESTAMP 1
    #define LV_LOG_TRACE_MEM 1
    #define LV_LOG_TRACE_TIMER 1
    #define LV_LOG_TRACE_INDEV 1
    #define LV_LOG_TRACE_DISP_REFR 1
    #define LV_LOG_TRACE_EVENT 1
    #define LV_LOG_TRACE_OBJ_CREATE 1
    #define LV_LOG_TRACE_LAYOUT 1
    #define LV_LOG_TRACE_ANIM 1
#endif

#define LV_USE_PROFILER 0
#if LV_USE_PROFILER
    #define LV_USE_PROFILER_BUILTIN 1
#endif

#define LV_TICK_CUSTOM 1
#if LV_TICK_CUSTOM
    #define LV_TICK_CUSTOM_INCLUDE "Arduino.h"
    #define LV_TICK_CUSTOM_SYS_TIME_EXPR (millis())
#endif

#define LV_DPI_DEF 130

#define LV_DRAW_COMPLEX 1
#if LV_DRAW_COMPLEX != 0
    #define LV_SHADOW_CACHE_SIZE 0
    #define LV_CIRCLE_CACHE_SIZE 4
#endif

#define LV_DISP_ROT_MAX_BUF (10*1024)

#define LV_USE_GPU_ARM2D 0
#define LV_USE_GPU_STM32_DMA2D 0

#define LV_USE_GPU_SWM341_DMA2D 0
#define LV_USE_GPU_NXP_PXP 0
#define LV_USE_GPU_NXP_VG_LITE 0
#define LV_USE_GPU_SDL 0

#define LV_USE_EXTERNAL_RENDERER 0

#define LV_USE_SNAPSHOT 0

#define LV_USE_MONKEY 0

#define LV_USE_GRIDNAV 0

#define LV_USE_FRAGMENT 0

#define LV_USE_IMGFONT 0

#define LV_USE_MSG 0

#define LV_USE_IME_PINYIN 0

#define LV_USE_FILE_EXPLORER 0

#define LV_USE_DEMO_WIDGETS 0

#define LV_USE_DEMO_KEYPAD_AND_ENCODER 0

#define LV_USE_DEMO_BENCHMARK 0

#define LV_USE_DEMO_STRESS 0

#define LV_USE_DEMO_MUSIC 0

#define LV_USE_FLEX 1

#define LV_USE_GRID 1

#endif /*LV_CONF_H*/