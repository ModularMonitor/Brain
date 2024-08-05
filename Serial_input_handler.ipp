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
        constexpr char cmd_mkdir[] = "mkdir";
        constexpr char cmd_rmdir[] = "rmdir";
        constexpr char cmd_rm[] = "rm";
        constexpr char cmd_rf[] = "read";
        constexpr char cmd_wf[] = "write";
        constexpr char cmd_sd[] = "sd";
        constexpr char cmd_builddate[] = "build";

        //const auto check_current_arg_is_num = [&]{
        //    if (off >= serialstdin_buffer_size) return false;
        //    return (buffer[off] >= '0' && buffer[off] <= '9');
        //};

        if (strncmp(cmd_help, buffer + off, sizeof(cmd_help) - 1) == 0) { // HELP!

            LOGI_NOSD(e_LOG_TAG::TAG_STDIN, "Help! List of commands available:");
            LOGI_NOSD(e_LOG_TAG::TAG_STDIN, "- help: shows this list");
            LOGI_NOSD(e_LOG_TAG::TAG_STDIN, "- read [file] <max_characters> <offset_begin> <is_offset_from_end: 'T' or (others => false)>: prints the file here");
            LOGI_NOSD(e_LOG_TAG::TAG_STDIN, "- write [file]: begin file write session on that path");
            LOGI_NOSD(e_LOG_TAG::TAG_STDIN, "- ls: list files in folder (try '/' for root)");
            LOGI_NOSD(e_LOG_TAG::TAG_STDIN, "- mkdir [directory]: create a directory (like '/path')");
            LOGI_NOSD(e_LOG_TAG::TAG_STDIN, "- rmdir [directory]: delete a directory (like '/path')");
            LOGI_NOSD(e_LOG_TAG::TAG_STDIN, "- rm [file]: delete a file (like '/path/file.txt')");
            LOGI_NOSD(e_LOG_TAG::TAG_STDIN, "- sd: Tells SD card info");
            LOGI_NOSD(e_LOG_TAG::TAG_STDIN, "- build: Tells build date time");

        }
        else if (strncmp(cmd_builddate, buffer + off, sizeof(cmd_builddate) - 1) == 0) { // BUILD DATE!

            LOGI_NOSD(e_LOG_TAG::TAG_STDIN, "Build date: %s %s (localtime of build)", __DATE__, __TIME__ );

        }
        else if (strncmp(cmd_ls, buffer + off, sizeof(cmd_ls) - 1) == 0) { // LS / READ FOLDER!
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
        else if (strncmp(cmd_mkdir, buffer + off, sizeof(cmd_mkdir) - 1) == 0) { // MAKE FOLDER!
            off += sizeof(cmd_mkdir);

            const int res = sscanf(buffer + off, "%s", aux_buffer);

            if (res <= 0) {
                LOGW_NOSD(e_LOG_TAG::TAG_STDIN, "Input for mkdir failed to be parsed. Please check help.");
                continue;
            }

            if (sd.make_dir(aux_buffer)) LOGI_NOSD(e_LOG_TAG::TAG_STDIN, "Created folder '%s'", aux_buffer);
            else                         LOGE_NOSD(e_LOG_TAG::TAG_STDIN, "Failed to create folder '%s'", aux_buffer);
        }
        else if (strncmp(cmd_rmdir, buffer + off, sizeof(cmd_rmdir) - 1) == 0) { // REMOVE FOLDER!
            off += sizeof(cmd_rmdir);

            const int res = sscanf(buffer + off, "%s", aux_buffer);

            if (res <= 0) {
                LOGW_NOSD(e_LOG_TAG::TAG_STDIN, "Input for rmdir failed to be parsed. Please check help.");
                continue;
            }

            if (sd.remove_dir(aux_buffer)) LOGI_NOSD(e_LOG_TAG::TAG_STDIN, "Deleted folder '%s'", aux_buffer);
            else                           LOGE_NOSD(e_LOG_TAG::TAG_STDIN, "Failed to delete folder '%s'", aux_buffer);
        }
        else if (strncmp(cmd_rm, buffer + off, sizeof(cmd_rm) - 1) == 0) { // REMOVE FILE!
            off += sizeof(cmd_rm);

            const int res = sscanf(buffer + off, "%s", aux_buffer);

            if (res <= 0) {
                LOGW_NOSD(e_LOG_TAG::TAG_STDIN, "Input for rm failed to be parsed. Please check help.");
                continue;
            }

            if (sd.remove_file(aux_buffer)) LOGI_NOSD(e_LOG_TAG::TAG_STDIN, "Deleted file '%s'", aux_buffer);
            else                            LOGE_NOSD(e_LOG_TAG::TAG_STDIN, "Failed to delete file '%s'", aux_buffer);
        }
        else if (strncmp(cmd_rf, buffer + off, sizeof(cmd_rf) - 1) == 0) { // READ FILE!
            off += sizeof(cmd_rf);

            unsigned max_characters = static_cast<unsigned>(-1);
            unsigned offset_begin = 0;
            char offset_inverted = 'f';

            const int res = sscanf(buffer + off, "%s %u %u %c", aux_buffer, &max_characters, &offset_begin, &offset_inverted);

            if (res <= 0) {
                LOGW_NOSD(e_LOG_TAG::TAG_STDIN, "Input for read file failed to be parsed. Please check help.");
                continue;
            }

            if (offset_inverted == 'T' || offset_inverted == 't') {
                const size_t file_size = sd.get_file_size(aux_buffer);
                if (file_size > offset_begin) offset_begin = static_cast<unsigned>(file_size) - offset_begin;
                else offset_begin = 0;
            }

            for(unsigned p = 0; p < max_characters;) {
                const size_t to_read = 
                    (max_characters - p) > (serialstdin_buffer_size - 1) ?
                        (serialstdin_buffer_size - 1) : (max_characters - p);
                
                memset(buffer, '\0', serialstdin_buffer_size);
                const size_t gread = sd.read_from(aux_buffer, buffer, to_read, offset_begin + p);

                if (gread > 0) LOGRAW(
                    "############### READ BLOCK OF '%s' [%u..%u]: ###############\n"
                    "%.*s\n"
                    "############################################################\n",
                    aux_buffer,
                    p + offset_begin,
                    p + offset_begin + (gread - 1),
                    gread, buffer
                );



                p += gread;

                if (gread == 0) {
                    LOGW_NOSD(e_LOG_TAG::TAG_STDIN, "Stopped reading of file '%s' at %u, probably end of file or file is not available anymore.", aux_buffer, p);
                    break;
                }
            }
        }
        else if (strncmp(cmd_wf, buffer + off, sizeof(cmd_wf) - 1) == 0) { // WRITE FILE!
            off += sizeof(cmd_wf);

            LOGW_NOSD(e_LOG_TAG::TAG_STDIN, "Not yet implemented.");

            const int res = sscanf(buffer + off, "%s", aux_buffer);

            if (res <= 0) {
                LOGW_NOSD(e_LOG_TAG::TAG_STDIN, "Input for write file failed to be parsed. Please check help.");
                continue;
            }

            LOGI_NOSD(e_LOG_TAG::TAG_STDIN, 
                "Please paste raw data on the terminal. On a %u milliseconds timeout after the "
                "last character read the file will be saved and closed. Everything is buffered before parsing and writing!", web_timeout_write
            );

            while(!Serial.available()) SLEEP(15);

//            struct _auto_data_handler {
//                char m_buffer[serialstdin_buffer_size]{};
//                size_t m_read = 0, m_tail = 0;
//                
//                void read() { m_read = Serial.readBytes((uint8_t*)m_buffer, serialstdin_buffer_size); }
//                int _raw_next() {
//                    if (m_tail == m_read) {
//                        read();
//                        m_tail = 0;
//                        if (m_read == 0) return -1; // nothing to read
//                    }
//                    return static_cast<int>(m_buffer[m_read++]);
//                }
//                int get_next(bool& backslash_was_there_but_not_recognized) {
//                    backslash_was_there_but_not_recognized = false;
//                    int ch = _raw_next();
//                    switch(ch) {
//                    case -1: return -1; // bad
//                    case '\\': // assume escape simple sequence
//                        switch(ch = _raw_next()) {
//                        case 'a':   return '\a';
//                        case 'b':   return '\b';
//                        case 'e':   return '\e';
//                        case 'f':   return '\f';
//                        case 'n':   return '\n';
//                        case 'r':   return '\r';
//                        case 't':   return '\t';
//                        case 'v':   return '\v';
//                        case '\\':  return '\\';
//                        case '\'':  return '\'';
//                        case '\"':  return '\"';
//                        case '\?':  return '\?';
//                        default: 
//                            backslash_was_there_but_not_recognized = true;
//                            return ch;
//                        }
//                    default:
//                        return ch;
//                    }
//                }
//            } hnd;
//
//            bool first = true, add_slash_before = false;
//            size_t total_bytes = 0;
//            off = 0;
//
//            const auto autowrite_flush = [&] {
//                if (off == 0) return;
//
//                if (first) sd.overwrite_on(aux_buffer, buffer, off);
//                else       sd.append_on(aux_buffer, buffer, off);
//
//                total_bytes += static_cast<size_t>(off);
//                off = 0;
//                first = false;
//            };
//            const auto autoput = [&] (const char ch) {
//                buffer[off++] = ch;
//                if (off >= serialstdin_buffer_size)
//                    autowrite_flush();
//            };
//
//
//            while(Serial.available()) {
//                const int ch = hnd.get_next(add_slash_before);
//
//                if (ch >= 0)
//                {
//                    if (add_slash_before) autoput('\\');
//                    autoput(ch);
//                }
//
//                if (Serial.available()) continue;
//
//                for (const auto rn = get_time_ms(); !Serial.available();) {
//                    if (get_time_ms() - rn > 250) SLEEP(15);
//                    if (get_time_ms() - rn > web_timeout_write) break; // the other while will cancel itself
//                }
//            }
//            autowrite_flush();


            class self_refd {
                char m_buf[serialstdin_readblock_buffer_size];
                size_t m_buf_len = 0;
                self_refd* m_next = nullptr;
            public:
                self_refd() = default;
                self_refd(const self_refd&) = delete;
                self_refd(self_refd&&) = delete;
                void operator=(const self_refd&) = delete;
                void operator=(self_refd&&) = delete;

                ~self_refd() {
                    if (m_next) delete m_next;
                }
            private:
                // utility for internal use
                void _move_left_all_reduce_recursive(size_t from_point = 0) {
                    for(size_t p = from_point; p < m_buf_len - 1; p++) {
                        m_buf[p] = m_buf[p+1];
                    }
                    if (m_next && m_next->m_buf_len > 0) {
                        m_buf[m_buf_len-1] = m_next->m_buf[0];
                        m_next->_move_left_all_reduce_recursive(0); // yes, from 0
                    }
                    else --m_buf_len;
                }

                // utility to auto wait for Serial stuff
                bool _auto_wait() {
                    if (Serial.available()) return true;
                    for (const auto rn = get_time_ms(); !Serial.available();) {
                        if (get_time_ms() - rn > 250) SLEEP(15);
                        if (get_time_ms() - rn > web_timeout_write) return false;
                    }
                    return true;
                }

                // read all from Serial automatically
                size_t fill_buffer() {
                    m_buf_len = Serial.readBytes((uint8_t*)m_buf, serialstdin_readblock_buffer_size);
                    if (m_buf_len > 0 && _auto_wait()) {
                        m_next = new self_refd();                        
                        return m_buf_len + m_next->fill_buffer();
                    }
                    return m_buf_len;
                }

                // After read, you can work on \code codes
                void auto_fix_backslashes_recursive()
                {
                    for(size_t p = 0; p < m_buf_len; ++p) {
                        if (m_buf[p] == '\\') {
                            const int test = 
                                (p + 1 == m_buf_len) ?
                                    (m_next && m_next->m_buf_len > 0 ? m_next->m_buf[0] : -1) :
                                    m_buf[p+1];
                            switch(test) {
                            case 'a':  m_buf[p] = '\a'; _move_left_all_reduce_recursive(p + 1); break;
                            case 'b':  m_buf[p] = '\b'; _move_left_all_reduce_recursive(p + 1); break;
                            case 'e':  m_buf[p] = '\e'; _move_left_all_reduce_recursive(p + 1); break;
                            case 'f':  m_buf[p] = '\f'; _move_left_all_reduce_recursive(p + 1); break;
                            case 'n':  m_buf[p] = '\n'; _move_left_all_reduce_recursive(p + 1); break;
                            case 'r':  m_buf[p] = '\r'; _move_left_all_reduce_recursive(p + 1); break;
                            case 't':  m_buf[p] = '\t'; _move_left_all_reduce_recursive(p + 1); break;
                            case 'v':  m_buf[p] = '\v'; _move_left_all_reduce_recursive(p + 1); break;
                            case '\\': m_buf[p] = '\\'; _move_left_all_reduce_recursive(p + 1); break;
                            case '\'': m_buf[p] = '\''; _move_left_all_reduce_recursive(p + 1); break;
                            case '\"': m_buf[p] = '\"'; _move_left_all_reduce_recursive(p + 1); break;
                            case '\?': m_buf[p] = '\?'; _move_left_all_reduce_recursive(p + 1); break;
                            default: break;
                            }
                        }
                    }
                    if (m_next) m_next->auto_fix_backslashes_recursive();
                    else if (m_buf[m_buf_len - 1] == '\n') --m_buf_len; // last breakline
                }
            public:

                // write on file
                size_t flush_to(const char* path, const bool first = true)
                {
                    if (!path) return 0;

                    auto& sd = GET(MySDcard);

                    if (first) sd.overwrite_on(path, m_buf, m_buf_len);
                    else       sd.append_on(path, m_buf, m_buf_len);
                    
                    if (m_next) return m_buf_len + m_next->flush_to(path, false);
                    return m_buf_len;
                }

                // use this to create, fill, and fix automatically
                static self_refd* auto_make_self() {
                    self_refd* root = new self_refd();
                    if (root->fill_buffer() == 0) {
                        delete root;
                        return nullptr;
                    }
                    root->auto_fix_backslashes_recursive();
                    return root;
                }
            };
            
            LOGI_NOSD(e_LOG_TAG::TAG_STDIN, "Reading stdin buffer has begun.");

            auto hnd = std::unique_ptr<self_refd>(self_refd::auto_make_self());
            
            LOGI_NOSD(e_LOG_TAG::TAG_STDIN, "Writing to file '%s'...", aux_buffer);

            const auto total_bytes = hnd->flush_to(aux_buffer);

            LOGI_NOSD(e_LOG_TAG::TAG_STDIN, "Ended writing to '%s'. Total bytes written: %zu.", aux_buffer, total_bytes);
        }
        else if (strncmp(cmd_sd, buffer + off, sizeof(cmd_sd) - 1) == 0) { // SD INFO!

            const size_t
                card_size = sd.sd_card_size(),
                max_bytes = sd.sd_max_bytes(),
                used_bytes = sd.sd_used_bytes(),
                tasks_size = sd.tasks_size();
            
            const float
                used_perc = sd.sd_used_perc();

            LOGI_NOSD(e_LOG_TAG::TAG_STDIN, "SD card information:");
            LOGI_NOSD(e_LOG_TAG::TAG_STDIN, "- Card size [bytes]: %zu", card_size);
            LOGI_NOSD(e_LOG_TAG::TAG_STDIN, "- Card max bytes [bytes]: %zu", max_bytes);
            LOGI_NOSD(e_LOG_TAG::TAG_STDIN, "- Card used bytes [bytes]: %zu", used_bytes);
            LOGI_NOSD(e_LOG_TAG::TAG_STDIN, "- Card use in float format: %.2f%%", 100.0f * used_perc);
            LOGI_NOSD(e_LOG_TAG::TAG_STDIN, "- Card system tasks in queue: %zu", tasks_size);

            switch(sd.sd_type()) {
            case SD_type::C_MMC:
                LOGI_NOSD(e_LOG_TAG::TAG_STDIN, "- Card type: MMC");
                break;
            case SD_type::C_SD:
                LOGI_NOSD(e_LOG_TAG::TAG_STDIN, "- Card type: SD");
                break;
            case SD_type::C_SDHC:
                LOGI_NOSD(e_LOG_TAG::TAG_STDIN, "- Card type: SD HC");
                break;
            default:
                LOGI_NOSD(e_LOG_TAG::TAG_STDIN, "- Card type: offline or unknown");
                break;
            }

        }
    }
}

inline MySerialReader::MySerialReader()
{
    async_class_method_pri(MySerialReader, async_serial_reader, serialstdin_thread_priority, cpu_core_id_for_serialstdin);

}