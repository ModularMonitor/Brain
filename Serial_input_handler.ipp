#pragma once

#include "Serial_input_handler.h"

inline void MySerialReader::async_serial_reader()
{
    LOGI_NOSD(e_LOG_TAG::TAG_STDIN, "Command input handler started. Use /help on terminal for help from now on.");

    char buffer[serialstdin_buffer_size], aux_buffer[serialstdin_buffer_size];
    auto& sd = GET(MySDcard);

    while(1) {
        vTaskPrioritySet(NULL, tskIDLE_PRIORITY);

        while (!Serial.available()) SLEEP(250);

        memset(buffer, '\0', serialstdin_buffer_size);
        uint16_t off = 0;

        vTaskPrioritySet(NULL, serialstdin_thread_priority);

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
        constexpr char cmd_reloadpages[] = "reloadweb";

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
            LOGI_NOSD(e_LOG_TAG::TAG_STDIN, "- reloadweb: Reload the web pages from SD card to RAM.");

        }
        else if (strncmp(cmd_reloadpages, buffer + off, sizeof(cmd_reloadpages) - 1) == 0) { // RELOAD WEB

            LOGI_NOSD(e_LOG_TAG::TAG_STDIN, "Reloading pages...");

            reload_webserver_items();

            LOGI_NOSD(e_LOG_TAG::TAG_STDIN, "Reloaded pages, sizes: %u, %u, %u",
                _get_webserver_idx(0).length(),
                _get_webserver_idx(1).length(),
                _get_webserver_idx(2).length()
            );

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
            if (aux_buffer[0] != '/') {
                LOGW_NOSD(e_LOG_TAG::TAG_STDIN, "Mistyped path, please start with '/' like '/potato/test.txt'");
                continue;
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
            if (aux_buffer[0] != '/') {
                LOGW_NOSD(e_LOG_TAG::TAG_STDIN, "Mistyped path, please start with '/' like '/potato/test.txt'");
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
            if (aux_buffer[0] != '/') {
                LOGW_NOSD(e_LOG_TAG::TAG_STDIN, "Mistyped path, please start with '/' like '/potato/test.txt'");
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
            if (aux_buffer[0] != '/') {
                LOGW_NOSD(e_LOG_TAG::TAG_STDIN, "Mistyped path, please start with '/' like '/potato/test.txt'");
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
            if (aux_buffer[0] != '/') {
                LOGW_NOSD(e_LOG_TAG::TAG_STDIN, "Mistyped path, please start with '/' like '/potato/test.txt'");
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

            const int res = sscanf(buffer + off, "%s", aux_buffer);

            if (res <= 0) {
                LOGW_NOSD(e_LOG_TAG::TAG_STDIN, "Input for write file failed to be parsed. Please check help.");
                continue;
            }
            if (aux_buffer[0] != '/') {
                LOGW_NOSD(e_LOG_TAG::TAG_STDIN, "Mistyped path, please start with '/' like '/potato/test.txt'");
                continue;
            }

            LOGI_NOSD(e_LOG_TAG::TAG_STDIN, "Please paste raw data on the terminal.");
            LOGI_NOSD(e_LOG_TAG::TAG_STDIN, "On a %u milliseconds timeout after the last character read the file will be saved and closed.", web_timeout_write);
            LOGI_NOSD(e_LOG_TAG::TAG_STDIN, "Everything is buffered before parsing and writing!");

            while(!Serial.available()) SLEEP(15);

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
                    for (self_refd* self = this; self != nullptr; self = self->m_next) {
                        for(size_t p = from_point; p < self->m_buf_len - 1; p++) {
                            self->m_buf[p] = self->m_buf[p+1];
                        }
                        if (self->m_next) {
                            if (self->m_next->m_buf_len > 0) {
                                self->m_buf[m_buf_len-1] = self->m_next->m_buf[0];
                                from_point = 0;
                                continue;
                            }
                            else {
                                delete self->m_next;
                                self->m_next = nullptr;
                                --self->m_buf_len;                                
                            }
                        }
                        else --self->m_buf_len;
                    }
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
                    self_refd* self = this;
                    size_t c = 0;

                    while(1) {
                        c += self->m_buf_len = Serial.readBytes((uint8_t*)self->m_buf, serialstdin_readblock_buffer_size);

                        if (self->m_buf_len > 0 && _auto_wait()) {
                            self->m_next = new self_refd();
                            self = self->m_next;
                        }
                        else break;
                    }
                    
                    return c;
                }

                // After read, you can work on \code codes
                void auto_fix_backslashes()
                {
                    for (self_refd* self = this; self != nullptr; self = self->m_next) {
                        for(size_t p = 0; p < self->m_buf_len; ++p) {
                            if (self->m_buf[p] == '\\') {
                                const int test = 
                                    (p + 1 >= self->m_buf_len) ?
                                        (self->m_next && self->m_next->m_buf_len > 0 ? self->m_next->m_buf[0] : -1) :
                                        self->m_buf[p+1];
                                switch(test) {
                                //case 'a':  _move_left_all_reduce_recursive(p); m_buf[p] = '\a'; break;
                                //case 'b':  _move_left_all_reduce_recursive(p); m_buf[p] = '\b'; break;
                                //case 'e':  _move_left_all_reduce_recursive(p); m_buf[p] = '\e'; break;
                                //case 'f':  _move_left_all_reduce_recursive(p); m_buf[p] = '\f'; break;
                                case 'n':  self->_move_left_all_reduce_recursive(p); self->m_buf[p] = '\n'; break;
                                case 'r':  self->_move_left_all_reduce_recursive(p); self->m_buf[p] = '\r'; break;
                                case 't':  self->_move_left_all_reduce_recursive(p); self->m_buf[p] = '\t'; break;
                                //case 'v':  _move_left_all_reduce_recursive(p); m_buf[p] = '\v'; break;
                                //case '\\': _move_left_all_reduce_recursive(p); m_buf[p] = '\\'; break;
                                //case '\'': _move_left_all_reduce_recursive(p); m_buf[p] = '\''; break;
                                //case '\"': _move_left_all_reduce_recursive(p); m_buf[p] = '\"'; break;
                                //case '\?': _move_left_all_reduce_recursive(p); m_buf[p] = '\?'; break;
                                default: break;
                                }
                            }
                        }
                        //if (m_next) m_next->auto_fix_backslashes();
                        //else if (m_buf[m_buf_len - 1] == '\n') --m_buf_len; // last breakline
                    }
                }
            public:

                // write on file
                size_t flush_to(const char* path)
                {
                    if (!path) return 0;

                    auto& sd = GET(MySDcard);
                    size_t so_far = 0;
                    size_t expected = calc_read();
                    if (expected == 0) expected = 1;
                    
                    LOGI_NOSD(e_LOG_TAG::TAG_STDIN, "Flushing data to '%s', writing %zu bytes...", path, m_buf_len);

                    for (self_refd* self = this; self != nullptr; self = self->m_next) {
                        const bool first = (self == this);
                        for (size_t rt = 0; rt < self->m_buf_len;){

                            size_t nw = 0;
                            if (first && rt == 0) {
                                nw = sd.overwrite_on(path, self->m_buf + rt, self->m_buf_len - rt);
                            }
                            else {
                                nw = sd.append_on(path, self->m_buf + rt, self->m_buf_len - rt);
                            }

                            if (nw == 0) {
                                LOGW_NOSD(e_LOG_TAG::TAG_STDIN, "Cannot write. Trying again soon...");
                                SLEEP(1000);
                            }
                            else {
                                rt += nw;
                                LOGI_NOSD(e_LOG_TAG::TAG_STDIN, "Writing... %zu bytes, progress: %.2f%%", so_far + rt, (100.0f * (so_far + rt) / expected));
                            }
                        }

                        so_far += self->m_buf_len;

                        SLEEP(50);
                    }

                    LOGI_NOSD(e_LOG_TAG::TAG_STDIN, "Ended writing on '%s'", path);
                    
                    return so_far;
                }

                // use this to create, fill, and fix automatically
                static self_refd* auto_make_self() {
                    self_refd* root = new self_refd();
                    if (root->fill_buffer() == 0) {
                        delete root;
                        return nullptr;
                    }
                    root->auto_fix_backslashes();
                    return root;
                }

                size_t calc_read() const {
                    size_t c = 0;

                    for (const self_refd* self = this; self != nullptr; self = self->m_next) c += self->m_buf_len;
                    
                    return c;
                }
            };
            
            LOGI_NOSD(e_LOG_TAG::TAG_STDIN, "Reading of stdin buffer has begun.");

            auto hnd = std::unique_ptr<self_refd>(self_refd::auto_make_self());
            
            LOGI_NOSD(e_LOG_TAG::TAG_STDIN, "Read %zu, writing to file '%s'...", hnd->calc_read(), aux_buffer);

            const auto total_bytes = hnd->flush_to(aux_buffer);

            LOGI_NOSD(e_LOG_TAG::TAG_STDIN, "Ended writing to '%s'. Total bytes written: %zu.", aux_buffer, total_bytes);
            LOGI_NOSD(e_LOG_TAG::TAG_STDIN, "Confirmed file size now: %zu", sd.get_file_size(aux_buffer));
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
    async_class_method_pri(MySerialReader, async_serial_reader, tskIDLE_PRIORITY, cpu_core_id_for_serialstdin);
}