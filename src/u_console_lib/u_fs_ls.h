#pragma once

#if \
    __has_include("u_cns/u_cns.h") && \
    __has_include("uffs/udir.h")

#include "u_cns/u_cns.h"
#include "uffs/udir.h"
#include "u_drivers/u_spiffs.h"

namespace ufo
{
    [[maybe_unused]] static void u_fs_ls(const ufo::u_console_msg_block_t& block)
    {
        ufo::u_spiffs_cfg_t cfg = ufo::get_spiffs_driver_cfg();

        ufo::udir_t mdr(cfg.base_path);
        auto list = mdr.list_files();
        for (const auto &i : list)
        {
            block << '\t' << i.c_str() << '\n';
        }
        block << '\n';
    }
}

#else
#error "The u_fs_cat requires libraries u_cns and uffs. Check your include paths." 
#endif