import os
import shutil

def backup_and_delete_lines(root_dir, target_file, N):
    for folder_name, subfolders, filenames in os.walk(root_dir):
        for filename in filenames:
            if filename == target_file:
                    file_path = os.path.join(folder_name, filename)
                    if "TESE_CAP5" in file_path:
                        backup_path = file_path + '.bkp'
                        if( not os.path.isfile(backup_path) ):
                            shutil.copyfile(file_path, backup_path)
                        
                        with open(file_path,'r') as file:
                            lines = file.readlines()
                        
                        with open(file_path,'w') as file:
                            file.writelines(lines[:N])

backup_and_delete_lines('../simulation', 'FITlog.tsv', 2155)
backup_and_delete_lines('../simulation', 'matex.txt', 6465)
backup_and_delete_lines('../simulation', 'SystemPower.tsv', 2155)
backup_and_delete_lines('../simulation', 'SystemTemperature.tsv', 2155)