# For data version == 3

""" This program process data collected by the kit. """

import os
import numpy
import datetime

path = input("Input the path name: ")
csv_filename = path + "/data.csv"

base_time = datetime.datetime.strptime(input("Input the start time (YYYY-MM-DD HH:MM:SS): "), "%Y-%m-%d %H:%M:%S")

data_list = ["Time",
             "Humidity", "Temperature",
             "Temperature_BMP180", "Pressure", "Altitude",
             "PM2.5(CF1)", "PM1.0(CF1)", "PM10(CF1)",
             "PM1.0(AMB)", "PM2.5(AMB)", "PM10(AMB)",
             "PM0.3(RAW)", "PM0.5(RAW)", "PM1.0(RAW)", "PM2.5(RAW)", "PM5.0(RAW)", "PM10(RAW)"]
data_unit = ["",
             "%", "C",
             "C", "Pa", "m",
             "ug/m3", "ug/m3", "ug/m3",
             "ug/m3", "ug/m3", "ug/m3",
             "", "", "", "", "", ""]

data_error_tolerance = [0,  # Time
                        0.05,  # Humidity
                        0.1,  # Temperature ~2C/20C
                        0.1,  # Temperature ~2C/20C
                        0.001,  # Pressure ~100Pa/100kPa
                        1,  # Altitude
                        0.5,  # PM2.5(CF1)
                        0.5,  # PM1.0(CF1)
                        0.6,  # PM10(CF1)
                        0.5,  # PM1.0(AMB)
                        0.5,  # PM2.5(AMB)
                        0.6,  # PM10(AMB)
                        10,  # PM0.3(RAW)
                        10,  # PM0.5(RAW)
                        10,  # PM1.0(RAW)
                        10,  # PM2.5(RAW)
                        10,  # PM5.0(RAW)
                        10,  # PM10(RAW)
                        ]

color_code = {"blue": 34,
              "red": 31,
              "yellow": 33,
              "green": 32,
              "none": 0}


def process_time(s):
    return base_time + datetime.timedelta(days=int(s[0:2]), hours=int(s[3:5]), minutes=int(s[6:8]), seconds=int(s[9:11]))


def format_print(info_time, info, color_name, data_line):
    print("\033[" + str(color_code[color_name]) + "m", end="")
    if info_time:
        info = "[" + datetime.datetime.strftime(info_time, "%d %H:%M:%S") + "] " + info
    print(info, end="")
    print("\033[0m", end="")
    print()
    if data_line:
        # data_line has '\n'
        print(data_line, end="")


csv = open(csv_filename, "w")

# Write header
for i in range(len(data_list)):
    if data_unit[i] != "":
        csv.write(data_list[i] + "(" + data_unit[i] + ")")
    else:
        csv.write(data_list[i])

    if i != len(data_list) - 1:
        csv.write(",")
    else:
        csv.write("\n")

# Load log
log_time = []
log_info = []

log_sd_recovery_count = 0
log_pms7003_error_count = 0
log_dht22_error_count = 0

with open(path + "/KIT.LOG", "r") as logFile:
    for line in logFile:
        log_time.append(process_time(line[:11]))
        log_info.append(line[15:-1])
        if line[15:-1] == "SD Recovered":
            log_sd_recovery_count += 1
        elif line[15:-1] == "PMS7003 Data Invalid":
            log_pms7003_error_count += 1
        elif line[15:24] == "DHT22 Err":
            log_dht22_error_count += 1

# Load data file names and sort
datafiles = []
for dataFile in os.listdir(path):
    if dataFile[-3:] == "TXT":
        datafiles.append(dataFile)
datafiles.sort()

# Process data
time = process_time("00:00:00:00")
last_time = time

current_log_index = 0

total_sample_count = 0
valid_sample_count = 0

sample_lag_count = 0
data_change_warning_count = 0
dht22_error_count = 0
pms7003_error_count = 0

data_cache = []
data_cache_time = time
data_cache_line = ""

last_line = ""

for filename in datafiles:
    with open(os.path.join(path, filename), "r") as file:
        for line in file:

            p = line.split(",")
            total_sample_count += 1
            validity = True
            data_change_warning = False

            # Get data Time
            try:
                time = process_time(p[0])
            except:

                format_print(None, "Invalid line found in " + filename, "red", line)
                validity = False

            else:

                data = [time]

                # Print log records before
                while current_log_index < len(log_time) and log_time[current_log_index] < time:
                    format_print(log_time[current_log_index], log_info[current_log_index], "blue", None)
                    current_log_index += 1

                # Check for unusual sample interval
                if (time - last_time).seconds > 6:
                    format_print(time, str((time - last_time).seconds) + "s sample interval", "yellow",
                                 "    " + last_line + "    " + line)
                    sample_lag_count += 1

                # Check data
                for i in range(1, len(data_list)):
                    try:
                        data.append(float(p[i]))
                    except:

                        if i == 1:
                            format_print(time, "DHT22 data invalid", "red", line)
                            dht22_error_count += 1
                        elif i == 6:
                            format_print(time, "PMS7003 data invalid", "red", line)
                            pms7003_error_count += 1
                        else:
                            format_print(time, data_list[i] + " data invalid", "red", line)

                        validity = False
                        break

                    else:
                        if i < len(data_cache) and data_cache[i] != 0:
                            if abs(data[i] - data_cache[i]) / data_cache[i] > data_error_tolerance[i]:
                                format_print(time,
                                             str(round(abs(data[i] - data_cache[i]) / data_cache[i] * 100, 2))
                                             + "% change (" + str(data_cache[i]) + " - " + str(data[i]) +
                                             ") in " + data_list[i] + " found since " +
                                             datetime.datetime.strftime(data_cache_time, "%H:%M:%S"),
                                             "yellow",
                                             "    " + data_cache_line + "    " + line)
                                data_change_warning = True

            if validity:
                csv.write(datetime.datetime.strftime(time, "%Y-%m-%d %H:%M:%S") + line[15:])
                data_cache = data
                data_cache_time = time
                data_cache_line = line
                valid_sample_count += 1
                if data_change_warning:
                    data_change_warning_count += 1
                last_time = time
                last_line = line

while current_log_index < len(log_time):
    format_print(log_time[current_log_index], log_info[current_log_index], "blue", None)
    current_log_index += 1

csv.close()

# Print summary
print()
format_print(None, "--------- Summary ---------", "none", None)
format_print(None, str(len(datafiles)) + " data file(s) processed (" + datafiles[0] + " - " + datafiles[-1] + ")", "none",
             None)
format_print(None, "Total samples: " + str(total_sample_count), "none", None)
format_print(None, "Valid samples: " + str(valid_sample_count) + " (" + str(
    round(valid_sample_count / total_sample_count * 100, 2)) + "%)", "green", None)
format_print(None, "SD recovery log: " + str(log_sd_recovery_count), "blue", None)
format_print(None, "DHT22 Error log: " + str(log_dht22_error_count), "blue", None)
format_print(None, "PMS7003 Error log: " + str(log_pms7003_error_count), "blue", None)
format_print(None,
             "Lag samples: " + str(sample_lag_count) + " (" + str(
                 round(sample_lag_count / total_sample_count * 100, 2)) + "%)", "yellow", None)
format_print(None, "Data change warnings: " + str(data_change_warning_count) + " (" + str(
    round(data_change_warning_count / total_sample_count * 100, 2)) + "%)", "yellow", None)
format_print(None, "DHT22 Error Data: " + str(dht22_error_count) + " (" + str(
    round(dht22_error_count / total_sample_count * 100, 2)) + "%)", "red", None)
format_print(None, "PMS7003 Error Data: " + str(pms7003_error_count) + " (" + str(
    round(pms7003_error_count / total_sample_count * 100, 2)) + "%)", "red", None)

print()
print("data.csv generated.")
