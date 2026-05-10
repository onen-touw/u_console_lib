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

#include "u_configurator/u_configurator.h"
#include "u_drivers/u_i2c.h"

namespace ufo
{
    
    static bool confifura_ads(const u_configure::configuration_config_list_t& list)
    {
        u_i2c_cfg_t cfg;
        
        int32_t val = 0;

        // if (list.bind(&val, "d-i2ch-sda"))
        // {
        //     cfg.i2c_sda = static_cast<gpio_num_t>(val);
        // }
        
        // if (list.bind(&val, "d-i2ch-sdl"))
        // {
        //     cfg.i2c_sda = static_cast<gpio_num_t>(val);
        // }

        if (list.bind(&val, "d-i2c-freq"))
        {
            if (val != 100000 || val != 400000)
            {
                return false;
            }
            cfg.frequency = val;
        }
        
        return true;
    } 

} // namespace ufo

