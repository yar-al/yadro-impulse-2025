#ifndef CLUB
#define CLUB

#include <string>
#include <vector>
#include <unordered_set>
#include <iostream>
#include <optional>

namespace clubcontrol{
    
    struct Table
    {
        bool is_occupied;
        std::string current_client;
        int start_time;
        int total_time;
        int revenue;
        Table() = default;
    };

    struct Event
    {
        int time;
        int id;
        std::string opttext;
        int opttable;
        Event(int event_time, int event_id, std::string event_text, int event_table = -1) :
        time(event_time), id(event_id), opttext(event_text), opttable(event_table) {};
    };

    class ClientQueue
    {
        std::vector<std::string> data;
        int front;
        int back;
        int current_size;
        public:
        ClientQueue(int size) : data(size), front(0), back(0), current_size(0) {};
        bool is_empty();
        bool push(std::string);
        bool pop();
        bool is_client_here(const std::string& client_name);
        std::string first();
        void remove_client(std::string);
        void print_queue(){
            std::cout << current_size << " ";
            for(int i = front; i != back; i = (i + 1) % data.size()){
                std::cout << data[i] << " ";
            }
        }
    };

    class Club
    {
        private:
        int opening_time;
        int closing_time;
        int price;
        std::vector<Table> tables;
        ClientQueue waiting_queue;
        std::unordered_set<std::string> unserved_clients;
        bool is_client_here(std::string client_name);
        int find_client(std::string client_name);
        int get_free_table();
        void recalculate_table(int table_number, int current_time);
        void print_debug_info();
        void print_table_info();
        public:
        Club(int table_count, int o_time, int c_time, int hour_price) : 
            opening_time(o_time), closing_time(c_time), price(hour_price), waiting_queue(table_count), tables(table_count) {};
        std::optional<Event> handle_event(Event e);
        std::vector<Event> close_club();
        int get_opening_time() const { return opening_time; }
        int get_closing_time() const { return closing_time; };
        void handle_day_and_report(const std::vector<Event>&events);
    };

    class Parser
    {
        public:
        static std::string describe_event(const Event& e);
        static int time_to_int(const std::string& time_str);
        static std::string int_to_time(int minutes);
        static void parse_file(std::string filename, std::vector<Event>& input_events, std::vector<int>& club_args);
        static void output_events(std::vector<Event> events) {
            for(auto &e : events){
                std::cout << describe_event(e) << "\n";
            }
        }
    };

    class InputException: public std::runtime_error
    {
    public:
        explicit InputException(const std::string &message) : runtime_error(message) {}
    };

}

#endif