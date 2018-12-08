#!/home/ypolyach/anaconda3/bin/python3 -tt

import os
import sys
import shutil
import datetime
import random
import string
import glob
import difflib
import filecmp

def run_it(command):
    print(command)
    return os.system(command) == 0
    
def randomword(length):
   letters = string.ascii_lowercase
   return ''.join(random.choice(letters) for i in range(length))

def main(): 
    # -------------------------- parse input ------------------------------
    rnd_name = 'qwertyuiopasdfghjklzxcvbnm' # random fixed string
    stop_filename = 'stop_backup'
    backup_logfile_name = 'backup.log'
    
    args = sys.argv[:]
    if(len(args) > 2):
        print('usage:\n' + args[0] + ' [flags]')
        return 0
        
    if(len(args) == 2):
        if('help' in args[1]):
            print('usage:\n     ' + args[0] + ' [flags]\n' + 
                  'info:\n'
                  '     stop file: "' + stop_filename + '"\n' + 
                  '     buffer file: "' + rnd_name + '"')
            return
        
    # -------------------------- parse paths ---------------------------------
    launch_path = os.getcwd()
    full_rnd_name = os.path.join(launch_path, rnd_name)
    root_path = launch_path[:launch_path.rfind('/')]
    launch_name = launch_path[len(root_path)+1:]
    backup_name = launch_name + '_backup_' + str(datetime.datetime.now()).replace(' ', '_')
    backup_path = os.path.join(root_path, backup_name)        
    if(os.path.isfile(os.path.join(launch_path, stop_filename))):
        print('backup of "' + launch_name + '" was terminated')
        return -1
    
    # ------------------------------ find last copy ---------------------------- r
    backup_dirs_pattern = launch_path + '_*/'
    backup_dirs = glob.glob(backup_dirs_pattern)
    #print(backup_dirs)
    if(backup_dirs):
        last_backup_dir = backup_dirs[-1]
    else:
        last_backup_dir = ''
    
    # ---------------------------- make backup -----------------------------
    root_backup_folder_created = False
    for root, directories, filenames in os.walk(launch_path):
        for filename in filenames: 
            # ------------- parse paths ---------------
            fullfile = os.path.join(root,filename)
            os.system('file ' + fullfile + ' > ' + full_rnd_name)
            rnd_file = open(full_rnd_name)
            is_text = ('text' in rnd_file.read())
            rnd_file.close()
            rel_path = fullfile[len(launch_path) + 1:]
            
            # ----------------- create a copy -----------------
            if((rel_path != rnd_name) and (rel_path != stop_filename) and is_text and (rel_path != backup_logfile_name)):
                if(backup_dirs):
                    # -------------- build revelant version ---------
                    original_found = False
                    original_path = ''
                    for old_backup in backup_dirs:
                        old_copy_path = os.path.join(old_backup, rel_path)
                        if(os.path.isfile(old_copy_path)):
                            if(original_found):
                                # ed -s orig < change
                                os.system('ed -s ' + full_rnd_name + ' < ' + old_copy_path)
                            else:
                                original_found = True
                                original_path = old_copy_path
                                shutil.copyfile(original_path, full_rnd_name)
                                
                need_to_copy = (backup_dirs and (not filecmp.cmp(full_rnd_name, fullfile))) or (not backup_dirs)
                
                if(need_to_copy):
                    # ------------------ create path ---------------
                    new_folder_index = rel_path.rfind('/')
                    if(not root_backup_folder_created):
                        os.mkdir(backup_path)
                        root_backup_folder_created = True
                    if(new_folder_index >= 0):
                        new_folder_path = os.path.join(backup_path, rel_path[:new_folder_index])
                        if(not os.path.isdir(new_folder_path)):
                            os.makedirs(new_folder_path)
                            #print('new folder: ' + new_folder_path)
                
                new_backup_file = os.path.join(backup_path, rel_path)
                if(backup_dirs):
                    # --------------- write new diff -----------------                    
                    if(not filecmp.cmp(full_rnd_name, fullfile)):
                        os.system('diff -e ' + full_rnd_name + ' ' + fullfile + ' > ' + new_backup_file)
                        os.system('echo "w" >> ' + new_backup_file)
                        os.system('echo "q" >> ' + new_backup_file)
                else:
                    # ----------------- write origianl copy -----------------
                    shutil.copyfile(fullfile, new_backup_file)
                    #print('new file: ' + os.path.join(backup_path, rel_path)) t
                        
    os.remove(full_rnd_name)
    return 0
            
if __name__ == '__main__':
    sys.exit(main())
