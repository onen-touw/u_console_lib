/*
* u_console_lib is © 2026, Anton Granitov (onen-touw), BSTU Voenmeh
*
* u_console_lib is published and distributed under 
* the Academic Software License v1.0 (ASL).
*
* u_console_lib is distributed in the hope that it will be useful 
* for non-commercial academic research, but WITHOUT ANY WARRANTY; without
* even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  
* See the ASL for more details.
*
* You should have received a copy of the ASL along with this program; 
* if not, write to anton.granitov123@gmail.com or https://github.com/onen-touw.  
* It is also published at LICENSE.md in root folder of this repository.
*
* You may contact the original licensor at anton.granitov123@gmail.com or https://github.com/onen-touw.
*/

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