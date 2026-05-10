#pragma once

#if \
    __has_include("u_drivers/u_i2c.h") && \
    __has_include("u_cns/u_cns.h")

#include "u_cns/u_cns.h"
#include "u_drivers/u_i2c.h"

namespace ufo
{
    static void i2c_driver_console(const ufo::u_console_msg_block_t& block)
    {
        const auto& opt_list = block.get_opt_list();

        std::string mode;

        if (opt_list.bind(&mode, "l,list"))
        {
            uint8_t adr_bf[16] = {};
            size_t cnt = 0;
            
#if U_I2C_DRIVERS_CNT > 1
            if (mode == "soft")
            {
                ufo::u_i2c_t drv(0, ufo::u_i2c_t::i2c_port_t::sofrware);
                cnt = drv.i2c_detecter(adr_bf, 16);
            }
            else
            {
                ufo::u_i2c_t drv(0);
                cnt = drv.i2c_detecter(adr_bf, 16);
            }
#else
            ufo::u_i2c_t drv(0);
            cnt = drv.i2c_detecter(adr_bf, 16);
#endif

            block << "i2c_detecter found " << cnt << " devices\n";
            auto rc = std::min(size_t(16), cnt);
            for (size_t i = 0; i < rc; i++)
            {
                block << "\tdev: " << adr_bf[i];
                block.fwrite("\t(0x%x)\n", adr_bf[i]);
            }
            block << '\n';
            return;
        }

        if (opt_list.bind(&mode, "i,info"))
        {
            ufo::u_i2c_cfg_t cfg;
            ufo::u_i2c_meta_t meta;

#if U_I2C_DRIVERS_CNT > 1
            if (mode == "soft")
            {
                cfg = ufo::get_i2c_driver_cfg(ufo::u_i2c_t::i2c_port_t::sofrware);
                meta = ufo::get_i2c_driver_meta(ufo::u_i2c_t::i2c_port_t::sofrware);
            }
            else
            {
                cfg = ufo::get_i2c_driver_cfg();
                meta = ufo::get_i2c_driver_meta();
                mode = "hard";
            }
#else
            if (mode == "soft")
            {
                block << "i2c soft mode disabled\n\n";
                return;
            }
            cfg = ufo::get_i2c_driver_cfg();
            meta = ufo::get_i2c_driver_meta();
            mode = "hard";
#endif

            block << "i2c driver info:\n";
            block << "mode: " << mode;
            block << "\ncfg:\n\tfreq: " << cfg.frequency;
            block << "\n\tsda/scl: " << cfg.i2c_sda << "/" << cfg.i2c_scl;
            block << "\nmeta:\n";
            block << "\tstate: " << meta.state.get(); 
            block << "\n\terror cnt: " << meta.error_count; 
            block << "\n\terror code: " << meta.error_code; 
            block << "\n\tuser cnt: " << meta.user_count;
            block << '\n';
            return;
        }
        if (opt_list.find("h,help"))
        {
            block << "Usage: ";
            block << opt_list.get_prog_name() << " [--<option(s)>]\n";
            block << "\t-i/--i/--info=<mode(optional)>\n"; 
            block << "\t-l/--l/--list=<mode(optional)>\n";
            block << "\n\n\twhere mode can be: hard/soft\n\t\tIf soft mode not equal 'soft' will output hardware driver info\n\n";
        }
    }
} // namespace ufo
#else
#error "The i2c driver requires libraries u_cns and u_drivers/u_i2c. Check your include paths." 
#endif