#include "club.h"
#include <set>

namespace clubcontrol{
    void Club::change_state(Event e)
    {
        int table_idx;
        //std::cout << "Handling event \"" << Parser::describe_event(e) << "\" " << "Current time: " << e.time << std::endl; 
        //print_debug_info();
        switch(e.id) {
            case 1:
                if (e.time < opening_time || e.time > closing_time) {
                    events.emplace_back(e.time, 13, "NotOpenYet");
                    return;
                }
                if (is_client_here(e.opttext)) {
                    events.emplace_back(e.time, 13, "YouShallNotPass");
                    return;
                }
                unserved_clients.insert(e.opttext);
            break;
            case 2:
                if (tables[e.opttable].is_occupied) {
                    events.emplace_back(e.time, 13, "PlaceIsBusy");
                    return;
                }
                if (!is_client_here(e.opttext)) {
                    events.emplace_back(e.time, 13, "ClientUnknown");
                    return;
                }
                unserved_clients.erase(e.opttext);
                table_idx = find_client(e.opttext);
                if (table_idx > -1 && table_idx < tables.size()) {
                    tables[table_idx].is_occupied = false;
                    recalculate_table(table_idx, e.time);
                }
                tables[e.opttable].is_occupied = true;
                tables[e.opttable].start_time = e.time;
                tables[e.opttable].current_client = e.opttext;
            break;
            case 3:
                if(get_free_table() > -1) {
                    events.emplace_back(e.time, 13, "ICanWaitNoLonger!");
                    return;
                }
                if(!waiting_queue.push(e.opttext)) {
                    events.emplace_back(e.time, 11, e.opttext);
                    return;
                }
            break;
            case 4:
                table_idx = find_client(e.opttext);   
                if(table_idx < 0) {
                    if(unserved_clients.erase(e.opttext)) return;
                    events.emplace_back(e.time, 13, "ClientUnknown");
                    return;
                }
                if(table_idx == tables.size()){
                    waiting_queue.remove_client(e.opttext);
                    return;
                }
                tables[table_idx].is_occupied = false;
                recalculate_table(table_idx, e.time);
                if(waiting_queue.is_empty()) {
                    return;
                }
                events.emplace_back(e.time, 12, waiting_queue.first(), table_idx);
                tables[table_idx].is_occupied = true;
                tables[table_idx].current_client = waiting_queue.first();
                tables[table_idx].start_time = e.time;
                waiting_queue.pop();
            break;
            default:
                events.emplace_back(e.time, 13, "UnknownID");
            break;
        }
    }
    
    bool Club::is_client_here(std::string client_name)
    {
        auto it = unserved_clients.find(client_name);
        if(it != unserved_clients.end()) return true;
        for (const auto& i : tables) {
            if (i.current_client == client_name) {
                return true;
            }
        }
        return waiting_queue.is_client_here(client_name);
    }

    int Club::find_client(std::string client_name)
    {
        for(int i = 0; i < tables.size(); i++){
            if(tables[i].current_client == client_name){
                return i;
            }
        }
        return waiting_queue.is_client_here(client_name) ? tables.size() : -1;
    }

    int Club::get_free_table()
    {
        for(int i = 0; i < tables.size(); i++){
            if(!tables[i].is_occupied){
                return i;
            }
        }
        return -1;
    }
    void Club::output_revenue()
    {
        for(int i = 0; i < tables.size(); i++) {
            std::cout << i << " " << tables[i].revenue << " " << Parser::int_to_time(tables[i].total_time) << "\n"; 
        }
    }
    void Club::recalculate_table(int table_number, int current_time)
    {
        int delta_time = current_time - tables[table_number].start_time;
        tables[table_number].total_time += delta_time;
        tables[table_number].revenue += (delta_time / 60 + (delta_time % 60 ? 1 : 0)) * price;
    }
    void Club::print_debug_info()
    {
        std::cout << "Queue state: ";
        waiting_queue.print_queue();
        std::cout << "Tables state:" << std::endl;
        for(int i = 0; i < tables.size(); i++) {
            std::cout << i << " " << tables[i].current_client << " " 
            << (tables[i].is_occupied ? "occupied" : "vacant") << " " << tables[i].start_time << " " << tables[i].revenue
            << " " << tables[i].total_time << std::endl;
        }
    }
    void Club::close_club()
    {
        std::set<std::string> last_clients;
        for(int i = 0; i < tables.size(); i++){
            if(!tables[i].is_occupied) continue;
            recalculate_table(i, closing_time);
            tables[i].is_occupied = false;
            last_clients.insert(tables[i].current_client);
        }
        while(!waiting_queue.is_empty()){
            last_clients.insert(waiting_queue.first());
            waiting_queue.pop();
        }
        for(auto i : unserved_clients){
            last_clients.insert(i);
        }
        for(const auto &i : last_clients){
            events.emplace_back(closing_time, 11, i);
        }
    }
    void Club::full_output()
    {
        std::cout << Parser::int_to_time(opening_time) << "\n";
        Parser::output_events(events);
        std::cout << Parser::int_to_time(closing_time) << "\n";
        output_revenue();
    }
}
