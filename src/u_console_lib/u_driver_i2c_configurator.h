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

