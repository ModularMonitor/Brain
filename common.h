//#pragma once

#define SPI_DISPLAY HSPI
#define SPI_SDCARD VSPI

#define cpu_stack_default  6144

#define def_spi_core_id  1 // spi (tft screen, touch)
#define def_alt_core_id  0 // other tasks (+ sd card)

#define cpu_core_id_for_ctl       def_alt_core_id
#define cpu_core_id_for_display   def_spi_core_id
#define cpu_core_id_for_sd_card   def_alt_core_id

#define DISPLAY_CALIBRATION_FILE "/display_calibration.ini"

#define DISPLAY_MISO     19
#define DISPLAY_MOSI     23
#define DISPLAY_SCLK     18
#define DISPLAY_CS       15
#define DISPLAY_DC        2
#define DISPLAY_RST       4
#define DISPLAY_TOUCH_CS 16

#define SDCARD_CS 33
#define SDCARD_MOSI 14
#define SDCARD_MISO 32
#define SDCARD_SCK 12


// DELETE X if X != nullptr then X = nullptr;
#define DEL_IT(X) { if (X) delete X; X = nullptr; }
// DELETE ARRAY (X) if X != nullptr then X = nullptr;
#define DEL_EM(X) { if (X) delete[] X; X = nullptr; }
// TARG << FROM, FROM << FROM_VAL
#define EXCHANGE(TARG, FROM, FROM_VAL) { TARG = FROM; FROM = FROM_VAL; }
// TARG << FROM, FROM << NULL
#define EXC_NULL(TARG, FROM) EXCHANGE(TARG, FROM, nullptr)
// EXCHANGE for return
#define EXC_RETURN(VALUE_RETURN, TO_SET_VALUE) [&]{ const auto ___v = VALUE_RETURN; VALUE_RETURN = TO_SET_VALUE; return ___v; }()

#define TABLE_FLIP_CHIP_I_AM_DEAD() esp_restart()

// allocates with new[], should dealloc with delete[]
#define saprintf(PPTR, SIZETPTR_LEN, FORMAT, ...) { int ____l = snprintf(nullptr, 0, FORMAT, __VA_ARGS__); PPTR = new char[____l + 1]; (*SIZETPTR_LEN) = static_cast<size_t>(____l); ____l = snprintf(PPTR, ____l + 1, FORMAT, __VA_ARGS__); if (____l < 0) { delete[] PPTR; PPTR = nullptr; (*SIZETPTR_LEN) = 0; }}

/* HEADER ONLY USE: */

#define MAKE_SINGLETON_CLASS_F(CLASSNAME, CLASSBODY, FUNCFLAGS) class CLASSNAME CLASSBODY; CLASSNAME& FUNCFLAGS get_singleton_of_##CLASSNAME(){ static CLASSNAME obj; return obj; }
#define MAKE_SINGLETON_CLASS_CF(CLASSNAME, CONSTRUCTOR, FUNCFLAGS) MAKE_SINGLETON_CLASS_F(CLASSNAME, { public: CLASSNAME() { CONSTRUCTOR; } }, FUNCFLAGS)
#define MAKE_SINGLETON_CLASS_INIT_F(CLASSNAME, CLASSBODY, FUNCFLAGS) MAKE_SINGLETON_CLASS_F(CLASSNAME, CLASSBODY, FUNCFLAGS) namespace ______dummy_namespace_for_##CLASSNAME{  const auto& ______dummy_ref_##CLASSNAME = get_singleton_of_##CLASSNAME(); }
#define MAKE_SINGLETON_CLASS_INIT_CF(CLASSNAME, CONSTRUCTOR, FUNCFLAGS) MAKE_SINGLETON_CLASS_CF(CLASSNAME, CONSTRUCTOR, FUNCFLAGS) namespace ______dummy_namespace_for_##CLASSNAME{  const auto& ______dummy_ref_##CLASSNAME = get_singleton_of_##CLASSNAME(); }

#define MAKE_SINGLETON_CLASS(CLASSNAME, CLASSBODY) class CLASSNAME CLASSBODY; CLASSNAME& get_singleton_of_##CLASSNAME(){ static CLASSNAME obj; return obj; }
#define MAKE_SINGLETON_CLASS_C(CLASSNAME, CONSTRUCTOR) MAKE_SINGLETON_CLASS(CLASSNAME, { public: CLASSNAME() { CONSTRUCTOR; } })
#define MAKE_SINGLETON_CLASS_INIT(CLASSNAME, CLASSBODY) MAKE_SINGLETON_CLASS(CLASSNAME, CLASSBODY) namespace ______dummy_namespace_for_##CLASSNAME{  const auto& ______dummy_ref_##CLASSNAME = get_singleton_of_##CLASSNAME(); }
#define MAKE_SINGLETON_CLASS_INIT_C(CLASSNAME, CONSTRUCTOR) MAKE_SINGLETON_CLASS_C(CLASSNAME, CONSTRUCTOR) namespace ______dummy_namespace_for_##CLASSNAME{  const auto& ______dummy_ref_##CLASSNAME = get_singleton_of_##CLASSNAME(); }

#define RUN_ONLY_ONCE(FUNCTIONNAME, ...) MAKE_SINGLETON_CLASS_INIT_C(RUNNABLE_##FUNCTIONNAME, FUNCTIONNAME(__VA_ARGS__), FUNCFLAGS );
#define RUN_ONLY_ONCE_FLAGGED(FUNCTIONNAME, FUNCFLAGS, ...) MAKE_SINGLETON_CLASS_INIT_C(RUNNABLE_##FUNCTIONNAME, FUNCTIONNAME(__VA_ARGS__), FUNCFLAGS );
#define RUN_ASYNC_ON_CORE_AUTO(CLASSNAME, THREADNAME, LOOPFUNC, COREID, PRIORITY) MAKE_SINGLETON_CLASS_INIT(ASYNC_##THREADNAME,  { CLASSNAME obj; public: ASYNC_##THREADNAME() { actcpba( while(1) { ((CLASSNAME*)arg)->LOOPFUNC(); yield(); }, COREID, PRIORITY, (void*)&obj); } const CLASSNAME& get_internal_variable() const { return obj; } CLASSNAME& get_internal_variable() { return obj; } }   )

#include "logger.h"
