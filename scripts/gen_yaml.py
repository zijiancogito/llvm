# generate yaml file
import os
import sys
import subprocess
import yaml
from utils.listdir import list_all_files
# from utils.tools import clang_yaml

# yaml file format
# ---
# - Offset:  123
#   NewName:        bar1
# - Offset:  456
#   NewName:        bar2
# ...

# generate yaml file from sourcecode
# --help
# python3 gen_yaml.py [source_code_dir] [yaml_dir] [clang-yaml-path]

root_dir = sys.argv[1]
yaml_dir = sys.argv[2]
clang_yaml = sys.argv[3]
files = list_all_files(root_dir)

class function_info:
    def __init__(self):
        self.Parms = []
        self.Vars = []
    def addParm(self, parm):
        if self.Parms.append(parm):
            return True
        else:
            return False
    def addVar(self, var):
        if self.Vars.append(var):
            return True
        else:
            return False

for filename in files:
    cmd = clang_yaml + " '" +filename+"'"
    print(filename)
    obj = subprocess.Popen(cmd, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE, universal_newlines=True)
    cmd_out = obj.stdout.read().strip().split('\n')
    obj.stdout.close()
    cmd_error = obj.stderr.read()
    obj.stderr.close()
    # function_dict = {"param_list":[], "local_var_list":[]}
    function_dict = {}
    global_list = []
    unknown_list = []
    function_list = []
    yaml_list = []
    # print(cmd_out)
    # print(cmd_error)
    for item in cmd_out:
        Location = item.split('\t')[0]
        # Type = item.split('\t')[1]
        # Name = item.split('\t')[2]
        if Location=='FunctionDecl:':
            function_name =item.split('\t')[2]
            offset = int(item.split('\t')[-1])
            if function_name == 'main':
                pass
            else:
                function_list.append(offset)
            function_dict[function_name] = function_info()
        elif Location=='LocalDecl:':
            function = item.split('\t')[1]
            name = item.split('\t')[3]
            offset = int(item.split('\t')[-1])
            if function not in function_dict:
                function_dict[function] = function_info()
            else:
                pass
            function_dict[function].addVar(offset)
        elif Location=='ParmDecl:':
            function = item.split('\t')[1]
            name = item.split('\t')[3]
            offset = int(item.split('\t')[-1])
            if function == 'main':
                pass
            else:
                if function not in function_dict:
                    function_dict[function] = function_info()
                else:
                    pass
                function_dict[function].addParm(offset)
        elif Location=='GlobalDecl:':
            name = item.split('\t')[2]
            offset = int(item.split('\t')[-1])
            global_list.append(offset)
        elif Location=='UnknownDecl:':
            name = item.split('\t')[2]
            decl_type = item.split('\t')[1]
            offset = int(item.split('\t')[-1])
            unknown_list.append(offset)
        else:
            pass
    count = 0
    for item in function_list:
        pair = {'Offset': item, 'NewName': 'Function'+str(count)}
        yaml_list.append(pair)
        count += 1
    count = 0
    for item in global_list:
        pair = {'Offset': item, 'NewName': 'Global'+str(count)}
        yaml_list.append(pair)
        count += 1
    count = 0
    for item in unknown_list:
        pair = {'Offset': item, 'NewName': 'Unknown'+str(count)}
        yaml_list.append(pair)
        count += 1
    count = 0
    for func in function_dict.keys():
        count = 0
        for item in function_dict[func].Parms:
            if func == 'main':
                break
            pair = {'Offset': item, 'NewName': 'Parm'+str(count)}
            yaml_list.append(pair)
            count += 1
        count = 0
        for item in function_dict[func].Vars:
            pair = {'Offset': item, 'NewName': 'LocalVar'+str(count)}
            yaml_list.append(pair)
            count += 1
    # print(yaml_list)
    basename, extension = os.path.splitext(os.path.basename(filename))
    out = open(yaml_dir+basename+'.yaml','w')
    out.write('---\n')
    out.write(yaml.dump(yaml_list))