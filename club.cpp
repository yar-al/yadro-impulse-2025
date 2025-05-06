#include "club.h"
#include <set>

namespace clubcontrol{
    std::optional<Event> Club::handle_event(Event e)
    {
        int table_idx;
        std::string client_name;
        //std::cout << "Handling event \"" << Parser::describe_event(e) << "\" " << "Current time: " << e.time << std::endl; 
        //print_debug_info();
        switch(e.id) {
            case 1:
                if (e.time < opening_time || e.time > closing_time) {
                    return Event(e.time, 13, "NotOpenYet");
                }
                if (is_client_here(e.opttext)) {
                    return Event(e.time, 13, "YouShallNotPass");
                }
                unserved_clients.insert(e.opttext);
            break;
            case 2:
                if (tables[e.opttable].is_occupied) {
                    return Event(e.time, 13, "PlaceIsBusy");
                }
                if (!is_client_here(e.opttext)) {
                    return Event(e.time, 13, "ClientUnknown");
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
                    return Event(e.time, 13, "ICanWaitNoLonger!");
                }
                unserved_clients.erase(e.opttext);
                if(!waiting_queue.push(e.opttext)) {
                    return Event(e.time, 11, e.opttext);
                }
            break;
            case 4:
                table_idx = find_client(e.opttext);   
                if(table_idx < 0) {
                    if(unserved_clients.erase(e.opttext)) return {};
                    return Event(e.time, 13, "ClientUnknown");
                }
                if(table_idx == tables.size()){
                    waiting_queue.remove_client(e.opttext);
                    return {};
                }
                tables[table_idx].is_occupied = false;
                recalculate_table(table_idx, e.time);
                if(waiting_queue.is_empty()) {
                    return {};
                }
                client_name = waiting_queue.first();
                tables[table_idx].is_occupied = true;
                tables[table_idx].current_client = client_name;
                tables[table_idx].start_time = e.time;
                waiting_queue.pop();
                return Event(e.time, 12, client_name, table_idx);
            break;
            default:
                return Event(e.time, 13, "UnknownID");
            break;
        }
        return {};
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
    void Club::print_table_info()
    {
        for(int i = 0; i < tables.size(); i++){
            std::cout << i+1 << " " << tables[i].revenue << " " << Parser::int_to_time(tables[i].total_time) << "\n";
        }
    }
    std::vector<Event> Club::close_club()
    {
        std::vector<Event> events;
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
        return events;
    }
    void Club::handle_day_and_report(const std::vector<Event> &events)
    {
        std::cout << Parser::int_to_time(opening_time) << "\n";
        for(auto e : events){
            std::cout << Parser::describe_event(e) << "\n";
            auto generated_event = handle_event(e);
            if(generated_event) std::cout << Parser::describe_event(*generated_event) << "\n";
        }
        for(auto e : close_club()) {
            std::cout << Parser::describe_event(e) << "\n";
        }
        std::cout << Parser::int_to_time(closing_time) << "\n";
        print_table_info();
    }
}
