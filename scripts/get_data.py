import csv

def get_label(heuristic):
    if heuristic == 'worst':
        return "Grouped"
    if heuristic == 'pattern':
        return "Pattern"
    if heuristic == 'pidtm':
        return "PID"
    if heuristic == 'pidtm_m':
        return "PID (MIG)"
    if heuristic == 'chronos':
        return "FLEA"
    if heuristic == 'chronos_m':
        return "FLEA (MIG)"
    if heuristic == 'chronos2':
        return "FLEA+"
    if heuristic == 'chronos2_m':
        return "FLEA+ (MIG)"

def get_nick(heuristic):
    if heuristic == 'Grouped':
        return "worst"
    if heuristic == 'Pattern':
        return "pattern"
    if heuristic == 'PID':
        return "pidtm"
    if heuristic == 'PID (MIG)':
        return "pidtm_m"
    if heuristic == 'FLEA':
        return "chronos"
    if heuristic == 'FLEA (MIG)':
        return "chronos_m"
    if heuristic == 'FLEA+':
        return "chronos2"
    if heuristic == 'FLEA+ (MIG)':
        return "chronos2_m"

def get_data(information, heurnick):
    file_path = 'TSV.tsv'
    heurnick = get_nick(heurnick)
    # Define a dictionary to store the information for each Heuristic
    heuristic_info = {}

    # Open the TSV file
    with open(file_path, 'r', newline='', encoding='utf-8') as tsvfile:
        # Create a CSV reader object specifying the tab delimiter
        tsv_reader = csv.DictReader(tsvfile, delimiter='\t')
        
        # Iterate through each row in the TSV
        for row in tsv_reader:
            # Extract the Heuristic and AvgHop values
            heuristic = row['Heuristic']
            info_char = row[information].replace(',', '.')
                        
            # Convert the information string to float
            try:
                info_value = float(info_char)
            except ValueError:
                continue  # Skipping invalid or header rows
            
            # Append the AvgHop value to the list corresponding to the Heuristic key
            if heuristic in heuristic_info:
                heuristic_info[heuristic].append(info_value)
            else:
                heuristic_info[heuristic] = [info_value]

    # Print the results
    for heuristic, avg_hop_list in heuristic_info.items():
        #print(f"{get_label(heuristic)} = {avg_hop_list}")
        if(heuristic == heurnick):
            return avg_hop_list


if __name__ == '__main__': # chamada da funcao principal
    get_data("MTTF monte")