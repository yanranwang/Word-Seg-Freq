import os
filepath = "./"
filename = 'query.1kw.truth.sort'
resultname = 'query.rm_space.1kw'
file = filepath + filename
file_r = open(file,'r')
file_w = open(filepath + resultname,'w')
for line in file_r.readlines():
    # remove empty line
    if not line.strip():
        continue
    rfile.write(line.rstrip(' \n\t') + '\n')
file_r.close()
file_w.close()
