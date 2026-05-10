#pragma once

#if \
    __has_include("u_cns/u_cns.h") && \
    __has_include("u_configurator/u_configurator.h") && \
    __has_include("uffs/udir.h")


#include "u_cns/u_cns.h"
#include "u_configurator/u_reconfigurator.h"

namespace ufo
{

    void parse_res(const ufo::u_console_msg_block_t& block, const char* op_name, u_configure::u_configurator_ack_t ack)
    {
        printf("result of %s\t", op_name);
        using namespace u_configure;

        switch (ack)
        {
        case u_configurator_ack_t::NO_ACCESS :
            block << "NO_ACCESS\n";
            break;
        case u_configurator_ack_t::OP_ERROR :
            block << "OP_ERROR\n";
            break;
        case u_configurator_ack_t::OP_FILE_ERROR :
            block << "OP_FILE_ERROR\n";
            break;
        case u_configurator_ack_t::OP_DIR_ERROR :
            block << "OP_DIR_ERROR\n";
            break;
        case u_configurator_ack_t::OP_LOCKED :
            block << "OP_LOCKED\n";
            break;
        case u_configurator_ack_t::OP_SUCCESS :
            block << "OP_SUCCESS\n";
            break;
        case u_configurator_ack_t::PARAM_DUBLICATION :
            block << "PARAM_DUBLICATION\n";
            break;
        case u_configurator_ack_t::PARSING_ERROR:
            block << "PARSING_ERROR\n";
            break;
        case u_configurator_ack_t::USER_FUNCTION_ERROR:
            block << "USER_FUNCTION_ERROR\n";
            break;
        case u_configurator_ack_t::OP_FILE_RECOVERY:
            block << "OP_FILE_RECOVERY\n";
            break;
        default:
            break;
        }
    }

    bool user_confirm_yesno(const ufo::u_console_msg_block_t& block)
    {
        block << "Are you sure? (y/any): ";
        auto ans = block.read_line();
        return ans == "y" || ans == "Y";
    }

    void rcfg_help_info(const ufo::u_console_msg_block_t& block)
    {
        block << "Type:" << 
        "\n\t'<key> <value>', where key is parameter name, to modify (or add) parameter" << 
        "\n\t'apply' to save cfg, exit to out from configurator mode" 
        "\n\t'help' to see this text" << 
        "\n\t'perm' to see permission list"
        "\n\t'see' to see this unsaved changes\n\n";
    }

    void cfg_help_info(const ufo::u_console_msg_block_t& block)
    {
        block << "\t-h/help - see this text\n"
        "\t-t/cat=<all/def/old(optional)> - cat config file(s) in console\n";
        block << "\t-m/mod - modify parameters\n"
        "\t-r/rb - rollback to previous\n"
        "\t-d/df - reset to default\n";
        block << "\t-c/chk - check cfg files\n\n";
    }

    void cat_cfg(const ufo::u_console_msg_block_t& block, const std::filesystem::path& p)
    {
        block << "Reading " << p.c_str() << '\n';
        std::string fn;
        ufo::ufile_t fl(p, ufo::file_mode::read_rb);
        if (!fl.is_open())
        {
            block << "Incorrect path to file or file does not exists\n\n";
            return;
        }
        while (fl.getline(fn))
        {
            block << fn << "\n";
        }
        block << "\n";
    }

    static void configurator_console(const ufo::u_console_msg_block_t& block)
    {
        const auto& opt_list = block.get_opt_list();

        std::string tmps;

        if (opt_list.bind(&tmps, "t,cat"))
        {
            using pt_t = u_configure::u_reconfigurator_t::paths_t;
            
            block << "Tracing:\n";
            
            std::filesystem::path dr = pt_t::cfg_dir;

            if (tmps == "all")
            {
                cat_cfg(block, dr / pt_t::cfg_file);
                cat_cfg(block, dr / pt_t::cfg_file_def);
                dr /= pt_t::cfg_file;
                dr += pt_t::old_postfix;
                cat_cfg(block,  dr);
                return;
            }
            else if (tmps == "def")
            {
                cat_cfg(block, dr / pt_t::cfg_file_def);
            }
            else if (tmps == "old")
            {
                dr /= pt_t::cfg_file;
                dr += pt_t::old_postfix;
                cat_cfg(block,  dr);
            }
            else 
            {
                cat_cfg(block, dr / pt_t::cfg_file);
            }
        }
        else if (opt_list.find("m,mod"))
        {
            u_configure::u_reconfigurator_t rcfg;
            block << "You are entering in configurator mode:\n";
            if (!rcfg.is_reconfiguration_enabled())
            {
                block << "reconfiguration locked\n";
                return;
            }

            if (rcfg.is_psw_requared())
            {
                block << "Requare password (Type 'exit' to exit from cfg-mode): \n";
                while (true)
                {
                    block << "psw: ";
                    auto ps = block.read_line();

                    if (rcfg.get_access(ps.c_str()))
                    {
                        break;
                    }

                    if (ps == "exit")
                    {
                        block << "Exit from cfg-mode\n";
                        return;
                    }
                }
                block << "Access allowed\n\n";
            }

            rcfg_help_info(block);
            while (true)
            {
                block << "cfg>";

                auto s = block.read_line();
                if (s == "apply")
                {
                    // bool access = false;
                    bool back_to = false;

                    if (rcfg.is_psw_requared())
                    {
                        block << "Type password again to save config\nType 'back' to continue changes and 'exit' to stop and quit without saving\n";
                        while (true)
                        {
                            block << "psw: ";
                            auto ps = block.read_line();

                            if (rcfg.get_access(ps.c_str()))
                            {
                                break;
                            }
                            else if (ps == "exit")
                            {
                                block << "You want to exit. Your changes will not be saved\n";
                                if (user_confirm_yesno(block))
                                {
                                    return;
                                }
                                back_to = true;
                                break;
                            }
                            else if (ps == "back")
                            {
                                back_to = true;
                                break;
                            }
                        }

                        if (back_to)
                        {
                            continue;
                        }
                    }

                    if (rcfg.is_autoreboot_enabled())
                    {
                        int i = 10;
                        block << "System automaticaly will be reconfigurated and rebooted after " << i << " sec\n";
                        while (true)
                        {
                            block << i-- << "...\n";
                            ufo::utl::sleep_for(1000);
                            if (!i)
                            {
                                break;
                            }
                        }
                    }
                    
                    block << "apply (test)\n";
                    auto res = rcfg.apply_config();

                    // never reach if auto reboot 
                    parse_res(block, "'apply' (error):", res);
                    return;
                }
                else if (s == "exit")
                {
                    block << "Exit without saving\n";
                    if (user_confirm_yesno(block))
                    {
                        return;
                    }
                    continue;                    
                }
                else if (s == "see")
                {
                    block << "Unsaved changes:\n";
                    const auto& changes =  rcfg.get_changes();
                    if (changes.empty())
                    {
                        block << "\tnothing\n";
                        continue;
                    }
                    for (const auto &i : changes)
                    {
                        block << "\t" << i.first << ": "<< i.second << '\n';
                    }
                    block << '\n';                    
                }
                else if (s == "perm")
                {
                    block << "permissions:\n" << "\tadd new k-v fields in config: " << (rcfg.is_add_enabled() ? 'y' : 'n') 
                    << "\n\tauto reboot: " << (rcfg.is_autoreboot_enabled() ? 'y': 'n') 
                    << "\n\trequare password: " << (rcfg.is_psw_requared() ? 'y' : 'n')
                    << "\n\trun-time reconfiguration: " << (rcfg.is_reconfiguration_enabled() ? 'y' : 'n')
                    << "\n\tadvanced configuration: " << (rcfg.is_self_modication_enabled() ? 'y' : 'n') << '\n';
                }
                else if (s == "help")
                {
                    rcfg_help_info(block);
                }
                else // reconfig
                {
                    if (!s.empty())
                    {
                        auto w = s.find(' ');
                        if (w == s.npos)
                        {
                            block << "Wrong k-v pair\n";
                            continue;
                        }
                        bool go_back = false;

                        auto key = s.substr(0, w);
                        if (!w)
                        {
                            block << "No key\n";
                            continue;
                        }
                            
                        for (const auto &c : key)
                        {
                            if (!std::isalnum(static_cast<unsigned int>(c)) && c != '-')
                            {
                                block << "Wrong key\n";
                                go_back = true;
                                break;
                            }
                        }
                        if (go_back) { continue; }

                        auto dw = s.size() - ++w;
                        if (dw)
                        {
                            auto value = s.substr(w, dw);
                            for (const auto &c : value)
                            {
                                if (!std::isalnum(static_cast<unsigned int>(c)) && c != '-' && c != '.')
                                {
                                    block << "Wrong value\n";
                                    go_back = true;
                                    break;
                                }
                            }   
                            if (go_back) { continue; }

                            block << "result(test): '" << key << "' '" << value << "'\n";
                            auto rcfres = rcfg.reconfig(std::move(key), std::move(value)); 
                            parse_res(block, "'reconfig' (test):", rcfres);

                            continue;
                        }
                        block << "Incorrect format (no value)\n";
                    }
                    continue;
                }
            }
        }
        else if (opt_list.find("h,help"))
        {
            cfg_help_info(block);
        }
        else if (opt_list.find("r,rb"))
        {
            block << "You want to recover to previous config.\n";
            if (!user_confirm_yesno(block))
            {
                return;
            }
            u_configure::u_reconfigurator_t rcfg;
            auto res = rcfg.recover_from_old();
            parse_res(block, "'recovery'", res);
        }
        else if (opt_list.find("d,df"))
        {
            block << "You want to reset to default config.\n";
            if (!user_confirm_yesno(block))
            {
                return;
            }
            u_configure::u_reconfigurator_t rcfg;
            auto res = rcfg.reset_to_default();
            parse_res(block, "'reset'", res);
        }
        else if (opt_list.find("c,chk"))
        {
            block << "Checking fs:\n";
            ufo::udir_t check_dir(u_configure::u_reconfigurator_t::paths_t::cfg_dir);
            block << "\tdefault: " << (check_dir.file_exists(u_configure::u_reconfigurator_t::paths_t::cfg_file_def) ? 'y' : 'n') << '\n';
            
            std::filesystem::path p = u_configure::u_reconfigurator_t::paths_t::cfg_file;
            block << "\tcfg: " << (check_dir.file_exists(p) ? 'y' : 'n') << '\n';

            p+= u_configure::u_reconfigurator_t::paths_t::old_postfix;
            block << "\t.old: " << (check_dir.file_exists(p) ? 'y' : 'n') << "\n\n";
            return;
        }
        else
        {
            cfg_help_info(block);
        }
    }
} // namespace ufo

#else
#error "Console configurator requires u_cns, uffs and u_configurator modules" 
#endif