#pragma once

#if \
    __has_include("u_drivers/u_spiffs.h") && \
    __has_include("u_cns/u_cns.h") && \
    __has_include("uffs/udir.h")

#include "u_cns/u_cns.h"
#include "u_drivers/u_spiffs.h"
#include "uffs/udir.h"

namespace ufo
{
    static void spiffs_driver_console(const ufo::u_console_msg_block_t& block)
    {
        const auto& opt_list = block.get_opt_list();

        std::string mode;
        if (opt_list.bind(&mode, "i,info"))
        {
            ufo::u_spiffs_t dev;
            ufo::u_spiffs_cfg_t cfg = ufo::get_spiffs_driver_cfg();
            ufo::u_spiffs_meta_t meta =  ufo::get_spiffs_driver_meta();

            block << "spiffs driver info:\n";
            block << "\ncfg:\n\tmount point: " << cfg.base_path;
            if (cfg.partition_label)
            {
                block << "\n\tpartition name: " <<cfg.partition_label;
            }
            block << "\n\topen-file limit: " <<cfg.max_files;
            block << "\n\tformat on fail: " << cfg.format_if_mount_failed;
            block << "\nmeta:\n";
            block << "\tstate: " << meta.state.get(); 
            block << "\n\terror cnt: " << meta.error_count; 
            block << "\n\terror code: " << meta.error_code; 
            block << "\n\tuser cnt: " << meta.user_count;
            block << '\n';

            {
                block << "Run check fs...\n";
                bool res = dev.check_fs();
                block << '\t' << (res ? "passed" : "not passed") << '\n';

                if (!res)
                {
                    return;
                }
                block << "Memory info\n";
                auto info = dev.get_fs_info();
                block << "\ttotal: " << info.total << " b\n\tused:  " << info.used << " b\n\tfree:  " << (info.total - info.used) << " b\n";
            }
            {
                block << "spiffs file list:\n";
                ufo::udir_t mdr(cfg.base_path);
                auto list = mdr.list_files();
                for (const auto &i : list)
                {
                    block << '\t' << i.c_str() << '\n';
                }
                block << '\n';
            }
            
            return;
        }
        else if (opt_list.find("h,help"))
        {
            block << "Usage: ";
            block << opt_list.get_prog_name() << " [--<option(s)>]\n";
            block << "\t-h/--h/--help   - this text \n"; 
            block << "\t-i/--i/--info   - trace fs info\n\n"; 
        }
    }
} // namespace ufo
#else
#error "The spiffs driver requires libraries u_cns, uffs and u_drivers/u_spiffs. Check your include paths." 
#endif