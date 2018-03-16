



#include <map>
#include <string>
#include <iostream>
#include <fstream>

enum Control
{
    liaison_start,
    liaison_hostname,
    liaison_partiton,
    liaison_client_sock_path,
    liaison_sock_path,
    liaison_daemon_port,
    liaison_cwd,
    liaison_init_shm_ops,
    liaison_get_shm,
    liaison_get_shm_success,
    liaison_init_socket,
    liaison_init_sock_success,
    liaison_signal,
    liaison_listen,
    liaison_listen_success,
    liaison_accept,
    liaison_accept_success,
    liaison_unattch_shm,
    liaison_unattch_success,
    liaison_get_peer,
    liaison_get_peer_success,
    liaison_command_received,
    liaison_command_sender,
    liaison_command,
    liaison_received_time,
    liaison_close,
    liaison_quit


};


class DebugMessages
{
public:
    DebugMessages();
    DebugMessages(std::string logfile_name);
    ~DebugMessages();
    void display(const Control flow);
    void log(const Control flow);
    void debug(const Control flow);
    void ON(void);
    void OFF(void);

    template <typename T>
    void display(const Control flow, T data)
    {
        if(!_DEBUG){return;}
        std::cout << MessageMap[flow] << data << std::endl;
        std::cout << "---------------------------------------------------------------------------------" << std::endl;
    }
    
    template <typename T>
    void log(const Control flow, T data)
    {
        if(!_DEBUG){return;}
        _logfile.open(_logfile_name, std::ios_base::app);
        _logfile << MessageMap[flow] << data << std::endl;
        _logfile << "---------------------------------------------------------------------------------" << std::endl; 
        _logfile.close();
    }
    
    template <typename T>
    void debug(const Control flow, T data)
    {
        if(!_DEBUG){return;}
        std::cout << MessageMap[flow] << data << std::endl;
        std::cout << "---------------------------------------------------------------------------------" << std::endl;    
        _logfile.open(_logfile_name, std::ios_base::app);
        _logfile << MessageMap[flow] << data << std::endl;
        _logfile << "---------------------------------------------------------------------------------" << std::endl; 
        _logfile.close();
    }

private:
    bool _DEBUG;
    std::ofstream _logfile;
    std::string _logfile_name = "Arboreal.log";
    std::map<const Control, const char*> MessageMap;
};

