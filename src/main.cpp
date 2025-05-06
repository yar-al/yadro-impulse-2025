#include "club.h"

using namespace clubcontrol;

int main(int argc, char* argv[]) {
    if (argc != 2) {
        return 1;
    }
    std::vector<Event> incoming_events;
    std::vector<int> club_args;
    try{
        Parser::parse_file(argv[1], incoming_events, club_args);
    } catch(InputException &ie) {
        std::cout << ie.what() << "\n";
        return 0;
    }
    //Parser::output_events(incoming_events);

    auto club = Club(club_args[0],club_args[1],club_args[2],club_args[3]);
    club.handle_day_and_report(incoming_events);
}