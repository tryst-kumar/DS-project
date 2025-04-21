#include <iostream>
#include <fstream>
#include <unordered_map>
#include <vector>
#include <string>
#include <algorithm>
#include <stack>
#include <queue>
#include <sstream>

struct Showtime {
    std::string time;
    std::string cinema;
    std::vector<std::vector<bool>> seats;  // 2D array for seat availability
};
 
struct Movie {
    int id;  // Movie ID
    std::string title;
    std::string genre;
    std::string director;
    int releaseYear;
    double rating;
    std::vector<std::string> actors;  // List of actors
    std::vector<Showtime> showtimes;  // Multiple showtimes for the movie
};

class MovieBookingSystem {
private:
    std::unordered_map<int, Movie> movies;  // Movie map (keyed by Movie ID)
    std::vector<std::string> bookingHistory;  // Booking history
    std::stack<std::string> bookingUndoHistory;  // For canceling bookings

    void saveBookingToFile(const std::string& booking) {
        std::ofstream outFile("bookings.txt", std::ios::app);
        outFile << booking << "\n";
    }

    void saveMoviesToFile() {
        std::ofstream outFile("movies.txt");
        outFile << "MovieID|Title|Genre|Director|ReleaseYear|Rating|Actors\n";  // Header
        for (const auto& pair : movies) {
            const Movie& movie = pair.second;
            outFile << movie.id << "|" << movie.title << "|" << movie.genre << "|" 
                    << movie.director << "|" << movie.releaseYear << "|" 
                    << movie.rating << "|";
            for (size_t i = 0; i < movie.actors.size(); ++i) {
                outFile << movie.actors[i];
                if (i < movie.actors.size() - 1) {
                    outFile << ",";
                }
            }
            outFile << "\n";
        }
    }

public:
    void loadMovies() {
        std::ifstream inFile("movies.txt");
        if (!inFile) {
            std::cerr << "Error: movies.txt not found.\n";
            return;
        }

        std::string line;
        std::getline(inFile, line);  // Skip the header line
        while (std::getline(inFile, line)) {
            std::istringstream ss(line);
            std::string idStr, title, genre, director, releaseYearStr, ratingStr, actorsStr;

            // Parse the line
            if (!std::getline(ss, idStr, '|') || !std::getline(ss, title, '|') ||
                !std::getline(ss, genre, '|') || !std::getline(ss, director, '|') ||
                !std::getline(ss, releaseYearStr, '|') || !std::getline(ss, ratingStr, '|') ||
                !std::getline(ss, actorsStr)) {
                std::cerr << "Error: Invalid line format in movies.txt: " << line << "\n";
                continue;  // Skip invalid lines
            }

            try {
                Movie movie;
                movie.id = std::stoi(idStr);
                movie.title = title;
                movie.genre = genre;
                movie.director = director;
                movie.releaseYear = std::stoi(releaseYearStr);
                movie.rating = std::stod(ratingStr);

                // Parse actors (comma-separated)
                std::istringstream actorsStream(actorsStr);
                std::string actor;
                while (std::getline(actorsStream, actor, ',')) {
                    movie.actors.push_back(actor);
                }

                // Add default showtimes with the specified timings
                movie.showtimes = {
                    {"10:00 AM", "Cinema 1", std::vector<std::vector<bool>>(5, std::vector<bool>(5, false))},
                    {"2:00 PM", "Cinema 1", std::vector<std::vector<bool>>(5, std::vector<bool>(5, false))},
                    {"5:30 PM", "Cinema 1", std::vector<std::vector<bool>>(5, std::vector<bool>(5, false))},
                    {"8:30 PM", "Cinema 1", std::vector<std::vector<bool>>(5, std::vector<bool>(5, false))}
                };

                movies[movie.id] = movie;
            } catch (const std::invalid_argument& e) {
                std::cerr << "Error: Invalid data in movies.txt: " << line << "\n";
            } catch (const std::out_of_range& e) {
                std::cerr << "Error: Out of range value in movies.txt: " << line << "\n";
            }
        }
    }

    void addMovie() {
        Movie movie;
        std::cout << "Enter movie ID: ";
        std::cin >> movie.id;
        std::cin.ignore();  // Clear input buffer
        std::cout << "Enter movie title: ";
        std::getline(std::cin, movie.title);
        std::cout << "Enter genre: ";
        std::getline(std::cin, movie.genre);
        std::cout << "Enter director: ";
        std::getline(std::cin, movie.director);
        std::cout << "Enter release year: ";
        std::cin >> movie.releaseYear;
        std::cout << "Enter rating: ";
        std::cin >> movie.rating;
        std::cin.ignore();  // Clear input buffer
        std::cout << "Enter actors (comma-separated): ";
        std::string actorsStr;
        std::getline(std::cin, actorsStr);

        // Parse actors
        std::istringstream actorsStream(actorsStr);
        std::string actor;
        while (std::getline(actorsStream, actor, ',')) {
            movie.actors.push_back(actor);
        }

        // Add default showtimes with the specified timings
        movie.showtimes = {
            {"10:00 AM", "Cinema 1", std::vector<std::vector<bool>>(5, std::vector<bool>(5, false))},
            {"2:00 PM", "Cinema 1", std::vector<std::vector<bool>>(5, std::vector<bool>(5, false))},
            {"5:30 PM", "Cinema 1", std::vector<std::vector<bool>>(5, std::vector<bool>(5, false))},
            {"8:30 PM", "Cinema 1", std::vector<std::vector<bool>>(5, std::vector<bool>(5, false))}
        };

        movies[movie.id] = movie;
        saveMoviesToFile();
        std::cout << "Movie added successfully.\n";
    }

    void removeMovie() {
        int id;
        std::cout << "Enter movie ID to remove: ";
        std::cin >> id;

        if (movies.erase(id)) {
            saveMoviesToFile();
            std::cout << "Movie removed successfully.\n";
        } else {
            std::cout << "Movie not found.\n";
        }
    }

    void listMovies(bool sortByRating = false, const std::string& genreFilter = "") {
        std::vector<Movie> movieList;
        for (auto& pair : movies) {
            const Movie& movie = pair.second;
            if (genreFilter.empty() || movie.genre == genreFilter) {
                movieList.push_back(movie);
            }
        }

        if (sortByRating) {
            std::sort(movieList.begin(), movieList.end(), [](const Movie& a, const Movie& b) {
                return a.rating > b.rating;
            });
        }

        for (const auto& movie : movieList) {
            std::cout << "ID: " << movie.id << ", Title: " << movie.title << ", Genre: " << movie.genre
                      << ", Director: " << movie.director << ", Release Year: " << movie.releaseYear
                      << ", Rating: " << movie.rating << ", Actors: ";
            for (size_t i = 0; i < movie.actors.size(); ++i) {
                std::cout << movie.actors[i];
                if (i < movie.actors.size() - 1) {
                    std::cout << ", ";
                }
            }
            std::cout << "\n";
        }
    }

    void showMovieDetails(const std::string& title) {
        auto it = std::find_if(movies.begin(), movies.end(), [&title](const auto& pair) {
            return pair.second.title == title;
        });

        if (it != movies.end()) {
            const Movie& m = it->second;
            std::cout << "ID: " << m.id << ", Title: " << m.title << ", Genre: " << m.genre
                      << ", Director: " << m.director << ", Release Year: " << m.releaseYear
                      << ", Rating: " << m.rating << ", Actors: ";
            for (size_t i = 0; i < m.actors.size(); ++i) {
                std::cout << m.actors[i];
                if (i < m.actors.size() - 1) {
                    std::cout << ", ";
                }
            }
            std::cout << "\n";
            for (size_t i = 0; i < m.showtimes.size(); ++i) {
                std::cout << i + 1 << ". Time: " << m.showtimes[i].time << ", Cinema: " << m.showtimes[i].cinema << "\n";
            }
        } else {
            std::cout << "Movie not found.\n";
        }
    }

    void displaySeats(const std::string& title, int showtimeIndex) {
        auto it = std::find_if(movies.begin(), movies.end(), [&title](const auto& pair) {
            return pair.second.title == title;
        });

        if (it != movies.end()) {
            auto& seats = it->second.showtimes[showtimeIndex].seats;
            for (int i = 0; i < seats.size(); ++i) {
                for (int j = 0; j < seats[i].size(); ++j) {
                    std::cout << (seats[i][j] ? "[X]" : "[ ]");
                }
                std::cout << "\n";
            }
        }
    }

    void bookSeat(const std::string& title, int showtimeIndex, int row, int col) {
        auto it = std::find_if(movies.begin(), movies.end(), [&title](const auto& pair) {
            return pair.second.title == title;
        });

        if (it != movies.end()) {
            Movie& m = it->second;
            Showtime& s = m.showtimes[showtimeIndex];
            if (!s.seats[row][col]) {
                s.seats[row][col] = true;
                std::string booking = "Movie: " + title + ", Cinema: " + s.cinema + ", Time: " + s.time + ", Seat: (" + std::to_string(row) + "," + std::to_string(col) + ")";
                bookingHistory.push_back(booking);
                saveBookingToFile(booking);
                std::cout << "Booking successful: " << booking << "\n";
            } else {
                std::cout << "Seat already booked.\n";
            }
        }
    }

    void cancelBooking() {
        if (!bookingHistory.empty()) {
            std::string lastBooking = bookingHistory.back();
            bookingHistory.pop_back();
            bookingUndoHistory.push(lastBooking);  // Store the canceled booking for potential undo
            std::cout << "Booking canceled: " << lastBooking << "\n";
        } else {
            std::cout << "No bookings to cancel.\n";
        }
    }

    void viewBookingHistory() {
        std::cout << "\nBooking History:\n";
        for (const std::string& b : bookingHistory) {
            std::cout << b << "\n";
        }
    }

    void adminStats() {
        std::cout << "\nTotal Movies: " << movies.size() << "\n";
        int totalBookings = 0;
        for (auto& pair : movies) {
            for (auto& s : pair.second.showtimes) {
                for (auto& row : s.seats) {
                    totalBookings += std::count(row.begin(), row.end(), true);
                }
            }
        }
        std::cout << "Total Bookings: " << totalBookings << "\n";
    }
};

int main() {
    MovieBookingSystem system;
    system.loadMovies();

    int choice;
    do {
        std::cout << "\n1. List Movies\n2. Show Movie Details\n3. Book Seat\n4. View Booking History\n5. Admin Stats\n6. Cancel Booking\n7. Add Movie\n8. Remove Movie\n9. Exit\nChoice: ";
        std::cin >> choice;
        std::cin.ignore();  // Clear input buffer
        if (choice == 1) {
            bool sortByRating;
            std::cout << "Sort by Rating (1 for Yes, 0 for No): ";
            std::cin >> sortByRating;
            std::cin.ignore();  // To clear the buffer after reading the bool
            std::string filterGenre;
            std::cout << "Enter genre to filter by (leave empty to skip): ";
            std::getline(std::cin, filterGenre);
            system.listMovies(sortByRating, filterGenre);
        } else if (choice == 2) {
            std::string title;
            std::cout << "Enter movie title: ";
            std::getline(std::cin, title);
            system.showMovieDetails(title);
        } else if (choice == 3) {
            std::string title;
            int showtimeIndex, row, col;
            std::cout << "Enter movie title: ";
            std::getline(std::cin, title);
            system.showMovieDetails(title);
            std::cout << "Enter showtime index: ";
            std::cin >> showtimeIndex;
            system.displaySeats(title, showtimeIndex - 1);
            std::cout << "Enter row and column to book: ";
            std::cin >> row >> col;
            system.bookSeat(title, showtimeIndex - 1, row, col);
        } else if (choice == 4) {
            system.viewBookingHistory();
        } else if (choice == 5) {
            system.adminStats();
        } else if (choice == 6) {
            system.cancelBooking();
        } else if (choice == 7) {
            system.addMovie();
        } else if (choice == 8) {
            system.removeMovie();
        }
    } while (choice != 9);

    return 0;
}
