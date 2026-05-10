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
    __has_include("u_drivers/u_uart.h") && \
    __has_include("u_cns/u_cns.h")

#include "u_cns/u_cns.h"
#include "u_drivers/u_uart.h"

namespace ufo
{
    static void uart_driver_console(const ufo::u_console_msg_block_t& block)
    {
        const auto& opt_list = block.get_opt_list();

        std::string mode;

        if (opt_list.bind(&mode, "i,info"))
        {
            ufo::u_uart_cfg_t cfg;
            ufo::u_uart_meta_t meta;

#if U_UART_DRIVERS_CNT > 1
            if (mode == "uart1")
            {
                cfg = ufo::get_uart_driver_cfg(ufo::u_uart_t::uart_port_t::uart1);
                meta = ufo::get_uart_driver_meta(ufo::u_uart_t::uart_port_t::uart1);
            }
#if U_UART_DRIVERS_CNT > 2
            else if (mode == "uart2")
            {
                cfg = ufo::get_uart_driver_cfg(ufo::u_uart_t::uart_port_t::uart2);
                meta = ufo::get_uart_driver_meta(ufo::u_uart_t::uart_port_t::uart2);
            }
#endif
            else
            {
                cfg = ufo::get_uart_driver_cfg();
                meta = ufo::get_uart_driver_meta();
                mode = "uart0";
            }
#else
            if (mode == "uart1" || mode == "uart2")
            {
                block << "uart1 and uart2 disabled\n\n";
                return;
            }
            cfg = ufo::get_uart_driver_cfg();
            meta = ufo::get_uart_driver_meta();
            mode = "uart0";
#endif

            block << "uart driver info:\ncfg:\n";
            block << "mode: " << mode;
            block << "\ncfg:\n\tboudrate: " << cfg.baudrate;
            block << "\n\ttx/rx: " << cfg.tx << "/" << cfg.rx;
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
            block << "\t-i/--i/--info=<mode(optional)> where mode can be: uart0/1/2\n\n";
        }
    }
} // namespace ufo

#else
#error "The uart driver requires libraries u_cns and u_drivers/u_uart. Check your include paths." 
#endif 
