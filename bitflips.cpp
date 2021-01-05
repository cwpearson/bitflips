#include <chrono>
#include <cstdint>
#include <cstring>
#include <thread>
#include <iostream>
#include <string>
#include <fstream>

typedef std::chrono::system_clock Clock;
typedef std::chrono::duration<double> Duration;
typedef std::chrono::time_point<Clock, Duration> Time;

const uint8_t PATTERN = 0xAA;
const Duration PERIOD(3600); // seconds between checks

void set_bytes(volatile uint8_t *bytes, std::size_t n)
{
    std::memset((void *)bytes, int(0xAA), n);
}

int main(int argc, char **argv)
{

    std::ofstream fout("counts.csv", std::fstream::app);
    if (!fout.is_open())
    {
        std::cerr << "couldn't open counts.csv\n";
        exit(1);
    }

    std::size_t n = 2ull * 1024ull * 1024ull * 1024ull;

    volatile uint8_t *bytes = new uint8_t[n];

    set_bytes(bytes, n);
    Time checked = Clock::now();
    Time next = checked;

    while (true)
    {
        std::this_thread::sleep_until(next);
        next = Clock::now() + PERIOD;

        std::size_t count = 0;
        for (std::size_t i = 0; i < n; ++i)
        {
            if (bytes[i] != PATTERN)
            {
                ++count;
                bytes[i] = PATTERN;
            }
        }
        Time now = Clock::now();
        Duration dur = now - checked;
        checked = now;

        std::time_t time = Clock::to_time_t(Clock::now());
        std::string stime = std::ctime(&time);
        stime.erase(stime.end() - 1); // remove newline
        std::cout << stime << ": " << count << " flips, " << double(count) / dur.count() << " flips/s\n";
        fout << stime << "," << count << "," << double(count) / dur.count() << "\n";
        fout << std::flush;
        if (fout.fail())
        {
            std::cerr << "fail() after write to counts.csv\n";
            exit(1);
        }
        if (fout.bad())
        {
            std::cerr << "bad() after write to counts.csv\n";
            exit(1);
        }
    }

    fout.close();

    delete[] bytes;
}