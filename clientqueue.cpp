#include "club.h"
#include <algorithm> 
namespace clubcontrol
{
    bool ClientQueue::is_client_here(const std::string& client_name)
    {
        for (const auto& i : data) {
            if (i == client_name) {
                return true;
            }
        }
        return false;
    }
    std::string ClientQueue::first()
    {
        if(current_size == 0){
            return std::string();
        }
        return data[front];
    }
    void ClientQueue::remove_client(std::string client_name)
    {
        int i = front;
        int remove_idx = front;
        while(i != back){
            if(data[i] == client_name){
                remove_idx = i;
                return;
            }
            i = (i + 1) % data.size();
        }
        if(remove_idx == front){
            pop();
            return;
        }
        i = remove_idx;
        while(i != back){
            data[i] = data[i-1];
            i = (i + 1) % data.size();
        }
        back = (back - 1 + data.size()) % data.size();
        current_size--;
    }
    bool ClientQueue::is_empty()
    {
        return current_size > 0 ? false : true;
    }
    bool ClientQueue::push(std::string new_client)
    {
        if(current_size == data.size()){
            return false;
        }
        data[back] = new_client;
        back = (back + 1) % data.size();
        current_size++;
        return true;
    }
    bool ClientQueue::pop()
    {
        if(current_size == 0) {
            return false;
        }
        front = (front + 1) % data.size();
        current_size--;
        return true;
    }
}
