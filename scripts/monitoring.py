import os
import re
import time

# Directory where the log files are located
log_dir = 'harness/'

# Interval in seconds for checking the files
check_interval = 6

def get_latest_files(directory):
    # Regular expression pattern to match the file names
    pattern = re.compile(r"SIMULATIONS_(\d+)_(\d+).log")

    # Dictionary to hold the latest files for each simulation number
    latest_files = {}

    # Loop through files in the directory
    for filename in os.listdir(directory):
        matched = pattern.match(filename)
        if matched:
            simulation_number = int(matched.group(1))
            type_number = int(matched.group(2))
            # Check if this type number is the highest so far for this simulation number
            if simulation_number not in latest_files or (type_number > latest_files[simulation_number][1]):
                # Store the filename and type number in the dictionary
                latest_files[simulation_number] = (filename, type_number)

    return latest_files

def print_last_lines(latest_files, directory):
    print("==============================")
    for simulation_number, (filename, type_number) in latest_files.items():
        filepath = os.path.join(directory, filename)
        try:
            with open(filepath, 'r') as f:
                lines = f.readlines()
                last_line = lines[-1].strip() if lines else ""
                print(f"Simulation \t{simulation_number} \tTry {type_number} \tLast Line: {last_line}")
        except Exception as e:
            print(f"Error reading file {filepath}: {e}")

def main():
    while True:
        latest_files = get_latest_files(log_dir)
        print_last_lines(latest_files, log_dir)
        time.sleep(check_interval)

if __name__ == "__main__":
    main()