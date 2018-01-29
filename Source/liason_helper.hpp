////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  liason.h
//  Liason headerfile
//  Primary Author: Adrian Barberis
//  For "Arboreal" Senior Design Project
//  
//  Sun. | Jan. 28th | 2018 | 8:30 PM
//
////////////////////////////////////////////////////////////////////////////////////////////////////


//[================================================================================================]
// Convert the first X characters in a 'Command Buffer' to an integer value
// X is the size of an integer
int get_cmnd_id(char* cmnd)
{
    char temp[sizeof(int)];
    for(unsigned int i = 0; i < sizeof(int); i++)
    {
        temp[i] = cmnd[i];
    }

    int* id = (int*)temp;
    return *id;
}
//[================================================================================================]



int delete_segment(int seg_id){
    if ((shmctl(seg_id,IPC_RMID,0))==-1){
    std::cout<<" ERROR(C++)with shmctl(IPC_RMID): "<<strerror(errno)<<std::endl;
    return -1;
    }else//on success
        return 0;
}

void clean_segments(){

    struct shmid_ds shm_info;
    struct shmid_ds shm_segment;
    int max_id = shmctl(0,IPC_STAT,&shm_info);
    if (max_id>=0){
        for (int i=0;i<=max_id;++i) {
                int shm_id = shmctl(i , IPC_STAT , &shm_segment);
                if (shm_id<=0)
                    continue;
                else if (shm_segment.shm_nattch==0){
                    delete_segment(shm_id);
                }
        }
    }
    return;
}