#pragma once

#if \
    __has_include("u_cns/u_cns.h") && \
    __has_include("uffs/udir.h")

#include "u_cns/u_cns.h"
#include "uffs/udir.h"

namespace ufo
{
    static void u_fs_cat(const ufo::u_console_msg_block_t& block)
    {
        const auto& opt_list = block.get_opt_list();

        std::string fn;
        
        if (opt_list.bind(&fn,"f,file"))
        {
            ufo::ufile_t fl(fn, ufo::file_mode::read_rb);
            if (!fl.is_open())
            {
                block << "incorrect path to file or file does not exists\n";
                return;
            }
            while (fl.getline(fn))
            {
                block << fn << "\n";
            }
            block << "\n";
        }
    }
}

#else
#error "The u_fs_cat requires libraries u_cns and uffs. Check your include paths." 
#endif