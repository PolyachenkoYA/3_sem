import os
import sys
import shutil
import datetime
import random
import string
import glob
import difflib

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
    root_path = launch_path[:launch_path.rfind('/')]
    launch_name = launch_path[len(root_path)+1:]        
    backup_name = launch_name + '_backup_' + str(datetime.datetime.now()).replace(' ', '_')
    backup_path = os.path.join(root_path, backup_name)        
    if(os.path.isfile(os.path.join(launch_path, stop_filename))):
        print('backup of "' + launch_name + '" was terminated')
        return -1
    
    # ------------------------------ find last copy ----------------------------
    backup_dirs_pattern = launch_path + '_*/'
    backup_dirs = glob.glob(backup_dirs_pattern)
    if(backup_dirs):
        last_backup_dir = backup_dirs[-1]
    else:
        last_backup_dir = ''
    
    # ------------------------------- create a new copy -----------------------------
    root_backup_folder_created = False
    for root, directories, filenames in os.walk(launch_path):
        for filename in filenames: 
            # ------------- parse paths ---------------
            fullfile = os.path.join(root,filename)
            os.system('file ' + fullfile + ' > ' + rnd_name)
            rnd_file = open(rnd_name)            
            is_text = ('text' in rnd_file.read())
            rnd_file.close()
            rel_path = fullfile[len(launch_path) + 1:]            
            
            # --------------- check for present backups ----------
            if((rel_path == rnd_name) or (rel_path == stop_filename) or (not is_text) or (rel_path == backup_logfile_name)):
                need_to_copy = False
            else:
                if(not backup_dirs):
                    need_to_copy = True
                else:
                    old_copy_found = False
                    for old_backup in reversed(backup_dirs):
                        old_copy_path = os.path.join(old_backup, rel_path)
                        if(os.path.isfile(old_copy_path)):
                            old_copy_found = True
                            need_to_copy = (os.path.getmtime(old_copy_path) < os.path.getmtime(fullfile))
                            break
                    if(not old_copy_found):
                        need_to_copy = True
            #print(rel_path + ' - ' + str(need_to_copy) + '\n------------------')
            
            # ----------------- write a copy -----------------
            if(need_to_copy):
                new_folder_index = rel_path.rfind('/')
                if(not root_backup_folder_created):
                    os.mkdir(backup_path)
                    root_backup_folder_created = True
                if(new_folder_index >= 0):
                    new_folder_path = os.path.join(backup_path, rel_path[:new_folder_index])
                    if(not os.path.isdir(new_folder_path)):
                        os.makedirs(new_folder_path)
                        #print('new: ' + new_folder_path)
                
                shutil.copyfile(fullfile, os.path.join(backup_path, rel_path))
                        
    os.remove(os.path.join(launch_path, rnd_name))
    return 0
            
if __name__ == '__main__':
    sys.exit(main())
