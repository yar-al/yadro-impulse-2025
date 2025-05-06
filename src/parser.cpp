#include "club.h"
#include <sstream>
#include <fstream>
#include <iomanip>

namespace clubcontrol{
    int Parser::time_to_int(const std::string& time_str) {
        if(time_str.size() != 5 || time_str[2] != ':') return -1;
        int hours, minutes;
        char colon;
        std::istringstream iss(time_str);
        if (!(iss >> hours >> colon >> minutes) || colon != ':' || hours < 0 || hours >= 24 || minutes < 0 || minutes >= 60) {
            return -1;
        }
        return hours * 60 + minutes;
    }
    std::string Parser::int_to_time(int minutes) {
        int hours = minutes / 60;
        minutes %= 60;
        std::ostringstream oss;
        oss << std::setw(2) << std::setfill('0') << hours << ":" 
            << std::setw(2) << std::setfill('0') << minutes;
        return oss.str();
    }

    std::string Parser::describe_event(const Event& e)
    {
        std::stringstream ss;
        ss << int_to_time(e.time) << " " << e.id << " " << e.opttext;
        if(e.opttable > -1) {
            ss << " " << e.opttable + 1;
        }
        return ss.str();
    }
    
    void Parser::parse_file(std::string filename, std::vector<Event>& input_events, std::vector<int>& club_args)
    {
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Error opening file" << std::endl;
            throw InputException("No file");
        }

        std::string line;

        // Parse number of tables
        if (!std::getline(file, line)) {
            throw InputException(line);
        }
        int num_tables;
        num_tables = std::stoi(line);
        if (num_tables <= 0) throw InputException(line);
        
        // Parse opening and closing time
        if (!std::getline(file, line)) {
            throw InputException(line);
        }
        std::istringstream iss(line);
        std::string open_str, close_str;
        if (!(iss >> open_str >> close_str)) {
            throw InputException(line);
        }
        int opening_time = time_to_int(open_str);
        int closing_time = time_to_int(close_str);
        if (opening_time == -1 || closing_time == -1 || opening_time >= closing_time) {
            throw InputException(line);
        }

        // Parse price per hour
        if (!std::getline(file, line)) {
            throw InputException(line);
        }
        int price_per_hour = std::stoi(line);
        if (price_per_hour <= 0) throw InputException(line);

        // Parse events
        int prev_event_time = -1;
        while (std::getline(file, line)) {
            std::istringstream event_iss(line);
            std::string time_str, id_str;
            if (!(event_iss >> time_str >> id_str)) {
                throw InputException(line);
            }
            int time = time_to_int(time_str);
            int id;
            try {
                id = std::stoi(id_str);
            } catch (...) {
                throw InputException(line);
            }
            if (time == -1) throw InputException(line);
            if(time < prev_event_time) throw InputException(line);

            std::vector<std::string> args;
            std::string arg;
            while (event_iss >> arg) {
                args.push_back(arg);
            }
            int opttable = -1;
            if (args.size() > 2) {
                throw InputException(line);
            }
            if (args.size() > 1) {
                try {
                    opttable = std::stoi(args[1]);
                } catch(...) {
                    throw InputException(line);
                }
                if(opttable < 1 || opttable > num_tables) throw InputException(line); 
                opttable--;
            }
            input_events.emplace_back(time, id, args[0], opttable);
            prev_event_time = time;
        }
        club_args.push_back(num_tables);
        club_args.push_back(opening_time);
        club_args.push_back(closing_time);
        club_args.push_back(price_per_hour);
    }
}