#pragma once

#include "Serial_input_handler.h"

inline void MySerialReader::async_serial_reader()
{
    LOGI_NOSD(e_LOG_TAG::TAG_STDIN, "Command input handler started. Use /help on terminal for help from now on.");

    char buffer[serialstdin_buffer_size], aux_buffer[serialstdin_buffer_size];
    auto& sd = GET(MySDcard);

    while(1) {
        while (!Serial.available()) SLEEP(100);

        memset(buffer, '\0', serialstdin_buffer_size);
        uint16_t off = 0;

        while(Serial.available()) {
            buffer[off++] = Serial.read();
            if (off >= (serialstdin_buffer_size - 1)) {
                LOGW_NOSD(e_LOG_TAG::TAG_STDIN, "Command too big, truncated at %u.", static_cast<unsigned>(serialstdin_buffer_size - 1));
                while(Serial.available()) { Serial.read(); SLEEP(3); }
                break;
            }
            if (!Serial.available()) SLEEP(3);
        }
        if (off > 0 && buffer[off-1] == '\n') buffer[off-1] = '\0';

        LOGI_NOSD(e_LOG_TAG::TAG_STDIN, "Parsing command: '%s'", buffer);

        off = 0;

        constexpr char cmd_help[] = "help";
        constexpr char cmd_ls[] = "ls";
        constexpr char cmd_wf[] = "writefile";
        constexpr char cmd_rf[] = "readfile";

        //const auto check_current_arg_is_num = [&]{
        //    if (off >= serialstdin_buffer_size) return false;
        //    return (buffer[off] >= '0' && buffer[off] <= '9');
        //};

        if (strncmp(cmd_help, buffer + off, sizeof(cmd_help) - 1) == 0) { // HELP!

            LOGI_NOSD(e_LOG_TAG::TAG_STDIN, "Help! List of commands available:");
            LOGI_NOSD(e_LOG_TAG::TAG_STDIN, "- help: shows this list");
            LOGI_NOSD(e_LOG_TAG::TAG_STDIN, "- readfile [file] <max_characters> <offset_begin>: prints the file here");
            LOGI_NOSD(e_LOG_TAG::TAG_STDIN, "- writefile [file]: begin file write session on that path");
            LOGI_NOSD(e_LOG_TAG::TAG_STDIN, "- ls: list files in folder (try \"/\" for root)");

        }
        else if (strncmp(cmd_ls, buffer + off, sizeof(cmd_ls) - 1) == 0) { // READ FOLDER!
            off += sizeof(cmd_ls);

            const int res = sscanf(buffer + off, "%s", aux_buffer);

            if (res <= 0) {
                LOGW_NOSD(e_LOG_TAG::TAG_STDIN, "Input for ls/dir was empty. Using '/' instead.");
                snprintf(aux_buffer, serialstdin_buffer_size, "/");
                //continue;
            }

            LOGI_NOSD(e_LOG_TAG::TAG_STDIN, "Listing directories and files under '%s':", aux_buffer);

            const auto list = sd.ls(aux_buffer);

            for(const auto& i : list) 
                LOGI_NOSD(e_LOG_TAG::TAG_STDIN, "- %s %s %u", i.fp_name.c_str(), i.is_file ? "(file)" : "(directory)", i.fp_size);
            
        }
        else if (strncmp(cmd_rf, buffer + off, sizeof(cmd_rf) - 1) == 0) { // READ FILE!
            off += sizeof(cmd_rf);

            unsigned max_characters = static_cast<unsigned>(-1);
            unsigned offset_begin = 0;

            const int res = sscanf(buffer + off, "%s %u %u", aux_buffer, &max_characters, &offset_begin);

            if (res <= 0) {
                LOGW_NOSD(e_LOG_TAG::TAG_STDIN, "Input for read file failed to be parsed. Please check help.");
                continue;
            }

            for(unsigned p = 0; p < max_characters;) {
                const size_t to_read = 
                    (max_characters - p) > (serialstdin_buffer_size - 1) ?
                        (serialstdin_buffer_size - 1) : (max_characters - p);
                
                memset(buffer, '\0', serialstdin_buffer_size);
                const size_t gread = sd.read_from(aux_buffer, buffer, to_read, offset_begin + p);

                LOGI_NOSD(e_LOG_TAG::TAG_STDIN, "[#%u] %s", p + offset_begin, buffer);

                p += gread;

                if (gread == 0) break;
            }
        }
        else if (strncmp(cmd_wf, buffer + off, sizeof(cmd_wf) - 1) == 0) { // WRITE FILE!
            off += sizeof(cmd_wf);

            LOGW_NOSD(e_LOG_TAG::TAG_STDIN, "Not yet implemented.");

            //const int res = sscanf(buffer + off, "%s", aux_buffer);

            //if (res <= 0) {
            //    LOGW_NOSD(e_LOG_TAG::TAG_STDIN, "Input for write file failed to be parsed. Please check help.");
            //    continue;
            //}


        }


    }
}

inline MySerialReader::MySerialReader()
{
    async_class_method_pri(MySerialReader, async_serial_reader, serialstdin_thread_priority, cpu_core_id_for_serialstdin);

}