


#include "DebugMessages.h"


DebugMessages::~DebugMessages(){}
void DebugMessages::ON(void){_DEBUG = true;}
void DebugMessages::OFF(void){_DEBUG = false;}

void DebugMessages::display(const Control flow)
{
    if(!_DEBUG){return;}
    std::cout << MessageMap[flow] << std::endl;
    std::cout << "---------------------------------------------------------------------------------" << std::endl;

}
void DebugMessages::log(const Control flow)
{
    if(!_DEBUG){return;}
    _logfile.open(_logfile_name, std::ios_base::app);
    _logfile << MessageMap[flow] << std::endl;
    _logfile << "---------------------------------------------------------------------------------" << std::endl; 
    _logfile.close();
}
void DebugMessages::debug(const Control flow)
{
    if(!_DEBUG){return;}
    std::cout << MessageMap[flow] << std::endl;
    std::cout << "---------------------------------------------------------------------------------" << std::endl;
    _logfile.open(_logfile_name, std::ios_base::app);
    _logfile << MessageMap[flow] << std::endl;
    _logfile << "---------------------------------------------------------------------------------" << std::endl; 
    _logfile.close();
}

DebugMessages::DebugMessages()
{
    _DEBUG = false;
    _logfile.open(_logfile_name);
    _logfile << "";
    _logfile.close();

    MessageMap[liaison_start] = "L: Beginning Liaison Process...";
    MessageMap[liaison_hostname] = "L: Filesystem Hostname: ";
    MessageMap[liaison_partiton] = "L: Partition Name: ";
    MessageMap[liaison_client_sock_path] = "L: Client Socket Path: ";
    MessageMap[liaison_sock_path] = "L: Liaison Socket Path: ";
    MessageMap[liaison_daemon_port] = "L: Daemon Port#: ";
    MessageMap[liaison_cwd] = "L: Current Working Directory: ";
    MessageMap[liaison_init_shm_ops] = "L: Shared Memory Key: "; 
    MessageMap[liaison_get_shm] = "L: Accessing Shared Memory For Interprocess Synchronization..."; 
    MessageMap[liaison_get_shm_success] = "L: Shared Memory Found; Attachment Successfull";
    MessageMap[liaison_init_socket] = "L: Setting Up Server Socket...";
    MessageMap[liaison_init_sock_success] = "L: Server Socket Set Up Successfull";
    MessageMap[liaison_signal] = "L: Signaling Client";
    MessageMap[liaison_listen] = "L: Listening On Server Socket...";
    MessageMap[liaison_listen_success] = "L: Listen Successfully Completed";
    MessageMap[liaison_accept] = "L: Attempting To Accept Client Connection";
    MessageMap[liaison_accept_success] = "L: Client Connection Accepted";
    MessageMap[liaison_unattch_shm] = "L: Unattatching Shared Memory Segment...";
    MessageMap[liaison_unattch_success] = "L: Shared Memory Succesfully Unattatched";
    MessageMap[liaison_get_peer] = "L: Retrieving Client Peername...";
    MessageMap[liaison_get_peer_success] = "L: Client Peername Retrieved Successfully: ";
    MessageMap[liaison_command_received] = "L: Client Command Received Successfully";
    MessageMap[liaison_command_sender] = "L: Command Received From: ";
    MessageMap[liaison_command] = "L: Command: ";
    MessageMap[liaison_received_time] = "L: Command Received @ ";
    MessageMap[liaison_close] = "L: Closing Connections...";
    MessageMap[liaison_quit] = "L: Liaison Process Closing; Goodbye";

}