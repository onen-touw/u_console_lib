#pragma once

#if \
    __has_include("u_drivers/u_spiffs.h") && \
    __has_include("u_cns/u_cns.h") && \
    __has_include("uffs/udir.h")

#include "u_cns/u_cns.h"
#include "u_drivers/u_can.h"

namespace ufo
{
    static void twai_driver_console(const ufo::u_console_msg_block_t& block)
    {
        const auto& opt_list = block.get_opt_list();

        uint32_t canid = 0;
        uint32_t d = 200;
        if (opt_list.find("i,info"))
        {
            ufo::u_can_cfg_t cfg = ufo::get_can_driver_cfg();
            ufo::u_can_meta_t meta =  ufo::get_can_driver_meta();

            block << "twai driver info:\n";
            block << "\ncfg:\n\tbinded uart: " << static_cast<int>(cfg.port);
            block << "\nmeta:\n";
            block << "\tstate: " << meta.state.get(); 
            block << "\n\terror cnt: " << meta.error_count; 
            block << "\n\terror code: " << meta.error_code; 
            block << "\n\tuser cnt: " << meta.user_count;
            block << '\n';
        }
        else if (opt_list.find("u,usable"))
        {
            ufo::u_can_meta_t meta =  ufo::get_can_driver_meta();
            block << "twai driver: " << (meta.user_count > 0 ? "locked" : "usable") << "\n\n";
        }
        else if (opt_list.bind(&canid, "s,send"))
        {
            {
                ufo::u_can_meta_t meta =  ufo::get_can_driver_meta();
                if (meta.user_count > 0)
                {
                    block << "twai driver locked\n\n";
                    return;
                }
            }
            
            ufo::u_can_t drv;
            ufo::u_can_t::msg_t msg;
            msg.identifier = canid;
            if (canid > 0x7FF)
            {
                msg.extd = 1;
            }
            std::vector<uint8_t> data;
            if (opt_list.bind(&data, "d,data"))
            {
                auto sz = std::min(size_t(8), data.size());
                msg.data_length_code = sz;
                for (size_t i = 0; i < sz; i++)
                {
                    msg.data[i] = data[i];
                }
            }
            else
            {
                msg.data_length_code = 0;
            }
            if (drv.write(msg) == ESP_OK)
            {
                block << "Message sent\n\n";
            }
            else
            {
                block << "Failed to send message\n\n";
            }
        }
        else if (opt_list.find("h,help"))
        {
            block << "Usage: ";
            block << opt_list.get_prog_name() << " [--<option(s)>]\n";
            block << "\t-i/--i/--info - show driver info\n";
            block << "\t-u/--u/--usable - show is driver usable (not locked by other user)\n";
            block << "\t-s/--s/--send=<can_id> -- send can message with specified id\n";
            block << "\t\t-d/--d/--data=<data_bytes> - optional data bytes for send (up to 8 bytes, default: no data)\n";
            block << "\t\t\tdata format: comma separated byte values. Example: --data=0x12,0x34,0x56\n";
            block << "\t-e/--e/--echo=<can_id> - echo messages with specified can id\n\n";
        }
        else if (opt_list.bind(&d, "e,echo"))
        {
            {
                ufo::u_can_meta_t meta = ufo::get_can_driver_meta();
                if (meta.user_count > 0)
                {
                    block << "twai driver locked\n\n";
                    return;
                }
            }

            ufo::u_can_t drv;
            ufo::u_can_t::msg_t msg = {};
            size_t cyc = 0;
            while (true)
            {
                ufo::utl::sleep_for(1);
                // block << "cyc\n";
                if (drv.read(msg, pdMS_TO_TICKS(d)) == ESP_OK)
                {
                    block << "rcv: id="  << msg.identifier;
                    if (msg.data_length_code > 0)
                    {
                        block << " data=[";
                        for (size_t i = 0; i < msg.data_length_code - 1; i++)
                        {
                            block << msg.data[i] << ", ";
                        }
                        block << msg.data[msg.data_length_code - 1] << "]\n";
                    }
                    ++cyc;
                }

                if (block.read())
                {
                    block << "echo cycles: " << cyc << "\n\n";
                    break;
                }
                ufo::utl::sleep_for(d);
            }
        }
    }

} // namespace ufo

    
#endif