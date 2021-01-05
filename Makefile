all: bitflips

bitflips: bitflips.cpp
	$(CXX) -Wall -Wextra -Wshadow -Wpedantic -march=native -O3 $< -o $@

clean:
	rm -f bitflips