#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <ctime>
#include <map>
#include <algorithm>
#include <iomanip>
#include <cstdlib>
#include <conio.h>
#include <sstream>

using namespace std;

// Seat Class Enumeration
enum SeatClass {
    ECONOMY,
    BUSINESS
};

// Flight Status Enumeration
enum FlightStatus {
    ON_TIME,
    DELAYED,
    CANCELED
};

// Structure for Seat
struct Seat {
    int seatNumber;
    bool isBooked;
    SeatClass type;
    double basePrice;
    
    Seat(int num, SeatClass t) : seatNumber(num), isBooked(false), type(t) {
        basePrice = (t == ECONOMY) ? 1000.0 : 2500.0;
    }
};

// Structure for Passenger
struct Passenger {
    string name;
    string email;
    string phone;
    int seatNumber;
    string ticketNumber;
    
    Passenger() {}
    
    Passenger(string n, string e, string p, int s) : 
        name(n), email(e), phone(p), seatNumber(s) {
        // Generate random ticket number
        ticketNumber = generateTicketNumber();
    }
    
    string generateTicketNumber() {
        stringstream ss;
        ss << "TKT" << rand() % 10000 << time(0) % 10000;
        return ss.str();
    }
};

// Structure for Flight
struct Flight {
    string flightNumber;
    string source;
    string destination;
    string sourceTime;
    string destinationTime;
    string date;
    double basePrice;
    string via;
    int stops;
    vector<Seat> seats;
    vector<Passenger> passengers;
    FlightStatus status;
    
    Flight() : stops(0), basePrice(0), status(ON_TIME) {}
    
    Flight(string fnum, string src, string dest, string srcTime, string destTime, 
           string d, double price, string v, int st) : 
        flightNumber(fnum), source(src), destination(dest), sourceTime(srcTime), 
        destinationTime(destTime), date(d), basePrice(price), via(v), stops(st), status(ON_TIME) {
        
        // Initialize seats - 30 economy seats and 10 business class seats
        for(int i = 1; i <= 30; i++) {
            seats.push_back(Seat(i, ECONOMY));
        }
        for(int i = 31; i <= 40; i++) {
            seats.push_back(Seat(i, BUSINESS));
        }
    }
    
    double getSeatPrice(int seatNum) {
        // Dynamic pricing - increases as more seats get filled
        double occupancyRate = getOccupancyRate();
        double priceFactor = 1.0 + (occupancyRate * 0.5); // Up to 50% price increase
        
        for(auto& seat : seats) {
            if(seat.seatNumber == seatNum) {
                return seat.basePrice * priceFactor;
            }
        }
        return basePrice;
    }
    
    double getOccupancyRate() {
        int booked = 0;
        for(const auto& seat : seats) {
            if(seat.isBooked) booked++;
        }
        return static_cast<double>(booked) / seats.size();
    }
    
    void markSeatBooked(int seatNum) {
        for(auto& seat : seats) {
            if(seat.seatNumber == seatNum) {
                seat.isBooked = true;
                break;
            }
        }
    }
    
    void markSeatAvailable(int seatNum) {
        for(auto& seat : seats) {
            if(seat.seatNumber == seatNum) {
                seat.isBooked = false;
                break;
            }
        }
    }
    
    bool isSeatAvailable(int seatNum) const {
        for(const auto& seat : seats) {
            if(seat.seatNumber == seatNum) {
                return !seat.isBooked;
            }
        }
        return false;
    }
    
    string getSeatType(int seatNum) const {
        for(const auto& seat : seats) {
            if(seat.seatNumber == seatNum) {
                return (seat.type == ECONOMY) ? "Economy" : "Business";
            }
        }
        return "Unknown";
    }
};

// Flight Booking System Class
class FlightBookingSystem {
private:
    vector<Flight> flights;
    map<string, vector<string>> bookingHistory; // phone -> ticketNumbers
    
    void saveData() {
        ofstream flightFile("flights.txt");
        if(!flightFile.is_open()) {
            cout << "Error saving flight data!" << endl;
            return;
        }
        
        for(const auto& flight : flights) {
            flightFile << flight.flightNumber << "|"
                       << flight.source << "|"
                       << flight.destination << "|"
                       << flight.sourceTime << "|"
                       << flight.destinationTime << "|"
                       << flight.date << "|"
                       << flight.basePrice << "|"
                       << flight.via << "|"
                       << flight.stops << "|"
                       << static_cast<int>(flight.status) << endl;
                       
            // Save seat data
            for(const auto& seat : flight.seats) {
                flightFile << seat.seatNumber << ","
                           << seat.isBooked << ","
                           << static_cast<int>(seat.type) << ","
                           << seat.basePrice << ";";
            }
            flightFile << endl;
            
            // Save passenger data
            for(const auto& passenger : flight.passengers) {
                flightFile << passenger.name << ","
                           << passenger.email << ","
                           << passenger.phone << ","
                           << passenger.seatNumber << ","
                           << passenger.ticketNumber << ";";
            }
            flightFile << endl;
        }
        
        flightFile.close();
        
        // Save booking history
        ofstream historyFile("bookingHistory.txt");
        if(!historyFile.is_open()) {
            cout << "Error saving booking history!" << endl;
            return;
        }
        
        for(const auto& entry : bookingHistory) {
            historyFile << entry.first << "|";
            for(const auto& ticketNum : entry.second) {
                historyFile << ticketNum << ",";
            }
            historyFile << endl;
        }
        
        historyFile.close();
    }
    
    void loadData() {
        ifstream flightFile("flights.txt");
        if(!flightFile.is_open()) {
            cout << "No saved flight data found. Starting fresh." << endl;
            return;
        }
        
        flights.clear();
        string line;
        
        while(getline(flightFile, line)) {
            if(line.empty()) continue;
            
            Flight flight;
            stringstream ss(line);
            string token;
            
            getline(ss, flight.flightNumber, '|');
            getline(ss, flight.source, '|');
            getline(ss, flight.destination, '|');
            getline(ss, flight.sourceTime, '|');
            getline(ss, flight.destinationTime, '|');
            getline(ss, flight.date, '|');
            
            string priceStr, stopsStr, statusStr;
            getline(ss, priceStr, '|');
            getline(ss, flight.via, '|');
            getline(ss, stopsStr, '|');
            getline(ss, statusStr, '|');
            
            flight.basePrice = stod(priceStr);
            flight.stops = stoi(stopsStr);
            flight.status = static_cast<FlightStatus>(stoi(statusStr));
            
            // Load seat data
            if(getline(flightFile, line)) {
                stringstream seatss(line);
                string seatData;
                
                while(getline(seatss, seatData, ';')) {
                    if(seatData.empty()) continue;
                    
                    stringstream seatStream(seatData);
                    string seatNumStr, isBookedStr, typeStr, basePriceStr;
                    
                    getline(seatStream, seatNumStr, ',');
                    getline(seatStream, isBookedStr, ',');
                    getline(seatStream, typeStr, ',');
                    getline(seatStream, basePriceStr, ',');
                    
                    Seat seat(stoi(seatNumStr), static_cast<SeatClass>(stoi(typeStr)));
                    seat.isBooked = (isBookedStr == "1");
                    seat.basePrice = stod(basePriceStr);
                    
                    flight.seats.push_back(seat);
                }
            }
            
            // Load passenger data
            if(getline(flightFile, line)) {
                stringstream passss(line);
                string passData;
                
                while(getline(passss, passData, ';')) {
                    if(passData.empty()) continue;
                    
                    stringstream passStream(passData);
                    string name, email, phone, seatNumStr, ticketNumber;
                    
                    getline(passStream, name, ',');
                    getline(passStream, email, ',');
                    getline(passStream, phone, ',');
                    getline(passStream, seatNumStr, ',');
                    getline(passStream, ticketNumber, ',');
                    
                    Passenger passenger;
                    passenger.name = name;
                    passenger.email = email;
                    passenger.phone = phone;
                    passenger.seatNumber = stoi(seatNumStr);
                    passenger.ticketNumber = ticketNumber;
                    
                    flight.passengers.push_back(passenger);
                }
            }
            
            flights.push_back(flight);
        }
        
        flightFile.close();
        
        // Load booking history
        ifstream historyFile("bookingHistory.txt");
        if(!historyFile.is_open()) {
            cout << "No booking history found." << endl;
            return;
        }
        
        bookingHistory.clear();
        
        while(getline(historyFile, line)) {
            if(line.empty()) continue;
            
            stringstream ss(line);
            string phone, ticketsStr;
            
            getline(ss, phone, '|');
            getline(ss, ticketsStr);
            
            stringstream ticketsSS(ticketsStr);
            string ticket;
            vector<string> tickets;
            
            while(getline(ticketsSS, ticket, ',')) {
                if(!ticket.empty()) {
                    tickets.push_back(ticket);
                }
            }
            
            bookingHistory[phone] = tickets;
        }
        
        historyFile.close();
    }
    
public:
    FlightBookingSystem() {
        // Seed random number generator
        srand(time(0));
        loadData();
    }
    
    ~FlightBookingSystem() {
        saveData();
    }
    
    void clearScreen() {
        system("cls");
    }
    
    void displayTitle() {
        clearScreen();
        cout << "\n\n";
        cout << "========================================================\n";
        cout << "||                                                    ||\n";
        cout << "||          HAWAII JAHAJH DE TICKET SYSTEM           ||\n";
        cout << "||                                                    ||\n";
        cout << "========================================================\n\n";
    }
    
    void mainMenu() {
        while(true) {
            displayTitle();
            cout << "1. Login as User\n";
            cout << "2. Login as Admin\n";
            cout << "3. Exit\n\n";
            cout << "Select an option: ";
            
            char choice;
            cin >> choice;
            
            switch(choice) {
                case '1':
                    userDashboard();
                    break;
                case '2':
                    adminLogin();
                    break;
                case '3':
                    cout << "\nThank you for using Hawaii Jahajh De Ticket System!\n";
                    return;
                default:
                    cout << "\nInvalid option. Please try again.\n";
                    cout << "Press any key to continue...";
                    getch();
            }
        }
    }
    
    void adminLogin() {
        displayTitle();
        cout << "ADMIN LOGIN\n\n";
        
        string password;
        cout << "Enter Password: ";
        cin >> password;
        
        if(password == "sai123") {
            adminDashboard();
        } else {
            cout << "\nAccess Denied! Incorrect Password.\n";
            cout << "Press any key to return to main menu...";
            getch();
        }
    }
    
    void adminDashboard() {
        while(true) {
            displayTitle();
            cout << "ADMIN DASHBOARD\n\n";
            cout << "1. Add Flight\n";
            cout << "2. View All Flights\n";
            cout << "3. Modify Flight\n";
            cout << "4. Delete Flight\n";
            cout << "5. Update Flight Status\n";
            cout << "6. View Bookings\n";
            cout << "7. Log Out\n\n";
            cout << "Select an option: ";
            
            char choice;
            cin >> choice;
            
            switch(choice) {
                case '1':
                    addFlight();
                    break;
                case '2':
                    viewAllFlights();
                    break;
                case '3':
                    modifyFlight();
                    break;
                case '4':
                    deleteFlight();
                    break;
                case '5':
                    updateFlightStatus();
                    break;
                case '6':
                    viewBookings();
                    break;
                case '7':
                    return;
                default:
                    cout << "\nInvalid option. Please try again.\n";
                    cout << "Press any key to continue...";
                    getch();
            }
        }
    }
    
    void addFlight() {
        displayTitle();
        cout << "ADD NEW FLIGHT\n\n";
        
        Flight flight;
        
        cout << "Enter Flight Number: ";
        cin >> flight.flightNumber;
        
        // Check if flight number already exists
        for(const auto& f : flights) {
            if(f.flightNumber == flight.flightNumber) {
                cout << "\nError: Flight with this number already exists!\n";
                cout << "Press any key to continue...";
                getch();
                return;
            }
        }
        
        cin.ignore();
        cout << "Enter Source: ";
        getline(cin, flight.source);
        
        cout << "Enter Destination: ";
        getline(cin, flight.destination);
        
        cout << "Enter Source Time (HH:MM): ";
        getline(cin, flight.sourceTime);
        
        cout << "Enter Destination Time (HH:MM): ";
        getline(cin, flight.destinationTime);
        
            cout << "Enter Date (DD/MM/YYYY): ";
            getline(cin, flight.date);
            
            cout << "Enter Base Price: ";
            cin >> flight.basePrice;
            
            cin.ignore();
            cout << "Enter Via (If any, otherwise leave blank): ";
            getline(cin, flight.via);
            
            cout << "Enter Number of Stops: ";
            cin >> flight.stops;
            
            // Initialize seats
            for(int i = 1; i <= 30; i++) {
                flight.seats.push_back(Seat(i, ECONOMY));
            }
            for(int i = 31; i <= 40; i++) {
                flight.seats.push_back(Seat(i, BUSINESS));
            }
            
            flights.push_back(flight);
            saveData();
            
            cout << "\nFlight added successfully!\n";
            cout << "Press any key to continue...";
            getch();
        }
        
        void viewAllFlights() {
            displayTitle();
            cout << "ALL FLIGHTS\n\n";
            
            if(flights.empty()) {
                cout << "No flights available.\n";
                cout << "Press any key to continue...";
                getch();
                return;
            }
            
            cout << left << setw(10) << "Flight#" 
                 << setw(15) << "Source" 
                 << setw(15) << "Destination" 
                 << setw(10) << "Date" 
                 << setw(10) << "Price" 
                 << setw(10) << "Status" << endl;
            cout << string(70, '-') << endl;
            
            for(const auto& flight : flights) {
                string statusStr;
                switch(flight.status) {
                    case ON_TIME: statusStr = "On Time"; break;
                    case DELAYED: statusStr = "Delayed"; break;
                    case CANCELED: statusStr = "Canceled"; break;
                }
                
                cout << left << setw(10) << flight.flightNumber 
                     << setw(15) << flight.source 
                     << setw(15) << flight.destination 
                     << setw(10) << flight.date 
                     << setw(10) << flight.basePrice 
                     << setw(10) << statusStr << endl;
            }
            
            cout << "\nPress any key to continue...";
            getch();
        }
        
        void modifyFlight() {
            displayTitle();
            cout << "MODIFY FLIGHT\n\n";
            
            if(flights.empty()) {
                cout << "No flights available to modify.\n";
                cout << "Press any key to continue...";
                getch();
                return;
            }
            
            string flightNumber;
            cout << "Enter Flight Number to modify: ";
            cin >> flightNumber;
            
            for(auto& flight : flights) {
                if(flight.flightNumber == flightNumber) {
                    cin.ignore();
                    cout << "\nCurrent details:\n";
                    cout << "Source: " << flight.source << endl;
                    cout << "Destination: " << flight.destination << endl;
                    cout << "Source Time: " << flight.sourceTime << endl;
                    cout << "Destination Time: " << flight.destinationTime << endl;
                    cout << "Date: " << flight.date << endl;
                    cout << "Base Price: " << flight.basePrice << endl;
                    cout << "Via: " << flight.via << endl;
                    cout << "Stops: " << flight.stops << endl;
                    
                    cout << "\nEnter new details (press Enter to keep current):\n";
                    
                    string input;
                    
                    cout << "Source: ";
                    getline(cin, input);
                    if(!input.empty()) flight.source = input;
                    
                    cout << "Destination: ";
                    getline(cin, input);
                    if(!input.empty()) flight.destination = input;
                    
                    cout << "Source Time (HH:MM): ";
                    getline(cin, input);
                    if(!input.empty()) flight.sourceTime = input;
                    
                    cout << "Destination Time (HH:MM): ";
                    getline(cin, input);
                    if(!input.empty()) flight.destinationTime = input;
                    
                    cout << "Date (DD/MM/YYYY): ";
                    getline(cin, input);
                    if(!input.empty()) flight.date = input;
                    
                    cout << "Base Price: ";
                    getline(cin, input);
                    if(!input.empty()) flight.basePrice = stod(input);
                    
                    cout << "Via: ";
                    getline(cin, input);
                    if(!input.empty()) flight.via = input;
                    
                    cout << "Stops: ";
                    getline(cin, input);
                    if(!input.empty()) flight.stops = stoi(input);
                    
                    saveData();
                    cout << "\nFlight details modified successfully!\n";
                    cout << "Press any key to continue...";
                    getch();
                    return;
                }
            }
            
            cout << "\nFlight not found!\n";
            cout << "Press any key to continue...";
            getch();
        }
        
        void deleteFlight() {
            displayTitle();
            cout << "DELETE FLIGHT\n\n";
            
            if(flights.empty()) {
                cout << "No flights available to delete.\n";
                cout << "Press any key to continue...";
                getch();
                return;
            }
            
            string flightNumber;
            cout << "Enter Flight Number to delete: ";
            cin >> flightNumber;
            
            for(auto it = flights.begin(); it != flights.end(); ++it) {
                if(it->flightNumber == flightNumber) {
                    // Check if there are any bookings
                    if(!it->passengers.empty()) {
                        cout << "\nWarning: This flight has booked passengers. Are you sure you want to delete? (Y/N): ";
                        char confirm;
                        cin >> confirm;
                        
                        if(toupper(confirm) != 'Y') {
                            cout << "\nDeletion canceled.\n";
                            cout << "Press any key to continue...";
                            getch();
                            return;
                        }
                    }
                    
                    flights.erase(it);
                    saveData();
                    cout << "\nFlight deleted successfully!\n";
                    cout << "Press any key to continue...";
                    getch();
                    return;
                }
            }
            
            cout << "\nFlight not found!\n";
            cout << "Press any key to continue...";
            getch();
        }
        
        void updateFlightStatus() {
            displayTitle();
            cout << "UPDATE FLIGHT STATUS\n\n";
            
            if(flights.empty()) {
                cout << "No flights available to update.\n";
                cout << "Press any key to continue...";
                getch();
                return;
            }
            
            string flightNumber;
            cout << "Enter Flight Number: ";
            cin >> flightNumber;
            
            for(auto& flight : flights) {
                if(flight.flightNumber == flightNumber) {
                    cout << "\nCurrent Status: ";
                    switch(flight.status) {
                        case ON_TIME: cout << "On Time"; break;
                        case DELAYED: cout << "Delayed"; break;
                        case CANCELED: cout << "Canceled"; break;
                    }
                    
                    cout << "\n\nSelect new status:\n";
                    cout << "1. On Time\n";
                    cout << "2. Delayed\n";
                    cout << "3. Canceled\n";
                    cout << "Enter choice: ";
                    
                    char choice;
                    cin >> choice;
                    
                    switch(choice) {
                        case '1': flight.status = ON_TIME; break;
                        case '2': flight.status = DELAYED; break;
                        case '3': flight.status = CANCELED; break;
                        default:
                            cout << "\nInvalid choice. Status not updated.\n";
                            cout << "Press any key to continue...";
                            getch();
                            return;
                    }
                    
                    saveData();
                    cout << "\nFlight status updated successfully!\n";
                    cout << "Press any key to continue...";
                    getch();
                    return;
                }
            }
            
            cout << "\nFlight not found!\n";
            cout << "Press any key to continue...";
            getch();
        }
        
        void viewBookings() {
            displayTitle();
            cout << "VIEW BOOKINGS\n\n";
            
            if(flights.empty()) {
                cout << "No flights available.\n";
                cout << "Press any key to continue...";
                getch();
                return;
            }
            
            string flightNumber;
            cout << "Enter Flight Number (or 'all' to view all bookings): ";
            cin >> flightNumber;
            
            if(flightNumber == "all") {
                bool hasBookings = false;
                
                for(const auto& flight : flights) {
                    if(!flight.passengers.empty()) {
                        hasBookings = true;
                        cout << "\nFlight: " << flight.flightNumber << " (" << flight.source << " to " << flight.destination << ")\n";
                        cout << string(50, '-') << endl;
                        cout << left << setw(20) << "Name" 
                             << setw(15) << "Phone" 
                             << setw(10) << "Seat" 
                             << setw(15) << "Ticket#" << endl;
                        cout << string(60, '-') << endl;
                        
                        for(const auto& passenger : flight.passengers) {
                            cout << left << setw(20) << passenger.name 
                                 << setw(15) << passenger.phone 
                                 << setw(10) << passenger.seatNumber 
                                 << setw(15) << passenger.ticketNumber << endl;
                        }
                        cout << endl;
                    }
                }
                
                if(!hasBookings) {
                    cout << "\nNo bookings found across all flights.\n";
                }
            } else {
                bool found = false;
                
                for(const auto& flight : flights) {
                    if(flight.flightNumber == flightNumber) {
                        found = true;
                        
                        if(flight.passengers.empty()) {
                            cout << "\nNo bookings found for this flight.\n";
                        } else {
                            cout << "\nFlight: " << flight.flightNumber << " (" << flight.source << " to " << flight.destination << ")\n";
                            cout << string(50, '-') << endl;
                            cout << left << setw(20) << "Name" 
                                 << setw(15) << "Phone" 
                                 << setw(10) << "Seat" 
                                 << setw(15) << "Ticket#" << endl;
                            cout << string(60, '-') << endl;
                            
                            for(const auto& passenger : flight.passengers) {
                                cout << left << setw(20) << passenger.name 
                                     << setw(15) << passenger.phone 
                                     << setw(10) << passenger.seatNumber 
                                     << setw(15) << passenger.ticketNumber << endl;
                            }
                        }
                        break;
                    }
                }
                
                if(!found) {
                    cout << "\nFlight not found!\n";
                }
            }
            
            cout << "\nPress any key to continue...";
            getch();
        }
        
        void userDashboard() {
            while(true) {
                displayTitle();
                cout << "USER DASHBOARD\n\n";
                cout << "1. Book Flight Ticket\n";
                cout << "2. View Flight Schedule\n";
                cout << "3. Cancel Ticket\n";
                cout << "4. View Booking History\n";
                cout << "5. Reprint Ticket\n";
                cout << "6. Log Out\n\n";
                cout << "Select an option: ";
                
                char choice;
                cin >> choice;
                
                switch(choice) {
                    case '1':
                        bookFlightTicket();
                        break;
                    case '2':
                        viewFlightSchedule();
                        break;
                    case '3':
                        cancelTicket();
                        break;
                    case '4':
                        viewBookingHistory();
                        break;
                    case '5':
                        reprintTicket();
                        break;
                    case '6':
                        return;
                    default:
                        cout << "\nInvalid option. Please try again.\n";
                        cout << "Press any key to continue...";
                        getch();
                }
            }
        }
        
        void bookFlightTicket() {
            displayTitle();
            cout << "BOOK FLIGHT TICKET\n\n";
            
            if(flights.empty()) {
                cout << "No flights available for booking.\n";
                cout << "Press any key to continue...";
                getch();
                return;
            }
            
            cin.ignore();
            string source, destination;
            
            cout << "Enter Source: ";
            getline(cin, source);
            
            cout << "Enter Destination: ";
            getline(cin, destination);
            
            vector<Flight*> matchingFlights;
            
            for(auto& flight : flights) {
                if(flight.source == source && flight.destination == destination && flight.status != CANCELED) {
                    matchingFlights.push_back(&flight);
                }
            }
            
            if(matchingFlights.empty()) {
                cout << "\nNo flights found for the specified route.\n";
                cout << "Press any key to continue...";
                getch();
                return;
            }
            
            cout << "\nAvailable Flights:\n";
            cout << left << setw(5) << "No." 
                 << setw(10) << "Flight#" 
                 << setw(15) << "Date" 
                 << setw(10) << "Time" 
                 << setw(10) << "Price" 
                 << setw(10) << "Status" << endl;
            cout << string(60, '-') << endl;
            
            for(size_t i = 0; i < matchingFlights.size(); ++i) {
                string statusStr;
                switch(matchingFlights[i]->status) {
                    case ON_TIME: statusStr = "On Time"; break;
                    case DELAYED: statusStr = "Delayed"; break;
                    case CANCELED: statusStr = "Canceled"; break;
                }
                
                cout << left << setw(5) << (i + 1)
                     << setw(10) << matchingFlights[i]->flightNumber
                     << setw(15) << matchingFlights[i]->date
                     << setw(10) << matchingFlights[i]->sourceTime
                     << setw(10) << matchingFlights[i]->basePrice
                     << setw(10) << statusStr << endl;
            }
            
            int selection;
            cout << "\nEnter flight number to book (1-" << matchingFlights.size() << "): ";
            cin >> selection;
            
            if(selection < 1 || selection > static_cast<int>(matchingFlights.size())) {
                cout << "\nInvalid selection!\n";
                cout << "Press any key to continue...";
                getch();
                return;
            }
            
            Flight* selectedFlight = matchingFlights[selection - 1];
            
            // Display seat map
            displaySeatMap(*selectedFlight);
            
            int seatNumber;
            cout << "\nEnter seat number to book: ";
            cin >> seatNumber;
            
                if(seatNumber < 1 || seatNumber > static_cast<int>(selectedFlight->seats.size())) {
                    cout << "\nInvalid seat number!\n";
                    cout << "Press any key to continue...";
                    getch();
                    return;
                }
                
                if(!selectedFlight->isSeatAvailable(seatNumber)) {
                    cout << "\nThis seat is already booked. Please select another seat.\n";
                    cout << "Press any key to continue...";
                    getch();
                    return;
                }
                
                // Get passenger details
                cin.ignore();
                string name, email, phone;
                
                cout << "\nEnter passenger details:\n";
                cout << "Name: ";
                getline(cin, name);
                
                cout << "Email: ";
                getline(cin, email);
                
                cout << "Phone: ";
                getline(cin, phone);
                
                // Check for duplicate booking
                for(const auto& passenger : selectedFlight->passengers) {
                    if(passenger.phone == phone && passenger.name == name) {
                        cout << "\nDuplicate booking detected! You have already booked a ticket on this flight.\n";
                        cout << "Press any key to continue...";
                        getch();
                        return;
                    }
                }
                
                // Calculate ticket price with dynamic pricing
                double seatPrice = selectedFlight->getSeatPrice(seatNumber);
                
                // Confirm booking
                cout << "\nBooking Summary:\n";
                cout << "Flight: " << selectedFlight->flightNumber << endl;
                cout << "Route: " << selectedFlight->source << " to " << selectedFlight->destination << endl;
                cout << "Date: " << selectedFlight->date << endl;
                cout << "Time: " << selectedFlight->sourceTime << endl;
                cout << "Seat: " << seatNumber << " (" << selectedFlight->getSeatType(seatNumber) << ")" << endl;
                cout << "Price: Rs. " << fixed << setprecision(2) << seatPrice << endl;
                
                cout << "\nConfirm booking? (Y/N): ";
                char confirm;
                cin >> confirm;
                
                if(toupper(confirm) != 'Y') {
                    cout << "\nBooking canceled.\n";
                    cout << "Press any key to continue...";
                    getch();
                    return;
                }
                
                // Create passenger and add to flight
                Passenger newPassenger(name, email, phone, seatNumber);
                selectedFlight->passengers.push_back(newPassenger);
                selectedFlight->markSeatBooked(seatNumber);
                
                // Add to booking history
                if(bookingHistory.find(phone) == bookingHistory.end()) {
                    bookingHistory[phone] = vector<string>();
                }
                bookingHistory[phone].push_back(newPassenger.ticketNumber);
                
                saveData();
                
                // Print ticket
                generateTicket(*selectedFlight, newPassenger);
                
                cout << "\nBooking successful! Ticket has been saved.\n";
                cout << "Press any key to continue...";
                getch();
            }
            
            void displaySeatMap(const Flight& flight) {
                displayTitle();
                cout << "SEAT MAP - Flight " << flight.flightNumber << "\n\n";
                
                cout << "BUSINESS CLASS:\n";
                cout << "---------------------------\n";
                for(int i = 31; i <= 40; i++) {
                    bool isAvailable = flight.isSeatAvailable(i);
                    cout << "[" << (isAvailable ? " " : "X") << "] " << i << "\t";
                    if((i - 30) % 5 == 0) cout << endl;
                }
                
                cout << "\nECONOMY CLASS:\n";
                cout << "---------------------------\n";
                for(int i = 1; i <= 30; i++) {
                    bool isAvailable = flight.isSeatAvailable(i);
                    cout << "[" << (isAvailable ? " " : "X") << "] " << (i < 10 ? " " : "") << i << "\t";
                    if(i % 6 == 0) cout << endl;
                }
                
                cout << "\nLegend: [ ] - Available  [X] - Booked\n";
            }
            
            void generateTicket(const Flight& flight, const Passenger& passenger) {
                // Generate ticket file
                string filename = "ticket_" + passenger.ticketNumber + ".txt";
                ofstream ticketFile(filename);
                
                if(!ticketFile.is_open()) {
                    cout << "\nError: Could not generate ticket file!\n";
                    return;
                }
                
                ticketFile << "=============================================\n";
                ticketFile << "            HAWAII JAHAJH DE TICKET          \n";
                ticketFile << "=============================================\n\n";
                ticketFile << "TICKET NUMBER: " << passenger.ticketNumber << "\n\n";
                ticketFile << "PASSENGER INFORMATION:\n";
                ticketFile << "Name: " << passenger.name << "\n";
                ticketFile << "Phone: " << passenger.phone << "\n";
                ticketFile << "Email: " << passenger.email << "\n\n";
                ticketFile << "FLIGHT INFORMATION:\n";
                ticketFile << "Flight Number: " << flight.flightNumber << "\n";
                ticketFile << "From: " << flight.source << "\n";
                ticketFile << "To: " << flight.destination << "\n";
                if(!flight.via.empty()) {
                    ticketFile << "Via: " << flight.via << "\n";
                }
                ticketFile << "Date: " << flight.date << "\n";
                ticketFile << "Departure Time: " << flight.sourceTime << "\n";
                ticketFile << "Arrival Time: " << flight.destinationTime << "\n";
                ticketFile << "Seat Number: " << passenger.seatNumber << " (" << flight.getSeatType(passenger.seatNumber) << ")\n\n";
                ticketFile << "=============================================\n";
                ticketFile << "         THANK YOU FOR FLYING WITH US!       \n";
                ticketFile << "=============================================\n";
                
                ticketFile.close();
                
                cout << "\nTicket saved as " << filename << endl;
            }
            
            void viewFlightSchedule() {
                displayTitle();
                cout << "FLIGHT SCHEDULE\n\n";
                
                if(flights.empty()) {
                    cout << "No flights available.\n";
                    cout << "Press any key to continue...";
                    getch();
                    return;
                }
                
                // Filter options
                cout << "Search Filters (leave blank to show all):\n";
                cin.ignore();
                
                string sourceFilter, destFilter, dateFilter;
                double maxPrice = -1;
                
                cout << "Source: ";
                getline(cin, sourceFilter);
                
                cout << "Destination: ";
                getline(cin, destFilter);
                
                cout << "Date (DD/MM/YYYY): ";
                getline(cin, dateFilter);
                
                string priceFilterStr;
                cout << "Max Price (leave blank for no limit): ";
                getline(cin, priceFilterStr);
                
                if(!priceFilterStr.empty()) {
                    maxPrice = stod(priceFilterStr);
                }
                
                vector<Flight*> filteredFlights;
                
                for(auto& flight : flights) {
                    bool matchesSource = sourceFilter.empty() || flight.source == sourceFilter;
                    bool matchesDest = destFilter.empty() || flight.destination == destFilter;
                    bool matchesDate = dateFilter.empty() || flight.date == dateFilter;
                    bool matchesPrice = maxPrice < 0 || flight.basePrice <= maxPrice;
                    
                    if(matchesSource && matchesDest && matchesDate && matchesPrice) {
                        filteredFlights.push_back(&flight);
                    }
                }
                
                if(filteredFlights.empty()) {
                    cout << "\nNo flights match your search criteria.\n";
                    cout << "Press any key to continue...";
                    getch();
                    return;
                }
                
                cout << "\nMatching Flights:\n";
                cout << left << setw(10) << "Flight#" 
                     << setw(15) << "Source" 
                     << setw(15) << "Destination" 
                     << setw(10) << "Date" 
                     << setw(10) << "Time" 
                     << setw(10) << "Price" 
                     << setw(10) << "Status" << endl;
                cout << string(80, '-') << endl;
                
                for(const auto& flight : filteredFlights) {
                    string statusStr;
                    switch(flight->status) {
                        case ON_TIME: statusStr = "On Time"; break;
                        case DELAYED: statusStr = "Delayed"; break;
                        case CANCELED: statusStr = "Canceled"; break;
                    }
                    
                    cout << left << setw(10) << flight->flightNumber 
                         << setw(15) << flight->source 
                         << setw(15) << flight->destination 
                         << setw(10) << flight->date 
                         << setw(10) << flight->sourceTime 
                         << setw(10) << flight->basePrice 
                         << setw(10) << statusStr << endl;
                }
                
                cout << "\nPress any key to continue...";
                getch();
            }
            
            void cancelTicket() {
                displayTitle();
                cout << "CANCEL TICKET\n\n";
                
                string ticketNumber;
                cout << "Enter Ticket Number: ";
                cin >> ticketNumber;
                
                for(auto& flight : flights) {
                    for(auto it = flight.passengers.begin(); it != flight.passengers.end(); ++it) {
                        if(it->ticketNumber == ticketNumber) {
                            // Confirm cancellation
                            cout << "\nTicket Details:\n";
                            cout << "Passenger: " << it->name << endl;
                            cout << "Flight: " << flight.flightNumber << " (" << flight.source << " to " << flight.destination << ")" << endl;
                            cout << "Date: " << flight.date << endl;
                            cout << "Seat: " << it->seatNumber << endl;
                            
                            cout << "\nAre you sure you want to cancel this ticket? (Y/N): ";
                            char confirm;
                            cin >> confirm;
                            
                            if(toupper(confirm) != 'Y') {
                                cout << "\nCancellation aborted.\n";
                                cout << "Press any key to continue...";
                                getch();
                                return;
                            }
                            
                            // Remove from booking history
                            if(bookingHistory.find(it->phone) != bookingHistory.end()) {
                                auto& tickets = bookingHistory[it->phone];
                                tickets.erase(remove(tickets.begin(), tickets.end(), ticketNumber), tickets.end());
                            }
                            
                            // Free up the seat
                            flight.markSeatAvailable(it->seatNumber);
                            
                            // Remove passenger
                            flight.passengers.erase(it);
                            
                            saveData();
                            
                            cout << "\nTicket canceled successfully!\n";
                            cout << "Press any key to continue...";
                            getch();
                            return;
                        }
                    }
                }
                
                cout << "\nTicket not found!\n";
                cout << "Press any key to continue...";
                getch();
            }
            
            void viewBookingHistory() {
                displayTitle();
                cout << "VIEW BOOKING HISTORY\n\n";
                
                string phone;
                cout << "Enter Phone Number: ";
                cin >> phone;
                
                if(bookingHistory.find(phone) == bookingHistory.end() || bookingHistory[phone].empty()) {
                    cout << "\nNo booking history found for this phone number.\n";
                    cout << "Press any key to continue...";
                    getch();
                    return;
                }
                
                cout << "\nBooking History for " << phone << ":\n";
                cout << string(60, '-') << endl;
                cout << left << setw(15) << "Ticket#" 
                     << setw(10) << "Flight#" 
                     << setw(15) << "Route" 
                     << setw(10) << "Date" 
                     << setw(10) << "Status" << endl;
                cout << string(60, '-') << endl;
                
                for(const auto& ticketNum : bookingHistory[phone]) {
                    bool found = false;
                    
                    for(const auto& flight : flights) {
                        for(const auto& passenger : flight.passengers) {
                            if(passenger.ticketNumber == ticketNum) {
                                found = true;
                                
                                string statusStr;
                                switch(flight.status) {
                                    case ON_TIME: statusStr = "On Time"; break;
                                    case DELAYED: statusStr = "Delayed"; break;
                                    case CANCELED: statusStr = "Canceled"; break;
                                }
                                
                                cout << left << setw(15) << ticketNum 
                                     << setw(10) << flight.flightNumber 
                                     << setw(15) << (flight.source + "-" + flight.destination) 
                                     << setw(10) << flight.date 
                                     << setw(10) << statusStr << endl;
                                
                                break;
                            }
                        }
                        
                        if(found) break;
                    }
                    
                    if(!found) {
                        cout << left << setw(15) << ticketNum 
                             << setw(10) << "N/A" 
                             << setw(15) << "N/A" 
                             << setw(10) << "N/A" 
                             << setw(10) << "Canceled" << endl;
                    }
                }
                
                cout << "\nPress any key to continue...";
                getch();
            }
            
            void reprintTicket() {
                displayTitle();
                cout << "REPRINT TICKET\n\n";
                
                string ticketNumber;
                cout << "Enter Ticket Number: ";
                cin >> ticketNumber;
                
                for(const auto& flight : flights) {
                    for(const auto& passenger : flight.passengers) {
                        if(passenger.ticketNumber == ticketNumber) {
                            generateTicket(flight, passenger);
                            cout << "\nTicket has been reprinted.\n";
                            cout << "Press any key to continue...";
                            getch();
                            return;
                        }
                    }
                }
                
                cout << "\nTicket not found!\n";
                cout << "Press any key to continue...";
                getch();
            }
        };
        
        int main() {
            FlightBookingSystem system;
            system.mainMenu();
            return 0;
        }