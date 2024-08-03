#pragma once

#include "defaults.h"
#include "LOG_ctl.h"
#include "SD_card.h"

MAKE_SINGLETON_CLASS(MySerialReader, {
    void async_serial_reader();
public:
    MySerialReader();
});

#include "Serial_input_handler.ipp"