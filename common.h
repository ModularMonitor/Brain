#pragma once

// DELETE X if X != nullptr then X = nullptr;
#define DEL_IT(X) { if (X) delete X; X = nullptr; }
// DELETE ARRAY (X) if X != nullptr then X = nullptr;
#define DEL_EM(X) { if (X) delete[] X; X = nullptr; }
// TARG << FROM, FROM << FROM_VAL
#define EXCHANGE(TARG, FROM, FROM_VAL) { TARG = FROM; FROM = FROM_VAL; }
// TARG << FROM, FROM << NULL
#define EXC_NULL(TARG, FROM) EXCHANGE(TARG, FROM, nullptr)

#define TABLE_FLIP_CHIP_I_AM_DEAD() esp_restart()

// allocates with new[], should dealloc with delete[]
#define saprintf(PPTR, SIZETPTR_LEN, FORMAT, ...) { int ____l = snprintf(nullptr, 0, FORMAT, __VA_ARGS__); PPTR = new char[____l + 1]; (*SIZETPTR_LEN) = static_cast<size_t>(____l); ____l = snprintf(PPTR, ____l + 1, FORMAT, __VA_ARGS__); if (____l < 0) { delete[] PPTR; PPTR = nullptr; (*SIZETPTR_LEN) = 0; }}

constexpr size_t cpu_stack_default = 6144;

constexpr int def_spi_core_id = 1; // spi (sd card, tft screen, touch)
constexpr int def_alt_core_id = 0; // other tasks

constexpr int cpu_core_id_for_ctl = def_alt_core_id;
constexpr int cpu_core_id_for_display = def_spi_core_id;

#define INITIALIZE_CLASS_SINGLETON(CLASSNAME, FUNCFLAGS) CLASSNAME& FUNCFLAGS get_singleton_of_##CLASSNAME(){ static CLASSNAME obj; return obj; } namespace ______dummy_namespace_for_##CLASSNAME{  const auto& ______dummy_ref_##CLASSNAME = get_singleton_of_##CLASSNAME(); } // init
#define INITIALIZE_CLASS_SINGLETON_S(CLASSNAME) CLASSNAME& get_singleton_of_##CLASSNAME(){ static CLASSNAME obj; return obj; } namespace ______dummy_namespace_for_##CLASSNAME{  const auto& ______dummy_ref_##CLASSNAME = get_singleton_of_##CLASSNAME(); } // init
#define INITIALIZE_ONCE_FUNCTION(NAME, BODY, FUNCFLAGS) class NAME { public: NAME() { BODY; } }; INITIALIZE_CLASS_SINGLETON(NAME, FUNCFLAGS)
#define INITIALIZE_ONCE_FUNCTION_S(NAME, BODY) class NAME { public: NAME() { BODY; } }; INITIALIZE_CLASS_SINGLETON_S(NAME)
#define INITIALIZE_ASYNC_CLASS(CLASSNAME, COREID, PRIORITY) INITIALIZE_ONCE_FUNCTION_S(INIT_##CLASSNAME, actacpb( CLASSNAME obj; while(1) { obj.task(); yield(); }, COREID, PRIORITY););