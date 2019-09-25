import os
import sys
import subprocess
from utils.listdir import list_all_files

# rename source code files
#python3 rename.py [yaml_file_dir] [code_file_dir]

yaml_dir = sys.argv[1]
code_dir = sys.argv[2]

#yamls = list_all_files(yaml_dir)
files = list_all_files(code_dir)

for filename in files:
    basename, extension = os.path.splitext(os.path.basename(filename))
    yaml = yaml_dir+basename+'.yaml'
    cmd = "clang-rename -i --input='"+yaml+"' '"+filename+"'"
    obj = subprocess.Popen(cmd, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE, universal_newlines=True)
    cmd_out = obj.stdout.read()
    obj.stdout.close()
    cmd_error = obj.stderr.read()
    obj.stderr.close()
    print(cmd_error)
