#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <iomanip>

class Music {
private:

    struct Song {
        std::string ID;
        std::string artist_name;
        std::string title;
        std::string release_name;
        int year;
        double end_of_fade_in;
        double start_of_fade_out;
        double loudness;
    };

    std::vector<Song> songs;

public:
    void loadFromFile(const std::string& filename) {
        std::ifstream file(filename);

        if (!file.is_open()) {
            std::cerr << "Error opening file " << filename << std::endl;
            return;
        }

        std::string line;
        std::getline(file, line);

        while (std::getline(file, line)) {
            std::stringstream ss(line);
            Song song;
            std::getline(ss, song.ID, ',');
            std::getline(ss, song.artist_name, ',');
            std::getline(ss, song.title, ',');
            std::getline(ss, song.release_name, ',');
            std::string yearStr, endFadeInStr, startFadeOutStr, loudnessStr;
            std::getline(ss, yearStr, ',');
            std::getline(ss, endFadeInStr, ',');
            std::getline(ss, startFadeOutStr, ',');
            std::getline(ss, loudnessStr, ',');

            song.year = std::stoi(yearStr);
            song.end_of_fade_in = std::stod(endFadeInStr);
            song.start_of_fade_out = std::stod(startFadeOutStr);
            song.loudness = std::stod(loudnessStr);

            songs.push_back(song);
        }


        file.close();
    }

    void generateReport() {
        std::cout << std::left << std::setw(20) << "ID"
                  << std::setw(20) << "Artist Name"
                  << std::setw(20) << "Title"
                  << std::setw(20) << "Release Name"
                  << std::setw(6) << "Year"
                  << std::setw(15) << "End of Fade In"
                  << std::setw(15) << "Start of Fade Out"
                  << std::setw(10) << "Loudness"
                  << std::setw(15) << "Duration" << std::endl;
        std::cout << "--------------------------------------------------------------"
                     "----------------------------------------------------------------------------" << std::endl;

        double totalDuration = 0;
        for (const auto& song : songs) {
            double duration = song.start_of_fade_out - song.end_of_fade_in;
            totalDuration += duration;

            std::cout << std::left << std::setw(20) << formatID(song.ID)
                      << std::setw(20) << truncateStr(song.artist_name)
                      << std::setw(20) << truncateStr(song.title)
                      << std::setw(20) << truncateStr(song.release_name)
                      << std::setw(6) << song.year
                      << std::setw(15) << std::fixed << std::setprecision(3) << song.end_of_fade_in
                      << std::setw(15) << std::fixed << std::setprecision(3) << song.start_of_fade_out
                      << std::setw(10) << std::fixed << std::setprecision(3) << song.loudness
                      << std::setw(15) << formatTime(duration) << std::endl;
        }

        std::cout << "------------------------------------------------------------------"
                     "-------------------------------------------------------------------------" << std::endl;

        std::cout << "Total Songs: " << songs.size() << std::endl;
        std::cout << "Average duration: " << formatTime(totalDuration / songs.size()) << std::endl;
        std::cout << "Total duration: " << formatTime(totalDuration) << std::endl;
    }

private:
    std::string truncateStr(std::string str) {
        if (str.length() <= 20) {
            return str;
        } else {
            return str.substr(0, 17) + "...";
        }
    }

    std::string formatID(std::string id) {
        if (id.length() == 18) {
            return id.substr(0, 7) + "-" + id.substr(7, 2) + "-" + id.substr(9);
        }
        return  id;
    }

    std::string formatTime(double duration) {
        int hours = static_cast<int>(duration) / 3600;
        int minutes = (static_cast<int>(duration) % 3600) / 60;
        int seconds = static_cast<int>(duration) % 60;

        std::ostringstream formatted;
        formatted << std::setw(3) << std::setfill('0') << hours << ":"
                  << std::setw(2) << std::setfill('0') << minutes << ":"
                  << std::setw(2) << std::setfill('0') << seconds;
        return formatted.str();
    }
};

int main() {
    Music music;
    music.loadFromFile("CORGIS_part.txt");
    music.generateReport();

    return 0;
}
