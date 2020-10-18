import os
import datetime
import sys

try:
	filename = sys.argv[1]
except:
	print('Wrong file path')
	sys.exit()

payload = []

modified = datetime.datetime.fromtimestamp(os.stat(filename).st_mtime).strftime('%Y-%m-%d %H:%M:%S')
print('Parsing file {}, modified at {}'.format(filename, modified))

with open(filename, 'r') as f:
	line = f.readline()
	while line:
		try:
			length = int(line[1:3], 16)
			if length > 0:
				address = line[3:7]
				data = line[9:9 + length * 2]
				
				for i in range(0, len(data) - 1, 2):
					payload.append(data[i:i+2])
		except:
			pass

		line = f.readline()

result = 'uint8_t {name}[{len}] = {{ {dump} }};'.format(name="_".join(['xr77129', filename.split('.')[0].lower(), 'cfg']), len=len(payload), dump=", ".join(['0x' + i for i in payload]))
print(result)